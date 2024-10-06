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
{
    auto [m, fl] = scheduler->fixedListeners.ToRef();
    std::scoped_lock lock(m);
    gameUpdateListener = &fl.emplace_back(
        [weak = get_weak(), uiThread = winrt::apartment_context()]() -> winrt::fire_and_forget {
            co_await uiThread;
            if (auto strong = weak.get(); strong) {
                auto score = strong->score();
                score.Text(winrt::to_hstring(strong->scheduler->Score()));

                auto button = strong->myButton();
                switch (strong->scheduler->state) {
                case SnakeGame::GameScheduler::GameState::LOST:
                    button.Content(box_value(L"You Lost!"));
                    button.Visibility(Visibility::Visible);
                    break;
                case SnakeGame::GameScheduler::GameState::PAUSED:
                    button.Content(box_value(L"Paused"));
                    button.Visibility(Visibility::Visible);
                    break;
                default:
                    button.Visibility(Visibility::Collapsed);
                    break;
                }
            }
        });
}

void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
{
    scheduler->state = SnakeGame::GameScheduler::GameState::RESETTING;
}

void MainWindow::swapChainPanel_SizeChanged(IInspectable const&, SizeChangedEventArgs const& e)
{
    auto [m, r] = scheduler->resources.ToRef();
    std::scoped_lock lock(m);
    r.SetSwapChainPanel(swapChainPanel(), e.NewSize());
}

void MainWindow::swapChainPanel_Unloaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    {
        auto [m, r] = scheduler->resources.ToRef();
        std::scoped_lock lock(m);
        r.ResetSwapChainPanel();
    }
    {
        auto [m, fl] = scheduler->fixedListeners.ToRef();
        std::scoped_lock lock(m);
        std::erase_if(fl, [&](auto&& l) { return &l == gameUpdateListener; });
    };
    scheduler.reset();
}

void MainWindow::swapChainPanel_KeyDown(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
{
    switch (e.Key()) {
    case winrt::Windows::System::VirtualKey::P:
        switch (scheduler->state) {
        case SnakeGame::GameScheduler::GameState::PAUSED:
            scheduler->state = SnakeGame::GameScheduler::GameState::RUNNING;
            break;
        case SnakeGame::GameScheduler::GameState::RUNNING:
            scheduler->state = SnakeGame::GameScheduler::GameState::PAUSED;
            break;
        }
        break;
    case winrt::Windows::System::VirtualKey::A:
        scheduler->direction = SnakeGame::GameScheduler::Direction::LEFT;
        break;
    case winrt::Windows::System::VirtualKey::D:
        scheduler->direction = SnakeGame::GameScheduler::Direction::RIGHT;
        break;
    case winrt::Windows::System::VirtualKey::W:
        scheduler->direction = SnakeGame::GameScheduler::Direction::UP;
        break;
    case winrt::Windows::System::VirtualKey::S:
        scheduler->direction = SnakeGame::GameScheduler::Direction::DOWN;
        break;
    case winrt::Windows::System::VirtualKey::Number0:
        scheduler->resources->swapInterval = 0;
        break;
    case winrt::Windows::System::VirtualKey::Number1:
        scheduler->resources->swapInterval = 1;
        break;
    case winrt::Windows::System::VirtualKey::Escape:
        Close();
        break;
    }
}
}
