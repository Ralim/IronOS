set(TARGET_REQUIRED_LIBS rt-thread)
set(TARGET_REQUIRED_SRCS ${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/${CHIP}/board/board.c
${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/${CHIP}/board/startup_gcc.S)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/components/rt-thread/bsp/bouffalolab/${CHIP}/board/bouffalo.ld)
set(mains main.c)
generate_bin()



