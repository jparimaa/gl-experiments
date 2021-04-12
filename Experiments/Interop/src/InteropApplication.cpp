#include "InteropApplication.h"

#include <fw/Framework.h>
#include <fw/Input.h>
#include <fw/Common.h>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <iostream>
#include <cmath>

namespace
{
const int c_textureSize = 256;

#define WGL_ACCESS_READ_WRITE_NV 0x0001
#define WGL_ACCESS_WRITE_DISCARD_NV 0x0002

// clang-format off
HANDLE(WINAPI* wglDXOpenDeviceNV)(void* dxDevice) = nullptr;
BOOL(WINAPI* wglDXCloseDeviceNV)(HANDLE hDevice) = nullptr;
HANDLE(WINAPI* wglDXRegisterObjectNV)(HANDLE hDevice, void* dxObject, GLuint name, GLenum type, GLenum access) = nullptr;
BOOL(WINAPI* wglDXUnregisterObjectNV)(HANDLE hDevice, HANDLE hObject) = nullptr;
BOOL(WINAPI* wglDXLockObjectsNV)(HANDLE hDevice, GLint count, HANDLE* hObjects) = nullptr;
BOOL(WINAPI* wglDXUnlockObjectsNV)(HANDLE hDevice, GLint count, HANDLE* hObjects) = nullptr;
// clang-format on
} // namespace

InteropApplication::InteropApplication()
{
}

InteropApplication::~InteropApplication()
{
    if (wglDXUnregisterObjectNV(m_interopDeviceHandle, m_interopTextureHandle) == FALSE)
    {
        abort();
    }

    glDeleteTextures(1, &m_interopTexture);

    if (wglDXCloseDeviceNV && m_interopDeviceHandle)
    {
        wglDXCloseDeviceNV(m_interopDeviceHandle);
    }

    glDeleteTextures(1, &m_texture);
}

bool InteropApplication::initialize()
{
    setupGLState();
    createTexture();
    createFullscreenShader();
    createDxDevice();
    setupInterop();
    createDxTexture();
    registerInteropTexture();

    return true;
}

void InteropApplication::update()
{
}

void InteropApplication::render()
{
    {
        m_clearColorSeed += 0.01f;
        const float color[] = {0.0f, 0.7f * cos(m_clearColorSeed), 0.7f * abs(sin(m_clearColorSeed)), 1.0f};
        m_deviceContext->OMSetRenderTargets(1, m_rtv.GetAddressOf(), nullptr);
        m_deviceContext->ClearRenderTargetView(m_rtv.Get(), color);
    }

    {
        if (wglDXLockObjectsNV(m_interopDeviceHandle, 1, &m_interopTextureHandle) == FALSE)
        {
            abort();
        }

        glCopyImageSubData(m_interopTexture, GL_TEXTURE_2D, 0, 0, 0, 0, m_texture, GL_TEXTURE_2D, 0, 0, 0, 0, c_textureSize, c_textureSize, 1);

        if (wglDXUnlockObjectsNV(m_interopDeviceHandle, 1, &m_interopTextureHandle) == FALSE)
        {
            abort();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_fullscreenShader.getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void InteropApplication::gui()
{
}

void InteropApplication::setupGLState()
{
    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

void InteropApplication::createTexture()
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, c_textureSize, c_textureSize);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const int totalSize = c_textureSize * c_textureSize * 4;
    std::vector<uint8_t> data(totalSize);
    for (size_t i = 0; i < totalSize; i += 4)
    {
        data[i + 0] = i % 255;
        data[i + 1] = 255 - (i % 255);
        data[i + 2] = 128 + (i % 127);
        data[i + 3] = 255;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c_textureSize, c_textureSize, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

void InteropApplication::createFullscreenShader()
{
    std::string path = std::string(ROOT_PATH) + "Experiments/Interop/shaders/fullscreen";
    bool status = m_fullscreenShader.createProgram({path + ".vert", path + ".frag"});
    assert(status);
}

void InteropApplication::createDxDevice()
{
    HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &m_device, nullptr, &m_deviceContext);
    if (FAILED(result))
    {
        std::cerr << "ERROR: Failed to create D3D11 device\n";
        abort();
    }
}

void InteropApplication::setupInterop()
{
    *(PROC*)&wglDXOpenDeviceNV = wglGetProcAddress("wglDXOpenDeviceNV");
    *(PROC*)&wglDXCloseDeviceNV = wglGetProcAddress("wglDXCloseDeviceNV");
    *(PROC*)&wglDXRegisterObjectNV = wglGetProcAddress("wglDXRegisterObjectNV");
    *(PROC*)&wglDXUnregisterObjectNV = wglGetProcAddress("wglDXUnregisterObjectNV");
    *(PROC*)&wglDXLockObjectsNV = wglGetProcAddress("wglDXLockObjectsNV");
    *(PROC*)&wglDXUnlockObjectsNV = wglGetProcAddress("wglDXUnlockObjectsNV");

    if (!wglDXOpenDeviceNV || !wglDXCloseDeviceNV || !wglDXRegisterObjectNV || !wglDXUnregisterObjectNV || !wglDXLockObjectsNV || !wglDXUnlockObjectsNV)
    {
        abort();
    }

    m_interopDeviceHandle = wglDXOpenDeviceNV(m_device.Get());
    if (m_interopDeviceHandle == nullptr)
    {
        abort();
    }
}

void InteropApplication::createDxTexture()
{
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = c_textureSize;
    texDesc.Height = c_textureSize;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    HRESULT result = m_device->CreateTexture2D(&texDesc, nullptr, m_dxTexture.GetAddressOf());
    if (FAILED(result))
    {
        abort();
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    result = m_device->CreateRenderTargetView(m_dxTexture.Get(), &rtvDesc, m_rtv.GetAddressOf());
    if (FAILED(result))
    {
        abort();
    }
}

void InteropApplication::registerInteropTexture()
{
    glGenTextures(1, &m_interopTexture);

    m_interopTextureHandle = wglDXRegisterObjectNV(m_interopDeviceHandle, m_dxTexture.Get(), m_interopTexture, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
    if (m_interopTextureHandle == nullptr)
    {
        abort();
    }
}
