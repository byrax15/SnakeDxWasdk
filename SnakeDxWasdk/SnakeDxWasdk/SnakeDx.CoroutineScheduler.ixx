module;
#include "pch.h"
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

class TrianglePass final : ShaderPass {
public:
    TrianglePass(ID3D11Device5& device)
        : ShaderPass(device, L"Triangle.vs.cso", L"PassThru.ps.cso")
    {
    }

    void Draw(ID3D11DeviceContext& context)
    {
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.VSSetShader(m_vertex.get(), 0, 0);
        context.PSSetShader(m_pixel.get(), 0, 0);
        context.DrawInstanced(3, 3, 0, 0);
    }
};

export class CoroutineScheduler final : public SnakeGame::GameScheduler {
private:
    winrt::apartment_context thread;
    winrt::IAsyncAction loop;

    timestep last = duration_cast<timestep>(clock::now().time_since_epoch());
    timestep next = duration_cast<timestep>(clock::now().time_since_epoch());

public:
    SnakeGame::Synchronized<std::list<std::function<winrt::fire_and_forget(void)>*>> deltaListeners {};
    timestep frameTime {};
    SnakeGame::Synchronized<Resources> resources { token };
    TrianglePass trianglePass { resources->D3DDevice() };

    enum class Message {
        Continue,
        Success,
        Error,
    } message
        = Message::Continue;

    CoroutineScheduler(Token)
        : loop(Run())
    {
    }

    ~CoroutineScheduler()
    {
        message = Message::Success;
    }

protected:
    void StepFixed() override;

    void StepDelta(timestep const& delta) override;

private:
    winrt::IAsyncAction Run();

} scheduler(token);
}