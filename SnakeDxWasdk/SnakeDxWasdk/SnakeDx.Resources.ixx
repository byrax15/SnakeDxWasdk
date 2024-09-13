module;
#include "pch.h"
export module SnakeDx:Resources;

import :Token;

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
    std::array<float, 4> m_clearColor { .2f, .1f, .4f, 1.f };

    friend void swap(Resources& l, Resources& r) noexcept
    {
        std::swap(l.m_featureLevel, r.m_featureLevel);
        std::swap(l.m_d3dDevice, r.m_d3dDevice);
        std::swap(l.m_d3dContext, r.m_d3dContext);
        std::swap(l.m_dxgiDevice, r.m_dxgiDevice);
        // std::swap(l.m_trianglePass, r.m_trianglePass);
        std::swap(l.m_swapChain, r.m_swapChain);
        std::swap(l.m_renderTargetView, r.m_renderTargetView);
    }

    Resources(Resources const&) = delete;
    Resources& operator=(Resources const&) = delete;

public:
    Resources(Token);

    void SetSwapChainPanel(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel panel, winrt::Windows::Foundation::Size newSize);

    bool Ready() const
    {
        return !!m_swapChain;
    }

    void Draw()
    {
        if (!m_swapChain)
            return;

        std::array views { m_renderTargetView.get() };
        m_d3dContext->OMSetRenderTargets(1, views.data(), nullptr);

        m_d3dContext->ClearRenderTargetView(m_renderTargetView.get(), m_clearColor.data());

        // m_trianglePass.Draw(*m_d3dContext);

        winrt::check_hresult(
            m_swapChain->Present(1, {}));
    }
};
}