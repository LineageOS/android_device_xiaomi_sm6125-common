/*
 * SPDX-FileCopyrightText: 2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/properties.h>
#include <cutils/log.h>
#include <hardware/camera_common.h>
#include <system/camera_metadata.h>
#include <CameraMetadata.h>

#include <dlfcn.h>
#include <string.h>
#include <memory>

camera_module HMI;
camera_module* HMI_qcom;

static int camera_device_open(const hw_module_t* /*module*/, const char* id,
                              hw_device_t** hw_device) {
    return HMI_qcom->common.methods->open(&HMI_qcom->common, id, hw_device);
}

int get_camera_info(int camera_id, struct camera_info* info) {
    int ret = HMI_qcom->get_camera_info(camera_id, info);
    if (camera_id == 61) {
        using android::hardware::camera::common::helper::CameraMetadata;
        CameraMetadata staticInfo(
                const_cast<camera_metadata_t*>(info->static_camera_characteristics));

        const char ids[] =
                "0\0"
                "20\0";
        staticInfo.update(ANDROID_LOGICAL_MULTI_CAMERA_PHYSICAL_IDS, (uint8_t*)ids, 5);
    }

    return ret;
}

__attribute__((constructor)) void camera_wrapper_init() {
    auto handle = dlopen("/vendor/lib/hw/camera.trinket.so", RTLD_NOW);
    HMI_qcom = reinterpret_cast<decltype(HMI_qcom)>(dlsym(handle, "HMI"));

    HMI = *HMI_qcom;

    if (android::base::GetProperty("ro.build.product", "") == "laurel_sprout") {
        // laurel_sprout has a get_cam_pos function added, which breaks the struct
        // Shift all functions by one to realign the struct with AOSP
        HMI.get_camera_info = reinterpret_cast<decltype(HMI.get_camera_info)>(HMI.set_callbacks);
        HMI.set_callbacks = reinterpret_cast<decltype(HMI.set_callbacks)>(HMI.get_vendor_tag_ops);
        HMI.get_vendor_tag_ops =
                reinterpret_cast<decltype(HMI.get_vendor_tag_ops)>(HMI.open_legacy);
        HMI.open_legacy = reinterpret_cast<decltype(HMI.open_legacy)>(HMI.set_torch_mode);
        HMI.set_torch_mode = reinterpret_cast<decltype(HMI.set_torch_mode)>(HMI.init);
        HMI.init = reinterpret_cast<decltype(HMI.init)>(HMI.get_physical_camera_info);
        HMI.get_physical_camera_info = reinterpret_cast<decltype(HMI.get_physical_camera_info)>(
                HMI.is_stream_combination_supported);
        HMI.is_stream_combination_supported =
                reinterpret_cast<decltype(HMI.is_stream_combination_supported)>(
                        HMI.notify_device_state_change);
        HMI.notify_device_state_change =
                reinterpret_cast<decltype(HMI.notify_device_state_change)>(HMI.reserved[0]);
        HMI.reserved[0] = nullptr;
    } else {
        // ginkgo/willow registers only 4 bytes for logical camera IDs
        // Resize the struct and correct the values to 0+20
        HMI.get_camera_info = get_camera_info;
    }

    static auto methods = *HMI.common.methods;
    methods.open = &camera_device_open;
    HMI.common.methods = &methods;
}
