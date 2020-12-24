include $(THEOS)/makefiles/common.mk

TWEAK_NAME = Fiona
Fiona_FILES = Fiona.xm
Fiona_FRAMEWORKS = CydiaSubstrate Foundation # UIKit

# .SECONDARY: /work/packs/fiona/.theos/obj/arm64/Fiona.xm.mm

CFLAGS = -Ofast -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-command-line-argument -Wno-unknown-pragmas -Wno-format-security -Wno-unused-function -funroll-loops -I/usr/include -flto=thin -fwhole-program-vtables

LDFLAGS = -lto_library /usr/lib/llvm-10/lib/libLTO.dylib

#Fiona_LDFLAGS = -Wl,-object_path_lto,/usr/lib/llvm-10/lib/libLTO.dylib # -Wl,-segalign,4000

# export ARCHS = arm64
# Fiona_ARCHS = arm64

# FINALPACKAGE=1
# FOR_RELEASE=1

#export PACKAGE_VERSION=10.3
#export TARGET=iphone:clang:10.3:10.3

include $(THEOS_MAKE_PATH)/tweak.mk

#all::
	#@echo "[+] Copying Files..."

	#@cp ./obj/obj/debug/Fiona.dylib /Library/MobileSubstrate/DynamicLibraries/Fiona.dylib

	#@/usr/bin/ldid -S /Library/MobileSubstrate/DynamicLibraries/Fiona.dylib

	#@echo "DONE"

	#@killall SpringBoard
