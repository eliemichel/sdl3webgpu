/**
 * This is an extension of SDL3 for WebGPU, abstracting away the details of
 * OS-specific operations.
 * 
 * This file is part of the "Learn WebGPU for C++" book.
 *   https://eliemichel.github.io/LearnWebGPU
 * 
 * Most of this code comes from the wgpu-native triangle example:
 *   https://github.com/gfx-rs/wgpu-native/blob/master/examples/triangle/main.c
 * 
 * MIT License
 * Copyright (c) 2022-2024 Elie Michel and the wgpu-native authors
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "sdl3webgpu.h"

#include <webgpu/webgpu.h>

#if defined(SDL_PLATFORM_MACOS)
#  include <Cocoa/Cocoa.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#elif defined(SDL_PLATFORM_IOS)
#  include <UIKit/UIKit.h>
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#  include <Metal/Metal.h>
#elif defined(SDL_PLATFORM_WIN32)
#  include <windows.h>
#elif defined(SDL_PLATFORM_LINUX)
#  include <X11/Xlib.h>
#  include <wayland-client-core.h>
#endif

#include <SDL3/SDL.h>

WGPUSurface SDL_GetWGPUSurface(WGPUInstance instance, SDL_Window* window) {
    SDL_PropertiesID props = SDL_GetWindowProperties(window);

#if defined(SDL_PLATFORM_MACOS)
    {
        id metal_layer = NULL;
        NSWindow *ns_window = (__bridge NSWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        if (!ns_window) return NULL;
        [ns_window.contentView setWantsLayer : YES];
        metal_layer = [CAMetalLayer layer];
        [ns_window.contentView setLayer : metal_layer];

        WGPUSurfaceSourceMetalLayer fromMetalLayer;
        fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
        fromMetalLayer.chain.next = NULL;
        fromMetalLayer.layer = metal_layer;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
        surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }
#elif defined(SDL_PLATFORM_IOS)
    {
        UIWindow *ui_window = (__bridge UIWindow *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
        if (!uiwindow) return NULL;

        UIView* ui_view = ui_window.rootViewController.view;
        CAMetalLayer* metal_layer = [CAMetalLayer new];
        metal_layer.opaque = true;
        metal_layer.frame = ui_view.frame;
        metal_layer.drawableSize = ui_view.frame.size;

        [ui_view.layer addSublayer: metal_layer];

        WGPUSurfaceSourceMetalLayer fromMetalLayer;
        fromMetalLayer.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
        fromMetalLayer.chain.next = NULL;
        fromMetalLayer.layer = metal_layer;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromMetalLayer.chain;
        surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }
#elif defined(SDL_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        Display *x11_display = (Display *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        Window x11_window = (Window)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (!x11_display || !x11_window) return NULL;

        WGPUSurfaceSourceXlibWindow fromXlibWindow;
        fromXlibWindow.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
        fromXlibWindow.chain.next = NULL;
        fromXlibWindow.display = x11_display;
        fromXlibWindow.window = x11_window;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromXlibWindow.chain;
        surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }
    else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        struct wl_display *wayland_display = (struct wl_display *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        struct wl_surface *wayland_surface = (struct wl_surface *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        if (!wayland_display || !wayland_surface) return NULL;

        WGPUSurfaceSourceWaylandSurface fromWaylandSurface;
        fromWaylandSurface.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
        fromWaylandSurface.chain.next = NULL;
        fromWaylandSurface.display = wayland_display;
        fromWaylandSurface.surface = wayland_surface;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromWaylandSurface.chain;
        surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    } else return NULL;
#elif defined(SDL_PLATFORM_WIN32)
    {
        HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (!hwnd) return NULL;
        HINSTANCE hinstance = GetModuleHandle(NULL);

        WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
        fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
        fromWindowsHWND.chain.next = NULL;
        fromWindowsHWND.hinstance = hinstance;
        fromWindowsHWND.hwnd = hwnd;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
        surfaceDescriptor.label = (WGPUStringView){ NULL, WGPU_STRLEN };

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }
#elif defined(__EMSCRIPTEN__)
    {
        WGPUSurfaceDescriptorFromCanvasHTMLSelector fromCanvasHTMLSelector;
        fromCanvasHTMLSelector.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
        fromCanvasHTMLSelector.chain.next = NULL;
        fromCanvasHTMLSelector.selector = "canvas";

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromCanvasHTMLSelector.chain;
        surfaceDescriptor.label = NULL;

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
    }  
#else
    // TODO: See SDL_syswm.h for other possible enum values!
#error "Unsupported WGPU_TARGET"
#endif
}

