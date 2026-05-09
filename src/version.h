#pragma once

#include <string_view>

namespace plat {

/**
 * Major version component.
 */
inline constexpr int kVersionMajor = 0;

/**
 * Minor version component.
 */
inline constexpr int kVersionMinor = 1;

/**
 * Patch version component.
 */
inline constexpr int kVersionPatch = 0;

/**
 * Human-readable semantic version string.
 */
inline constexpr std::string_view kVersion = "0.1.0";

} // namespace plat
