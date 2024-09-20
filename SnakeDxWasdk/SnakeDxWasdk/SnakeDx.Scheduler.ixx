module;
#include "pch.h"
export module SnakeDx:Scheduler;

import std;
using namespace std::literals;

import SnakeGame;
import :Resources;
import :Token;
import :ShaderPass;
import ref_cast;

namespace SnakeDx {

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
        context.DrawInstanced(3, 2, 0, 0);
    }
};

export class Scheduler final : SnakeGame::Scheduler {
protected:
    void StepFixed() override
    {
        auto& r = *resources;
        static std::mt19937 g;
        static std::uniform_int_distribution d { 0, 200 };
        if (d(g) == 0)
            std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
    }

    void StepDelta(timestep const&) override
    {
        if (!resources->Ready())
            return;

        auto [m, r] = resources.ToRef();
        std::unique_lock lock(m, std::try_to_lock);
        if (lock) {
            r.Draw(trianglePass);
        }
    }

public:
    Scheduler(Token)
    {
    }

    SnakeGame::Synchronized<Resources> resources { token };

private:
    TrianglePass trianglePass { resources->D3DDevice() };

} scheduler(token);
}