module;
#include "pch.h"
#include <cstddef>
export module SnakeDx:CoroutineScheduler;

import std;

import SnakeGame;

import :Token;
import :ShaderPass;
import :Resources;

namespace SnakeDx {
namespace winrt {
    using namespace ::winrt;
    using namespace ::winrt::Windows::Foundation;
}

export using AsyncCallback = std::function<winrt::fire_and_forget()>;

export class CoroutineScheduler final : public SnakeGame::GameScheduler {
private:
    winrt::apartment_context thread;
    winrt::IAsyncAction loop;

    timestep last = duration_cast<timestep>(clock::now().time_since_epoch());
    timestep next = duration_cast<timestep>(clock::now().time_since_epoch());

public:
    // Scheduling
    SnakeGame::Synchronized<std::vector<AsyncCallback>> deltaListeners {};
    SnakeGame::Synchronized<std::vector<AsyncCallback>> fixedListeners {};

    // DirectX draw resources
    SnakeGame::Synchronized<Resources> resources;
    ShaderPass pass;

    struct Camera {
        DirectX::XMFLOAT4X4 vp;
    };
    winrt::com_ptr<ID3D11Buffer> instances;
    winrt::com_ptr<ID3D11Buffer> camera;

public:
    enum class Message {
        Continue,
        Success,
        Error,
    } message
        = Message::Continue;

    CoroutineScheduler(Token);

    ~CoroutineScheduler();

protected:
    void StepFixed() override;

    void StepDelta(timestep const& delta) override;

private:
    winrt::IAsyncAction Run();
};

export auto scheduler = std::make_shared<CoroutineScheduler>(token);
}