#include "D3D11Renderer.h"



using Microsoft::WRL::ComPtr;


void D3D11Renderer::GetPresetTexture2d(ComPtr<ID3D11Device> D3D11Device, ComPtr<ID3D11Texture2D>& Out, TexPreset2D Preset, int texWidth, int texHeight) {
	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = static_cast<UINT>(texWidth);
	desc.Height = static_cast<UINT>(texHeight);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	switch (Preset) {
	case RenderTargetPreset:
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		break;

	case DepthStencilPreset:
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		break;

	case DynamicPreset:
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

	case StagingBufferPresetCPUR:
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		break;
	case StagingBufferPresetCPUWR:
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	}

	D3D11Device->CreateTexture2D(&desc, nullptr, Out.GetAddressOf());
}


D3D11Renderer::D3D11Renderer() {};

void D3D11Renderer::Clear(ID3D11DeviceContext* Context, ID3D11RenderTargetView* RTV, float r, float g, float b, float a) {
    if (!Context || !RTV) return;
    float color[4] = { r, g, b, a };
    Context->ClearRenderTargetView(RTV, color);
}

void D3D11Renderer::SetViewport(ID3D11DeviceContext* Context, int x, int y, int width, int height) {
    if (!Context) return;
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = (FLOAT)x;
    vp.TopLeftY = (FLOAT)y;
    Context->RSSetViewports(1, &vp);
}

void D3D11Renderer::Present(IDXGISwapChain3* SwapChain, bool VSync) {
    if (!SwapChain) return;
    SwapChain->Present(VSync ? 1 : 0, VSync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
}

HRESULT D3D11Renderer::CreateD3d11Device(D3D_FEATURE_LEVEL(FeatureLevels)[], UINT FeatureLevelCount, UINT& CreationFlags, D3DDevice* OutDevice) {
	if (!OutDevice) return E_POINTER;

	D3D_FEATURE_LEVEL SelectedFeatureLevel;

#if defined(_DEBUG)
	CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, CreationFlags,
		FeatureLevels, FeatureLevelCount, D3D11_SDK_VERSION,
		&OutDevice->D3D11Device, &SelectedFeatureLevel, &OutDevice->D3D11Context);
}

HRESULT D3D11Renderer::CreateDXGIFactory2(IDXGIFactory2** OutFactory) {
	if (!OutFactory) return E_POINTER;

	ComPtr<IDXGIFactory> factory = nullptr;
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	if (FAILED(hr)) return hr;

	return factory->QueryInterface(__uuidof(IDXGIFactory2), (void**)OutFactory);
}

HRESULT D3D11Renderer::CreateSwapChain(
		ID3D11Device* D3D11Device, 
		HWND& hwnd, 
		UINT Width, 
		UINT Height, 
		IDXGIFactory2* Factory2,
		SwapChainConfig SwapChainConfig,
		IDXGISwapChain3** OutSwapChain
) {
	if (!OutSwapChain) return E_POINTER;

	ComPtr<IDXGISwapChain1> SwapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapchainConfig = {};

	swapchainConfig.SwapEffect = SwapChainConfig.SwapEffect;
	swapchainConfig.BufferCount = SwapChainConfig.BufferCount;
	swapchainConfig.Width = Width;
	swapchainConfig.Height = Height;
	swapchainConfig.Format = SwapChainConfig.Format;
	swapchainConfig.BufferUsage = SwapChainConfig.BufferUsage;
	swapchainConfig.Scaling = SwapChainConfig.Scaling;
	swapchainConfig.Flags = SwapChainConfig.Flags;
	swapchainConfig.Stereo = SwapChainConfig.Stereo;
	swapchainConfig.SampleDesc.Count = SwapChainConfig.SampleDesc.Count;
	swapchainConfig.SampleDesc.Quality = SwapChainConfig.SampleDesc.Quality;


	HRESULT hr = Factory2->CreateSwapChainForHwnd(D3D11Device, hwnd, &swapchainConfig, nullptr, nullptr, &SwapChain);
	if (FAILED(hr)) return hr;
	
	return SwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)OutSwapChain);
}

HRESULT D3D11Renderer::GetSwapChainBuffer(IDXGISwapChain3* SwapChain, UINT Buffer, ID3D11Texture2D** OutBuffer) {
	if (!SwapChain || !OutBuffer) return E_POINTER;
	return SwapChain->GetBuffer(Buffer, IID_PPV_ARGS(OutBuffer));
}

HRESULT D3D11Renderer::GetSwapChainBuffersArray(IDXGISwapChain3* SwapChain, UINT Count, std::vector<ComPtr<ID3D11Texture2D>>& OutBuffers) {
	if (!SwapChain) return E_POINTER;

	OutBuffers.clear();
	for (UINT i = 0; i < Count; i++) {
		ComPtr<ID3D11Texture2D> buffer;
		HRESULT hr = SwapChain->GetBuffer(i, IID_PPV_ARGS(&buffer));
		if (FAILED(hr)) return hr;
		OutBuffers.push_back(buffer);
	}
	return S_OK;
}

HRESULT D3D11Renderer::CreateRTV(ID3D11Device* D3D11Device, ID3D11Texture2D* targetBuffer, ID3D11RenderTargetView** OutRTV) {
	if (!D3D11Device || !targetBuffer || !OutRTV) return E_POINTER;
	return D3D11Device->CreateRenderTargetView(targetBuffer, nullptr, OutRTV);
}

HRESULT D3D11Renderer::CreateRTVArray(ID3D11Device* D3D11Device, IDXGISwapChain3* SwapChain, UINT Count, std::vector<ComPtr<ID3D11RenderTargetView>>& OutRTVArray) {
	if (!D3D11Device || !SwapChain) return E_POINTER;

	OutRTVArray.clear();
	for (UINT i = 0; i < Count; i++) {
		ComPtr<ID3D11Texture2D> buffer;
		HRESULT hr = SwapChain->GetBuffer(i, IID_PPV_ARGS(&buffer));
		if (FAILED(hr)) return hr;

		ComPtr<ID3D11RenderTargetView> rtv;
		hr = D3D11Device->CreateRenderTargetView(buffer.Get(), nullptr, &rtv);
		if (FAILED(hr)) return hr;

		OutRTVArray.push_back(rtv);
	}
	return S_OK;
}

HRESULT D3D11Renderer::CreatePixelShader(ID3D11Device* D3D11Device, LPCWSTR FileName, ID3D11PixelShader** OutShader) {
	if (!D3D11Device || !OutShader) return E_POINTER;

	ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
	HRESULT hr = D3DReadFileToBlob(FileName, &pixelShaderBlob);
	if (FAILED(hr)) return hr;

	return D3D11Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr, OutShader);
}

HRESULT D3D11Renderer::CreateVertexShader(ID3D11Device* D3D11Device, LPCWSTR FileName, ID3D11VertexShader** OutShader, ID3DBlob** OutBlob) {
	if (!D3D11Device || !OutShader) return E_POINTER;
	
	ComPtr<ID3DBlob> vertexShaderBlob;
	HRESULT hr = D3DReadFileToBlob(FileName, &vertexShaderBlob);
	if (FAILED(hr)) return hr;

	hr = D3D11Device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		nullptr, OutShader);
	if (FAILED(hr)) return hr;

	if (OutBlob) {
		*OutBlob = vertexShaderBlob.Detach();
	}

	return S_OK;
}

HRESULT D3D11Renderer::CreateInputLayout(ID3D11Device* D3D11Device, D3D11_INPUT_ELEMENT_DESC* layout, UINT ArraySize, ID3DBlob* VertexShaderBlob, ID3D11InputLayout** OutLayout) {
	if (!D3D11Device || !VertexShaderBlob || !OutLayout) return E_POINTER;

	return D3D11Device->CreateInputLayout(layout, ArraySize, VertexShaderBlob->GetBufferPointer(),
			VertexShaderBlob->GetBufferSize(), OutLayout);
}

HRESULT D3D11Renderer::CreateIndexBuffer(ID3D11Device* D3D11Device, const void* Indices, UINT ByteWidth, ID3D11Buffer** OutBuffer, IndexBufferConfig BufferConfig) {
	if (!D3D11Device || !OutBuffer) return E_POINTER;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = ByteWidth;
	desc.Usage = BufferConfig.Usage;
	desc.BindFlags = BufferConfig.BindFlags;
	desc.CPUAccessFlags = BufferConfig.CPUAccessFlags;
	desc.MiscFlags = BufferConfig.MiscFlags;
	desc.StructureByteStride = BufferConfig.StructureByteStride;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = Indices;
	initData.SysMemPitch = BufferConfig.SysMemPitch;
	initData.SysMemSlicePitch = BufferConfig.SysMemSlicePitch;
	
	return D3D11Device->CreateBuffer(&desc, &initData, OutBuffer);
}

HRESULT D3D11Renderer::CreateConstantBuffer(ID3D11Device* D3D11Device, const void* Data, UINT ByteWidth, ID3D11Buffer** OutBuffer, ConstantBufferConfig BufferConfig) {
	if (!D3D11Device || !OutBuffer) return E_POINTER;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = ByteWidth;
	desc.Usage = BufferConfig.Usage;
	desc.BindFlags = BufferConfig.BindFlags;
	desc.CPUAccessFlags = BufferConfig.CPUAccessFlags;
	desc.MiscFlags = BufferConfig.MiscFlags;
	desc.StructureByteStride = BufferConfig.StructureByteStride;

	if (Data) {
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = Data;
		initData.SysMemPitch = BufferConfig.SysMemPitch;
		initData.SysMemSlicePitch = BufferConfig.SysMemSlicePitch;
		return D3D11Device->CreateBuffer(&desc, &initData, OutBuffer);
	} else {
		return D3D11Device->CreateBuffer(&desc, nullptr, OutBuffer);
	}
}

HRESULT D3D11Renderer::CreateTexture2D(ID3D11Device* D3D11Device, const D3D11_TEXTURE2D_DESC* Desc, const D3D11_SUBRESOURCE_DATA* InitialData, ID3D11Texture2D** OutTexture) {
	if (!D3D11Device || !Desc || !OutTexture) return E_POINTER;
	return D3D11Device->CreateTexture2D(Desc, InitialData, OutTexture);
}

HRESULT D3D11Renderer::CreateSamplerState(ID3D11Device* D3D11Device, const D3D11_SAMPLER_DESC* Desc, ID3D11SamplerState** OutSampler) {
	if (!D3D11Device || !Desc || !OutSampler) return E_POINTER;
	return D3D11Device->CreateSamplerState(Desc, OutSampler);
}

HRESULT D3D11Renderer::CreateRasterizerState(ID3D11Device* D3D11Device, const D3D11_RASTERIZER_DESC* Desc, ID3D11RasterizerState** OutState) {
	if (!D3D11Device || !Desc || !OutState) return E_POINTER;
	return D3D11Device->CreateRasterizerState(Desc, OutState);
}

HRESULT D3D11Renderer::CreateBlendState(ID3D11Device* D3D11Device, const D3D11_BLEND_DESC* Desc, ID3D11BlendState** OutState) {
	if (!D3D11Device || !Desc || !OutState) return E_POINTER;
	return D3D11Device->CreateBlendState(Desc, OutState);
}

HRESULT D3D11Renderer::CreateDepthStencilState(ID3D11Device* D3D11Device, const D3D11_DEPTH_STENCIL_DESC* Desc, ID3D11DepthStencilState** OutState) {
	if (!D3D11Device || !Desc || !OutState) return E_POINTER;
	return D3D11Device->CreateDepthStencilState(Desc, OutState);
}

void D3D11Renderer::SetViewPort(ID3D11DeviceContext* D3D11Context) {
	
}