/**
 * @file PersonalityTest.cpp
 *
 * @author dankeenan
 * @date 5/7/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include <csprofile/Personality.h>

using namespace csprofile;

TEST(PersonalityValidationTest, Valid) {
  Personality personality;
  personality.SetManufacturerName("Custom");
  personality.SetModelName("Test");
  auto param = std::make_unique<parameter::IntensityParameter>();
  param->SetName("Param");
  param->SetAddressCourse(1);
  personality.parameters_.push_back(std::move(param));
  EXPECT_EQ(Personality::InvalidReason::kIsValid, personality.IsInvalid());
}

TEST(PersonalityValidationTest, InvalidMissingManufacturerName) {
  Personality personality;
  personality.SetManufacturerName("");
  personality.SetModelName("Test");
  auto param = std::make_unique<parameter::IntensityParameter>();
  param->SetName("Param");
  param->SetAddressCourse(1);
  personality.parameters_.push_back(std::move(param));
  EXPECT_EQ(Personality::InvalidReason::kMissingManufacturerName, personality.IsInvalid());
}

TEST(PersonalityValidationTest, InvalidMissingModelName) {
  Personality personality;
  personality.SetManufacturerName("Custom");
  personality.SetModelName("");
  auto param = std::make_unique<parameter::IntensityParameter>();
  param->SetName("Param");
  param->SetAddressCourse(1);
  personality.parameters_.push_back(std::move(param));
  EXPECT_EQ(Personality::InvalidReason::kMissingModelName, personality.IsInvalid());
}

TEST(PersonalityValidationTest, InvalidNoParameters) {
  Personality personality;
  personality.SetManufacturerName("Custom");
  personality.SetModelName("Test");
  personality.parameters_.clear();
  EXPECT_EQ(Personality::InvalidReason::kNoParameters, personality.IsInvalid());
}

TEST(PersonalityValidationTest, InvalidParameter) {
  Personality personality;
  personality.SetManufacturerName("Custom");
  personality.SetModelName("Test");
  auto param = std::make_unique<parameter::IntensityParameter>();
  param->SetName("");
  param->SetAddressCourse(0);
  EXPECT_NE(parameter::Parameter::InvalidReason::kIsValid, param->IsInvalid());
  personality.parameters_.push_back(std::move(param));
  EXPECT_EQ(Personality::InvalidReason::kInvalidParameter, personality.IsInvalid());
}
