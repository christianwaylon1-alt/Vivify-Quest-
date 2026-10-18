cmake_minimum_required(VERSION 3.22)

# Pulls in MOD_ID, COMPILE_ID, the NDK toolchain and the RECURSE_FILES macro.
include(qpm_defines.cmake)

project(${COMPILE_ID} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect every source/header so new files are picked up automatically.
RECURSE_FILES(cpp_files src/*.cpp)
RECURSE_FILES(hpp_files include/*.hpp)

add_library(${COMPILE_ID} SHARED ${cpp_files} ${hpp_files})

target_include_directories(${COMPILE_ID} PRIVATE include)

target_compile_options(${COMPILE_ID} PRIVATE
    -frtti
    -fexceptions
    -O3
    -fno-omit-frame-pointer
    -Wno-unused-parameter
    -Wno-unused-variable
)

# Runtime metadata baked into the binary. Keep MOD_ID in sync with qpm.json.
add_compile_definitions(VERSION="${MOD_VERSION}")
add_compile_definitions(MOD_ID="qivify")

# Adds all QPM dependency include dirs, compile options and link libraries.
include(extern.cmake)

# BSML ships its headers under extern/includes/bsml/shared and uses absolute
# includes internally (e.g. "BSML/BSML.hpp"), so that directory must be on the
# include path. QPM does not add it automatically.
if (EXISTS ${EXTERN_DIR}/includes/bsml/shared)
    target_include_directories(${COMPILE_ID} PRIVATE ${EXTERN_DIR}/includes/bsml/shared)
endif()

# beatsaber-hook's inline hooking (used by MAKE_HOOK_MATCH) needs its
# And64InlineHook/relocate sources compiled into the mod, otherwise the linker
# reports an undefined reference to A64HookFunction.
RECURSE_FILES(inline_hook_c ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
RECURSE_FILES(inline_hook_cpp ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)
if (inline_hook_c OR inline_hook_cpp)
    target_sources(${COMPILE_ID} PRIVATE ${inline_hook_c} ${inline_hook_cpp})
    target_include_directories(${COMPILE_ID} PRIVATE ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook)
endif()

# The inline hook uses __android_log_print from the NDK's liblog.
find_library(LOG_LIB log)
if (LOG_LIB)
    target_link_libraries(${COMPILE_ID} PRIVATE ${LOG_LIB})
else()
    target_link_libraries(${COMPILE_ID} PRIVATE log)
endif()

# Output libqivify.so (matches overrideSoName in qpm.json).
set_target_properties(${COMPILE_ID} PROPERTIES PREFIX "lib" OUTPUT_NAME "${COMPILE_ID}")

# Strip debug symbols from the shipped library to keep the qmod small.
if (CMAKE_STRIP)
    add_custom_command(TARGET ${COMPILE_ID} POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${COMPILE_ID}>
        COMMENT "Stripped $<TARGET_FILE_NAME:${COMPILE_ID}>"
    )
endif()
