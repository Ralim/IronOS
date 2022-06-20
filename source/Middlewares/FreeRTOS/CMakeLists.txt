
set(sources 
croutine.c
event_groups.c
list.c
queue.c
tasks.c
timers.c
CMSIS_RTOS/cmsis_os.c
)


add_library(FreeRTOS ${sources})

#Reqired to be able to grab the freertos config header
target_link_libraries(FreeRTOS PRIVATE BSP PUBLIC BSPImplementation)
target_include_directories(FreeRTOS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_RTOS)