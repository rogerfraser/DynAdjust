# Static build optimizations
# This file contains common optimizations for static builds

function(optimize_static_target TARGET_NAME)
    # Enable LTO for this specific target if supported
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
    
    # Platform-specific optimizations
    if(UNIX AND NOT APPLE)
        # Linux-specific optimizations
        target_link_options(${TARGET_NAME} PRIVATE
            -static
            -Wl,--gc-sections      # Remove unused sections
            -Wl,--as-needed         # Only link libraries that are actually used
            -Wl,-O2                 # Optimize at link time
            -Wl,--strip-all         # Strip all symbols
        )
        target_compile_options(${TARGET_NAME} PRIVATE
            -ffunction-sections     # Put each function in its own section
            -fdata-sections         # Put each data item in its own section
        )
    elseif(APPLE)
        # macOS-specific optimizations
        target_link_options(${TARGET_NAME} PRIVATE
            -Wl,-dead_strip         # Remove unused code
            -Wl,-dead_strip_dylibs  # Remove unused dylibs
        )
    elseif(WIN32 AND MSVC)
        # Windows MSVC-specific optimizations
        target_compile_options(${TARGET_NAME} PRIVATE
            /Gy                     # Enable function-level linking
        )
        target_link_options(${TARGET_NAME} PRIVATE
            /OPT:REF                # Remove unreferenced code
            /OPT:ICF                # Identical COMDAT folding
        )
    endif()
endfunction()