module;
#include "pch.h"
export module SnakeDx:ShaderPass;

import std;

namespace SnakeDx {

export class ShaderPass {
public:
    ShaderPass(ShaderPass const&) = delete;
    ShaderPass& operator=(ShaderPass const&) = delete;

public:
    ShaderPass(ID3D11Device& device, std::wstring_view vertex_fname, std::wstring_view pixel_fname)
    {
        const auto vbytes = ReadBytes(vertex_fname);
        device.CreateVertexShader(vbytes.data(), vbytes.size(), nullptr, m_vertex.put());

        const auto pbytes = ReadBytes(pixel_fname);
        device.CreatePixelShader(pbytes.data(), pbytes.size(), nullptr, m_pixel.put());
    }

protected:
    ~ShaderPass() { }

private:
    winrt::com_ptr<ID3D11VertexShader> m_vertex;
    winrt::com_ptr<ID3D11PixelShader> m_pixel;

private:
    static std::vector<char> ReadBytes(std::wstring_view fName)
    {
        const auto path = [&]() -> winrt::hstring {
            try {
                return winrt::Windows::ApplicationModel::Package::Current().InstalledPath();
            } catch (winrt::hresult const&) {
                return L".";
            }
        }() + L"\\"
            + fName;
        std::ifstream file { path.data(), std::ios::ate | std::ios::binary };
        if (!file)
            throw std::exception("File not found");
        const auto size = file.tellg();
        file.seekg(0);
        std::vector<char> bytecode(size);
        file.read(bytecode.data(), size);
        return bytecode;
    }
};
}