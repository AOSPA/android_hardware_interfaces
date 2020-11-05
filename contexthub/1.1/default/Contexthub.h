/*
 * Copyright (C) 2020 The Android Open Source Project
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
#pragma once

#include "ContextHub.h"

#include <android/hardware/contexthub/1.1/IContexthub.h>

namespace android {
namespace hardware {
namespace contexthub {
namespace V1_1 {
namespace implementation {

class Contexthub
    : public ::android::hardware::contexthub::V1_X::implementation::ContextHub<IContexthub> {
  public:
    // Methods from V1_1::IContexthub
    Return<void> onSettingChanged(Setting setting, SettingValue newValue) override;
};

}  // namespace implementation
}  // namespace V1_1
}  // namespace contexthub
}  // namespace hardware
}  // namespace android
