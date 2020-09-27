/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "gtest/gtest.h"

TEST(FIRST, TEST)
{
    EXPECT_EQ(1,1);
}

//==============================================================================
int main (int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
