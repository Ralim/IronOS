################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}"
"${CMAKE_CURRENT_SOURCE_DIR}/src/include/lwip"
"${CMAKE_CURRENT_SOURCE_DIR}/src/include/lwip/apps"
"${CMAKE_CURRENT_SOURCE_DIR}/src/include"
"${CMAKE_CURRENT_SOURCE_DIR}/system"
)
#######################################################
# list(REMOVE_ITEM ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/fsdata.h"
# )
################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources
"${CMAKE_CURRENT_SOURCE_DIR}/src/apps/lwiperf/lwiperf.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/fs.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/fsdata_custom.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/fsdata.c"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/httpd.c"

"${CMAKE_CURRENT_SOURCE_DIR}/src/api/api_lib.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/api_msg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/err.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/netbuf.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/netdb.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/netifapi.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/sockets.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/api/tcpip.c"

"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/autopip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/dhcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/etharp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/icmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/igmp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/ip4_addr.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/ip4_frag.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ipv4/ip4.c"

"${CMAKE_CURRENT_SOURCE_DIR}/src/core/def.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/dns.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/inet_chksum.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/init.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/ip.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/mem.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/memp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/netif.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/pbuf.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/raw.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/stats.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/sys.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/tcp_in.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/tcp_out.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/tcp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/timeouts.c"
"${CMAKE_CURRENT_SOURCE_DIR}/src/core/udp.c"

"${CMAKE_CURRENT_SOURCE_DIR}/src/netif/ethernet.c"

"${CMAKE_CURRENT_SOURCE_DIR}/system/os/sys_arch.c"
)
list(APPEND ADD_SRCS  ${sources})
# aux_source_directory(src ADD_SRCS)
# list(REMOVE_ITEM ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}")
#######################################################
# list(REMOVE_ITEM ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/apps/http/fsdata.c"
# )

########### Add required/dependent components #########
list(APPEND ADD_REQUIREMENTS freertos)
#######################################################

############ Add static libs ##########################
#list(APPEND ADD_STATIC_LIB "libxxx.a")
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
#list(APPEND ADD_DEFINITIONS -Dxxx=yyy)
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
