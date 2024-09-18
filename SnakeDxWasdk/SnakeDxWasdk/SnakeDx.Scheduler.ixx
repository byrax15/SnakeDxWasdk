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

    void Draw(ID3D11DeviceContext& context) { }
};

export class Scheduler final : SnakeGame::Scheduler {
protected:
    // void StepFixed() override { }

    void StepDelta(timestep const&) override
    {
        if (!resources->Ready())
            return;
        resources.TryLock([&](SnakeDx::Resources& r) {
            r.Draw();

            static std::mt19937 g;
            static std::uniform_int_distribution d { 0, 200 };
            if (d(g) == 0)
                std::shuffle(r.m_clearColor.begin(), r.m_clearColor.begin() + 3, g);
        });
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