#!/bin/bash

# Environment variable
echo " "
echo -e "Environment variable prepared.\n"

LLVM_PATH="/home/joaquimiguel/toolchains/neutron-24.0/bin/"

HOST_BUILD_ENV="ARCH=arm64 \
                CC=${LLVM_PATH}clang \
                CROSS_COMPILE=${LLVM_PATH}aarch64-linux-gnu- \
                LLVM=1 \
                LLVM_IAS=1 \
                PATH=$LLVM_PATH:$PATH \
                -j$(nproc --all)"

KERNEL_MAKE_ENV="DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"

OUT_DIR="$(pwd)/out"
BOOT_DIR="$OUT_DIR/arch/arm64/boot"
AK3_DIR="$(pwd)/AnyKernel3"
DEFCONFIG="gki_defconfig vendor/kalama_GKI.config vendor/ext_config/moto-kalama.config vendor/ext_config/moto-kalama-gki.config vendor/ext_config/moto-kalama-rtwo.config"

# Clear old build
echo -e "Old build cleaned up.\n"

rm -rf "$AK3_DIR/Image"
rm -rf .version .local

# Execute make clean && mrproper question
read -p "Execute make clean && mrproper? (Y/n): " r
case "$r" in 
  Y|y) make clean && make mrproper;;
  N|n) echo "Aborted.";;
esac

# Clear /out question
echo " "

read -p "Execute rm -rf out? (Y/n): " r
case "$r" in 
  Y|y) rm -rf out;;
  N|n) echo "Aborted.";;
esac

# Build defconfig
echo " "
echo -e "Building defconfig...\n"

make O=out $HOST_BUILD_ENV $DEFCONFIG

# Starting compilation
echo " "
echo -e "Starting compilation...\n"

make O=out -j$(nproc) $KERNEL_MAKE_ENV $HOST_BUILD_ENV Image

ls "$BOOT_DIR"

# Package Kernel
echo " "
echo -e "Preparing zip...\n"

cp "$BOOT_DIR/Image" "$AK3_DIR/Image"

build_date=$(date +%Y%m%d)
gitsha=$(git rev-parse --short=7 HEAD)

cd "$AK3_DIR" || exit 1
rm -f *.zip

zip -r9 "Destiny-${build_date}-${gitsha}-rtwo.zip" .

# Build completed
echo " "
echo "Build finished sucessfully."
