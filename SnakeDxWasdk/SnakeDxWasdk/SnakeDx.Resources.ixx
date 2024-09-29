module;
#include "pch.h"
export module SnakeDx:Resources;

import :Token;
import std;

namespace SnakeDx {

export class Resources {

    // Device resources
    D3D_FEATURE_LEVEL m_featureLevel {};
    winrt::com_ptr<ID3D11Device5> m_d3dDevice;
    winrt::com_ptr<ID3D11DeviceContext4> m_d3dContext;
    winrt::com_ptr<IDXGIDevice4> m_dxgiDevice;

    // Swapchain resources
    winrt::com_ptr<IDXGISwapChain4> m_swapChain;
    winrt::com_ptr<ID3D11RenderTargetView> m_renderTargetView;

public:
    // Settings
    UINT swapInterval = 0;
    std::array<float, 4> m_clearColor { .2f, .1f, .4f, 1.f };

    Resources(Resources const&) = delete;
    Resources& operator=(Resources const&) = delete;

public:
    Resources(Token);
    bool Ready() const { return m_swapChain && m_renderTargetView; }

    auto& D3DDevice() { return *m_d3dDevice; }
    auto& D3DContext() { return *m_d3dContext; }

    void SetSwapChainPanel(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel panel, winrt::Windows::Foundation::Size newSize);
    void ResetSwapChainPanel()
    {
        m_renderTargetView.put();
        m_swapChain.put();
    }

    auto& DrawStart()
    {
        std::array views { m_renderTargetView.get() };
        m_d3dContext->OMSetRenderTargets(1, views.data(), nullptr);
        m_d3dContext->ClearRenderTargetView(m_renderTargetView.get(), m_clearColor.data());
        return *m_d3dContext;
    }

    void DrawEnd()
    {
        winrt::check_hresult(m_swapChain->Present(swapInterval, {}));
    }
};
}