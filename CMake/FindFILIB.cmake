# FindFILIB.cmake
#
# Finds the FILIB++ library
#
# This will define the following variables
#
#    FILIB_FOUND
#    FILIB_INCLUDE_DIRS
#
# and the following imported targets
#
#     FILIB::Filib++
#

find_package(PkgConfig)
pkg_check_modules(PC_FILIB QUIET FILIB)

find_path(FILIB_INCLUDE_DIR
    NAMES interval/interval.hpp interval/filib.hpp interval/interval_fo.hpp
    PATHS ${PC_FILIB_INCLUDE_DIRS} $ENV{FILIB_ROOT}/include $ENV{FILIB_ROOT}
)
find_library(FILIB_LIBRARY
    NAMES prim
    PATHS ${PC_FILIB_INCLUDE_DIRS} $ENV{FILIB_ROOT}/lib $ENV{FILIB_ROOT}/libprim/.libs
)

mark_as_advanced(FILIB_FOUND FILIB_INCLUDE_DIR FILIB_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FILIB
    FOUND_VAR
        FILIB_FOUND
    REQUIRED_VARS
        FILIB_INCLUDE_DIR
        FILIB_LIBRARY
    FAIL_MESSAGE "FILIB files where not found. Try setting the environment variable FILIB_ROOT."
)

if(FILIB_FOUND)
    set(FILIB_LIBRARIES ${FILIB_LIBRARY})
    set(FILIB_INCLUDE_DIRS ${FILIB_INCLUDE_DIR})
endif()

if(FILIB_FOUND AND NOT TARGET FILIB::filib++)
    add_library(FILIB::filib++ UNKNOWN IMPORTED)
    set_target_properties(FILIB::filib++ PROPERTIES
        IMPORTED_LOCATION "${FILIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FILIB_INCLUDE_DIR}"
    )
endif()

