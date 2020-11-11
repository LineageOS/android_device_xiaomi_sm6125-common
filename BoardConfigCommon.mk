#
# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

COMMON_PATH := device/xiaomi/sm6125-common

BOARD_VENDOR := xiaomi

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := trinket
TARGET_NO_BOOTLOADER := true

# Platform
TARGET_BOARD_PLATFORM := trinket
TARGET_BOARD_PLATFORM_GPU := qcom-adreno610

# Architecture
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := generic
TARGET_CPU_VARIANT_RUNTIME := cortex-a73

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv8-a
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := generic
TARGET_2ND_CPU_VARIANT_RUNTIME := cortex-a73

TARGET_USES_64_BIT_BINDER := true

# Kernel
BOARD_BOOT_HEADER_VERSION := 2
BOARD_KERNEL_CMDLINE := console=ttyMSM0,115200n8 androidboot.console=ttyMSM0 earlycon=msm_serial_dm,0x4a90000 androidboot.hardware=qcom msm_rtb.filter=0x237 lpm_levels.sleep_disabled=1 service_locator.enable=1 swiotlb=1 androidboot.configfs=true androidboot.usbcontroller=4e00000.dwc3
BOARD_KERNEL_CMDLINE += loop.max_part=7
BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_PAGESIZE := 4096
BOARD_KERNEL_TAGS_OFFSET := 0x00000100
BOARD_RAMDISK_OFFSET     := 0x01000000
BOARD_KERNEL_IMAGE_NAME := Image.gz-dtb
BOARD_KERNEL_SEPARATED_DTBO := true
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)
TARGET_KERNEL_ARCH := arm64
TARGET_KERNEL_HEADER_ARCH := arm64
TARGET_KERNEL_SOURCE := kernel/xiaomi/sm6125
TARGET_KERNEL_CLANG_COMPILE := true

# QCOM hardware
BOARD_USES_QCOM_HARDWARE := true

# ANT+
BOARD_ANT_WIRELESS_DEVICE := "qualcomm-hidl"

# APEX image
DEXPREOPT_GENERATE_APEX_IMAGE := true

# Audio
BOARD_USES_ALSA_AUDIO := true
USE_CUSTOM_AUDIO_POLICY := 1
USE_XML_AUDIO_POLICY_CONF := 1
BOARD_SUPPORTS_SOUND_TRIGGER := true
AUDIO_FEATURE_ENABLED_GEF_SUPPORT := true
AUDIO_FEATURE_ENABLED_FM_POWER_OPT := true

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(COMMON_PATH)/bluetooth
TARGET_FWK_SUPPORTS_FULL_VALUEADDS := true

# Camera
TARGET_USES_QTI_CAMERA_DEVICE := true

# Charger
BOARD_CHARGER_ENABLE_SUSPEND := true
BOARD_CHARGER_DISABLE_INIT_BLANK := true

# CNE and DPM
BOARD_USES_QCNE := true

# Display
USE_DEVICE_SPECIFIC_DISPLAY := true
DEVICE_SPECIFIC_DISPLAY_PATH := $(COMMON_PATH)/qcom-caf/display
TARGET_USES_HWC2 := true
TARGET_USES_GRALLOC1 := true
TARGET_USES_ION := true

MAX_EGL_CACHE_KEY_SIZE := 12*1024
MAX_EGL_CACHE_SIZE := 2048*1024

OVERRIDE_RS_DRIVER := libRSDriver_adreno.so

# DRM
TARGET_ENABLE_MEDIADRM_64 := true

# DT2W
TARGET_TAP_TO_WAKE_NODE := "/sys/touchpanel/double_tap"

# Enable dexpreopt to speed boot time
ifeq ($(HOST_OS),linux)
  ifneq ($(TARGET_BUILD_VARIANT),eng)
    ifeq ($(WITH_DEXPREOPT),)
      WITH_DEXPREOPT := true
      WITH_DEXPREOPT_BOOT_IMG_AND_SYSTEM_SERVER_ONLY := true
    endif
  endif
endif

# FM
BOARD_HAVE_QCOM_FM := true
BOARD_HAS_QCA_FM_SOC := cherokee

# GPS
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
LOC_HIDL_VERSION := 3.0

# HIDL
DEVICE_FRAMEWORK_MANIFEST_FILE := $(COMMON_PATH)/configs/manifests/framework_manifest.xml
DEVICE_MANIFEST_FILE := $(COMMON_PATH)/configs/manifests/manifest.xml
DEVICE_MATRIX_FILE := $(COMMON_PATH)/configs/manifests/compatibility_matrix.xml

# Init
TARGET_INIT_VENDOR_LIB := //$(COMMON_PATH):libinit_trinket
TARGET_RECOVERY_DEVICE_MODULES := libinit_trinket

# Keystore
TARGET_PROVIDES_KEYMASTER := true

# Partitions
BOARD_FLASH_BLOCK_SIZE := 131072 # (BOARD_KERNEL_PAGESIZE * 64)
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x04000000
BOARD_DTBOIMG_PARTITION_SIZE := 8388608
ifneq ($(AB_OTA_UPDATER), true)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_CACHEIMAGE_PARTITION_SIZE := 402653184
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x04000000
endif
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
BOARD_SYSTEMIMAGE_PARTITION_TYPE := ext4
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4

BOARD_ROOT_EXTRA_SYMLINKS := \
    /vendor/firmware_mnt:/firmware \
    /vendor/bt_firmware:/bt_firmware

TARGET_COPY_OUT_VENDOR := vendor
TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_USE_F2FS := true

TARGET_FS_CONFIG_GEN := $(COMMON_PATH)/config.fs

# Power
BOARD_POWER_CUSTOM_BOARD_LIB := libpower_6125
TARGET_USES_INTERACTION_BOOST := true

# Recovery
BOARD_INCLUDE_DTB_IN_BOOTIMG := true
BOARD_INCLUDE_RECOVERY_DTBO := true
BOARD_HAS_LARGE_FILESYSTEM := true
TARGET_RECOVERY_PIXEL_FORMAT := "RGBX_8888"

# RIL
TARGET_PROVIDES_QTI_TELEPHONY_JAR := true

# SELinux
include device/qcom/sepolicy/sepolicy.mk
BOARD_SEPOLICY_DIRS += $(COMMON_PATH)/sepolicy/vendor
BOARD_PLAT_PUBLIC_SEPOLICY_DIR += $(COMMON_PATH)/sepolicy/public
BOARD_PLAT_PRIVATE_SEPOLICY_DIR += $(COMMON_PATH)/sepolicy/private

# Treble
PRODUCT_VENDOR_MOVE_ENABLED := true
BOARD_VNDK_VERSION := current

# Verified Boot
BOARD_AVB_ENABLE := true
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --set_hashtree_disabled_flag
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --flags 2

# WiFi
BOARD_WLAN_DEVICE := qcwcn
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_DRIVER_DEFAULT := qca_cld3
WIFI_DRIVER_STATE_CTRL_PARAM := "/dev/wlan"
WIFI_DRIVER_STATE_OFF := "OFF"
WIFI_DRIVER_STATE_ON := "ON"
WIFI_HIDL_FEATURE_DUAL_INTERFACE := true
WPA_SUPPLICANT_VERSION := VER_0_8_X

# Inherit the proprietary files
include vendor/xiaomi/sm6125-common/BoardConfigVendor.mk
