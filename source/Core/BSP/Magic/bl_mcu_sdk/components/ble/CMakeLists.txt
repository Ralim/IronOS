include(${CMAKE_CURRENT_SOURCE_DIR}/ble_flags.cmake)
################# Add global include #################
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/port/include")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/include")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/include/zephyr")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/include/misc")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/include/toolchain")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/include/tinycrypt")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/hci_onchip")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/bl_hci_wrapper")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/include/bluetooth")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/include/drivers/bluetooth")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/cli_cmds")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/blecontroller/ble_inc")

if(CONFIG_BT_OAD_SERVER OR CONFIG_BT_OAD_CLIENT)
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/oad")
endif()
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/port/bl_port.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/atomic_c.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/buf.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/log.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/poll.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/rpa.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/work_q.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/dec.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/utils.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/dummy.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/aes_decrypt.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/aes_encrypt.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/cbc_mode.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ccm_mode.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/cmac_mode.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ctr_mode.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ctr_prng.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ecc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ecc_dh.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ecc_dsa.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/ecc_platform_specific.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/hmac.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/hmac_prng.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/sha256.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/common/tinycrypt/source/utils.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/bl_hci_wrapper/bl_hci_wrapper.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/hci_onchip/hci_driver.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/att.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/conn.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/crypto.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/gatt.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/hci_core.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/hci_ecc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/l2cap.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/uuid.c")

if(NOT CONFIG_DISABLE_BT_SMP)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/smp.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/keys.c")
endif()

if(CONFIG_BT_DEBUG_MONITOR)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/monitor.c")
endif()

if(CONFIG_BT_OAD_CLIENT)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/oad/oad_client.c")
endif()

if(NOT CONFIG_DBG_RUN_ON_FPGA)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/settings.c")
endif()

if(CONFIG_BT_OAD_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/oad/oad_main.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/oad/oad_service.c")
endif()

if(CONFIG_BT_BAS_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/bas.c")
endif()

if(CONFIG_BT_SCPS_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/scps.c")
endif()

if(CONFIG_BT_DIS_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/bas.c")
endif()

if(CONFIG_BT_BAS_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/dis.c")
endif()

if(CONFIG_BLE_TP_SERVER)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/services/ble_tp_svc.c")
endif()

if(CONFIG_BLE_MULTI_ADV)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/multi_adv.c")
endif()

if(CONFIG_BT_BREDR)
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/a2dp.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/avdtp.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/keys_br.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/l2cap_br.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/host/sdp.c")

list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/alloc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/bitalloc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/bitalloc-sbc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/bitstream-decode.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/decoder-oina.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/decoder-private.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/decoder-sbc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/dequant.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/framing.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/framing-sbc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/oi_codec_version.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/synthesis-8-generated.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/synthesis-dct8.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/synthesis-sbc.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_analysis.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_dct.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_dct_coeffs.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec/sbc_enc_bit_alloc_mono.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_enc_bit_alloc_ste.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_enc_coeffs.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_encoder.c")
list(APPEND ADD_SRCS  "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc/sbc_packing.c")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/dec")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ble_stack/sbc/enc")
endif()


add_library(blecontroller STATIC IMPORTED)
set_target_properties(blecontroller PROPERTIES
IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/components/ble/blecontroller/lib/libblecontroller${LIBBLECONTROLLER_SUFFIX}.a"
INTERFACE_LINK_LIBRARIES  ${CHIP}_driver)

# aux_source_directory(src ADD_SRCS)
#######################################################

########### Add required/dependent components #########
list(APPEND ADD_REQUIREMENTS freertos blecontroller mbedtls)
#######################################################

############ Add static libs ##########################
list(APPEND ADD_STATIC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/bl702_rf/lib/libbl702_rf.a")
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
list(APPEND ADD_DEFINITIONS ${CFLAGS})
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
