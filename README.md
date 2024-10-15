<img src="https://github.com/eliemichel/sdl3webgpu/actions/workflows/cmake.yml/badge.svg" alt="CMake Badge" />

<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/eliemichel/LearnWebGPU/main/images/webgpu-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/eliemichel/LearnWebGPU/main/images/webgpu-light.svg">
    <img alt="Learn WebGPU Logo" src="images/webgpu-dark.svg" width="200">
  </picture>

  <a href="https://github.com/eliemichel/LearnWebGPU">LearnWebGPU</a> &nbsp;|&nbsp; <a href="https://github.com/eliemichel/WebGPU-Cpp">WebGPU-C++</a> &nbsp;|&nbsp; <a href="https://github.com/eliemichel/WebGPU-distribution">WebGPU-distribution</a><br/>
  <a href="https://github.com/eliemichel/glfw3webgpu">glfw3webgpu</a> &nbsp;|&nbsp; <a href="https://github.com/eliemichel/sdl2webgpu">sdl2webgpu</a> &nbsp;|&nbsp; <a href="https://github.com/eliemichel/sdl3webgpu">sdl3webgpu</a>
  
  <a href="https://discord.gg/2Tar4Kt564"><img src="https://img.shields.io/static/v1?label=Discord&message=Join%20us!&color=blue&logo=discord&logoColor=white" alt="Discord | Join us!"/></a>
</div>


SDL3 WebGPU Extension
=====================

This is an extension for the great [SDL3](https://wiki.libsdl.org/SDL3/FrontPage) library for using it with **WebGPU native**. It was written as part of the [Learn WebGPU for native C++](https://eliemichel.github.io/LearnWebGPU) tutorial series.

Table of Contents
-----------------

 - [Overview](#overview)
 - [Usage](#usage)
 - [Example](#example)
 - [License](#license)

Overview
--------

This extension simply provides the following function:

```C
WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window* window);
```

Given an SDL window, `SDL_GetWGPUSurface` returns a WebGPU *surface* that corresponds to the window's backend. This is a process that is highly platform-specific, which is why I believe it belongs to SDL.

Usage
-----

Your project must link to an implementation of WebGPU (providing `webgpu.h`) and of course to SDL. Then:

**Option A** If you use CMake, you can simply include this project as a subdirectory with `add_subdirectory(sdl3webgpu)` (see the content of [`CMakeLists.txt`](CMakeLists.txt)).

**Option B** Just copy [`sdl3webgpu.h`](sdl3webgpu.h) and [`sdl3webgpu.c`](sdl3webgpu.c) to your project's source tree. On MacOS, you must add the compile option `-x objective-c` and the link libraries `-framework Cocoa`, `-framework CoreVideo`, `-framework IOKit`, and `-framework QuartzCore`. For iOS, do similarly but use `-framework UIKit` instead of `-framework Cocoa`.

Example
-------

Thanks to this extension it is possible to simply write a fully cross-platform WebGPU hello world:

```C
#define SDL_MAIN_HANDLED
#include "sdl3webgpu.h"
#include <SDL3/SDL.h>
#include <webgpu/webgpu.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	// Init WebGPU
	WGPUInstanceDescriptor desc;
	desc.nextInChain = NULL;
	WGPUInstance instance = wgpuCreateInstance(&desc);

	// Init SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Learn WebGPU", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	// Here we create our WebGPU surface from the window!
	WGPUSurface surface = SDL_GetWGPUSurface(instance, window);
	printf("surface = %p", surface);

	// Wait for close
	SDL_Event event;
	while (1)
		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				break;
	// Terminate SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
```

**NB** The linking process depends on the implementation of WebGPU that you are using. You can find detailed instructions for the `wgpu-native` implementation in [this Hello WebGPU chapter](https://eliemichel.github.io/LearnWebGPU/getting-started/hello-webgpu.html). You may also check out [`examples/CMakeLists.txt`](examples/CMakeLists.txt).

License
-------

See [LICENSE.txt](LICENSE.txt).
