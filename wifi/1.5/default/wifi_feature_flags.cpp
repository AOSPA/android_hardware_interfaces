/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>

#include <android-base/logging.h>
#include <cutils/properties.h>

#include "wifi_feature_flags.h"

namespace android {
namespace hardware {
namespace wifi {
namespace V1_5 {
namespace implementation {
namespace feature_flags {

using V1_0::ChipModeId;
using V1_0::IfaceType;
using V1_0::IWifiChip;

/* The chip may either have a single mode supporting any number of combinations,
 * or a fixed dual-mode (so it involves firmware loading to switch between
 * modes) setting. If there is a need to support more modes, it needs to be
 * implemented manually in WiFi HAL (see changeFirmwareMode in
 * WifiChip::handleChipConfiguration).
 *
 * Supported combinations are defined in device's makefile, for example:
 *    WIFI_HAL_INTERFACE_COMBINATIONS := {{{STA, AP}, 1}, {{P2P, NAN}, 1}},
 *    WIFI_HAL_INTERFACE_COMBINATIONS += {{{STA}, 1}, {{AP}, 2}}
 * What means:
 *    Interface combination 1: 1 STA or AP and 1 P2P or NAN concurrent iface
 *                             operations.
 *    Interface combination 2: 1 STA and 2 AP concurrent iface operations.
 *
 * For backward compatibility, the following makefile flags can be used to
 * generate combinations list:
 *  - WIFI_HIDL_FEATURE_DUAL_INTERFACE
 *  - WIFI_HIDL_FEATURE_DISABLE_AP
 *  - WIFI_HIDL_FEATURE_AWARE
 * However, they are ignored if WIFI_HAL_INTERFACE_COMBINATIONS was provided.
 * With WIFI_HIDL_FEATURE_DUAL_INTERFACE flag set, there is a single mode with
 * two interface combinations:
 *    Interface Combination 1: Will support 1 STA and 1 P2P or NAN (optional)
 *                             concurrent iface operations.
 *    Interface Combination 2: Will support 1 STA and 1 AP concurrent
 *                             iface operations.
 *
 * The only dual-mode configuration supported is for alternating STA and AP
 * mode, that may involve firmware reloading. In such case, there are 2 separate
 * modes of operation with 1 interface combination each:
 *    Mode 1 (STA mode): Will support 1 STA and 1 P2P or NAN (optional)
 *                       concurrent iface operations.
 *    Mode 2 (AP mode): Will support 1 AP iface operation.
 *
 * If Aware is enabled, the iface combination will be modified to support either
 * P2P or NAN in place of just P2P.
 */
// clang-format off
#ifdef WIFI_HAL_INTERFACE_COMBINATIONS
constexpr ChipModeId kMainModeId = chip_mode_ids::kV3;
#elif defined(WIFI_HIDL_FEATURE_DUAL_INTERFACE)
// former V2 (fixed dual interface) setup expressed as V3
constexpr ChipModeId kMainModeId = chip_mode_ids::kV3;
#  ifdef WIFI_HIDL_FEATURE_DISABLE_AP
#    ifdef WIFI_HIDL_FEATURE_AWARE
//     1 STA + 1 of (P2P or NAN)
#      define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{P2P, NAN}, 1}}
#    else
//     1 STA + 1 P2P
#      define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{P2P}, 1}}
#    endif
#  else
#    if defined(QC_WIFI_HIDL_FEATURE_DUAL_AP) && defined(QC_WIFI_HIDL_FEATURE_DUAL_STA)
#      ifdef WIFI_HIDL_FEATURE_AWARE
//       (1 STA + 1 AP) or (1 STA + 1 of (P2P or NAN)) or (2 AP) or (2 STA)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P, NAN}, 1}},\
                                                {{{AP}, 2}},\
                                                {{{STA}, 2}}
#      else
//       (1 STA + 1 AP) or (1 STA + 1 P2P) or (2 AP) or (2 STA)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P}, 1}},\
                                                {{{AP}, 2}},\
                                                {{{STA}, 2}}
#      endif
#    elif defined(QC_WIFI_HIDL_FEATURE_DUAL_AP)
#      ifdef WIFI_HIDL_FEATURE_AWARE
//       (1 STA + 1 AP) or (1 STA + 1 of (P2P or NAN)) or (2 AP)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P, NAN}, 1}},\
                                                {{{AP}, 2}}
#      else
//       (1 STA + 1 AP) or (1 STA + 1 P2P) or (2 AP)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P}, 1}},\
                                                {{{AP}, 2}}
#      endif
#    elif defined(QC_WIFI_HIDL_FEATURE_DUAL_STA)
#      ifdef WIFI_HIDL_FEATURE_AWARE
//       (1 STA + 1 AP) or (1 STA + 1 of (P2P or NAN)) or (2 STA)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P, NAN}, 1}},\
                                                {{{STA}, 2}}
#      else
//       (1 STA + 1 AP) or (1 STA + 1 P2P) or (2 STA)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P}, 1}},\
                                                {{{STA}, 2}}
#      endif
#    else
#      ifdef WIFI_HIDL_FEATURE_AWARE
//       (1 STA + 1 AP) or (1 STA + 1 of (P2P or NAN))
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P, NAN}, 1}}
#      else
//       (1 STA + 1 AP) or (1 STA + 1 P2P)
#        define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{AP}, 1}},\
                                                {{{STA}, 1}, {{P2P}, 1}}
#      endif
#    endif
#  endif
#else
// V1 (fixed single interface, dual-mode chip)
constexpr ChipModeId kMainModeId = chip_mode_ids::kV1Sta;
#  ifdef WIFI_HIDL_FEATURE_AWARE
//   1 STA + 1 of (P2P or NAN)
#    define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{P2P, NAN}, 1}}
#  else
//   1 STA + 1 P2P
#    define WIFI_HAL_INTERFACE_COMBINATIONS {{{STA}, 1}, {{P2P}, 1}}
#  endif

#  ifndef WIFI_HIDL_FEATURE_DISABLE_AP
#    define WIFI_HAL_INTERFACE_COMBINATIONS_AP {{{AP}, 1}}
#  endif
#endif
// clang-format on

/**
 * Helper class to convert a collection of combination limits to a combination.
 *
 * The main point here is to simplify the syntax required by
 * WIFI_HAL_INTERFACE_COMBINATIONS.
 */
struct ChipIfaceCombination
    : public hidl_vec<IWifiChip::ChipIfaceCombinationLimit> {
    ChipIfaceCombination(
        const std::initializer_list<IWifiChip::ChipIfaceCombinationLimit> list)
        : hidl_vec(list) {}

    operator IWifiChip::ChipIfaceCombination() const { return {*this}; }

    static hidl_vec<IWifiChip::ChipIfaceCombination> make_vec(
        const std::initializer_list<ChipIfaceCombination> list) {
        return hidl_vec<IWifiChip::ChipIfaceCombination>(  //
            std::begin(list), std::end(list));
    }
};

#define STA IfaceType::STA
#define AP IfaceType::AP
#define P2P IfaceType::P2P
#define NAN IfaceType::NAN
static const std::vector<IWifiChip::ChipMode> kChipModesPrimary{
    {kMainModeId,
     ChipIfaceCombination::make_vec({WIFI_HAL_INTERFACE_COMBINATIONS})},
#ifdef WIFI_HAL_INTERFACE_COMBINATIONS_AP
    {chip_mode_ids::kV1Ap,
     ChipIfaceCombination::make_vec({WIFI_HAL_INTERFACE_COMBINATIONS_AP})},
#endif
};

static const std::vector<IWifiChip::ChipMode> kChipModesSecondary{
#ifdef WIFI_HAL_INTERFACE_COMBINATIONS_SECONDARY_CHIP
    {chip_mode_ids::kV3, ChipIfaceCombination::make_vec(
                             {WIFI_HAL_INTERFACE_COMBINATIONS_SECONDARY_CHIP})},
#endif
};

constexpr char kDebugPresetInterfaceCombinationIdxProperty[] =
    "persist.vendor.debug.wifi.hal.preset_interface_combination_idx";
// List of pre-defined interface combinations that can be enabled at runtime via
// setting the property: "kDebugPresetInterfaceCombinationIdxProperty" to the
// corresponding index value.
static const std::vector<
    std::pair<std::string, std::vector<IWifiChip::ChipMode>>>
    kDebugChipModes{
        // Legacy combination - No STA/AP concurrencies.
        // 0 - (1 AP) or (1 STA + 1 of (P2P or NAN))
        {"No STA/AP Concurrency",
         {{kMainModeId,
           ChipIfaceCombination::make_vec(
               {{{{AP}, 1}}, {{{STA}, 1}, {{P2P, NAN}, 1}}})}}},

        // STA + AP concurrency
        // 1 - (1 STA + 1 AP) or (1 STA + 1 of (P2P or NAN))
        {"STA + AP Concurrency",
         {{kMainModeId,
           ChipIfaceCombination::make_vec(
               {{{{STA}, 1}, {{AP}, 1}}, {{{STA}, 1}, {{P2P, NAN}, 1}}})}}},

        // STA + STA concurrency
        // 2 - (1 STA + 1 AP) or (2 STA + 1 of (P2P or NAN))
        {"Dual STA Concurrency",
         {{kMainModeId,
           ChipIfaceCombination::make_vec(
               {{{{STA}, 1}, {{AP}, 1}}, {{{STA}, 2}, {{P2P, NAN}, 1}}})}}},

        // AP + AP + STA concurrency
        // 3 - (1 STA + 2 AP) or (1 STA + 1 of (P2P or NAN))
        {"Dual AP Concurrency",
         {{kMainModeId,
           ChipIfaceCombination::make_vec(
               {{{{STA}, 1}, {{AP}, 2}}, {{{STA}, 1}, {{P2P, NAN}, 1}}})}}},

        // STA + STA concurrency and AP + AP + STA concurrency
        // 4 - (1 STA + 2 AP) or (2 STA + 1 of (P2P or NAN))
        {"Dual STA & Dual AP Concurrency",
         {{kMainModeId,
           ChipIfaceCombination::make_vec(
               {{{{STA}, 1}, {{AP}, 2}}, {{{STA}, 2}, {{P2P, NAN}, 1}}})}}}};

#undef STA
#undef AP
#undef P2P
#undef NAN

#ifdef WIFI_HIDL_FEATURE_DISABLE_AP_MAC_RANDOMIZATION
#pragma message                                                               \
    "WIFI_HIDL_FEATURE_DISABLE_AP_MAC_RANDOMIZATION is deprecated; override " \
    "'config_wifi_ap_randomization_supported' in "                            \
    "frameworks/base/core/res/res/values/config.xml in the device overlay "   \
    "instead"
#endif  // WIFI_HIDL_FEATURE_DISABLE_AP_MAC_RANDOMIZATION

WifiFeatureFlags::WifiFeatureFlags() {}

std::vector<IWifiChip::ChipMode> WifiFeatureFlags::getChipModesForPrimary() {
    std::array<char, PROPERTY_VALUE_MAX> buffer;
    auto res = property_get(kDebugPresetInterfaceCombinationIdxProperty,
                            buffer.data(), nullptr);
    // Debug propety not set, use the device preset interface combination.
    if (res <= 0) return kChipModesPrimary;

    // Debug propety set, use one of the debug preset interface combination.
    unsigned long idx = std::stoul(buffer.data());
    if (idx >= kDebugChipModes.size()) {
        LOG(ERROR) << "Invalid index set in property: "
                   << kDebugPresetInterfaceCombinationIdxProperty;
        return kChipModesPrimary;
    }
    std::string name;
    std::vector<IWifiChip::ChipMode> chip_modes;
    std::tie(name, chip_modes) = kDebugChipModes[idx];
    LOG(INFO) << "Using debug chip mode: <" << name << "> set via property: "
              << kDebugPresetInterfaceCombinationIdxProperty;
    return chip_modes;
}

std::vector<IWifiChip::ChipMode> WifiFeatureFlags::getChipModes(
    bool is_primary) {
    return (is_primary) ? getChipModesForPrimary() : kChipModesSecondary;
}

}  // namespace feature_flags
}  // namespace implementation
}  // namespace V1_5
}  // namespace wifi
}  // namespace hardware
}  // namespace android
