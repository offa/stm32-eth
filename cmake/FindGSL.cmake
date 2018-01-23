find_package(PkgConfig)
pkg_check_modules(PKG GSL QUIET GSL)
set(GSL_DEFINITIONS ${PKG_GSL_CFLAGS_OTHER})

find_path(GSL_INCLUDE_DIR "gsl/gsl"
                            HINTS ${PKG_GSL_INCLUDE_DIRS}
                                    ${GSL_DIR}
                                    ${GSL_DIR}/include
                            ENV GSL_INCLUDE_DIR
                            )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GSL DEFAULT_MSG
                                    GSL_INCLUDE_DIR
                                    )

mark_as_advanced(GSL_INCLUDE_DIR)
