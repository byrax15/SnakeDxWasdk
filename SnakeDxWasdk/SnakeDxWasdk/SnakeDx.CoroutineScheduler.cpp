module;
#include "pch.h"
module SnakeDx:CoroutineScheduler;

import std;

namespace SnakeDx {
namespace {
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::chrono_literals;
}

static UINT byte_size(std::ranges::sized_range auto&& sized)
{
    using value_type = std::remove_cvref_t<decltype(sized)>::value_type;
    return gsl::narrow<UINT>(std::ranges::size(sized) * sizeof(value_type));
}

static UINT byte_size(ID3D11Buffer& buffer)
{
    D3D11_BUFFER_DESC desc;
    buffer.GetDesc(&desc);
    return desc.ByteWidth;
}

static void buffer_realloc(ID3D11Device& device, winrt::com_ptr<ID3D11Buffer>& buffer, UINT byte_size)
{
    D3D11_BUFFER_DESC const INSTANCE_DESC {
        .ByteWidth = byte_size,
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    winrt::check_hresult(device.CreateBuffer(&INSTANCE_DESC, nullptr, buffer.put()));
}

CoroutineScheduler::CoroutineScheduler(Token)
    : resources(token)
    , trianglePass(resources->D3DDevice())
{
    buffer_realloc(resources->D3DDevice(), instances, byte_size(positions));
    loop = Run();
}

CoroutineScheduler::~CoroutineScheduler()
{
    message = Message::Success;
}

void CoroutineScheduler::StepFixed()
{
    GameScheduler::StepFixed();
    if (const auto psize = byte_size(positions); psize > byte_size(*instances)) {
        buffer_realloc(resources->D3DDevice(), instances, psize);
    }
    auto& context = resources->D3DContext();
    D3D11_MAPPED_SUBRESOURCE mapped;
    context.Map(instances.get(), {}, D3D11_MAP_WRITE_DISCARD, {}, &mapped);
    std::ranges::copy(positions, reinterpret_cast<float4*>(mapped.pData));
    context.Unmap(instances.get(), {});

    static std::mt19937 g;
    static std::uniform_int_distribution d { 0, 200 };
    if (d(g) == 0) {
        auto [m, r] = resources.ToRef();
        std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
    }
}

void CoroutineScheduler::StepDelta(timestep const& delta)
{
    GameScheduler::StepDelta(delta);
    {
        if (!resources->Ready())
            goto RENDER_DONE;
        auto [m, r] = resources.ToRef();
        std::unique_lock lock(m, std::try_to_lock);
        if (lock) {
            auto& context = r.DrawStart();
            trianglePass.Draw(context, instances.get(), gsl::narrow<UINT>(positions.size()));
            r.DrawEnd();
            frameTime = delta;
        }
    }
    {
    RENDER_DONE:
        auto [m, dl] = deltaListeners.ToRef();
        std::scoped_lock lock(m);
        if (dl.empty())
            return;
        for (auto& l : dl) {
            l->operator()();
        }
    }
}

winrt::IAsyncAction CoroutineScheduler::Run()
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

        // TODO : Remove delay
        co_await winrt::resume_after(1us);
    }
    if (message == Message::Error) {
        throw std::exception("Event Loop crash");
    }
}
}
