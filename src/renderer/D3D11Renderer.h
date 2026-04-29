#ifndef D3DRENDERER_H
#define D3DRENDERER_H

#pragma warning(default : 4710)
#pragma warning(default : 4711)
#pragma warning(default : 4714)

#pragma once
#include "Logger.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <wrl/client.h>
#include <iostream>
#include <d3d11.h>
#include <dxgi1_5.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

enum TexPreset2D {
	RenderTargetPreset,
	DepthStencilPreset,
	DynamicPreset,
	StagingBufferPresetCPUR,
	StagingBufferPresetCPUWR
};


struct D3DDevice {
	ComPtr<ID3D11Device> D3D11Device = nullptr;
	ComPtr<ID3D11DeviceContext> D3D11Context = nullptr;
};


struct SwapChainConfig {
	DXGI_FORMAT Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	BOOL Stereo = false;
	DXGI_SAMPLE_DESC SampleDesc = {1, 0};
	DXGI_USAGE BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	UINT BufferCount = 2;
	DXGI_SCALING Scaling = DXGI_SCALING_NONE;
	DXGI_SWAP_EFFECT SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	DXGI_ALPHA_MODE AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	UINT Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
};



struct VertexBufferConfig {
	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT BindFlags = D3D11_BIND_VERTEX_BUFFER;
	UINT CPUAccessFlags = 0;
	UINT MiscFlags = 0;
	UINT StructureByteStride = 0;
	UINT SysMemPitch = 0;
	UINT SysMemSlicePitch = 0;
};

struct IndexBufferConfig {
	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT BindFlags = D3D11_BIND_INDEX_BUFFER;
	UINT CPUAccessFlags = 0;
	UINT MiscFlags = 0;
	UINT StructureByteStride = 0;
	UINT SysMemPitch = 0;
	UINT SysMemSlicePitch = 0;
};

struct ConstantBufferConfig {
	D3D11_USAGE Usage = D3D11_USAGE_DEFAULT;
	UINT BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	UINT CPUAccessFlags = 0;
	UINT MiscFlags = 0;
	UINT StructureByteStride = 0;
	UINT SysMemPitch = 0;
	UINT SysMemSlicePitch = 0;
};





class D3D11Renderer {
private:
	HRESULT hr = S_OK;

	void GetPresetTexture2d(ComPtr<ID3D11Device> D3D11Device, ComPtr<ID3D11Texture2D>& Out, TexPreset2D Preset, int bufferWidth, int bufferHeight);

public:
	D3D11Renderer();

	void Clear(ID3D11DeviceContext* Context, ID3D11RenderTargetView* RTV, float r, float g, float b, float a);
	void SetViewport(ID3D11DeviceContext* Context, int x, int y, int width, int height);
	void Present(IDXGISwapChain3* SwapChain, bool VSync);

	void SetViewPort(ID3D11DeviceContext* D3D11Context);

	HRESULT CreateD3d11Device(D3D_FEATURE_LEVEL(FeatureLevels)[], UINT FeatureLevelCount, UINT& CreationFlags, D3DDevice* OutDevice);

	inline HRESULT GetDeferredContext(ID3D11Device* D3D11Device, ID3D11DeviceContext** D3D11Context) {
		return D3D11Device->CreateDeferredContext(0, D3D11Context);
	}

	HRESULT CreateDXGIFactory2(IDXGIFactory2** OutFactory);

	HRESULT CreateSwapChain(
		ID3D11Device* D3D11Device,
		HWND& hwnd,
		UINT Width,
		UINT Height,
		IDXGIFactory2* Factory2,
		SwapChainConfig SwapChainConfig,
		IDXGISwapChain3** OutSwapChain
	);

	HRESULT GetSwapChainBuffer(IDXGISwapChain3* SwapChain, UINT Buffer, ID3D11Texture2D** OutBuffer);

	HRESULT GetSwapChainBuffersArray(IDXGISwapChain3* SwapChain, UINT Count, std::vector<ComPtr<ID3D11Texture2D>>& OutBuffers);

	HRESULT CreateRTV(ID3D11Device* D3D11Device, ID3D11Texture2D* targetBuffer, ID3D11RenderTargetView** OutRTV);

	HRESULT CreateRTVArray(ID3D11Device* D3D11Device, IDXGISwapChain3* SwapChain, UINT Count, std::vector<ComPtr<ID3D11RenderTargetView>>& OutRTVArray);

	HRESULT CreatePixelShader(ID3D11Device* D3D11Device, LPCWSTR FileName, ID3D11PixelShader** OutShader);

	HRESULT CreateVertexShader(ID3D11Device* D3D11Device, LPCWSTR FileName, ID3D11VertexShader** OutShader, ID3DBlob** OutBlob);

	HRESULT CreateInputLayout(ID3D11Device* D3D11Device, D3D11_INPUT_ELEMENT_DESC* layout, UINT ArraySize, ID3DBlob* VertexShaderBlob, ID3D11InputLayout** OutLayout);

	template <typename VertexStruct, size_t size>
	HRESULT CreateVertexBuffer(
		ID3D11Device* D3D11Device,
		VertexStruct(&Vertices)[size],
		ID3D11Buffer** OutBuffer,
		VertexBufferConfig BufferConfig = {}
	) {

		D3D11_BUFFER_DESC VertexBufferDesc = {
			sizeof(Vertices),
			BufferConfig.Usage,
			BufferConfig.BindFlags,
			BufferConfig.CPUAccessFlags,
			BufferConfig.MiscFlags,
			BufferConfig.StructureByteStride
		};

		D3D11_SUBRESOURCE_DATA initBufferData = { Vertices, 0, 0 };

		return D3D11Device->CreateBuffer(&VertexBufferDesc, &initBufferData, OutBuffer);
	};

	HRESULT CreateIndexBuffer(ID3D11Device* D3D11Device, const void* Indices, UINT ByteWidth, ID3D11Buffer** OutBuffer, IndexBufferConfig BufferConfig = {});

	HRESULT CreateConstantBuffer(ID3D11Device* D3D11Device, const void* Data, UINT ByteWidth, ID3D11Buffer** OutBuffer, ConstantBufferConfig BufferConfig = {});

	HRESULT CreateTexture2D(ID3D11Device* D3D11Device, const D3D11_TEXTURE2D_DESC* Desc, const D3D11_SUBRESOURCE_DATA* InitialData, ID3D11Texture2D** OutTexture);

	HRESULT CreateSamplerState(ID3D11Device* D3D11Device, const D3D11_SAMPLER_DESC* Desc, ID3D11SamplerState** OutSampler);

	HRESULT CreateRasterizerState(ID3D11Device* D3D11Device, const D3D11_RASTERIZER_DESC* Desc, ID3D11RasterizerState** OutState);

	HRESULT CreateBlendState(ID3D11Device* D3D11Device, const D3D11_BLEND_DESC* Desc, ID3D11BlendState** OutState);

	HRESULT CreateDepthStencilState(ID3D11Device* D3D11Device, const D3D11_DEPTH_STENCIL_DESC* Desc, ID3D11DepthStencilState** OutState);

	
	

};


#endif