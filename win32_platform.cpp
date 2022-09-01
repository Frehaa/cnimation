#include "renderer.h"

#include <windows.h>
#include <math.h>
#include <dsound.h>
#include <stdint.h>
#include <stdio.h>

#define PI 3.141592653589793238462643383279502884197169399375108209749445923078164062
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

void println(LPCSTR m) {
    OutputDebugStringA(m);
    OutputDebugStringA("\n");
}

#define BYTES_PER_PIXEL 4
struct PlatformWin32Bitmap {
    BITMAPINFO info;
    void *memory;
    int height;
    int width;
};

bool globalIsRunning = false;
PlatformWin32Bitmap globalBitmap;

typedef void RenderFn(Bitmap);

void render(Bitmap b) {
    HMODULE renderDll = LoadLibraryA("api_use_test.dll");
    if (!renderDll) {
        println("Failed to load render dll");
        return;
    }
    RenderFn *renderFn = (RenderFn *) GetProcAddress(renderDll, "render");
    if (!renderFn) {
        println("Failed to load procedure");
        return;
    }
    renderFn(b);
}

void resizeBitmapMemory(int width, int height) {
    if (globalBitmap.memory != nullptr) {
        VirtualFree(globalBitmap.memory, 0, MEM_RELEASE);
    }
    // Updates bitmap globals 
    globalBitmap.height = height;
    globalBitmap.width = width;
    globalBitmap.info.bmiHeader.biWidth = width;
    globalBitmap.info.bmiHeader.biHeight = -height;

    // Allocate memory
    int bitmapMemorySize = (width*height)*BYTES_PER_PIXEL;
    globalBitmap.memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

void updateBuffer(HDC deviceContext) {
    StretchDIBits(deviceContext,
        0, 0, globalBitmap.width, globalBitmap.height,
        0, 0, globalBitmap.width, globalBitmap.height,
        globalBitmap.memory,
        &globalBitmap.info,
        DIB_RGB_COLORS, SRCCOPY
    );    
}

void handleKeyInput(WPARAM wParam, LPARAM lParam) {
    uint32_t vkCode = wParam;
    bool wasDown = (lParam >> 30) & 0x01;
    switch (vkCode) {
        case VK_ESCAPE: {
            globalIsRunning = false;
        } break;
        default:
            break;
    }
}

LRESULT CALLBACK windowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (message) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP: {
            handleKeyInput(wParam, lParam);
        } break;
        case WM_DESTROY: 
        case WM_CLOSE: {
            globalIsRunning = false;
        } break;
        case WM_ACTIVATEAPP: {
            println("WM_ACTIVATE");
            SetCursor(NULL);
        } break;
        case WM_PAINT: {
            PAINTSTRUCT paint; 
            HDC deviceContext = BeginPaint(windowHandle, &paint);
            Bitmap bitmap;
            bitmap.memory = globalBitmap.memory;
            bitmap.height = globalBitmap.height;
            bitmap.width = globalBitmap.width;
            render(bitmap);
            updateBuffer(deviceContext);
            EndPaint(windowHandle, &paint);
        } break;
        default: {
            result = DefWindowProc(windowHandle, message, wParam, lParam);
        } break;
    }
    return result;
}

WNDCLASSA setupWindowClass(HINSTANCE instance) {
    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "rnime";
    return windowClass;
}

void setupBitmapGlobals() {
    globalBitmap.info.bmiHeader.biSize = sizeof(globalBitmap.info.bmiHeader);
    globalBitmap.info.bmiHeader.biPlanes = 1; // Must be set to 1 as per doc
    globalBitmap.info.bmiHeader.biBitCount = 8 * BYTES_PER_PIXEL;
    globalBitmap.info.bmiHeader.biCompression = BI_RGB; // Uncompressed format
}

HWND setupWindowHandle(WNDCLASSA windowClass, HINSTANCE instance) {
    return CreateWindowExA(
        0,
        windowClass.lpszClassName,
        "rnime",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );
}


int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode){
    setupBitmapGlobals();
    resizeBitmapMemory(720, 480);

    WNDCLASSA windowClass = setupWindowClass(instance);
    if (!RegisterClassA(&windowClass)) { // Error 
        println("Register Class failed");
        return 0;
    }

    HWND windowHandle = setupWindowHandle(windowClass, instance);
    if (!windowHandle) { // Error 
        println("Create Window failed");
        return 0;
    }

    Bitmap bitmap;
    bitmap.memory = globalBitmap.memory;
    bitmap.height = globalBitmap.height;
    bitmap.width = globalBitmap.width;
    render(bitmap);

    HDC deviceContext = GetDC(windowHandle);
    updateBuffer(deviceContext);
    ReleaseDC(windowHandle, deviceContext);

    // Main Loop
    globalIsRunning = true;
    while (globalIsRunning) {
        MSG message;
        while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE) != 0) {
            if (message.message == WM_QUIT) {
                globalIsRunning = false;
            }
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        sleep(50);
    }

    return 0;
}