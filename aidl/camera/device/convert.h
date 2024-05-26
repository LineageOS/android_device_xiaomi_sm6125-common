/*
 * Copyright (C) 2022 The Android Open Source Project
 * Copyright (C) 2024 The LineageOS Project
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

#ifndef HARDWARE_INTERFACES_CAMERA_DEVICE_DEFAULT_CONVERT_H_
#define HARDWARE_INTERFACES_CAMERA_DEVICE_DEFAULT_CONVERT_H_

#include <aidl/android/hardware/camera/common/Status.h>
#include <aidl/android/hardware/camera/device/BufferStatus.h>
#include <aidl/android/hardware/camera/device/CameraMetadata.h>
#include <aidl/android/hardware/camera/device/HalStream.h>
#include <aidl/android/hardware/camera/device/NotifyMsg.h>
#include <aidl/android/hardware/camera/device/Stream.h>
#include <aidlcommonsupport/NativeHandle.h>
#include <hardware/camera3.h>
#include <system/camera_metadata.h>

namespace android {
namespace hardware {
namespace camera {
namespace device {
namespace implementation {

// The camera3_stream_t sent to conventional HAL. Added mId fields to enable stream ID lookup
// fromt a downcasted camera3_stream
struct Camera3Stream : public camera3_stream {
    int mId;
};

using ::aidl::android::hardware::camera::common::Status;
using ::aidl::android::hardware::camera::device::BufferStatus;
using ::aidl::android::hardware::camera::device::CameraMetadata;
using ::aidl::android::hardware::camera::device::HalStream;
using ::aidl::android::hardware::camera::device::NotifyMsg;
using ::aidl::android::hardware::camera::device::Stream;

void convertToAidl(const camera_metadata_t* src, CameraMetadata* dest);

bool convertFromAidl(const CameraMetadata& src, const camera_metadata_t** dst);

void convertToAidl(const Camera3Stream* src, HalStream* dst);
void convertToAidl(const camera3_stream_configuration_t& src, std::vector<HalStream>* dst);

void convertFromAidl(const Stream& src, Camera3Stream* dst);

void convertFromAidl(buffer_handle_t*, BufferStatus, camera3_stream_t*, int acquireFence,  // inputs
                     camera3_stream_buffer_t* dst);

void convertToAidl(const camera3_notify_msg* src, NotifyMsg* dst);

inline aidl::android::hardware::common::NativeHandle makeToAidlIfNotNull(
        const native_handle_t* nh) {
    if (nh == nullptr) {
        return aidl::android::hardware::common::NativeHandle();
    }
    return makeToAidl(nh);
}

inline ndk::ScopedAStatus fromStatus(Status status) {
    return status == Status::OK
                   ? ndk::ScopedAStatus::ok()
                   : ndk::ScopedAStatus::fromServiceSpecificError(static_cast<int32_t>(status));
}

}  // namespace implementation
}  // namespace device
}  // namespace camera
}  // namespace hardware
}  // namespace android

#endif  // HARDWARE_INTERFACES_CAMERA_DEVICE_DEFAULT_CONVERT_H_
