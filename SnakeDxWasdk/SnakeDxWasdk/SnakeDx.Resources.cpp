module;
#include "pch.h"
module SnakeDx:Resources;

SnakeDx::Resources::Resources(SnakeDx::Token)
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT const creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT
#if defined(_DEBUG)
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        | D3D11_CREATE_DEVICE_DEBUG
#endif
        ;

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] {
        D3D_FEATURE_LEVEL_11_1,
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    winrt::check_hresult(D3D11CreateDevice(
        nullptr, // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE, // Create a device using the hardware graphics driver.
        0, // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags, // Set debug and Direct2D compatibility flags.
        featureLevels, // List of feature levels this app can support.
        ARRAYSIZE(featureLevels), // Size of the list above.
        D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        device.put(), // Returns the Direct3D device created.
        &m_featureLevel, // Returns feature level of device created.
        context.put() // Returns the device immediate context.
        ));
    device.as(m_d3dDevice);
    device.as(m_dxgiDevice);
    context.as(m_d3dContext);
    // m_trianglePass = { *m_d3dDevice, L"Triangle.vs.cso", L"Triangle.ps.cso" };
}

void SnakeDx::Resources::SetSwapChainPanel(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel panel, winrt::Windows::Foundation::Size newSize)
{
    DXGI_SWAP_CHAIN_DESC1 constexpr DESC {
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
        .SampleDesc = { .Count = 1, .Quality = 0 },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
        .AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED,
    };
    if (m_swapChain) {
        m_d3dContext->ClearState();
        m_renderTargetView.put();
        const auto [w, h] = newSize;
        winrt::check_hresult(m_swapChain->ResizeBuffers(DESC.BufferCount, w, h, DESC.Format, DESC.Flags));
    } else {
        winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
        winrt::check_hresult(
            m_dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)));

        winrt::com_ptr<IDXGIFactory2> dxgiFactory;
        winrt::check_hresult(
            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

        const auto swapDesc = std::invoke([&] {
            auto desc { DESC };
            desc.Width = static_cast<UINT>(newSize.Width);
            desc.Height = static_cast<UINT>(newSize.Height);
            return desc;
        });
        winrt::com_ptr<IDXGISwapChain1> swap;
        winrt::check_hresult(
            dxgiFactory->CreateSwapChainForComposition(
                m_dxgiDevice.get(),
                &swapDesc,
                nullptr,
                swap.put()));
        swap.as(m_swapChain);

        winrt::com_ptr<ISwapChainPanelNative> nativePanel;
        winrt::check_hresult(
            panel.as<IUnknown>()->QueryInterface(nativePanel.put()));
        nativePanel->SetSwapChain(m_swapChain.get());
        winrt::check_hresult(m_dxgiDevice->SetMaximumFrameLatency(1));
    }

    winrt::com_ptr<ID3D11Texture2D> backBuffer;
    winrt::check_hresult(
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
    winrt::check_hresult(
        m_d3dDevice->CreateRenderTargetView(backBuffer.get(), nullptr, m_renderTargetView.put()));

    D3D11_VIEWPORT const viewport {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = static_cast<FLOAT>(newSize.Width),
        .Height = static_cast<FLOAT>(newSize.Height),
        .MinDepth = 0.f,
        .MaxDepth = 1.f,
    };
    m_d3dContext->RSSetViewports(1, &viewport);
}
