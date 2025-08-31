/**
 * Firmware build version - format: xx.yy+[.zzzzzzzz]
 *
 * x: major version
 * y: minor version
 * +: build type:
 * * R - git-related release tag vXX.YY
 * * T - git-related release tag but version is not vXX.YY !
 * * D - git-related dev branch
 * * B - git-related custom branch
 * * G - neither above but git-related
 * * H - build outside of a git tree (i.e. release tarball)
 * * S - something special (should not happen?)
 * * V - something very special (should not happen!)
 * z: short commit ID hash generated automaticaly from git
 * * (for git-related build types only)
 *
 * i.e.:
 * * BUILD_VERSION = 'v2.22' -> from tarball:            'v2.22H'
 * * BUILD_VERSION = 'v2.22' -> from git dev branch:     'v2.22D.1A2B3C4D'
 * * BUILD_VERSION = 'v2.22' -> from stable git release: 'v2.22R.5E6F7G8H'
 */

#define BUILD_VERSION "v2.23"
