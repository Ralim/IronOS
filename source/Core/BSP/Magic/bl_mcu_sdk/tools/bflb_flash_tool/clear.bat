del /a /f /s /q "*.log"
del /a /f /s /q "*.ini"
del /a /f /s /q "iot.toml"
del /a /f /s /q "mcu.toml"
del /a /f /s /q "storage.toml"
del /a /f /s /q "partition.bin"
del /a /f /s /q "ro_params.dtb"
del /a /f /s /q "flash_para.bin"
del /a /f /s /q .\chips\bl602\img_create_iot\*.bin
del /a /f /s /q .\chips\bl702\img_create_iot\*.bin
del /a /f /s /q .\chips\bl56x\img_create_iot\*.bin
del /a /f /s /q .\chips\bl60x\img_create_iot\*.bin
del /a /f /s /q .\chips\bl606p\img_create_iot\*.bin
del /a /f /s /q .\chips\bl602u\img_create_iot\*.bin
del /a /f /s /q .\chips\bl808\img_create_iot\*.bin
del /a /f /s /q .\chips\bl602\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl702\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl56x\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl60x\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl606p\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl602u\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl808\img_create_mcu\*.bin
del /a /f /s /q .\chips\bl602\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl702\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl56x\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl60x\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl606p\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl602u\efuse_bootheader\*.bin
del /a /f /s /q .\chips\bl808\efuse_bootheader\*.bin
del /a /f /s /q .\chips\*.pack
rd /s /Q .\chips\bl602\ota
rd /s /Q .\chips\bl702\ota
rd /s /Q .\chips\bl56x\ota
rd /s /Q .\chips\bl60x\ota
rd /s /Q .\chips\bl606p\ota
rd /s /Q .\chips\bl602u\ota
rd /s /Q .\chips\bl808\ota
pause