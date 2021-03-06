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
   Copyright (C) Wallix 2011
   Author(s): Christophe Grosjean, Martin Potier

   Unit test to write / read a "movie" from a file
   Using lib boost functions for testing
*/


#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestCapture
#include <errno.h>
#include <boost/test/auto_unit_test.hpp>
#include <algorithm>

#include <staticcapture.hpp>

BOOST_AUTO_TEST_CASE(TestCreateCapture)
{
    // Create a simple capture image and dump it to file
    Rect screen_rect(0, 0, 640, 480);
    StaticCapture gd(screen_rect.cx, screen_rect.cy, 24, NULL, NULL, NULL); 
    gd.opaque_rect(RDPOpaqueRect(screen_rect, WHITE), screen_rect);
    gd.opaque_rect(RDPOpaqueRect(screen_rect.shrink(5), BLACK), screen_rect);
    uint16_t y = screen_rect.cy - 1;
    for (uint16_t x = 0 ; x < screen_rect.cx ; x++){
        gd.line_to(RDPLineTo(0, 0, 0, x, y, BLUE, 0xCC, RDPPen(0, 1, GREEN)), screen_rect);
        gd.line_to(RDPLineTo(0, x, y, 0, 0, WHITE, 0xCC, RDPPen(0, 1, BLACK)), screen_rect);
    }
    y = screen_rect.cy - 1;
    for (uint16_t x = 0 ; x < screen_rect.cx ; x++){
        gd.line_to(RDPLineTo(0, screen_rect.cx - 1, 0, x, y, BLUE, 0xCC, RDPPen(0, 1, RED)), screen_rect);
        gd.line_to(RDPLineTo(0, x, y, screen_rect.cx - 1, 0, WHITE, 0xCC, RDPPen(0, 1, BLACK)), screen_rect);
    }
    gd.dump_png();
}
