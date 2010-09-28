# Inherit AOSP device configuration for passion.
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full.mk)

# Inherit some common SamdroidMod stuff.
$(call inherit-product, vendor/spica/products/common.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := spica
PRODUCT_BRAND := samsung
PRODUCT_DEVICE := GT-I5700
PRODUCT_MODEL := GT-I5700
PRODUCT_MANUFACTURER := SAMSUNG
PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=FRG83 BUILD_DISPLAY_ID=FRG83 PRODUCT_NAME=spica BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2.1/FRG83/60505:user/release-keys TARGET_BUILD_TYPE=userdebug BUILD_VERSION_TAGS=release-keys
PRIVATE_BUILD_DESC="spica-user 2.2.1 FRG83 60505 release-keys"

PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=$(TOP)/vendor/spica/prelink-linux-arm-spica.map

#
# Set ro.modversion
#
ifdef SAMDROID_NIGHTLY
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=CyanogenMod-2-$(shell date +%m%d%Y)-NIGHTLY-Spica
else
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=CyanogenMod-2.0.0-Spica-alpha6
endif

# Time between scans in seconds. Keep it high to minimize battery drain.
# This only affects the case in which there are remembered access points,
# but none are in range.
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.supplicant_scan_interval=15

# density in DPI of the LCD of this board. This is used to scale the UI
# appropriately. If this property is not defined, the default value is 160 dpi. 
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=160

PRODUCT_COPY_FILES += \
        device/common/gps/gps.conf_EU_SUPL:system/etc/gps.conf

# Install the features available on this device.
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml 

#
# Copy spica specific prebuilt files
#

#
# Wifi
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/wifi/libwlmlogger.so:system/lib/libwlmlogger.so \
    vendor/spica/prebuilt/spica/wifi/libwlservice.so:system/lib/libwlservice.so \
    vendor/spica/prebuilt/spica/wifi/nvram.txt:system/etc/nvram.txt \
    vendor/spica/prebuilt/spica/wifi/rtecdc.bin:system/etc/rtecdc.bin \
    vendor/spica/prebuilt/spica/wifi/nvram_mfg.txt:system/etc/nvram_mfg.txt \
    vendor/spica/prebuilt/spica/wifi/rtecdc_mfg.bin:system/etc/rtecdc_mfg.bin \
    vendor/spica/prebuilt/spica/wifi/bcm_supp.conf:system/etc/bcm_supp.conf \
    vendor/spica/prebuilt/spica/wifi/wifi.conf:system/etc/wifi.conf \
    vendor/spica/prebuilt/spica/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    vendor/spica/prebuilt/spica/wifi/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    vendor/spica/prebuilt/spica/wifi/wlservice:system/bin/wlservice \
    vendor/spica/prebuilt/spica/wifi/wpa_supplicant:system/bin/wpa_supplicant

#
# Display (2D)
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/gralloc-libs/libs3c2drender.so:system/lib/libs3c2drender.so \
    vendor/spica/prebuilt/spica/gralloc-libs/libsavscmn.so:system/lib/libsavscmn.so \
    vendor/spica/prebuilt/spica/gralloc-libs/hw/gralloc.GT-I5700.so:system/lib/hw/gralloc.GT-I5700.so

#
# Display (3D)
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/fimg-libs/egl.cfg:system/lib/egl/egl.cfg \
    vendor/spica/prebuilt/spica/fimg-libs/libChunkAlloc.so:system/lib/egl/libChunkAlloc.so \
    vendor/spica/prebuilt/spica/fimg-libs/libEGL_fimg.so:system/lib/egl/libEGL_fimg.so \
    vendor/spica/prebuilt/spica/fimg-libs/libGLESv1_CM_fimg.so:system/lib/egl/libGLESv1_CM_fimg.so \
    vendor/spica/prebuilt/spica/fimg-libs/libGLESv2_fimg.so:system/lib/egl/libGLESv2_fimg.so

#
# Keys
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/keys/s3c-keypad-rev0020.kl:system/usr/keylayout/s3c-keypad-rev0020.kl \
    vendor/spica/prebuilt/spica/keys/sec_headset.kl:system/usr/keylayout/sec_headset.kl \
    vendor/spica/prebuilt/spica/keys/s3c-keypad-rev0020.kcm.bin:system/usr/keychars/s3c-keypad-rev0020.kcm.bin

#
# Sensors, Lights etc
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/hw/copybit.GT-I5700.so:system/lib/hw/copybit.GT-I5700.so \
    vendor/spica/prebuilt/spica/hw/lights.GT-I5700.so:system/lib/hw/lights.GT-I5700.so \
    vendor/spica/prebuilt/spica/hw/sensors.GT-I5700.so:system/lib/hw/sensors.GT-I5700.so 

#
# Vold
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/vold/vold.fstab:system/etc/vold.fstab

#
# RIL
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/ril/drexe:system/bin/drexe \
    vendor/spica/prebuilt/spica/ril/efsd:system/bin/efsd \
    vendor/spica/prebuilt/spica/ril/rilclient-test:system/bin/rilclient-test \
    vendor/spica/prebuilt/spica/ril/libsec-ril.so:system/lib/libsec-ril.so
#    vendor/spica/prebuilt/spica/ril/libsecril-client.so:system/lib/libsecril-client.so

#
# GSM APN list
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/common/etc/apns-conf.xml:system/etc/apns-conf.xml

#
# Audio
#
PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/spica/audio/asound.conf:system/etc/asound.conf
