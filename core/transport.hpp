/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010
   Author(s): Christophe Grosjean, Javier Caverni
   Based on xrdp Copyright (C) Jay Sorg 2004-2010

   Transport layer abstraction

*/


#if !defined(__TRANSPORT_HPP__)
#define __TRANSPORT_HPP__

#include <sys/types.h> // recv, send
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "error.hpp"
#include "log.hpp"


static inline int connect(const char* ip, int port,
             int nbretry = 2, int retry_delai_ms = 1000) throw (Error)
{
    int sck = 0;
    LOG(LOG_INFO, "connecting to %s:%d\n", ip, port);
    // we will try connection several time
    // the trial process include socket opening, hostname resolution, etc
    // because some problems can come from the local endpoint,
    // not necessarily from the remote endpoint.
    for (int trial = 0; ; trial++){
        try {
            sck = socket(PF_INET, SOCK_STREAM, 0);

            /* set snd buffer to at least 32 Kbytes */
            int snd_buffer_size;
            unsigned int option_len = sizeof(snd_buffer_size);
            if (0 == getsockopt(sck, SOL_SOCKET, SO_SNDBUF, &snd_buffer_size, &option_len)) {
                if (snd_buffer_size < 32768) {
                    snd_buffer_size = 32768;
                    if (-1 == setsockopt(sck,
                            SOL_SOCKET,
                            SO_SNDBUF,
                            &snd_buffer_size, sizeof(snd_buffer_size))){
                        LOG(LOG_WARNING, "setsockopt failed with errno=%d", errno);
                        throw Error(ERR_SOCKET_CONNECT_FAILED);
                    }
                }
            }
            else {
                LOG(LOG_WARNING, "getsockopt failed with errno=%d", errno);
                throw Error(ERR_SOCKET_CONNECT_FAILED);
            }

            struct sockaddr_in s;
            memset(&s, 0, sizeof(struct sockaddr_in));
            s.sin_family = AF_INET;
            s.sin_port = htons(port);
            s.sin_addr.s_addr = inet_addr(ip);
            if (s.sin_addr.s_addr == INADDR_NONE) {
            #warning gethostbyname is obsolete use new function getnameinfo
                LOG(LOG_INFO, "Asking ip to DNS for %s\n", ip);
                struct hostent *h = gethostbyname(ip);
                if (!h) {
                    LOG(LOG_ERR, "DNS resolution failed for %s"
                        " with errno =%d (%s)\n",
                        ip,
                        errno, strerror(errno));
                    throw Error(ERR_SOCKET_GETHOSTBYNAME_FAILED);
                }
                s.sin_addr.s_addr = *((int*)(*(h->h_addr_list)));
            }
            #warning we should control and detect timeout instead of relying on default connect behavior. Maybe set O_NONBLOCK and use poll to manage timeouts ?
            if (-1 == ::connect(sck, (struct sockaddr*)&s, sizeof(s))){
                LOG(LOG_INFO, "connection to %s failed"
                    " with errno = %d (%s)\n",
                    ip,
                    errno, strerror(errno));
                throw Error(ERR_SOCKET_CONNECT_FAILED);
            }
            fcntl(sck, F_SETFL, fcntl(sck, F_GETFL) | O_NONBLOCK);
            break;
        }
        catch (Error){
            if (trial >= nbretry){
                LOG(LOG_INFO, "All trials done connecting to %s\n", ip);
                throw;
            }
        }
        LOG(LOG_INFO, "Will retry connecting to %s in %d ms (trial %d on %d)\n",
            ip, retry_delai_ms, trial, nbretry);
        usleep(retry_delai_ms);
    }
    LOG(LOG_INFO, "connection to %s succeeded : socket %d\n", ip, sck);
    return sck;
}

class Transport {
public:
    uint64_t total_received;
    uint64_t last_quantum_received;
    uint64_t total_sent;
    uint64_t last_quantum_sent;
    uint64_t quantum_count;

    Transport() :
        total_received(0),
        last_quantum_received(0),
        total_sent(0),
        last_quantum_sent(0),
        quantum_count(0)
    {}

    void tick() {
        quantum_count++;
        last_quantum_received = 0;
        last_quantum_sent = 0;
    }

    virtual void recv(char ** pbuffer, size_t len) throw (Error) = 0;
    virtual void send(const char * buffer, int len) throw (Error) = 0;
    virtual void disconnect() = 0;
};

class GeneratorTransport : public Transport {

    size_t current;
    char * data;
    size_t len;

    public:

    GeneratorTransport(const char * data, size_t len)
        : Transport(), current(0), data(0), len(len)
    {
        this->data = (char *)malloc(len);
        memcpy(this->data, data, len);
    }

    virtual void recv(char ** pbuffer, size_t len) throw (Error) {
        if (current > len){
            throw Error(ERR_SOCKET_ERROR, 0);
        }
        memcpy(*pbuffer, (const char *)(&this->data[current]), len);
        *pbuffer += len;
        current += len;
    }

    virtual void send(const char * buffer, int len) throw (Error) {
        // send perform like a /dev/null and does nothing in generator transport
    }

    virtual void disconnect() {
    }
};


class LoopTransport : public Transport {

    virtual void recv(char ** pbuffer, size_t len) throw (Error) {
    }
    virtual void send(const char * buffer, int len) throw (Error) {
    }
    virtual void disconnect() {
    }
};

class SocketTransport : public Transport {
    public:
        int sck;
        int sck_closed;

    SocketTransport(int sck) : Transport()
    {
        this->sck = sck;
        this->sck_closed = 0;
    }

    ~SocketTransport(){
        if (!this->sck_closed){
            this->disconnect();
        }
    }
    static bool try_again(int errnum){
        int res = false;
        switch (errno){
            case EAGAIN:
            /* case EWOULDBLOCK: */ // same as EAGAIN on Linux
            case EINPROGRESS:
            case EALREADY:
            case EBUSY:
            case EINTR:
                res = true;
                break;
            default:
                ;
        }
        return res;
    }

    virtual void disconnect(){
        LOG(LOG_INFO, "Socket %d : closing connection\n", this->sck);
        if (this->sck != 0) {
            shutdown(this->sck, 2);
            close(this->sck);
        }
        this->sck = 0;
        this->sck_closed = 1;
    }

    enum direction_t {
        NONE = 0,
        RECV = 1,
        SEND = 2
    };

    void wait_recv_ready(int delay_ms) throw (Error)
    {
        this->wait_ready(RECV, delay_ms);
    }

    void wait_send_ready(int delay_ms) throw (Error)
    {
        this->wait_ready(SEND, delay_ms);
    }

    void wait_ready(direction_t d, int delay_ms) throw (Error)
    {
        fd_set fds;
        struct timeval time;

        time.tv_sec = delay_ms / 1000;
        time.tv_usec = (delay_ms * 1000) % 1000000;
        FD_ZERO(&fds);
        FD_SET(((unsigned int)this->sck), &fds);
        if (select(this->sck + 1,
            (d & RECV)? &fds : 0,
            (d & SEND)? &fds : 0,
            0, &time) > 0) {
            int opt = 0;
            unsigned int opt_len = sizeof(opt);
            getsockopt(this->sck, SOL_SOCKET, SO_ERROR, (char*)(&opt), &opt_len);
            // Test if we got a socket error

            if (opt) {
                LOG(LOG_INFO, "Socket error detected");
                throw Error(ERR_SESSION_TERMINATED);
            }

        }
    }

    virtual void recv(char ** input_buffer, size_t total_len) throw (Error)
    {
//        uint8_t * start = (uint8_t*)(*input_buffer);
        int len = total_len;
        char * pbuffer = *input_buffer;

        if (this->sck_closed) {
            LOG(LOG_INFO, "socket allready closed\n");
            throw Error(ERR_SOCKET_ALLREADY_CLOSED);
        }

        while (len > 0) {
            int rcvd = ::recv(this->sck, pbuffer, len, 0);
            switch (rcvd) {
                case -1: /* error, maybe EAGAIN */
                    if (!this->try_again(errno)) {
                        LOG(LOG_INFO, "closing socket on recv\n");
                        this->sck_closed = 1;
                        throw Error(ERR_SOCKET_ERROR, errno);
                    }
                    this->wait_ready(RECV, 10);
                    break;
                case 0: /* no data received, socket closed */
                    LOG(LOG_INFO, "no data received socket %d closed on recv\n", this->sck);
                    this->sck_closed = 1;
                    throw Error(ERR_SOCKET_CLOSED);
                default: /* some data received */
                    pbuffer += rcvd;
                    len -= rcvd;
            }
        }
        *input_buffer = pbuffer;
        total_received += total_len;
        last_quantum_received += total_len;

//        uint8_t * bb = start;
//        LOG(LOG_INFO, "recv on socket %u : len=%u buffer=%p"
//            " [%0.2X %0.2X %0.2X %0.2X]"
//            " [%0.2X %0.2X %0.2X]"
//            " [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X ...]",
//            this->sck, total_len, *input_buffer,
//            bb[0], bb[1], bb[2], bb[3],
//            bb[4], bb[5], bb[6],
//            bb[7], bb[8], bb[9], bb[10], bb[11],
//            bb[12], bb[13], bb[14], bb[15], bb[16]);
    }

    virtual void send(const char * buffer, int len) throw (Error)
    {
//        LOG(LOG_INFO, "send on socket %u : len=%u buffer=%p"
//            " [%0.2X %0.2X %0.2X %0.2X]"
//            " [%0.2X %0.2X %0.2X]"
//            " [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X ...]",
//            this->sck, len, buffer,
//            (uint8_t)buffer[0], (uint8_t)buffer[1], (uint8_t)buffer[2], (uint8_t)buffer[3],
//            (uint8_t)buffer[4], (uint8_t)buffer[5], (uint8_t)buffer[6],
//            (uint8_t)buffer[7], (uint8_t)buffer[8], (uint8_t)buffer[9], (uint8_t)buffer[10], (uint8_t)buffer[11],
//            (uint8_t)buffer[12], (uint8_t)buffer[13], (uint8_t)buffer[14], (uint8_t)buffer[15], (uint8_t)buffer[16]);
        if (this->sck_closed) {
            throw Error(ERR_SOCKET_ALLREADY_CLOSED);
        }
        int total = 0;
        while (total < len) {
            int sent = ::send(this->sck, buffer + total, len - total, 0);
            switch (sent){
            case -1:
                if (!this->try_again(errno)) {
                    LOG(LOG_INFO, "%s sck=%d\n", strerror(errno), this->sck);
                    this->sck_closed = 1;
                    throw Error(ERR_SOCKET_ERROR, errno);
                }
                this->wait_ready(SEND, 10);
                break;
            case 0:
                LOG(LOG_INFO, "socket closed on sending %s sck=%d\n", strerror(errno), this->sck);
                this->sck_closed = 1;
                throw Error(ERR_SOCKET_CLOSED, errno);
            default:
                total = total + sent;
            }
        }
        total_sent += len;
        last_quantum_sent += len;
    }

    private:

};

#endif
