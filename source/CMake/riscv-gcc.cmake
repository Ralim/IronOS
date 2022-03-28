# which compilers to use for C and C++
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_C_COMPILER riscv-none-elf-gcc  CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER riscv-none-elf-g++ CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER riscv-none-elf-gcc -x assembler-with-cpp CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY riscv-none-elf-objcopy CACHE INTERNAL "Object Copy")
set(CMAKE_OBJDUMP riscv-none-elf-objdump CACHE INTERNAL "Object Dump")


