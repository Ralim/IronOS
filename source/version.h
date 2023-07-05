/**
 * Firmware build version - format: xx.yy.zzzzzzzz
 * x: Major  -  y: Minor  -  z: git short hash generated automaticaly from git
 * i.e.: BUILD_VERSION = 'v2.08' --> Will generated to: 'v2.08.1a2b3c4d'
 *
 * vX.NM      - means stable release version with a related git release tag
 * vX.N(M+1)d - means a version under development with next rel.ver. X.N(M+1)
 * i.e.:
 *   BUILD_VERSION = 'v2.21'  --> Stable release
 *   BUILD_VERSION = 'v2.22d' --> Devel. for the next stable release of 2.22
 */

#define BUILD_VERSION "v2.22d"
