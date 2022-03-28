set(sources 
freertos.c
FreeRTOSHooks.c
gui.cpp
main.cpp
power.cpp
QC3.cpp
ScrollMessage.cpp
Settings.cpp
syscalls.c
Translation.cpp
)


add_library(mainSource ${sources})
target_include_directories(mainSource PUBLIC .)
target_link_libraries(mainSource PUBLIC brieflz BSP FreeRTOS drivers languages)
