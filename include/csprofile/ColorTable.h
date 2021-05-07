/**
 * @file ColorTable.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_COLORTABLE_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_COLORTABLE_H_

#include <unordered_set>
#include <string>
#include <vector>
#include <optional>

namespace csprofile {

/**
 * The ColorSource console stores information about colors parameters in several tables.
 */
class ColorTable {
 public:
  enum class Color {
    kRed = 0,
    kRedOrange,
    kAmber,
    kLime,
    kGreen,
    kCyan,
    kBlue,
    kIndigo,
    kUv,
    kMagenta,
    kYellow,
    kWhite,
    kWarmWhite,
    kCoolWhite,
    kHue,
    kSaturation,
  };
  static const unsigned int kColorCount = static_cast<unsigned int>(Color::kSaturation) + 1;
  using ColorList = std::vector<Color>;

  enum class ColorMixingType {
    /** Hue/Saturation */
    kHueSat,
    /** Warm/Cool White */
    kWarmCoolWhite,
    /** RGI (Red, Green, Indigo) */
    kRgi,
    /** Subtractive CMY (Cyan, Magenta, Yellow) */
    kCmy,
    /** RGB (Red, Green, Blue) */
    kRgb,
    /** RGBI (Red, Green, Blue, Indigo) */
    kRgbi,
    /** RGBA (Red, Green, Blue, Amber) */
    kRgba,
    /** RGB+UV (Red, Green, Blue, UV) */
    kRgbUv,
    /** RGBW (Red, Green, Blue, White) */
    kRgbw,
    /** RGIW (Red, Green, Indigo, White) */
    kRgiw,
    /** RGBW+UV (Red, Green, Blue, White, UV) */
    kRgbwUv,
    /** RAGCB (Red, Amber, Green, Cyan, Blue) */
    kRagcb,
    /** RAGBI (Red, Amber, Green, Blue, Indigo) */
    kRagbi,
    /** RGBAW (Red, Amber, Green, Blue, White) */
    kRgbaw,
    /** ROAGI (Red, Red-Orange, Amber, Green, Indigo) */
    kRoagi,
    /** RGBCI (Red, Green, Cyan, Blue, Indigo) */
    kRgcbi,
    /** RGBAW+UV (Red, Amber, Green, Blue, UV, White) */
    kRgbawUv,
    /** ROAGCBI (Red, Red-Orange, Amber, Green, Cyan, Blue, Indigo) */
    kRoagcbi,
    /** RALGCBI (Red, Amber, Lime, Green, Cyan, Blue, Indigo) */
    kRalgcbi,
    /** RWAGCBI (Red, Amber, Green, Cyan, Blue, Indigo, White) */
    kRwagcbi,
  };

  /**
 * Get the colors in a color table
 * @param color_mixing_type
 * @return A list of colors in the proper order
 */
  [[nodiscard]] static const ColorList &GetColorTableColors(ColorMixingType color_mixing_type);

  /**
   * Get the UUID for a color table
   * @param color_mixing_type
   * @return The UUID string
   */
  [[nodiscard]] static const std::string &GetColorTableUuid(ColorMixingType color_mixing_type);

  /**
   * Get the color mixing type that uses the given colors
   *
   * @param colors
   * @return The color mixing type, or none if there is no mixing type.
   */
  [[nodiscard]] static std::optional<ColorMixingType> GetColorMixingType(const ColorList &colors);

  /**
   * Get the name of the color
   * @param color
   * @return
   */
  [[nodiscard]] static const std::string &GetColorName(Color color);

  /**
   * Get a color from the name
   * @param color_name
   * @return
   */
  [[nodiscard]] static std::optional<Color> GetColorFromName(const std::string &color_name);

  /**
   * Get the color RGB values, packed as AARRGGBB.
   * @param color
   * @return The color value, or none for colors with no color (i.e. Hue and Saturation)
   */
  [[nodiscard]] static std::optional<uint32_t> GetColorRgb(Color color);
};

} // csprofile

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_COLORTABLE_H_
