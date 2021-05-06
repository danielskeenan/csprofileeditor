/**
 * @file ColorTable.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/ColorTable.h"
#include <unordered_map>
#include <set>
#include <stdexcept>

namespace csprofile {

/**
 * Map color mixing types to their contained colors
 */
static const std::unordered_map<ColorTable::ColorTable::ColorMixingType, ColorTable::ColorTable::ColorList>
    kColorTableColors{
    {ColorTable::ColorTable::ColorMixingType::kHueSat, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kHue,
        ColorTable::Color::kSaturation
    }},
    {ColorTable::ColorTable::ColorMixingType::kWarmCoolWhite, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kWarmWhite,
        ColorTable::Color::kCoolWhite
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kCmy, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kCyan,
        ColorTable::Color::kMagenta,
        ColorTable::Color::kYellow
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgb, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgba, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kAmber
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbUv, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kUv
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbw, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kWhite
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgiw, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kIndigo,
        ColorTable::Color::kWhite
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbwUv, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kWhite,
        ColorTable::Color::kUv
    }},
    {ColorTable::ColorTable::ColorMixingType::kRagcb, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kAmber,
        ColorTable::Color::kGreen,
        ColorTable::Color::kCyan,
        ColorTable::Color::kBlue
    }},
    {ColorTable::ColorTable::ColorMixingType::kRagbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kAmber,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbaw, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kAmber,
        ColorTable::Color::kWhite
    }},
    {ColorTable::ColorTable::ColorMixingType::kRoagi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kRedOrange,
        ColorTable::Color::kAmber,
        ColorTable::Color::kGreen,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgcbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kCyan,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRgbawUv, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kGreen,
        ColorTable::Color::kBlue,
        ColorTable::Color::kAmber,
        ColorTable::Color::kWhite,
        ColorTable::Color::kUv
    }},
    {ColorTable::ColorTable::ColorMixingType::kRoagcbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kRedOrange,
        ColorTable::Color::kAmber,
        ColorTable::Color::kGreen,
        ColorTable::Color::kCyan,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRalgcbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kAmber,
        ColorTable::Color::kLime,
        ColorTable::Color::kGreen,
        ColorTable::Color::kCyan,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
    {ColorTable::ColorTable::ColorMixingType::kRwagcbi, ColorTable::ColorTable::ColorList{
        ColorTable::Color::kRed,
        ColorTable::Color::kWhite,
        ColorTable::Color::kAmber,
        ColorTable::Color::kGreen,
        ColorTable::Color::kCyan,
        ColorTable::Color::kBlue,
        ColorTable::Color::kIndigo
    }},
};

/**
 * Map color mixing types to their UUIDs
 */
static const std::unordered_map<ColorTable::ColorMixingType, std::string> kColorTableUuidMap{
    {ColorTable::ColorMixingType::kHueSat, "B074A2D3-0C40-45A7-844A-7C2721E0B267"},
    {ColorTable::ColorMixingType::kWarmCoolWhite, "7B365530-A4DF-44AD-AEF5-225472BE02AE"},
    {ColorTable::ColorMixingType::kRgi, "637E8789-5540-45D5-BD83-D7C2A7618B45"},
    {ColorTable::ColorMixingType::kCmy, "EF4970BA-2536-4725-9B0F-B2D7A021E139"},
    {ColorTable::ColorMixingType::kRgb, "3874B444-A11E-47D9-8295-04556EAEBEA7"},
    {ColorTable::ColorMixingType::kRgbi, "B043D095-95A4-4DDB-AB38-252C991B13A8"},
    {ColorTable::ColorMixingType::kRgba, "D3E71EC8-3406-4572-A64C-52A38649C795"},
    {ColorTable::ColorMixingType::kRgbUv, "74EF89F4-0B78-4DC6-8E8A-68E3298B7CD2"},
    {ColorTable::ColorMixingType::kRgbw, "77A82F8A-9B24-4C3F-98FC-B6A29FB1AAE6"},
    {ColorTable::ColorMixingType::kRgiw, "77597794-7BFF-46A3-878B-906D3780E6C9"},
    {ColorTable::ColorMixingType::kRgbwUv, "B28E1514-AE8C-4E06-8472-B52D575B1CF2"},
    {ColorTable::ColorMixingType::kRagcb, "3F90A9F9-209F-4505-A9F2-FEC17BC6A426"},
    {ColorTable::ColorMixingType::kRagbi, "1D16DE15-5F4C-46A9-9C3D-2380C2D2793A"},
    {ColorTable::ColorMixingType::kRgbaw, "C7A1FB0A-AA23-468F-9060-AC1625155DE8"},
    {ColorTable::ColorMixingType::kRoagi, "91189886-6A6A-47CF-9137-5F5A7A88D829"},
    {ColorTable::ColorMixingType::kRgcbi, "04493BB0-7B6E-4B6C-B3B7-D9641F7511AD"},
    {ColorTable::ColorMixingType::kRgbawUv, "EDDEAC65-BD2E-4D87-B163-D7A2434EC081"},
    {ColorTable::ColorMixingType::kRoagcbi, "373673E3-571E-4CE2-B12D-CDD44085A1EB"},
    {ColorTable::ColorMixingType::kRalgcbi, "75FEB905-EA2A-4643-B4F8-1A84141F8E98"},
    {ColorTable::ColorMixingType::kRwagcbi, "02A2F87C-AB4C-41F5-8779-A51B99D0BE1C"},
};

/**
 * Map colors to their names
 */
static const std::unordered_map<ColorTable::Color, std::string> kColorNames{
    {ColorTable::Color::kRed, "Red"},
    {ColorTable::Color::kRedOrange, "RedOrange"},
    {ColorTable::Color::kAmber, "Amber"},
    {ColorTable::Color::kLime, "Lime"},
    {ColorTable::Color::kGreen, "Green"},
    {ColorTable::Color::kCyan, "Cyan"},
    {ColorTable::Color::kBlue, "Blue"},
    {ColorTable::Color::kIndigo, "Indigo"},
    {ColorTable::Color::kUv, "UV"},
    {ColorTable::Color::kMagenta, "Magenta"},
    {ColorTable::Color::kYellow, "Yellow"},
    {ColorTable::Color::kWhite, "White"},
    {ColorTable::Color::kWarmWhite, "WarmWhite"},
    {ColorTable::Color::kCoolWhite, "CoolWhite"},
    {ColorTable::Color::kHue, "Hue"},
    {ColorTable::Color::kSaturation, "Saturation"},
};

/**
 * Map color names to their colors
 */
static const std::unordered_map<std::string, ColorTable::Color> kNameColors{
    {"Red", ColorTable::Color::kRed},
    {"RedOrange", ColorTable::Color::kRedOrange},
    {"Amber", ColorTable::Color::kAmber},
    {"Lime", ColorTable::Color::kLime},
    {"Green", ColorTable::Color::kGreen},
    {"Cyan", ColorTable::Color::kCyan},
    {"Blue", ColorTable::Color::kBlue},
    {"Indigo", ColorTable::Color::kIndigo},
    {"UV", ColorTable::Color::kUv},
    {"Magenta", ColorTable::Color::kMagenta},
    {"Yellow", ColorTable::Color::kYellow},
    {"White", ColorTable::Color::kWhite},
    {"WarmWhite", ColorTable::Color::kWarmWhite},
    {"CoolWhite", ColorTable::Color::kCoolWhite},
    {"Hue", ColorTable::Color::kHue},
    {"Saturation", ColorTable::Color::kSaturation},
};

/**
 * Map colors 
 */
static const std::unordered_map<ColorTable::Color, std::optional<uint32_t>> kColorRgb{
    {ColorTable::Color::kRed, 0xffc00000},
    {ColorTable::Color::kRedOrange, 0xffc04000},
    {ColorTable::Color::kAmber, 0xffffc000},
    {ColorTable::Color::kLime, 0xff80ff00},
    {ColorTable::Color::kGreen, 0xff00c000},
    {ColorTable::Color::kCyan, 0xff00c0c0},
    {ColorTable::Color::kBlue, 0xff0000c0},
    {ColorTable::Color::kIndigo, 0xff4000c0},
    {ColorTable::Color::kUv, 0xff200060},
    {ColorTable::Color::kMagenta, 0xffc00080},
    {ColorTable::Color::kYellow, 0xffe6e600},
    {ColorTable::Color::kWhite, 0xffe6e6e6},
    {ColorTable::Color::kWarmWhite, 0xffffebc0},
    {ColorTable::Color::kCoolWhite, 0xffd1ffff},
    {ColorTable::Color::kHue, {}},
    {ColorTable::Color::kSaturation, {}},
};

const ColorTable::ColorList &ColorTable::GetColorTableColors(ColorTable::ColorMixingType color_mixing_type) {
  return kColorTableColors.at(color_mixing_type);
}

const std::string &ColorTable::GetColorTableUuid(ColorTable::ColorMixingType color_mixing_type) {
  return kColorTableUuidMap.at(color_mixing_type);
}

std::optional<ColorTable::ColorMixingType> ColorTable::GetColorMixingType(const ColorTable::ColorList &colors) {
  const std::set<ColorList::value_type> search_colors(colors.cbegin(), colors.cend());

  for (const auto &colorMixingTypeColors : kColorTableColors) {
    const std::set<ColorList::value_type>
        check_colors(colorMixingTypeColors.second.cbegin(), colorMixingTypeColors.second.cend());
    if (search_colors == check_colors) {
      return colorMixingTypeColors.first;
    }
  }

  return {};
}

const std::string &ColorTable::GetColorName(ColorTable::Color color) {
  return kColorNames.at(color);
}

std::optional<ColorTable::Color> ColorTable::GetColorFromName(const std::string &color_name) {
  try {
    return kNameColors.at(color_name);
  } catch (const std::out_of_range &) {
    return {};
  }
}

std::optional<uint32_t> ColorTable::GetColorRgb(ColorTable::Color color) {
  return kColorRgb.at(color);
}

} // csprofile
