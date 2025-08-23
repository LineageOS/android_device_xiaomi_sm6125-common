#include <android-base/properties.h>
#include <cutils/log.h>
#include <hardware/camera_common.h>
#include <system/camera_metadata.h>

#include <dlfcn.h>
#include <string.h>
#include <memory>

camera_module HMI;
camera_module* HMI_qcom;

static int camera_device_open(const hw_module_t* /*module*/, const char* id, hw_device_t** hw_device) {
    return HMI_qcom->common.methods->open(&HMI_qcom->common, id, hw_device);
}

int get_camera_info(int camera_id, struct camera_info* info) {
    int ret = HMI_qcom->get_camera_info(camera_id, info);
    if (camera_id == 61) {
        size_t entryCap = get_camera_metadata_entry_capacity(info->static_camera_characteristics);
        size_t dataCap = get_camera_metadata_data_capacity(info->static_camera_characteristics);

        camera_metadata_entry_t entry;
        ret = find_camera_metadata_entry(
                const_cast<camera_metadata_t*>(info->static_camera_characteristics),
                ANDROID_LOGICAL_MULTI_CAMERA_PHYSICAL_IDS, &entry);

        if (entry.count == 4) {
            // We need one more byte for an additional number in the second camera ID
            std::unique_ptr<camera_metadata_t, decltype(&free)> metadata(
                    allocate_camera_metadata(entryCap, dataCap + 1), free);
            if (!metadata.get()) {
                ALOGE("%s: Failed to allocate new camera metadata!", __FUNCTION__);
                return ret;
            }

            ret = append_camera_metadata(metadata.get(), info->static_camera_characteristics);
            if (ret != 0) {
                ALOGE("%s: Failed to append camera metadata!", __FUNCTION__);
                return ret;
            }

            ret = find_camera_metadata_entry(metadata.get(),
                                             ANDROID_LOGICAL_MULTI_CAMERA_PHYSICAL_IDS, &entry);
            if (ret != 0) {
                ALOGE("%s: Failed to find ANDROID_LOGICAL_MULTI_CAMERA_PHYSICAL_IDS entry!",
                      __FUNCTION__);
                return ret;
            }

            // ID 61 is supposed to be 0+20
            const char ids[] =
                    "0\0"
                    "20\0";
            const uint8_t* data = reinterpret_cast<const uint8_t*>(ids);
            size_t dataSize = sizeof(ids) - 1;

            ret = update_camera_metadata_entry(metadata.get(), entry.index, data, dataSize, &entry);
            if (ret != 0) {
                ALOGE("%s: Failed to update ANDROID_LOGICAL_MULTI_CAMERA_PHYSICAL_IDS entry!",
                      __FUNCTION__);
                return -1;
            }

            info->static_camera_characteristics = metadata.get();
        }
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
