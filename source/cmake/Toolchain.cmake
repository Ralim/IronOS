# Toolchain file for IronOS cross-compilation
# This file sets up the cross-compilation environment for different target architectures

# Set the target system
set(CMAKE_SYSTEM_NAME Generic)

# Determine the target architecture based on the model
if(MODEL IN_LIST ALL_MINIWARE_MODELS OR MODEL IN_LIST ALL_MHP30_MODELS OR MODEL IN_LIST ALL_SEQURE_MODELS)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(COMPILER_PREFIX "arm-none-eabi")
elseif(MODEL IN_LIST ALL_PINECIL_MODELS OR MODEL IN_LIST ALL_PINECIL_V2_MODELS)
    set(CMAKE_SYSTEM_PROCESSOR riscv)
    set(COMPILER_PREFIX "riscv-none-elf")
else()
    message(FATAL_ERROR "Unknown model architecture for: ${MODEL}")
endif()

# Set compiler executables
set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-gcc)

# Set additional tools
set(CMAKE_AR ${COMPILER_PREFIX}-ar)
set(CMAKE_OBJCOPY ${COMPILER_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${COMPILER_PREFIX}-objdump)
set(CMAKE_SIZE ${COMPILER_PREFIX}-size)
set(CMAKE_RANLIB ${COMPILER_PREFIX}-ranlib)
set(CMAKE_STRIP ${COMPILER_PREFIX}-strip)

# Configure assembler to use preprocessor
set(CMAKE_ASM_COMPILE_OPTIONS_PIE "")
set(CMAKE_ASM_COMPILE_OPTIONS_PIC "")
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "S;s;asm")

# Prevent CMake from trying to link during compiler testing
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search for programs only in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers only in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Don't use the install rpath
set(CMAKE_SKIP_INSTALL_RPATH TRUE)

# Set default compile flags for different languages
set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_FLAGS_INIT "")
set(CMAKE_ASM_FLAGS_INIT "")

# Ensure we don't link against host libraries
set(CMAKE_EXE_LINKER_FLAGS_INIT "")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "")

# Set the assembler to use C preprocessor
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> -x assembler-with-cpp <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
