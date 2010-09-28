# Generic leshakmod product
PRODUCT_NAME := CyanogenMod
PRODUCT_BRAND := criminal
PRODUCT_DEVICE := generic

# Use edify for otapackage
PRODUCT_SPECIFIC_DEFINES += TARGET_OTA_SCRIPT_MODE=amend

# Used by BusyBox
KERNEL_MODULES_DIR:=/lib/modules

# Tiny toolbox
TINY_TOOLBOX:=true

# Enable Windows Media if supported by the board
WITH_WINDOWS_MEDIA:=true

PRODUCT_PROPERTY_OVERRIDES += \
    ro.url.legal=http://www.google.com/intl/%s/mobile/android/basic/phone-legal.html \
    ro.url.legal.android_privacy=http://www.google.com/intl/%s/mobile/android/basic/privacy.html \
    ro.com.google.clientidbase=android-google \
    ro.com.android.wifi-watchlist=GoogleGuest \
    ro.setupwizard.enterprise_mode=1 \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.com.android.dataroaming=false \
    dalvik.vm.dexopt-data-only=1

PRODUCT_PACKAGES += \
    ADWLauncher \
    CMParts \
    CMPartsHelper \
    CMWallpapers \
    DSPManager \
    Stk \
    Superuser

# Common CM overlay
PRODUCT_PACKAGE_OVERLAYS += vendor/spica/overlay/spica

# Bring in some audio files
include frameworks/base/data/sounds/AudioPackage4.mk

PRODUCT_COPY_FILES += \
    vendor/spica/prebuilt/common/bin/backuptool.sh:system/bin/backuptool.sh \
    vendor/spica/prebuilt/common/lib/libncurses.so:system/lib/libncurses.so \
    vendor/spica/prebuilt/common/etc/resolv.conf:system/etc/resolv.conf \
    vendor/spica/prebuilt/common/etc/terminfo/l/linux:system/etc/terminfo/l/linux \
    vendor/spica/prebuilt/common/etc/terminfo/u/unknown:system/etc/terminfo/u/unknown \
    vendor/spica/prebuilt/common/etc/profile:system/etc/profile \
    vendor/spica/prebuilt/common/xbin/bash:system/xbin/bash \
    vendor/spica/prebuilt/common/xbin/htop:system/xbin/htop \
    vendor/spica/prebuilt/common/xbin/irssi:system/xbin/irssi \
    vendor/spica/prebuilt/common/xbin/lsof:system/xbin/lsof \
    vendor/spica/prebuilt/common/xbin/nano:system/xbin/nano \
    vendor/spica/prebuilt/common/xbin/powertop:system/xbin/powertop \
    vendor/spica/prebuilt/common/xbin/openvpn-up.sh:system/xbin/openvpn-up.sh

#SPICA_WITH_GOOGLE:=true

ifdef SPICA_WITH_GOOGLE

    PRODUCT_COPY_FILES += \
	vendor/spica/proprietary/CarHomeGoogle.apk:system/app/CarHomeGoogle.apk \
	vendor/spica/proprietary/CarHomeLauncher.apk:system/app/CarHomeLauncher.apk \
	vendor/spica/proprietary/com.amazon.mp3.apk:system/app/com.amazon.mp3.apk \
	vendor/spica/proprietary/Facebook.apk:system/app/Facebook.apk \
	vendor/spica/proprietary/GenieWidget.apk:system/app/GenieWidget.apk \
	vendor/spica/proprietary/Gmail.apk:system/app/Gmail.apk \
	vendor/spica/proprietary/GoogleBackupTransport.apk:system/app/GoogleBackupTransport.apk \
	vendor/spica/proprietary/GoogleCalendarSyncAdapter.apk:system/app/GoogleCalendarSyncAdapter.apk \
	vendor/spica/proprietary/GoogleContactsSyncAdapter.apk:system/app/GoogleContactsSyncAdapter.apk \
	vendor/spica/proprietary/GoogleFeedback.apk:system/app/GoogleFeedback.apk \
	vendor/spica/proprietary/GooglePartnerSetup.apk:system/app/GooglePartnerSetup.apk \
	vendor/spica/proprietary/GoogleQuickSearchBox.apk:system/app/GoogleQuickSearchBox.apk \
	vendor/spica/proprietary/GoogleServicesFramework.apk:system/app/GoogleServicesFramework.apk \
	vendor/spica/proprietary/googlevoice.apk:system/app/googlevoice.apk \
	vendor/spica/proprietary/HtcCopyright.apk:system/app/HtcCopyright.apk \
	vendor/spica/proprietary/HtcEmailPolicy.apk:system/app/HtcEmailPolicy.apk \
	vendor/spica/proprietary/HtcSettings.apk:system/app/HtcSettings.apk \
	vendor/spica/proprietary/kickback.apk:system/app/kickback.apk \
	vendor/spica/proprietary/LatinImeTutorial.apk:system/app/LatinImeTutorial.apk \
	vendor/spica/proprietary/Maps.apk:system/app/Maps.apk \
	vendor/spica/proprietary/MarketUpdater.apk:system/app/MarketUpdater.apk \
	vendor/spica/proprietary/MediaUploader.apk:system/app/MediaUploader.apk \
	vendor/spica/proprietary/NetworkLocation.apk:system/app/NetworkLocation.apk \
	vendor/spica/proprietary/OneTimeInitializer.apk:system/app/OneTimeInitializer.apk \
	vendor/spica/proprietary/PassionQuickOffice.apk:system/app/PassionQuickOffice.apk \
	vendor/spica/proprietary/SetupWizard.apk:system/app/SetupWizard.apk \
	vendor/spica/proprietary/soundback.apk:system/app/soundback.apk \
	vendor/spica/proprietary/Street.apk:system/app/Street.apk \
	vendor/spica/proprietary/Talk.apk:system/app/Talk.apk \
	vendor/spica/proprietary/talkback.apk:system/app/talkback.apk \
	vendor/spica/proprietary/Twitter.apk:system/app/Twitter.apk \
	vendor/spica/proprietary/Vending.apk:system/app/Vending.apk \
	vendor/spica/proprietary/VoiceSearch.apk:system/app/VoiceSearch.apk \
	vendor/spica/proprietary/YouTube.apk:system/app/YouTube.apk \
	vendor/spica/proprietary/com.google.android.maps.xml:./system/etc/permissions/com.google.android.maps.xml \
	vendor/spica/proprietary/com.google.android.maps.jar:./system/framework/com.google.android.maps.jar \
	vendor/spica/proprietary/features.xml:./system/etc/permissions/features.xml \
	vendor/spica/proprietary/libspeech.so:./system/lib/libspeech.so
else
    PRODUCT_PACKAGES += \
        Provision \
        GoogleSearch \
        LatinIME
endif

# Copy over the changelog to the device
PRODUCT_COPY_FILES += \
    vendor/spica/CHANGELOG:system/etc/CHANGELOG.txt
