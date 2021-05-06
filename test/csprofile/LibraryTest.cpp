/**
 * @file LibraryTest.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include <gtest/gtest.h>
#include "csprofile/Library.h"

using namespace csprofile;

TEST(LibraryTest, LoadJson) {
  const auto json = R"EOF(
{
    "date": "2020-06-28T15:11:11Z",
    "editorVersion": "1.1.1.9.0.4",
    "personalities": [
        {
            "colortable": "B074A2D3-0C40-45A7-844A-7C2721E0B267",
            "dcid": "EFDB8293-3E80-4048-908B-306E37842D59",
            "hasIntensity": true,
            "manufacturerName": "Custom",
            "maxOffset": 7,
            "modeName": "-",
            "modelName": "test",
            "parameters": [
                {
                    "coarse": 0,
                    "fadeWithIntensity": false,
                    "fine": 1,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Hue",
                    "size": 16,
                    "snap": false,
                    "type": 5
                },
                {
                    "coarse": 2,
                    "fadeWithIntensity": false,
                    "fine": 3,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Saturation",
                    "size": 16,
                    "snap": false,
                    "type": 5
                },
                {
                    "coarse": 4,
                    "fadeWithIntensity": true,
                    "fine": 5,
                    "highlight": 65535,
                    "home": 0,
                    "invert": false,
                    "name": "Intensity",
                    "size": 16,
                    "snap": false,
                    "type": 1
                },
                {
                    "coarse": 6,
                    "fadeWithIntensity": false,
                    "highlight": 0,
                    "home": 255,
                    "invert": false,
                    "name": "Gobo",
                    "ranges": [
                        {
                            "begin": 0,
                            "default": 0,
                            "end": 32,
                            "label": "ClearOpen",
                            "media": {
                                "b": 250,
                                "g": 255,
                                "name": "Clear",
                                "r": 254
                            }
                        }
                    ],
                    "size": 8,
                    "snap": true,
                    "type": 4
                },
                {
                    "coarse": 7,
                    "fadeWithIntensity": false,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Speed",
                    "size": 8,
                    "snap": false,
                    "type": 2
                }
            ]
        }
    ]
}
  )EOF";
  std::stringstream json_stream(json);
  Library library;
  json_stream >> library;

  ASSERT_EQ(library.personalities.size(), 1);
  const Personality &personality = library.personalities.at(0);
  ASSERT_EQ(personality.parameters_.size(), 5);

  // Hue
  const auto &hue = personality.parameters_.at(0);
  EXPECT_EQ(hue->GetName(), "Hue");
  EXPECT_EQ(hue->GetType(), parameter::Type::kColor);
  EXPECT_EQ(hue->GetAddressCourse(), 1);
  EXPECT_EQ(hue->GetAddressFine(), 2);
  EXPECT_FALSE(hue->Is8Bit());
  EXPECT_TRUE(hue->Is16Bit());
  EXPECT_FALSE(hue->GetInvert());
  EXPECT_EQ(hue->GetHomeValue(), 0);
  EXPECT_FALSE(hue->GetSnap());
  EXPECT_FALSE(hue->GetFadeWithIntensity());
  EXPECT_EQ(hue->ranges_.size(), 0);
  EXPECT_EQ(dynamic_cast<parameter::ColorParameter *>(hue.get())->GetColorParam(),
            ColorTable::Color::kHue);
  EXPECT_EQ(hue->GetColor(), std::nullopt);

  // Saturation
  const auto &saturation = personality.parameters_.at(1);
  EXPECT_EQ(saturation->GetName(), "Saturation");
  EXPECT_EQ(saturation->GetType(), parameter::Type::kColor);
  EXPECT_EQ(saturation->GetAddressCourse(), 3);
  EXPECT_EQ(saturation->GetAddressFine(), 4);
  EXPECT_FALSE(saturation->Is8Bit());
  EXPECT_TRUE(saturation->Is16Bit());
  EXPECT_FALSE(saturation->GetInvert());
  EXPECT_EQ(saturation->GetHomeValue(), 0);
  EXPECT_FALSE(saturation->GetSnap());
  EXPECT_FALSE(saturation->GetFadeWithIntensity());
  EXPECT_EQ(saturation->ranges_.size(), 0);
  EXPECT_EQ(dynamic_cast<parameter::ColorParameter *>(saturation.get())->GetColorParam(),
            ColorTable::Color::kSaturation);
  EXPECT_EQ(saturation->GetColor(), std::nullopt);

  // Intensity
  const auto &intensity = personality.parameters_.at(2);
  EXPECT_EQ(intensity->GetName(), "Intensity");
  EXPECT_EQ(intensity->GetType(), parameter::Type::kIntensity);
  EXPECT_EQ(intensity->GetAddressCourse(), 5);
  EXPECT_EQ(intensity->GetAddressFine(), 6);
  EXPECT_FALSE(intensity->Is8Bit());
  EXPECT_TRUE(intensity->Is16Bit());
  EXPECT_FALSE(intensity->GetInvert());
  EXPECT_EQ(intensity->GetHomeValue(), 0);
  EXPECT_FALSE(intensity->GetSnap());
  EXPECT_TRUE(intensity->GetFadeWithIntensity());
  EXPECT_EQ(intensity->ranges_.size(), 0);

  // Gobo
  const auto &gobo = personality.parameters_.at(3);
  EXPECT_EQ(gobo->GetName(), "Gobo");
  EXPECT_EQ(gobo->GetType(), parameter::Type::kBeam);
  EXPECT_EQ(gobo->GetAddressCourse(), 7);
  EXPECT_EQ(gobo->GetAddressFine(), 0);
  EXPECT_TRUE(gobo->Is8Bit());
  EXPECT_FALSE(gobo->Is16Bit());
  EXPECT_FALSE(gobo->GetInvert());
  EXPECT_EQ(gobo->GetHomeValue(), 255);
  EXPECT_TRUE(gobo->GetSnap());
  EXPECT_FALSE(gobo->GetFadeWithIntensity());
  EXPECT_EQ(gobo->ranges_.size(), 1);
  // Allow other checks to test even if range is parsed incorrectly
  if (gobo->ranges_.size() == 1) {
    const parameter::Range &gobo_clear_open = gobo->ranges_.at(0);
    EXPECT_EQ(gobo_clear_open.GetBeginValue(), 0);
    EXPECT_EQ(gobo_clear_open.GetDefaultValue(), 0);
    EXPECT_EQ(gobo_clear_open.GetEndValue(), 32);
    EXPECT_EQ(gobo_clear_open.GetLabel(), "ClearOpen");
    EXPECT_TRUE(gobo_clear_open.GetMedia().has_value());
    // Allow other checks to test even if media is parsed incorrectly
    if (gobo_clear_open.GetMedia().has_value()) {
      const auto &gobo_media = gobo_clear_open.GetMedia().value();
      EXPECT_EQ(gobo_media.GetName(), "Clear");
      EXPECT_FALSE(gobo_media.GetGoboDcid().has_value());
      EXPECT_TRUE(gobo_media.GetRgb().has_value());
      if (gobo_media.GetRgb().has_value()) {
        const uint32_t rgba = gobo_media.GetRgb().value();
        const uint8_t r = (rgba & (0xFF << 16)) >> 16;
        const uint8_t g = (rgba & (0xFF << 8)) >> 8;
        const uint8_t b = (rgba & (0xFF << 0)) >> 0;
        const uint8_t a = (rgba & (0xFF << 24)) >> 24;
        EXPECT_EQ(r, 254);
        EXPECT_EQ(g, 255);
        EXPECT_EQ(b, 250);
        EXPECT_EQ(a, 0xFF);
      }
    }
  }

  // Speed
  const auto &speed = personality.parameters_.at(4);
  EXPECT_EQ(speed->GetName(), "Speed");
  EXPECT_EQ(speed->GetType(), parameter::Type::kPosition);
  EXPECT_EQ(speed->GetAddressCourse(), 8);
  EXPECT_EQ(speed->GetAddressFine(), 0);
  EXPECT_TRUE(speed->Is8Bit());
  EXPECT_FALSE(speed->Is16Bit());
  EXPECT_FALSE(speed->GetInvert());
  EXPECT_EQ(speed->GetHomeValue(), 0);
  EXPECT_FALSE(speed->GetSnap());
  EXPECT_FALSE(speed->GetFadeWithIntensity());
  EXPECT_EQ(speed->ranges_.size(), 0);
}

TEST(LibraryTest, SaveJson) {
  // Force a specific DCID and time for consistent output
  Library library;
  library.SetUpdated(boost::posix_time::ptime(boost::gregorian::date(2020, 6, 28),
                                              boost::posix_time::time_duration(15, 11, 11)));
  Personality personality("EFDB8293-3E80-4048-908B-306E37842D59");
  personality.SetManufacturerName("Custom");
  personality.SetModeName("-");
  personality.SetModelName("test");

  // Hue
  auto hue = std::unique_ptr<parameter::Parameter>(new parameter::ColorParameter);
  dynamic_cast<parameter::ColorParameter *>(hue.get())->SetColorParam(ColorTable::Color::kHue);
  hue->SetAddressCourse(1);
  hue->SetAddressFine(2);
  personality.parameters_.push_back(std::move(hue));

  // Saturation
  auto saturation = std::unique_ptr<parameter::Parameter>(new parameter::ColorParameter);
  dynamic_cast<parameter::ColorParameter *>(saturation.get())->SetColorParam(ColorTable::Color::kSaturation);
  saturation->SetAddressCourse(3);
  saturation->SetAddressFine(4);
  personality.parameters_.push_back(std::move(saturation));

  // Intensity
  auto intensity = std::unique_ptr<parameter::Parameter>(new parameter::IntensityParameter);
  intensity->SetAddressCourse(5);
  intensity->SetAddressFine(6);
  personality.parameters_.push_back(std::move(intensity));

  // Gobo
  auto gobo = std::unique_ptr<parameter::Parameter>(new parameter::BeamParameter);
  gobo->SetName("Gobo");
  gobo->SetAddressCourse(7);
  gobo->SetHomeValue(255);
  gobo->SetSnap(true);
  parameter::Range gobo_clear_open;
  gobo_clear_open.SetBeginValue(0);
  gobo_clear_open.SetDefaultValue(0);
  gobo_clear_open.SetEndValue(32);
  gobo_clear_open.SetLabel("ClearOpen");
  parameter::Media gobo_media;
  gobo_media.SetName("Clear");
  gobo_media.SetRgb(254, 255, 250);
  gobo_clear_open.SetMedia(gobo_media);
  gobo->ranges_.push_back(std::move(gobo_clear_open));
  personality.parameters_.push_back(std::move(gobo));

  // Speed
  auto speed = std::unique_ptr<parameter::Parameter>(new parameter::PositionParameter);
  speed->SetName("Speed");
  speed->SetAddressCourse(8);
  personality.parameters_.push_back(std::move(speed));

  library.personalities.push_back(std::move(personality));

  // When changing this, mind the whitespace!  Use 4 spaces for indents.
  const auto expected = R"EOF({
    "date": "2020-06-28T15:11:11Z",
    "editorVersion": "1.1.1.9.0.4",
    "personalities": [
        {
            "colortable": "B074A2D3-0C40-45A7-844A-7C2721E0B267",
            "dcid": "EFDB8293-3E80-4048-908B-306E37842D59",
            "hasIntensity": true,
            "manufacturerName": "Custom",
            "maxOffset": 7,
            "modeName": "-",
            "modelName": "test",
            "parameters": [
                {
                    "coarse": 0,
                    "fadeWithIntensity": false,
                    "fine": 1,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Hue",
                    "size": 16,
                    "snap": false,
                    "type": 5
                },
                {
                    "coarse": 2,
                    "fadeWithIntensity": false,
                    "fine": 3,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Saturation",
                    "size": 16,
                    "snap": false,
                    "type": 5
                },
                {
                    "coarse": 4,
                    "fadeWithIntensity": true,
                    "fine": 5,
                    "highlight": 65535,
                    "home": 0,
                    "invert": false,
                    "name": "Intensity",
                    "size": 16,
                    "snap": false,
                    "type": 1
                },
                {
                    "coarse": 6,
                    "fadeWithIntensity": false,
                    "highlight": 0,
                    "home": 255,
                    "invert": false,
                    "name": "Gobo",
                    "ranges": [
                        {
                            "begin": 0,
                            "default": 0,
                            "end": 32,
                            "label": "ClearOpen",
                            "media": {
                                "b": 250,
                                "g": 255,
                                "name": "Clear",
                                "r": 254
                            }
                        }
                    ],
                    "size": 8,
                    "snap": true,
                    "type": 4
                },
                {
                    "coarse": 7,
                    "fadeWithIntensity": false,
                    "highlight": 0,
                    "home": 0,
                    "invert": false,
                    "name": "Speed",
                    "size": 8,
                    "snap": false,
                    "type": 2
                }
            ]
        }
    ]
}
)EOF";
  std::ostringstream actual_stream;
  actual_stream << std::setw(4) << library << std::endl;
  EXPECT_EQ(expected, actual_stream.str());
}
