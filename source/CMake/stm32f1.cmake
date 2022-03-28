# core flags

set(CORE_FLAGS "-mthumb -mcpu=cortex-m3 -mlittle-endian -mfloat-abi=soft" CACHE INTERNAL "CPU flags")
add_definitions(${CORE_FLAGS})
# link with linker file
target_link_libraries(${elf_file} PUBLIC -T ${CMAKE_CURRENT_SOURCE_DIR}/linkers/stm32f103.ld)
