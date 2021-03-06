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

   Unit test to region object
   Using lib boost functions, some tests need to be added

*/


#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestRegion
#include <boost/test/auto_unit_test.hpp>

#include "rect.hpp"
#include "region.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include <boost/foreach.hpp>


BOOST_AUTO_TEST_CASE(TestRegion)
{
    /* create a region */

    Rect r1(10, 110, 10, 10);
    Region region;
    region.rects.push_back(r1);
    region.rects.push_back(r1);
    region.rects.push_back(r1);

    int sum_left = 0;
    for (size_t i = 0 ; i < region.rects.size() ; i++){
        sum_left += region.rects[i].x;
    }
    BOOST_CHECK_EQUAL(30, sum_left);

    /* A region is basically a zone defined by adding or substracting rects */
    // if we subtract a rectangle inside region, we get 4 smaller rectangle around it

    //   x----------------x
    //   x                x
    //   x     x-----x    x
    //   x     x     x    x
    //   x     x-----x    x
    //   x                x
    //   x                x
    //   x----------------x
    //
    Region region2;
    region2.rects.push_back(Rect(10,10,90,90));
    BOOST_CHECK_EQUAL(1, region2.rects.size());

    // (10,10)
    //   x----------------x
    //   x        A       x A= (10, 10, 100, 30) Rect(10, 10, 90, 20)
    //   x-----x-----x----x
    //   x  B  x     x C  x B= (10, 30, 30, 50)  Rect(10, 30, 20, 20)
    //   x-----x-----x----x C= (50, 30, 100, 50) Rect(50, 30, 50, 20)
    //   x                x
    //   x       D        x D= (10, 50, 100, 100) Rect(10, 50, 90, 50);
    //   x----------------x
    //                  (100, 100)
    region2.subtract_rect(Rect(30, 30, 20, 20));
    BOOST_CHECK_EQUAL(4, region2.rects.size());

    BOOST_CHECK(region2.rects[0].equal(Rect(10, 10, 90, 20))); // A
    BOOST_CHECK(region2.rects[1].equal(Rect(10, 30, 20, 20))); // B
    BOOST_CHECK(region2.rects[2].equal(Rect(50, 30, 50, 20))); // C
    BOOST_CHECK(region2.rects[3].equal(Rect(10, 50, 90, 50))); // D

    // we substract a traversing rectangle
    Region region3;
    region3.rects.push_back(Rect(10,10,90,90));
    BOOST_CHECK_EQUAL(1, region3.rects.size());


    //         x-----x
    //         x     x
    //   x-----x-----x----x
    //   x     x     x    x
    //   x     x     x    x
    //   x  A  x     x  B x
    //   x     x     x    x
    //   x     x     x    x
    //   x     x     x    x
    //   x-----x-----x----x
    //         x     x
    //         x-----x

    region3.subtract_rect(Rect(30, 5, 20, 150));
    BOOST_CHECK_EQUAL(2, region3.rects.size());

    BOOST_CHECK(region3.rects[0].equal(Rect(10, 10, 20, 90))); // A
    BOOST_CHECK(region3.rects[1].equal(Rect(50, 10, 50, 90))); // B

}
