module;
#include "pch.h"
export module SnakeDx:ShaderPass;

import std;

namespace SnakeDx {

export class ShaderPass final {
public:
    winrt::com_ptr<ID3D11VertexShader> m_vertex;
    winrt::com_ptr<ID3D11PixelShader> m_pixel;
    winrt::com_ptr<ID3D11InputLayout> m_layout;

public:
    ShaderPass(ShaderPass const&) = delete;
    ShaderPass& operator=(ShaderPass const&) = delete;

    ShaderPass(ID3D11Device& device,
        std::wstring_view vertex_fname, std::wstring_view pixel_fname,
        std::initializer_list<D3D11_INPUT_ELEMENT_DESC> inputs = {})
    {
        const auto vbytes = ReadBytes(vertex_fname);
        winrt::check_hresult(
            device.CreateVertexShader(vbytes.data(), vbytes.size(), nullptr, m_vertex.put()));

        const auto pbytes = ReadBytes(pixel_fname);
        winrt::check_hresult(
            device.CreatePixelShader(pbytes.data(), pbytes.size(), nullptr, m_pixel.put()));

        if (inputs.size() != 0) {
            winrt::check_hresult(
                device.CreateInputLayout(std::data(inputs), inputs.size(), vbytes.data(), vbytes.size(), m_layout.put()));
        }
    }

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