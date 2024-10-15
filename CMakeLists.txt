# This is only meant to be included as a subdirectory in another project.
# It assumes that targets 'SDL3' and 'webgpu' exist.
# Look at examples/CMakeLists.txt to see how to use it in a project.

# The sdl3webgpu target
add_library(sdl3webgpu STATIC sdl3webgpu.c)
target_include_directories(sdl3webgpu PUBLIC .)
target_link_libraries(sdl3webgpu PUBLIC SDL3::SDL3 webgpu)

if (APPLE)
    target_compile_options(sdl3webgpu PRIVATE -x objective-c)
    target_link_libraries(sdl3webgpu PRIVATE "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")

    if (IOS)
        target_link_libraries(sdl3webgpu PRIVATE "-framework UIKit")
    else()
        target_link_libraries(sdl3webgpu PRIVATE "-framework Cocoa")
    endif()
endif()
