#pragma once

#include <fw/Application.h>
#include <fw/Camera.h>
#include <fw/CameraController.h>
#include <fw/Shader.h>
#include <fw/Model.h>
#include <fw/Image.h>

#include <d3d11.h>
#include <wrl/client.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>

class InteropApplication : public fw::Application
{
public:
    explicit InteropApplication();
    virtual ~InteropApplication();

    virtual bool initialize() final;
    virtual void update() final;
    virtual void render() final;
    virtual void gui() final;

private:
    fw::Shader m_fullscreenShader;

    GLuint m_texture = 0;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;

    HANDLE m_interopDeviceHandle;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_dxTexture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv = nullptr;
    GLuint m_interopTexture = 0;
    HANDLE m_interopTextureHandle = nullptr;

    float m_clearColorSeed = 0.0f;

    void setupGLState();
    void createTexture();
    void createFullscreenShader();
    void createDxDevice();
    void setupInterop();
    void createDxTexture();
    void registerInteropTexture();
};
