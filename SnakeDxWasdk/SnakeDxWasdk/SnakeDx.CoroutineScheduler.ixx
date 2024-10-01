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
        : ShaderPass(
            device,
            L"Triangle.vs.cso",
            L"PassThru.ps.cso",
            { D3D11_INPUT_ELEMENT_DESC {
                .SemanticName = "InstancePosition",
                .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
                .InstanceDataStepRate = 1,
            } })
    {
    }

    void Draw(ID3D11DeviceContext& context, ID3D11Buffer* camera_buf, ID3D11Buffer* instance_buf, UINT size_instance);
};

export class CoroutineScheduler final : public SnakeGame::GameScheduler {
private:
    winrt::apartment_context thread;
    winrt::IAsyncAction loop;

    timestep last = duration_cast<timestep>(clock::now().time_since_epoch());
    timestep next = duration_cast<timestep>(clock::now().time_since_epoch());

public:
    // Scheduling
    SnakeGame::Synchronized<std::list<std::function<winrt::fire_and_forget(void)>*>> deltaListeners {};
    timestep frameTime {};

    // DirectX draw resources
    SnakeGame::Synchronized<Resources> resources;
    TrianglePass trianglePass;
    winrt::com_ptr<ID3D11Buffer> instances;

    struct Camera {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT4X4 v;
        DirectX::XMFLOAT4X4 p;
    };
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
} scheduler(token);
}