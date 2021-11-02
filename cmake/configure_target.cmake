# vm-tutorial - Virtual machine tutorial targeting CHIP-8
#
# Written in 2021 by Michael Rodriguez aka kaichiuchu <mike@kaichiuchu.dev>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.

function(vmtutorial_configure_target TARGET_NAME)
  # Throughout this tutorial, the C++17 standard will be required with the
  # possibility of using extensions.
  set_target_properties(${TARGET_NAME} PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS ON)

  # We do not support exceptions or RTTI throughout the tutorial, so disable
  # them for the compilers we support.
  #
  # An exception to this rule is with clang if the frontend is clang-cl;
  # neither exceptions nor RTTI can be disabled if this is the case as the
  # following flags are not recognized, we'll just have to suck it up.
  #
  # g++ handles these flags just fine on every platform that I'm aware of.
  #
  # These flags are set regardless of the build type specified.
  if ((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND
       CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU") OR
       CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(MAIN_COMPILER_FLAGS
        "-fno-exceptions;-fno-rtti;-fno-unwind-tables;-fno-asynchronous-unwind-tables")
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # I would've put -Werror here, but there are too many warnings from
    # dependencies and warnings within our own code that our mostly
    # informational; to selectively disable them all would be quite annoying
    # and the amount of them would be... immense.
    #
    # We'll just have to remain vigilant.
      set(ADDITIONAL_COMPILER_FLAGS
          "-Wall;-Wextra;-Wno-c++98-compat;-Wno-c++98-compat-pedantic")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      set(ADDITIONAL_COMPILER_FLAGS "-Wall;-Werror;-Wextra")
    endif()
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL "Release" OR "RelWithDebInfo" OR "MinSizeRel")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)

    if (ipo_supported)
      set_target_properties(${TARGET_NAME} PROPERTIES
                            INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
  endif()

  target_compile_options(${TARGET_NAME} PRIVATE ${MAIN_COMPILER_FLAGS}
                                                ${ADDITIONAL_COMPILER_FLAGS})
endfunction()