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

   Unit test for bitmap cache class

*/


#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBitmapCache
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>
#include "bitmap.hpp"
#include "bitmap_cache.hpp"
#include "colors.hpp"
#include "config.hpp"
#include <sys/time.h>


BOOST_AUTO_TEST_CASE(TestCreateBitmapCache)
{
    BGRPalette palette;
    std::stringstream oss("");
    Inifile ini(oss);
    ClientInfo ci(&ini);
    ci.use_bitmap_comp = true;
    ci.bitmap_cache_persist_enable = true;
    ci.bitmap_cache_version = 0;
    // "small" bitmaps (limit values reduced for tests... 4 pixels max)
    ci.cache1_entries = 30;
    ci.cache1_size = 16;
    // "medium" bitmaps (limit values reduced for tests... 40 pixels max)
    ci.cache2_entries = 10;
    ci.cache2_size = 160;
    // "big" bitmaps (limit values reduced for tests... 100 pixels max)
    ci.cache3_entries = 10;
    ci.cache3_size = 400;

    BitmapCache cache(&ci);

    #define RED30 \
        {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF},\
        {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF},\
        {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF},\
        {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF},\
        {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}, {0,0,0xFF}

    #define GREEN30 \
        {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0},\
        {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0},\
        {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0},\
        {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0},\
        {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}, {0,0xFF,0}

    #define BLUE30 \
        {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0},\
        {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0},\
        {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0},\
        {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0},\
        {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}, {0xFF,0,0}

    #define WHITE10 \
        {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, \
        {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}, {0xFF,0xFF,0xFF}

    #define WHITE100 WHITE10, WHITE10, WHITE10, WHITE10, WHITE10,\
        WHITE10, WHITE10, WHITE10, WHITE10, WHITE10

    #define RGB100x10 \
        { RED30, GREEN30, BLUE30, WHITE10 },{ RED30, GREEN30, BLUE30, WHITE10 },\
        { RED30, GREEN30, BLUE30, WHITE10 },{ RED30, GREEN30, BLUE30, WHITE10 },\
        { RED30, GREEN30, BLUE30, WHITE10 },{ RED30, GREEN30, BLUE30, WHITE10 },\
        { RED30, GREEN30, BLUE30, WHITE10 },{ RED30, GREEN30, BLUE30, WHITE10 },\
        { RED30, GREEN30, BLUE30, WHITE10 },{ RED30, GREEN30, BLUE30, WHITE10 }

    #define WHITE100x10 \
        { WHITE100 }, { WHITE100 }, { WHITE100 }, { WHITE100 }, { WHITE100 },\
        { WHITE100 }, { WHITE100 }, { WHITE100 }, { WHITE100 }, { WHITE100 }

    uint8_t big_picture8[100][100][3] = {
        WHITE100x10,
        RGB100x10, RGB100x10, RGB100x10,
        RGB100x10, RGB100x10, RGB100x10,
        RGB100x10, RGB100x10, RGB100x10
        };

    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBWWWWWWWWWW

    const uint8_t * big_picture = (uint8_t*)big_picture8;

    {


    {
    uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(25, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
    uint8_t cache_id  = (cache_ref >> 16);
    uint16_t cache_idx = (cache_ref & 0xFFFF);

    BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);

    BOOST_CHECK_EQUAL(2521917587U, cache.big_bitmaps[0].pbmp->get_crc());

    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..
    // RrrRrrRrrRrrRrrGggGggGggGggGgg..

    using namespace std;

    // width is padded to next multiple of 4 for rdesktop compatibility
    BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
    BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    BOOST_CHECK(entry->pbmp->isset_bpp(24));

    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[0]);
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[1]);
    BOOST_CHECK_EQUAL(0xFF, entry->pbmp->data_co(24)[2]);

    // Check first pixel
    // -----------------
    // :::RrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......

    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[0]+(entry->pbmp->data_co(24)[1]<<8)+(entry->pbmp->data_co(24)[2]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[3]+(entry->pbmp->data_co(24)[4]<<8)+(entry->pbmp->data_co(24)[5]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[6]+(entry->pbmp->data_co(24)[7]<<8)+(entry->pbmp->data_co(24)[8]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[9]+(entry->pbmp->data_co(24)[10]<<8)+(entry->pbmp->data_co(24)[11]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[12]+(entry->pbmp->data_co(24)[13]<<8)+(entry->pbmp->data_co(24)[14]<<16));

    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[15]+(entry->pbmp->data_co(24)[16]<<8)+(entry->pbmp->data_co(24)[17]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[18]+(entry->pbmp->data_co(24)[19]<<8)+(entry->pbmp->data_co(24)[20]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[21]+(entry->pbmp->data_co(24)[22]<<8)+(entry->pbmp->data_co(24)[23]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[24]+(entry->pbmp->data_co(24)[25]<<8)+(entry->pbmp->data_co(24)[26]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[27]+(entry->pbmp->data_co(24)[28]<<8)+(entry->pbmp->data_co(24)[29]<<16));

    // padded with 2 black pixels
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[30]+(entry->pbmp->data_co(24)[31]<<8)+(entry->pbmp->data_co(24)[32]<<16));
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[33]+(entry->pbmp->data_co(24)[34]<<8)+(entry->pbmp->data_co(24)[35]<<16));


    int row_size = 36;
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size+0]+(entry->pbmp->data_co(24)[row_size+1]<<8)+(entry->pbmp->data_co(24)[row_size+2]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size+3]+(entry->pbmp->data_co(24)[row_size+4]<<8)+(entry->pbmp->data_co(24)[row_size+5]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size+6]+(entry->pbmp->data_co(24)[row_size+7]<<8)+(entry->pbmp->data_co(24)[row_size+8]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size+9]+(entry->pbmp->data_co(24)[row_size+10]<<8)+(entry->pbmp->data_co(24)[row_size+11]<<16));
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size+12]+(entry->pbmp->data_co(24)[row_size+13]<<8)+(entry->pbmp->data_co(24)[row_size+14]<<16));

    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[row_size+15]+(entry->pbmp->data_co(24)[row_size+16]<<8)+(entry->pbmp->data_co(24)[row_size+17]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[row_size+18]+(entry->pbmp->data_co(24)[row_size+19]<<8)+(entry->pbmp->data_co(24)[row_size+20]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[row_size+21]+(entry->pbmp->data_co(24)[row_size+22]<<8)+(entry->pbmp->data_co(24)[row_size+23]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[row_size+24]+(entry->pbmp->data_co(24)[row_size+25]<<8)+(entry->pbmp->data_co(24)[row_size+26]<<16));
    BOOST_CHECK_EQUAL(0xFF00, entry->pbmp->data_co(24)[row_size+27]+(entry->pbmp->data_co(24)[row_size+28]<<8)+(entry->pbmp->data_co(24)[row_size+29]<<16));

    // padded with 2 black pixels
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[row_size+30]+(entry->pbmp->data_co(24)[row_size+31]<<8)+(entry->pbmp->data_co(24)[row_size+32]<<16));
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[row_size+33]+(entry->pbmp->data_co(24)[row_size+34]<<8)+(entry->pbmp->data_co(24)[row_size+35]<<16));

    // Check pixel at coordinates (5,4)
    // --------------------------------
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrr:::GggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......
    // RrrRrrRrrRrrRrrGggGggGggGggGgg......

    BOOST_CHECK_EQUAL(0x00FF00, entry->pbmp->data_co(24)[row_size*4+5*3]+(entry->pbmp->data_co(24)[row_size*4+5*3+1]<<8)+(entry->pbmp->data_co(24)[row_size*4+5*3+2]<<16));

    // Check Padding at end of line 3
    // ------------------------------
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[row_size*3-6]);
    BOOST_CHECK_EQUAL(0, entry->pbmp->data_co(24)[row_size*3-3]);

    // Now beginning of line 4
    // -----------------------
    BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[row_size*3]+(entry->pbmp->data_co(24)[row_size*3+1]<<8)+(entry->pbmp->data_co(24)[row_size*3+2]<<16));

    // Check cache id and idx
    // ----------------------
    BOOST_CHECK_EQUAL(2, cache_id);
    BOOST_CHECK_EQUAL(0, cache_idx);

    // another cache request, we should get another cache_id
    // -----------------------------------------------------
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    // RrrRrrRrrRrrRrrRrrRrrRrrRrrRrr..
    }
    {
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(0, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[0]+(entry->pbmp->data_co(24)[1]<<8)+(entry->pbmp->data_co(24)[2]<<16));
        BOOST_CHECK_EQUAL(0xFF0000, entry->pbmp->data_co(24)[315]+(entry->pbmp->data_co(24)[316]<<8)+(entry->pbmp->data_co(24)[317]<<16));
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(1, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }
    {
        // request again the same picture, we should get the same cache_id
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(0, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(1, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }

    {
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(25, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(0, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }
    {
        // another part of the big image, but with the same drawing, same cache_id
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(25, 40, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(0, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }
    {
        // another picture, new cache
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(35, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(2, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }
    {
        // request again the same picture, we should get the same cache_id
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(25, 0, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(0, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }

    // another part of the big image, but with the same drawing, same cache_id
    {
        uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(25, 40, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
        uint8_t cache_id  = (cache_ref >> 16);
        uint16_t cache_idx = (cache_ref & 0xFFFF);

        BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
        BOOST_CHECK_EQUAL(2, cache_id);
        BOOST_CHECK_EQUAL(0, cache_idx);
        BOOST_CHECK_EQUAL(12, entry->pbmp->cx);
        BOOST_CHECK_EQUAL(10, entry->pbmp->cy);
    }

    uint32_t cache_ref = cache.add_bitmap(100, 100, big_picture, Rect(99, 98, 10, 10), 24, palette);

//    uint8_t send_type = (cache_ref >> 24);
    uint8_t cache_id  = (cache_ref >> 16);
    uint16_t cache_idx = (cache_ref & 0xFFFF);

    BitmapCacheItem * entry =  cache.get_item(cache_id, cache_idx);
    BOOST_CHECK_EQUAL(1, cache_id);
    BOOST_CHECK_EQUAL(0, cache_idx);
    BOOST_CHECK_EQUAL(4, entry->pbmp->cx);
    BOOST_CHECK_EQUAL(2, entry->pbmp->cy);

    #warning we should also create tests at bitmap levels (inner layers called by add_bitmap not involving cache)

    #warning add more test to check that will really be the older bitmap in cache that will be replaced when cache is full

    #warning add more tests to check behavior of different sizes of cache

    }

    BOOST_CHECK(true);
}
