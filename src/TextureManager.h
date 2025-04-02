#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <unordered_map>
#include <d3d11.h>
#include <string>
#include <queue>
#include <vector>
#include "Texture.h"
#include <wrl/client.h>

#define TEX TextureManager

struct SamplerDescHash
{
    size_t operator()(const D3D11_SAMPLER_DESC& desc) const
    {
        size_t h1 = std::hash<int>()(static_cast<int>(desc.Filter));
        size_t h2 = std::hash<int>()(desc.MaxAnisotropy);
        size_t h3 = std::hash<int>()(static_cast<int>(desc.AddressU));
        size_t h4 = std::hash<int>()(static_cast<int>(desc.AddressV));
        size_t h5 = std::hash<int>()(static_cast<int>(desc.AddressW));
        size_t h6 = std::hash<int>()(static_cast<int>(desc.ComparisonFunc));
        size_t h7 = std::hash<float>()(desc.MinLOD);
        size_t h8 = std::hash<float>()(desc.MaxLOD);

        size_t seed = h1;
        seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h6 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h7 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h8 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

struct SamplerDescEqual
{
    bool operator()(const D3D11_SAMPLER_DESC& lhs, const D3D11_SAMPLER_DESC& rhs) const
    {
        return lhs.Filter == rhs.Filter &&
            lhs.AddressU == rhs.AddressU &&
            lhs.AddressV == rhs.AddressV &&
            lhs.AddressW == rhs.AddressW &&
            lhs.MipLODBias == rhs.MipLODBias &&
            lhs.MaxAnisotropy == rhs.MaxAnisotropy &&
            lhs.ComparisonFunc == rhs.ComparisonFunc &&
            lhs.BorderColor[0] == rhs.BorderColor[0] &&
            lhs.BorderColor[1] == rhs.BorderColor[1] &&
            lhs.BorderColor[2] == rhs.BorderColor[2] &&
            lhs.BorderColor[3] == rhs.BorderColor[3] &&
            lhs.MinLOD == rhs.MinLOD &&
            lhs.MaxLOD == rhs.MaxLOD;
    }
};

class TextureManager
{
public:
    TextureManager() = delete;
    TextureManager(const Texture&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(const TextureManager&) & = delete;
    TextureManager& operator=(TextureManager&&) & = delete;
    ~TextureManager() = default;

    static Texture* CreateTexObj(ID3D11Device* d3dDev);
    static void RemoveTexObj(Texture* texture);
    static void ReleaseAllTextObjs();

    static size_t GetAvailableNumofTexObj() { return privGetInstance().availableTexObjs.size(); }
    static size_t GetTotalNumofTexObj() { return privGetInstance().allTexObjs.size(); }
    static size_t GetNumofTextures() { return privGetInstance().textureMap.size(); }
    static size_t GetNumofSamplers() { return privGetInstance().samplerMap.size(); }

    static void RemoveTexture(ID3D11ShaderResourceView* texture);
    static void RemoveSampler(ID3D11SamplerState* sampler);
    static void ReleaseAllSamplers();
    static void ReleaseAllTextures();
    static void ReleaseAll();

    static ID3D11ShaderResourceView* LoadTexture(
        ID3D11Device* device,
        LPCWSTR filepath,
        bool computeMip,
        size_t miplevel,
        DirectX::TEX_FILTER_FLAGS filterflags);

    static ID3D11SamplerState* LoadSampler(
        ID3D11Device* device,
        const D3D11_SAMPLER_DESC& desc);

    static void Init(size_t poolSize = 10);
    static void Destroy();

private:
    static TextureManager& privGetInstance()
    {
        assert(initialized &&
               "TextureManager not initialized! Call Init first.");
        return *instance;
    }

private:
    ID3D11ShaderResourceView* privLoadTexture(
        ID3D11Device* device,
        LPCWSTR filepath,
        bool computeMip,
        size_t miplevel,
        DirectX::TEX_FILTER_FLAGS filterflags);

    ID3D11SamplerState* privLoadSampler(
        ID3D11Device* device,
        const D3D11_SAMPLER_DESC& desc);

    Texture* privCreateTexObj(ID3D11Device* d3dDev);
    void privRemoveTexObj(Texture* texture);
    void privReleaseAllTextObjs();
    void privRemoveTexture(ID3D11ShaderResourceView* texture);
    void privRemoveSampler(ID3D11SamplerState* sampler);
    void privReleaseAllSamplers();
    void privReleaseAllTextures();
    void privReleaseAll();

private:
    TextureManager(size_t poolSize);

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureMap;
    std::unordered_map<D3D11_SAMPLER_DESC, Microsoft::WRL::ComPtr<ID3D11SamplerState>, SamplerDescHash, SamplerDescEqual> samplerMap;

    std::queue<Texture*> availableTexObjs;
    std::vector<Texture*> allTexObjs; // To track allocated textures for cleanup

private:
    static std::unique_ptr<TextureManager> instance;
    static bool initialized;
    static std::once_flag initFlag;
};

#endif // _TEXTURE_MANAGER_H_