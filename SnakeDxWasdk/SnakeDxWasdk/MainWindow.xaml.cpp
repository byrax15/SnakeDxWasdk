#include "pch.h"

#include "MainWindow.xaml.h"

#include "MainWindow.g.cpp"

import SnakeDx;

using namespace winrt;
using namespace Microsoft::UI::Xaml;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnakeDxWasdk::implementation {
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

void MainWindow::swapChainPanel_SizeChanged(IInspectable const&, SizeChangedEventArgs const& size)
{
    SnakeDx::scheduler.dxResources.WithLock([&](SnakeDx::Resources& r) { return; });
}

}
