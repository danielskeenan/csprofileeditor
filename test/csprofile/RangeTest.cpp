/**
 * @file RangeTest.cpp
 *
 * @author dankeenan
 * @date 5/6/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include <csprofile/parameter/Range.h>

using namespace csprofile::parameter;

TEST(RangeTest, Is16Bit) {
  EXPECT_TRUE(Range(513, 0, 0).Is16Bit())
            << "Range should be considered 16-bit.";
  EXPECT_FALSE(Range(513, 0, 0).Is8Bit())
            << "Range should be considered 16-bit.";
  EXPECT_TRUE(Range(0, 513, 0).Is16Bit())
            << "Range should be considered 16-bit.";
  EXPECT_FALSE(Range(0, 513, 0).Is8Bit())
            << "Range should be considered 16-bit.";
  EXPECT_TRUE(Range(0, 0, 513).Is16Bit())
            << "Range should be considered 16-bit.";
  EXPECT_FALSE(Range(0, 0, 513).Is8Bit())
            << "Range should be considered 16-bit.";
  EXPECT_FALSE(Range(512, 0, 0).Is16Bit())
            << "Range should be considered 8-bit.";
  EXPECT_TRUE(Range(512, 0, 0).Is8Bit())
            << "Range should be considered 8-bit.";
  EXPECT_FALSE(Range(0, 512, 0).Is16Bit())
            << "Range should be considered 8-bit.";
  EXPECT_TRUE(Range(0, 512, 0).Is8Bit())
            << "Range should be considered 8-bit.";
  EXPECT_FALSE(Range(0, 0, 512).Is16Bit())
            << "Range should be considered 8-bit.";
  EXPECT_TRUE(Range(0, 0, 512).Is8Bit())
            << "Range should be considered 8-bit.";
}

TEST(RangeValidationTest, Valid) {
  Range range_1(0, 10, 5);
  range_1.SetLabel("Range 1");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range_1.IsInvalid())
            << "Default value inside beginning and end should be valid.";
  Range range_2(0, 10, 0);
  range_2.SetLabel("Range 2");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range_2.IsInvalid())
            << "Default value equal to beginning should be valid.";
  Range range_3(0, 10, 10);
  range_3.SetLabel("Range 3");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range_3.IsInvalid())
            << "Default value equal to end should be valid.";
  Range range_4(0, 0, 0);
  range_4.SetLabel("Range 4");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range_4.IsInvalid())
            << "Start/end values are allowed to be equal.";
}

TEST(RangeValidationTest, InvalidMissingLabel) {
  Range range(0, 10, 0);
  range.SetLabel("");
  EXPECT_EQ(Range::InvalidReason::kMissingLabel, range.IsInvalid());
}

TEST(RangeValidationTest, InvalidEndBeforeBegin) {
  Range range(10, 0, 5);
  range.SetLabel("Range");
  EXPECT_EQ(Range::InvalidReason::kEndBeforeBegin, range.IsInvalid())
            << "End must come after beginning.";
}

TEST(RangeValidationTest, InvalidDefaultOutOfRange) {
  Range range(0, 10, 20);
  range.SetLabel("Range");
  EXPECT_EQ(Range::InvalidReason::kDefaultOutOfRange, range.IsInvalid())
            << "Default is not in range.";
}

TEST(RangeValidationTest, InvalidOutOfDmxRange) {
  Range range_1(65536, 65536, 65536);
  range_1.SetLabel("Range");
  EXPECT_EQ(Range::InvalidReason::kOutOfDmxRange, range_1.IsInvalid())
            << "Value is out of range.";
  Range range_2(0, 65536, 0);
  range_2.SetLabel("End Range");
  EXPECT_EQ(Range::InvalidReason::kOutOfDmxRange, range_2.IsInvalid())
            << "Value is out of range.";
}
