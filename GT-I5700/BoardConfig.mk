# 
# Config for Samsung Spica
#

TARGET_CPU_ABI := armeabi
TARGET_ARCH_VARIANT := armv5te-vfp
TARGET_GLOBAL_CFLAGS += -mtune=arm1176jzf-s -mfpu=vfp
TARGET_GLOBAL_CPPFLAGS += -mtune=arm1176jzf-s -mfpu=vfp

TARGET_NO_RECOVERY := true
TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := true
TARGET_NO_RADIOIMAGE := true

USE_CAMERA_STUB := false
BOARD_USES_ECLAIR_LIBCAMERA := true

TARGET_BOOTLOADER_BOARD_NAME := GT-I5700
TARGET_BOARD_PLATFORM := s3c6410

BOARD_USES_GENERIC_AUDIO := false
BOARD_USES_ALSA_AUDIO := true
BUILD_WITH_ALSA_UTILS := true

BOARD_WLAN_DEVICE           := eth0
WIFI_DRIVER_MODULE_PATH     := "/lib/modules/dhd.ko"
WIFI_DRIVER_MODULE_ARG      := "firmware_path=/system/etc/rtecdc.bin nvram_path=/system/etc/nvram.txt"
WIFI_DRIVER_MODULE_NAME     := "dhd"

BOARD_HAVE_BLUETOOTH    := false
#BOARD_HAVE_BLUETOOTH    := true
#BOARD_HAVE_BLUETOOTH_BCM    := true
#BT_USE_BTL_IF := true
#BT_ALT_STACK := true
#BRCM_BTL_INCLUDE_A2DP := true
#BRCM_BT_USE_BTL_IF := true

BOARD_GPS_LIBRARIES := libsecgps libsecril-client

