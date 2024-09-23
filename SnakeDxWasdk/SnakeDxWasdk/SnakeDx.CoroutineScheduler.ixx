module;
#include "pch.h"
export module SnakeDx:CoroutineScheduler;

import std;

import SnakeGame;

import :Token;
import :ShaderPass;
import :Resources;

namespace SnakeDx {
namespace {
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::chrono_literals;
}
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
    void StepFixed() override
    {
        GameScheduler::StepFixed();

        static std::mt19937 g;
        static std::uniform_int_distribution d { 0, 200 };
        if (d(g) == 0) {
            auto [m, r] = resources.ToRef();
            // std::unique_lock lock(m);
            std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
        }
    }

    void StepDelta(timestep const& delta) override
    {
        GameScheduler::StepDelta(delta);

        if (!resources->Ready())
            return;

        auto [m, r] = resources.ToRef();
        std::unique_lock lock(m, std::try_to_lock);
        if (lock) {
            r.Draw(trianglePass);
        }
    }

private:
    winrt::IAsyncAction Run()
    {
        co_await winrt::resume_background();
        thread = {};

        while (message == Message::Continue) {
            const auto now
                = duration_cast<timestep>(clock::now().time_since_epoch());
            if (now >= next) {
                next += PERIOD;
                StepFixed();
            }

            StepDelta(now - last);
            last = now;
            co_await winrt::resume_after(1ms);
        }
        if (message == Message::Error) {
            throw std::exception("Event Loop crash");
        }
    }

} scheduler(token);
}