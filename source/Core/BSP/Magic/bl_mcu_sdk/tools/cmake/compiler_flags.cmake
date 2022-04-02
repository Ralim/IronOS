list(APPEND GLOBAL_C_FLAGS -O2 -g3)
list(APPEND GLOBAL_C_FLAGS -fno-jump-tables -fshort-enums -fno-common -fms-extensions -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -ffast-math)
list(APPEND GLOBAL_C_FLAGS -Wall -Wshift-negative-value -Wchar-subscripts -Wformat -Wuninitialized -Winit-self -Wignored-qualifiers -Wunused -Wundef)
list(APPEND GLOBAL_C_FLAGS -msmall-data-limit=4)
list(APPEND GLOBAL_C_FLAGS -Wtype-limits)

list(APPEND GLOBAL_LD_FLAGS -Wl,--cref -Wl,--gc-sections -nostartfiles -g3)
list(APPEND GLOBAL_LD_FLAGS -fms-extensions -ffunction-sections -fdata-sections)
list(APPEND GLOBAL_LD_FLAGS -Wall -Wchar-subscripts -std=c99)
list(APPEND GLOBAL_LD_FLAGS --specs=nano.specs)


if(CONFIG_PRINT_FLOAT)
list(APPEND GLOBAL_LD_FLAGS -u _printf_float)
endif()

# if(${SUPPORT_BACKTRACE} STREQUAL "y")
# list(APPEND GLOBAL_C_FLAGS -fno-omit-frame-pointer)
# endif()



