/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "CameraDeviceSession.h"

#include <CameraModule.h>
#include <aidl/android/hardware/camera/common/Status.h>
#include <aidl/android/hardware/camera/device/BnCameraDevice.h>

namespace android {
namespace hardware {
namespace camera {
namespace device {
namespace implementation {

using ::aidl::android::hardware::camera::common::CameraResourceCost;
using ::aidl::android::hardware::camera::common::Status;
using ::aidl::android::hardware::camera::device::BnCameraDevice;
using ::aidl::android::hardware::camera::device::CameraMetadata;
using ::aidl::android::hardware::camera::device::ICameraDeviceCallback;
using ::aidl::android::hardware::camera::device::ICameraDeviceSession;
using ::aidl::android::hardware::camera::device::ICameraInjectionSession;
using ::aidl::android::hardware::camera::device::StreamConfiguration;
using ::android::hardware::camera::common::helper::CameraModule;

class CameraDevice : public BnCameraDevice {
  public:
    // Called by provider HAL.
    // Provider HAL must ensure the uniqueness of CameraDevice object per cameraId, or there could
    // be multiple CameraDevice trying to access the same physical camera.  Also, provider will have
    // to keep track of all CameraDevice objects in order to notify CameraDevice when the underlying
    // camera is detached.
    CameraDevice(sp<CameraModule> module, const std::string& cameraId,
                 const SortedVector<std::pair<std::string, std::string>>& cameraDeviceNames);
    virtual ~CameraDevice();

    ndk::ScopedAStatus getCameraCharacteristics(CameraMetadata* _aidl_return) override;
    ndk::ScopedAStatus getPhysicalCameraCharacteristics(const std::string& in_physicalCameraId,
                                                        CameraMetadata* _aidl_return) override;
    ndk::ScopedAStatus getResourceCost(CameraResourceCost* _aidl_return) override;
    ndk::ScopedAStatus isStreamCombinationSupported(const StreamConfiguration& in_streams,
                                                    bool* _aidl_return) override;
    ndk::ScopedAStatus open(const std::shared_ptr<ICameraDeviceCallback>& in_callback,
                            std::shared_ptr<ICameraDeviceSession>* _aidl_return) override;
    ndk::ScopedAStatus openInjectionSession(
            const std::shared_ptr<ICameraDeviceCallback>& in_callback,
            std::shared_ptr<ICameraInjectionSession>* _aidl_return) override;
    ndk::ScopedAStatus setTorchMode(bool in_on) override;
    ndk::ScopedAStatus turnOnTorchWithStrengthLevel(int32_t in_torchStrength) override;
    ndk::ScopedAStatus getTorchStrengthLevel(int32_t* _aidl_return) override;

    binder_status_t dump(int fd, const char** args, uint32_t numArgs) override;

    // Caller must use this method to check if CameraDevice ctor failed
    bool isInitFailed() { return mInitFail; }
    // Used by provider HAL to signal external camera disconnected
    void setConnectionStatus(bool connected);

    // Device version to be used by the external camera provider.
    // Should be of the form <major>.<minor>
    static std::string kDeviceVersion;

  protected:
    virtual std::shared_ptr<CameraDeviceSession> createSession(
            camera3_device_t*, const camera_metadata_t* deviceInfo,
            const std::shared_ptr<ICameraDeviceCallback>&);

    const sp<CameraModule> mModule;
    const std::string mCameraId;
    // const after ctor
    int mCameraIdInt;
    int mDeviceVersion;
    bool mInitFail = false;
    // Set by provider (when external camera is connected/disconnected)
    bool mDisconnected;
    std::weak_ptr<CameraDeviceSession> mSession{};

    const SortedVector<std::pair<std::string, std::string>>& mCameraDeviceNames;

    // gating access to mSession and mDisconnected
    mutable Mutex mLock;

    // convert conventional HAL status to AIDL Status
    static Status getAidlStatus(int);

    Status initStatus() const;
};

}  // namespace implementation
}  // namespace device
}  // namespace camera
}  // namespace hardware
}  // namespace android
