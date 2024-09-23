#pragma once

#include "MainWindow.g.h"

import std;

namespace winrt::SnakeDxWasdk::implementation {
struct MainWindow : MainWindowT<MainWindow> {
    MainWindow();

    void myButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    void swapChainPanel_SizeChanged(IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
    void swapChainPanel_Unloaded(IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    std::function<winrt::fire_and_forget(void)> frameTimeListener;
};
}

namespace winrt::SnakeDxWasdk::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {
};
}
