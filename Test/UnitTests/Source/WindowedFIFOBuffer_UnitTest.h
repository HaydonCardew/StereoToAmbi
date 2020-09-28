/*
  ==============================================================================

    WindowedFIFIOBuffer_UnitTest.h
    Created: 27 Sep 2020 10:49:14pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "WindowedFIFOBuffer.h"
#include "gtest/gtest.h"
#include <vector>
   
TEST(WindowedFIFOBuffer, Create)
{
    WindowedFIFOBuffer fifo(10);
    std::vector<float> data(10, 0);
    EXPECT_TRUE(fifo.write(&data[0], 10) == 10);
}
