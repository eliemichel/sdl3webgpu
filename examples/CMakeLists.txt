# This example comes from https://eliemichel.github.io/LearnWebGPU
cmake_minimum_required(VERSION 3.11.0...3.25.0)
project(sdl3webgpu-examples VERSION 1.0.0 LANGUAGES C)

include(FetchContent)
include(utils.cmake)

# Dependencies
if (EMSCRIPTEN)

  add_library(SDL3_SDL3 INTERFACE)
  target_compile_options(SDL3_SDL3 INTERFACE --use-port=sdl3) # NB: This port does not exist yet, emscripten is not supported
  target_link_options(SDL3_SDL3 INTERFACE --use-port=sdl3) # NB: This port does not exist yet, emscripten is not supported
  add_library(SDL3::SDL3 ALIAS SDL3_SDL3)
  function(target_copy_sdl3_binaries)
  endfunction()

  add_library(webgpu INTERFACE)
  target_link_options(webgpu INTERFACE -sUSE_WEBGPU=1)
  function(target_copy_webgpu_binaries)
  endfunction()

else()

  FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        preview-3.1.3
  )
  FetchContent_Declare(
    webgpu
    GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution.git
    GIT_TAG        479541d24699fddb60a903da081203bdbe4735a7  # fetch
  )
  FetchContent_MakeAvailable(sdl3 webgpu)

  function(target_copy_sdl3_binaries Target)
    add_custom_command(
      TARGET ${Target}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND}
        -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:${Target}>
    )
  endfunction()
  
endif()

# The sdl3webgpu target
# NB: We specify a second argument only because this is an out-of-tree
# directory, no need to do this in your case.
add_subdirectory(.. sdl3webgpu)

# Example
add_executable(hello-sdl3webgpu hello-sdl3webgpu.c)
target_link_libraries(hello-sdl3webgpu PRIVATE SDL3::SDL3 webgpu sdl3webgpu)
set_target_properties(hello-sdl3webgpu PROPERTIES CXX_STANDARD 17)
target_treat_all_warnings_as_errors(hello-sdl3webgpu)

# Copy wgpu-native or Dawn dll/so/dylib next to the executable
target_copy_webgpu_binaries(hello-sdl3webgpu)
# Copy SDL3.dll/so/dylib next to the executable
target_copy_sdl3_binaries(hello-sdl3webgpu)

