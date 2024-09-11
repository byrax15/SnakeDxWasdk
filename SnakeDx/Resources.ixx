module;
#define WINRT_LEAN_AND_MEAN
#include <winrt/base.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

#include "dx11.h"
export module Resources;

import <mutex>;
import <thread>;

namespace {
struct token_t {
} token;
}

export namespace SnakeDx {

inline winrt::hstring hello()
{
    return winrt::hstring(L"Hello from SnakeDX!");
}

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

    Resources(token_t)
    {
        CreateDeviceResources();
        // m_trianglePass = { *m_d3dDevice, L"Triangle.vs.cso", L"Triangle.ps.cso" };
    }

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

    Resources(Resources&& o) noexcept
    {
        swap(*this, o);
    }

    Resources& operator=(Resources&& o) noexcept
    {
        if (this != &o)
            swap(*this, o);
        return *this;
    }

    void SetSwapChainPanel(winrt::Windows::UI::Xaml::Controls::SwapChainPanel panel, winrt::Windows::Foundation::Size newSize);

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

private:
    void CreateDeviceResources();
};

export template <typename T>
class Synchronized {
    std::mutex mutex;
    T data;

public:
    template <typename... TArgs>
    Synchronized(TArgs&&... args)
        : data(std::forward<TArgs>(args)...)
    {
    }

    void WithLock(std::invocable<T&> auto&& callable)
    {
        std::scoped_lock l { mutex };
        callable(data);
    }
};

export Synchronized<Resources> singleton(token);

}
