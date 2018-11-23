#!/bin/bash
#------------------------------------------
# postbuild.sh - create mixed c/asm listing
#------------------------------------------
#set -x
export BUILD_PATH=$1;   shift
export PROJECT_NAME=$1; shift
export BIN=${BUILD_PATH}/${PROJECT_NAME}.bin
export ELF=${BUILD_PATH}/${PROJECT_NAME}.elf
export LSS=${BUILD_PATH}/${PROJECT_NAME}.lss

arm-none-eabi-objcopy -O binary ${ELF} ${BIN}

arm-none-eabi-readelf -x .isr_vector ${ELF} >${LSS}
arm-none-eabi-objdump -CS -x ${ELF} >>${LSS}
arm-none-eabi-readelf -x .rodata -x .data -x .preinit_array -x .init_array -x .fini_array ${ELF} >>${LSS} 2>/dev/null

exit 0
