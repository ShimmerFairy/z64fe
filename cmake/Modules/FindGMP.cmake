# attempting to find gmp, after trying these variables will be set:
#
#     GMP_FOUND
#     GMP_INCLUDE_DIRS
#     GMP_LIBRARIES
#     GMP_DEFINITIONS

find_package(PkgConfig)
pkg_check_modules(PC_GMP QUIET libgmp)
pkg_check_modules(PC_GMPXX QUIET libgmpxx)
set(GMP_DEFINITIONS ${PC_GMP_CLFAGS_OTHER} ${PC_GMPXX_CFLAGS_OTHER})

find_path(GMP_INCLUDE_DIR gmp.h
          HINTS ${PC_GMP_INCLUDEDIR} ${PC_GMP_INCLUDE_DIRS})
find_path(GMPXX_INCLUDE_DIR gmpxx.h
          HINTS ${PC_GMPXX_INCLUDEDIR} ${PC_GMPXX_INCLUDE_DIRS})

find_library(GMP_LIBRARY gmp libgmp
             HINTS ${PC_GMP_LIBDIR} ${PC_GMP_LIBRARY_DIRS})
find_library(GMPXX_LIBRARY gmpxx libgmpxx
             HINTS ${PC_GMPXX_LIBDIR} ${PC_GMPXX_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG
                                  GMP_INCLUDE_DIR GMP_LIBRARY
                                  GMPXX_INCLUDE_DIR GMPXX_LIBRARY)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY GMPXX_INCLUDE_DIR GMPXX_LIBRARY)

set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR} ${GMPXX_INCLUDE_DIR})
set(GMP_LIBRARIES ${GMP_LIBRARY} ${GMPXX_LIBRARY})