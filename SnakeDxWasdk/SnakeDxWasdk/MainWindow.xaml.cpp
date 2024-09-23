#include "pch.h"

#include "MainWindow.xaml.h"

#include "MainWindow.g.cpp"

import SnakeDx;
import std;
import <chrono>;

using namespace std::literals;
using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnakeDxWasdk::implementation {
MainWindow::MainWindow()
    : frameTimeListener([&, uiThread = winrt::apartment_context {}]() -> winrt::fire_and_forget {
        co_await uiThread;
        frameTime().Text(winrt::to_hstring(SnakeDx::scheduler.frameTime.count()));
    })
{
    auto [m, dl] = SnakeDx::scheduler.deltaListeners.ToRef();
    std::scoped_lock lock(m);
    dl.emplace_back(&frameTimeListener);
}

void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
{
    static auto start_value = myButton().Content();
    static auto hello = box_value(L"Your mom");
    static auto toggle = false;
    if (toggle)
        myButton().Content(hello);
    else
        myButton().Content(start_value);
    toggle = !toggle;
}

void MainWindow::swapChainPanel_SizeChanged(IInspectable const&, SizeChangedEventArgs const& e)
{
    auto [m, r] = SnakeDx::scheduler.resources.ToRef();
    std::scoped_lock lock(m);
    r.SetSwapChainPanel(swapChainPanel(), e.NewSize());
}

void MainWindow::swapChainPanel_Unloaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    {
        auto [m, r] = SnakeDx::scheduler.resources.ToRef();
        std::scoped_lock lock(m);
        r.ResetSwapChainPanel();
    }
    {
        frameTimeListener = []() -> winrt::fire_and_forget { co_return; };
        auto [m, dl] = SnakeDx::scheduler.deltaListeners.ToRef();
        std::scoped_lock lock(m);
        dl.remove(&frameTimeListener);
    };
}
}
