#pragma once

#include "MainWindow.g.h"

namespace winrt::SnakeDxWasdk::implementation {
struct MainWindow : MainWindowT<MainWindow> {
    MainWindow()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    }

    void myButton_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    void swapChainPanel_SizeChanged(IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
    void swapChainPanel_Unloaded(IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
};
}

namespace winrt::SnakeDxWasdk::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {
};
}
