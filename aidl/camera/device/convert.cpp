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

#define LOG_TAG "android.hardware.camera.device-convert-impl"
#include <log/log.h>

#include "convert.h"

#include <aidl/android/hardware/graphics/common/BufferUsage.h>
#include <aidl/android/hardware/graphics/common/Dataspace.h>
#include <aidl/android/hardware/graphics/common/PixelFormat.h>
#include <hardware/camera_common.h>

namespace android {
namespace hardware {
namespace camera {
namespace device {
namespace implementation {

using ::aidl::android::hardware::camera::device::ErrorCode;
using ::aidl::android::hardware::camera::device::ErrorMsg;
using ::aidl::android::hardware::camera::device::ShutterMsg;
using ::aidl::android::hardware::graphics::common::BufferUsage;
using ::aidl::android::hardware::graphics::common::Dataspace;
using ::aidl::android::hardware::graphics::common::PixelFormat;

void convertToAidl(const camera_metadata_t* src, CameraMetadata* dest) {
    if (src == nullptr) {
        return;
    }

    size_t size = get_camera_metadata_size(src);
    auto* src_start = (uint8_t*)src;
    uint8_t* src_end = src_start + size;
    dest->metadata.assign(src_start, src_end);
}

bool convertFromAidl(const CameraMetadata& src, const camera_metadata_t** dst) {
    const std::vector<uint8_t>& metadata = src.metadata;
    if (metadata.empty()) {
        // Special case for null metadata
        *dst = nullptr;
        return true;
    }

    const uint8_t* data = metadata.data();
    // check that the size of CameraMetadata match underlying camera_metadata_t
    if (get_camera_metadata_size((camera_metadata_t*)data) != metadata.size()) {
        ALOGE("%s: input CameraMetadata is corrupt!", __FUNCTION__);
        return false;
    }
    *dst = (camera_metadata_t*)data;
    return true;
}

void convertToAidl(const Camera3Stream* src, HalStream* dst) {
    dst->overrideDataSpace = (Dataspace)src->data_space;
    dst->id = src->mId;
    dst->overrideFormat = (PixelFormat)src->format;
    dst->maxBuffers = src->max_buffers;
    dst->physicalCameraId = src->physical_camera_id;
    if (src->stream_type == CAMERA3_STREAM_OUTPUT) {
        dst->consumerUsage = (BufferUsage)0;
        dst->producerUsage = (BufferUsage)src->usage;
    } else if (src->stream_type == CAMERA3_STREAM_INPUT) {
        dst->producerUsage = (BufferUsage)0;
        dst->consumerUsage = (BufferUsage)src->usage;
    } else {
        // Should not reach here per current AIDL spec, but we might end up adding
        //  bi-directional stream to AIDL.
        ALOGW("%s: Stream type %d is not currently supported!", __FUNCTION__, src->stream_type);
    }
}

void convertToAidl(const camera3_stream_configuration_t& src, std::vector<HalStream>* dst) {
    dst->resize(src.num_streams);
    for (uint32_t i = 0; i < src.num_streams; i++) {
        convertToAidl(static_cast<Camera3Stream*>(src.streams[i]), &dst->at(i));
    }
}

void convertFromAidl(const Stream& src, Camera3Stream* dst) {
    dst->mId = src.id;
    dst->stream_type = (int)src.streamType;
    dst->width = src.width;
    dst->height = src.height;
    dst->format = (int)src.format;
    dst->data_space = (android_dataspace_t)src.dataSpace;
    dst->rotation = (int)src.rotation;
    dst->usage = (uint32_t)src.usage;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    dst->max_buffers = 0;
    dst->priv = 0;
    // Initialize physical_camera_id
    dst->physical_camera_id = nullptr;
}

void convertFromAidl(buffer_handle_t* bufPtr, BufferStatus status, camera3_stream_t* stream,
                     int acquireFence, camera3_stream_buffer_t* dst) {
    dst->stream = stream;
    dst->buffer = bufPtr;
    dst->status = (int)status;
    dst->acquire_fence = acquireFence;
    dst->release_fence = -1;  // meant for HAL to fill in
}

void convertToAidl(const camera3_notify_msg* src, NotifyMsg* dst) {
    switch (src->type) {
        case CAMERA3_MSG_ERROR: {
            ErrorMsg error;
            // The camera3_stream_t* must be the same as what wrapper HAL passed to conventional
            // HAL, or the ID lookup will return garbage. Caller should validate the ID here is
            // indeed one of active stream IDs
            Camera3Stream* stream = static_cast<Camera3Stream*>(src->message.error.error_stream);
            error.frameNumber = src->message.error.frame_number;
            error.errorStreamId = (stream != nullptr) ? stream->mId : -1;
            error.errorCode = (ErrorCode)src->message.error.error_code;
            dst->set<NotifyMsg::Tag::error>(error);
        } break;
        case CAMERA3_MSG_SHUTTER: {
            ShutterMsg shutter;
            shutter.frameNumber = src->message.shutter.frame_number;
            shutter.timestamp = src->message.shutter.timestamp;
            dst->set<NotifyMsg::Tag::shutter>(shutter);
        } break;
        default:
            ALOGE("%s: HIDL type converion failed. Unknown msg type 0x%x", __FUNCTION__, src->type);
    }
    return;
}

}  // namespace implementation
}  // namespace device
}  // namespace camera
}  // namespace hardware
}  // namespace android
