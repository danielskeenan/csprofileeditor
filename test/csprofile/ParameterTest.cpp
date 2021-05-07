/**
 * @file ParameterTest.cpp
 *
 * @author dankeenan
 * @date 5/7/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include <csprofile/parameter/Parameter.h>

using namespace csprofile::parameter;

TEST(ParameterTest, Is16Bit) {
  IntensityParameter param_16bit;
  param_16bit.SetAddressCourse(1);
  param_16bit.SetAddressFine(2);
  EXPECT_TRUE(param_16bit.Is16Bit());
  EXPECT_FALSE(param_16bit.Is8Bit());

  IntensityParameter param_8bit;
  param_8bit.SetAddressCourse(1);
  param_8bit.SetAddressFine(0);
  EXPECT_FALSE(param_8bit.Is16Bit());
  EXPECT_TRUE(param_8bit.Is8Bit());
}

TEST(ParameterValidationTest, Valid) {
  IntensityParameter param_1;
  param_1.SetName("Param 1");
  param_1.SetAddressCourse(1);
  EXPECT_EQ(Parameter::InvalidReason::kIsValid, param_1.IsInvalid());

  IntensityParameter param_2;
  param_2.SetName("Param 2");
  param_2.SetAddressCourse(1);
  param_2.SetAddressFine(2);
  EXPECT_EQ(Parameter::InvalidReason::kIsValid, param_1.IsInvalid());

  IntensityParameter param_3;
  param_3.SetName("Param 2");
  param_3.SetAddressCourse(2);
  param_3.SetAddressFine(1);
  EXPECT_EQ(Parameter::InvalidReason::kIsValid, param_3.IsInvalid());

  IntensityParameter param_4;
  param_4.SetName("Param 4");
  param_4.SetAddressCourse(1);
  Range range(0, 10, 0);
  range.SetLabel("Range");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range.IsInvalid());
  param_4.ranges_.push_back(range);
  EXPECT_EQ(Parameter::InvalidReason::kIsValid, param_4.IsInvalid());
}

TEST(ParameterValidationTest, InvalidMissingName) {
  IntensityParameter param;
  param.SetName("");
  param.SetAddressCourse(1);
  EXPECT_EQ(Parameter::InvalidReason::kMissingName, param.IsInvalid());
}

TEST(ParameterValidationTest, InvalidHomeOutOfRange) {
  IntensityParameter param_8bit;
  param_8bit.SetName("Param 8-bit");
  param_8bit.SetAddressCourse(1);
  param_8bit.SetAddressFine(0);
  param_8bit.SetHomeValue(256);
  EXPECT_EQ(Parameter::InvalidReason::kHomeOutOfRange, param_8bit.IsInvalid());

  IntensityParameter param_16bit;
  param_16bit.SetName("Param 16-bit");
  param_16bit.SetAddressCourse(1);
  param_16bit.SetAddressFine(2);
  param_16bit.SetHomeValue(0);
  EXPECT_EQ(Parameter::InvalidReason::kIsValid, param_16bit.IsInvalid());
  param_16bit.SetHomeValue(65536);
  EXPECT_EQ(Parameter::InvalidReason::kHomeOutOfRange, param_16bit.IsInvalid());
}

TEST(ParameterValidationTest, InvalidOutOfDmxRange) {
  IntensityParameter param;
  param.SetName("Param");
  param.SetAddressCourse(0);
  EXPECT_EQ(Parameter::InvalidReason::kOutOfDmxRange, param.IsInvalid());
  param.SetAddressCourse(513);
  EXPECT_EQ(Parameter::InvalidReason::kOutOfDmxRange, param.IsInvalid());
  param.SetAddressCourse(1);
  param.SetAddressFine(513);
  EXPECT_EQ(Parameter::InvalidReason::kOutOfDmxRange, param.IsInvalid());
}

TEST(ParameterValidationTest, InvalidOverlappingAddresses) {
  IntensityParameter param;
  param.SetName("Param");
  param.SetAddressCourse(1);
  param.SetAddressFine(1);
  EXPECT_EQ(Parameter::InvalidReason::kOverlappingAddresses, param.IsInvalid());
}

TEST(ParameterValidationTest, InvalidRange) {
  IntensityParameter param;
  param.SetName("Param");
  param.SetAddressCourse(1);
  Range range(513, 0, 10);
  range.SetLabel("");
  EXPECT_NE(Range::InvalidReason::kIsValid, range.IsInvalid());
  param.ranges_.push_back(range);
  EXPECT_EQ(Parameter::InvalidReason::kInvalidRange, param.IsInvalid());
}

TEST(ParameterValidationTest, InvalidRangeOutOfRange) {
  IntensityParameter param;
  param.SetName("Param");
  param.SetAddressCourse(1);
  param.SetAddressFine(0);
  Range range(513, 513, 513);
  range.SetLabel("Range");
  EXPECT_EQ(Range::InvalidReason::kIsValid, range.IsInvalid());
  param.ranges_.push_back(range);
  EXPECT_EQ(Parameter::InvalidReason::kRangeOutOfRange, param.IsInvalid());
}
