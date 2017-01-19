/*
 * Copyright 2016 The Android Open Source Project
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

#define LOG_TAG "android.hardware.drm@1.0-service"

#include <1.0/default/CryptoFactory.h>
#include <1.0/default/DrmFactory.h>

#include <hidl/HidlTransportSupport.h>
#include <hidl/LegacySupport.h>

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::registerPassthroughServiceImplementation;

using android::hardware::drm::V1_0::ICryptoFactory;
using android::hardware::drm::V1_0::IDrmFactory;

int main() {
    ALOGD("android.hardware.drm@1.0-service starting...");
    configureRpcThreadpool(8, true /* callerWillJoin */);
    registerPassthroughServiceImplementation<IDrmFactory>("drm");
    registerPassthroughServiceImplementation<ICryptoFactory>("crypto");
    joinRpcThreadpool();
}
