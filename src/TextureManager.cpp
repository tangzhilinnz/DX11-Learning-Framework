#include "TextureManager.h"
#include "DirectXTex.h"
#include <windows.h>

// Define static members
std::unique_ptr<TextureManager> TextureManager::instance = nullptr;
bool TextureManager::initialized = false;
std::once_flag TextureManager::initFlag;

void TextureManager::Init(size_t poolSize)
{
    std::call_once(initFlag, [poolSize](){
        instance.reset(new TextureManager(poolSize));
        initialized = true;
        });
}

void TextureManager::Destroy()
{
    TextureManager::ReleaseAll();

    if (instance)
    {
        instance.reset();
        initialized = false;
    } 
}

ID3D11ShaderResourceView* TextureManager::LoadTexture(
    ID3D11Device* device,
    LPCWSTR filepath,
    bool computeMip,
    size_t miplevel,
    DirectX::TEX_FILTER_FLAGS filterflags)
{
    TextureManager& pMan = TextureManager::privGetInstance();
    return pMan.privLoadTexture(device, filepath, computeMip, miplevel, filterflags);
}

ID3D11SamplerState* TextureManager::LoadSampler(
    ID3D11Device* device,
    const D3D11_SAMPLER_DESC& desc)
{
    TextureManager& pMan = TextureManager::privGetInstance();
    return pMan.privLoadSampler(device, desc);
}

void TextureManager::RemoveTexture(ID3D11ShaderResourceView* texture)
{
    TextureManager& pMan = TextureManager::privGetInstance();
    pMan.privRemoveTexture(texture);
}

void TextureManager::RemoveSampler(ID3D11SamplerState* sampler)
{
    TextureManager& pMan = TextureManager::privGetInstance();
    pMan.privRemoveSampler(sampler);
}

void TextureManager::ReleaseAllSamplers()
{
    TextureManager& pMan = TextureManager::privGetInstance();
    pMan.privReleaseAllSamplers();
}

void TextureManager::ReleaseAllTextures()
{
    TextureManager& pMan = TextureManager::privGetInstance();
    pMan.privReleaseAllTextures();
}

void TextureManager::ReleaseAll()
{
    TextureManager& pMan = TextureManager::privGetInstance();
    pMan.privReleaseAll();
}

Texture* TextureManager::CreateTexObj(ID3D11Device* d3dDev)
{
    TextureManager& tm = privGetInstance();
    return tm.privCreateTexObj(d3dDev);
}

void TextureManager::RemoveTexObj(Texture* texture)
{
    TextureManager& tm = privGetInstance();
    tm.privRemoveTexObj(texture);
}

void TextureManager::ReleaseAllTextObjs()
{
    TextureManager& tm = privGetInstance();
    tm.privReleaseAllTextObjs();
}


// ----------------------- priv implementation Functions ----------------------
ID3D11ShaderResourceView* TextureManager::privLoadTexture(
    ID3D11Device* device,
    LPCWSTR filepath,
    bool computeMip,
    size_t miplevel,
    DirectX::TEX_FILTER_FLAGS filterflags)
{
    wchar_t buffer[1024];
    // Format the key string using swprintf_s for efficiency
    swprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), L"%s_%d_%zu_%d",
        filepath, computeMip, miplevel, static_cast<int>(filterflags));

    std::wstring key(buffer);

    // Check if the texture with the same key already exists in the map
    auto it = textureMap.find(key);
    if (it != textureMap.end())
    {
        return it->second.Get();
    }

    // If not found, load the texture
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureRV;

    std::wstring fpath(filepath);
    std::wstring ext = fpath.substr(fpath.find_last_of(L".") + 1);

    // Load the texture from file based on format
    DirectX::ScratchImage scrtTex;
    HRESULT hr = S_OK;

    if (ext == L"tga" || ext == L"TGA")
    {
        hr = LoadFromTGAFile(filepath, nullptr, scrtTex);
    }
    else if (ext == L"dds" || ext == L"DDS")
    {
        hr = LoadFromDDSFile(filepath, DirectX::DDS_FLAGS_NONE, nullptr, scrtTex);
    }
    else
    {
        assert(false && "ERROR: Invalid file format");
    }
    assert(SUCCEEDED(hr));

    // Compute Mip if requested and needed
    if (computeMip)
    {
        assert(scrtTex.GetImageCount() == 1 && "ERROR: File already contains MIP map.");
        DirectX::ScratchImage mipchain;
        hr = DirectX::GenerateMipMaps(*(scrtTex.GetImage(0, 0, 0)), filterflags, miplevel, mipchain);
        assert(SUCCEEDED(hr));

        hr = CreateShaderResourceView(device, mipchain.GetImage(0, 0, 0), mipchain.GetImageCount(),
            mipchain.GetMetadata(), textureRV.GetAddressOf());
    }
    else
    {
        hr = CreateShaderResourceView(device, scrtTex.GetImage(0, 0, 0), scrtTex.GetImageCount(),
            scrtTex.GetMetadata(), textureRV.GetAddressOf());
    }

    assert(SUCCEEDED(hr));

    // Cache the newly loaded texture with the key
    textureMap[key] = textureRV;
    return textureRV.Get();
}

ID3D11SamplerState* TextureManager::privLoadSampler(
    ID3D11Device* device,
    const D3D11_SAMPLER_DESC& desc)
{
    auto it = samplerMap.find(desc);
    if (it != samplerMap.end())
    {
        return it->second.Get();
    }

    HRESULT hr = S_OK;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    hr = device->CreateSamplerState(&desc, sampler.GetAddressOf());
    assert(SUCCEEDED(hr));

    samplerMap[desc] = sampler;
    return sampler.Get();
}

void TextureManager::privReleaseAllSamplers()
{
    samplerMap.clear();
}

void TextureManager::privReleaseAllTextures()
{
    textureMap.clear();
}

void TextureManager::privReleaseAll()
{
    wchar_t buffer[256];
    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"AvailableNumofTexObj: %lu\n", GetAvailableNumofTexObj());
    OutputDebugStringW(buffer);
    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"TotalNumofTexObj: %lu\n", GetTotalNumofTexObj());
    OutputDebugStringW(buffer);
    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"NumofTextures: %lu\n", GetNumofTextures());
    OutputDebugStringW(buffer);
    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"NumofSamplers: %lu\n", GetNumofSamplers());
    OutputDebugStringW(buffer);

    this->privReleaseAllTextObjs();
    this->privReleaseAllSamplers();
    this->privReleaseAllTextures();
}

void TextureManager::privRemoveTexture(ID3D11ShaderResourceView* texture)
{
    for (auto it = textureMap.begin(); it != textureMap.end(); it++)
    {
        if (it->second.Get() == texture)
        {
            textureMap.erase(it);
            return;
        }
    }
}

void TextureManager::privRemoveSampler(ID3D11SamplerState* sampler)
{
    for (auto it = samplerMap.begin(); it != samplerMap.end(); it++)
    {
        if (it->second.Get() == sampler)
        {
            samplerMap.erase(it);
            return;
        }
    }
}

// Initialize the pool with 10 textures
TextureManager::TextureManager(size_t poolSize)
{
    allTexObjs.reserve(poolSize);

    for (int i = 0; i < (int)poolSize; ++i)
    {
        Texture* tex = new Texture();
        availableTexObjs.push(tex);
        allTexObjs.push_back(tex);
    }
}

Texture* TextureManager::privCreateTexObj(ID3D11Device* d3dDev)
{
    if (availableTexObjs.empty())
    {
        Texture* newTex = new Texture();
        allTexObjs.push_back(newTex);
        newTex->SetDevice(d3dDev);
        return newTex;
    }
    else
    {
        Texture* tex = availableTexObjs.front();
        availableTexObjs.pop();
        tex->SetDevice(d3dDev);
        return tex;
    }
}

void TextureManager::privRemoveTexObj(Texture* texture)
{
    assert(texture);

    auto& tm = privGetInstance();
    texture->Release();
    tm.availableTexObjs.push(texture);
}

void TextureManager::privReleaseAllTextObjs()
{
    for (Texture* texture : allTexObjs)
    {
        delete texture;
    }

    availableTexObjs = std::queue<Texture*>();
    allTexObjs.clear();
}