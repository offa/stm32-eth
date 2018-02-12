find_package(PkgConfig)
pkg_check_modules(PKG_CppUTest QUIET libCppUTest)
set(CppUTest_DEFINITIONS ${PKG_CppUTest_CFLAGS_OTHER})

find_path(CppUTest_INCLUDE_DIR "CppUTest/TestHarness.h"
                                HINTS ${PKG_CppUTest_INCLUDE_DIRS}
                                        "${CppUTest_DIR}/include"
                                        ENV CppUTest_INCLUDE_DIR
                                )

find_library(CppUTest_LIBRARY NAMES CppUTest CppUTest
                                HINTS ${PKG_CppUTest_LIBDIR}
                                        ${PKG_CppUTest_LIBRARY_DIRS}
                                        "${CppUTest_DIR}/lib"
                                        ENV CppUTest_LIBRARY
                                )
find_library(CppUTest_Ext_LIBRARY NAMES CppUTestExt CppUTestExt
                                HINTS ${PKG_CppUTest_LIBDIR}
                                        ${PKG_CppUTest_LIBRARY_DIRS}
                                        "${CppUTest_DIR}/lib"
                                        ENV CppUTest_Ext_LIBRARY
                                )


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CppUTest DEFAULT_MSG
                                        CppUTest_LIBRARY
                                        CppUTest_Ext_LIBRARY
                                        CppUTest_INCLUDE_DIR
                                        )

mark_as_advanced(CppUTest_INCLUDE_DIR
                CppUTest_LIBRARY
                CppUTest_Ext_LIBRARY
                )


add_library(CppUTest::CppUTest UNKNOWN IMPORTED)
set_target_properties(CppUTest::CppUTest PROPERTIES
                        IMPORTED_LOCATION "${CppUTest_LIBRARY}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES CXX
                        INTERFACE_INCLUDE_DIRECTORIES "${CppUTest_INCLUDE_DIR}"
                        )

add_library(CppUTest::Mock UNKNOWN IMPORTED)
set_target_properties(CppUTest::Mock PROPERTIES
                        IMPORTED_LOCATION "${CppUTest_Ext_LIBRARY}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES CXX
                        INTERFACE_INCLUDE_DIRECTORIES "${CppUTest_INCLUDE_DIR}"
                        )

