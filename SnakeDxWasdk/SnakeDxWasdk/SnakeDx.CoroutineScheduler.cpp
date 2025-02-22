module;
#include "pch.h"
module SnakeDx:CoroutineScheduler;

import std;

namespace SnakeDx {
namespace {
    using namespace std::chrono;
    using namespace std::literals;
    using namespace std::chrono_literals;
    using namespace DirectX;
}

static constexpr UINT byte_size(std::ranges::sized_range auto&& sized)
{
    using value_type = std::remove_cvref_t<decltype(sized)>::value_type;
    return gsl::narrow<UINT>(std::ranges::size(sized) * sizeof(value_type));
}

static constexpr UINT byte_size(ID3D11Buffer& buffer)
{
    D3D11_BUFFER_DESC desc;
    buffer.GetDesc(&desc);
    return desc.ByteWidth;
}

static void buffer_realloc(ID3D11Device& device, winrt::com_ptr<ID3D11Buffer>& buffer, UINT byte_size, decltype(D3D11_BUFFER_DESC::BindFlags) bindFlags = D3D11_BIND_VERTEX_BUFFER)
{
    D3D11_BUFFER_DESC const INSTANCE_DESC {
        .ByteWidth = byte_size,
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = bindFlags,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    winrt::check_hresult(device.CreateBuffer(&INSTANCE_DESC, nullptr, buffer.put()));
}

static void Draw(ID3D11DeviceContext& context, ShaderPass& pass, ID3D11Buffer* camera, ID3D11Buffer* instances, size_t n_instances)
{
    auto& [m_vertex, m_pixel, m_layout] = pass;
    context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context.IASetInputLayout(m_layout.get());
    constexpr UINT strides = gsl::narrow<UINT>(sizeof SnakeGame::GridSquare);
    constexpr UINT offsets = 0;
    context.IASetVertexBuffers(0, 1, &instances, &strides, &offsets);

    context.VSSetShader(m_vertex.get(), 0, 0);
    context.VSSetConstantBuffers(0, 1, &camera);

    context.PSSetShader(m_pixel.get(), 0, 0);

    context.DrawInstanced(6, n_instances, 0, 0);
}

CoroutineScheduler::CoroutineScheduler(Token)
    : resources(token)
    , pass(
          resources->D3DDevice(),
          L"Square.vs.cso",
          L"PassThru.ps.cso",
          {
              D3D11_INPUT_ELEMENT_DESC {
                  .SemanticName = "InstancePosition",
                  .Format = DXGI_FORMAT_R32G32B32A32_SINT,
                  .InputSlot = 0,
                  .AlignedByteOffset = offsetof(SnakeGame::GridSquare, position),
                  .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
                  .InstanceDataStepRate = 1,
              },
              D3D11_INPUT_ELEMENT_DESC {
                  .SemanticName = "InstanceColor",
                  .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                  .InputSlot = 0,
                  .AlignedByteOffset = offsetof(SnakeGame::GridSquare, color),
                  .InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA,
                  .InstanceDataStepRate = 1,
              },
          })
{
    buffer_realloc(resources->D3DDevice(), grid, sizeof(SnakeGame::GridSquare) * BOUNDS_AREA);
    {
        auto& context = resources->D3DContext();
        D3D11_MAPPED_SUBRESOURCE mapped;
        context.Map(grid.get(), {}, D3D11_MAP_WRITE_DISCARD, {}, &mapped);
        std::span<SnakeGame::GridSquare, BOUNDS_AREA> grid_elems {
            reinterpret_cast<SnakeGame::GridSquare*>(mapped.pData),
            BOUNDS_AREA,
        };
        const auto iter_ij = std::views::cartesian_product(
            std::views::iota(BOUNDS.first.x, BOUNDS.second.x + 1),
            std::views::iota(BOUNDS.first.y, BOUNDS.second.y + 1));
        std::ranges::transform(iter_ij, grid_elems.begin(), [](auto&& ij) {
            auto const& [i, j] = ij;
            return SnakeGame::GridSquare {
                XMINT4 { i, j, 0, 0 },
                (i + j) % 2 ? XMFLOAT4 { .9, .9f, 1.f, 1 } : XMFLOAT4 { .7f, .7, 1.f, 1 },
            };
        });
    }
    buffer_realloc(resources->D3DDevice(), instances, SizeBytesSquares());
    buffer_realloc(resources->D3DDevice(), camera, sizeof(Camera), D3D11_BIND_CONSTANT_BUFFER);
    loop = Run();
}

CoroutineScheduler::~CoroutineScheduler()
{
    message = Message::Success;
}

void CoroutineScheduler::StepFixed()
{
    GameScheduler::StepFixed();
    if (const auto psize = SizeBytesSquares(); psize > byte_size(*instances)) {
        buffer_realloc(resources->D3DDevice(), instances, psize);
    }

    auto& context = resources->D3DContext();
    D3D11_MAPPED_SUBRESOURCE mapped;
    context.Map(instances.get(), {}, D3D11_MAP_WRITE_DISCARD, {}, &mapped);
    std::ranges::copy(Squares(), reinterpret_cast<SnakeGame::GridSquare*>(mapped.pData));
    context.Unmap(instances.get(), {});

    //static std::mt19937 g;
    //static std::uniform_int_distribution d { 0, 200 };
    //if (d(g) == 0) {
    //    auto [m, r] = resources.ToRef();
    //    std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
    //}

    auto [m, fl] = fixedListeners.ToRef();
    std::scoped_lock lock(m);
    if (fl.empty())
        return;
    for (auto& l : fl) {
        l();
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

            D3D11_MAPPED_SUBRESOURCE mapped;
            context.Map(camera.get(), {}, D3D11_MAP_WRITE_DISCARD, {}, &mapped);
            auto cam = reinterpret_cast<Camera*>(mapped.pData);
            XMStoreFloat4x4(
                &cam->vp,
                XMMatrixMultiplyTranspose(
                    XMMatrixLookAtLH(
                        XMVectorSet(0, 0, -BOUNDS_MAX_DISTANCE, 1),
                        XMVectorSet(0, 0, 0, 0),
                        XMVectorSet(0, 1, 0, 0)),
                    XMMatrixPerspectiveFovLH(
                        XMConvertToRadians(60.f),
                        resources->ViewportAspect(),
                        1e-4f,
                        1e4f)));
            context.Unmap(camera.get(), {});

            Draw(context, pass, camera.get(), grid.get(), BOUNDS_AREA);
            Draw(context, pass, camera.get(), instances.get(), SizeSquares());

            r.DrawEnd();
        }
    }
    {
    RENDER_DONE:
        auto [m, dl] = deltaListeners.ToRef();
        std::scoped_lock lock(m);
        if (dl.empty())
            return;
        for (auto& l : dl) {
            l();
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
            next = now + PERIOD;
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
