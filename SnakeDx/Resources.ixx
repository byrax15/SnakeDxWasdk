module;
#define WINRT_LEAN_AND_MEAN
#include <winrt/base.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include "dx11.h"
export module Resources;


export namespace SnakeDx {
inline winrt::hstring hello()
{
    return winrt::hstring(L"Hello from SnakeDX!");
}

class Resources {
    winrt::com_ptr<ID3D11Device5> device;
    winrt::com_ptr<ID3D11DeviceContext4> context;
    winrt::com_ptr<IDXGISwapChain4> swapchain;
    winrt::com_ptr<ID3D11RenderTargetView1> swapRtv;

    Resources(winrt::Windows::UI::Xaml::Controls::SwapChainPanel panel) { }
};
}