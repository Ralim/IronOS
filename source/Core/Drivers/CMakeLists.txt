include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources 
BMA223.cpp        
BootLogo.cpp      
Buttons.cpp       
I2CBB.cpp         
LIS2DH12.cpp    
MMA8652FC.cpp         
MSA301.cpp            
OLED.cpp              
SC7A20.cpp            
Si7210.cpp
TipThermoModel.cpp
usb-pd
USBPD.cpp
Utils.cpp
)


add_library(drivers ${sources})
target_link_libraries(drivers PUBLIC  BSP USBPDLib)
target_include_directories(drivers PUBLIC .)

add_subdirectory(usb-pd)