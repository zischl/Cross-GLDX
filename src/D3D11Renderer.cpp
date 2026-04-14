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

	HRESULT hr = D3D11Device->CreateTexture2D(&desc, nullptr, Out.GetAddressOf());

 
}


D3D11Renderer::D3D11Renderer() {


};

//Device DeviceStruct = Renderer.CreateD3d11Device(featureLevels, _countof(featureLevels), creationFlags);
D3DDevice D3D11Renderer::CreateD3d11Device(D3D_FEATURE_LEVEL (FeatureLevels)[],UINT FeatureLevelCount, UINT& CreationFlags) {
	D3DDevice DeviceStruct = {};
	D3D_FEATURE_LEVEL SelectedFeatureLevel;

#if defined(_DEBUG)
	CreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, CreationFlags,
		FeatureLevels, FeatureLevelCount, D3D11_SDK_VERSION,
		&DeviceStruct.D3D11Device, &SelectedFeatureLevel, &DeviceStruct.D3D11Context);
	if (FAILED(hr)) {
		OutputDebugString("D3D11 Device Creation Failed.\n");
	}

	return DeviceStruct;
}



ComPtr<IDXGIFactory2> D3D11Renderer::CreateDXGIFactory2() {
	ComPtr<IDXGIFactory> factory = nullptr;
	ComPtr<IDXGIFactory2> Factory2 = nullptr;

	hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	if (FAILED(hr)) {
		OutputDebugString("Factory Creation Failed.\n");
	}

	hr = factory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&Factory2);
	if (FAILED(hr)) {
		OutputDebugString("Factory2 Query Failed.\n");
	}

	return Factory2;
}

ComPtr<IDXGISwapChain3> D3D11Renderer::CreateSwapChain(
		ID3D11Device* D3D11Device, 
		HWND& hwnd, 
		UINT Width, 
		UINT Height, 
		IDXGIFactory2* Factory2,
		SwapChainConfig SwapChainConfig
) {

	ComPtr<IDXGISwapChain1> SwapChain = nullptr;
	ComPtr<IDXGISwapChain3> SwapChain3 = nullptr;

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


	hr = Factory2->CreateSwapChainForHwnd(D3D11Device, hwnd, &swapchainConfig, nullptr, nullptr, &SwapChain);
	if (FAILED(hr)) {
		OutputDebugString("Swapchain Creation Failed.\n");
	}
	
	SwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&SwapChain3);

	return SwapChain3;
}

ComPtr<ID3D11Texture2D> D3D11Renderer::GetSwapChainBuffer(IDXGISwapChain3* SwapChain, UINT Buffer) {

	ComPtr<ID3D11Texture2D> mainBuffer;
	hr = SwapChain->GetBuffer(Buffer, IID_PPV_ARGS(&mainBuffer));
	if (FAILED(hr)) {
		OutputDebugString("Swapchain Get Buffer Failed.\n");
	}

	return mainBuffer;
}

std::vector<ComPtr<ID3D11Texture2D>> D3D11Renderer::GetSwapChainBuffersArray(IDXGISwapChain3* SwapChain, UINT Count) {

	std::vector<ComPtr<ID3D11Texture2D>> SCBArray = {};

	for (int count = 0; count < Count; count++) {
		ComPtr<ID3D11Texture2D> mainBuffer;
		hr = SwapChain->GetBuffer(count, IID_PPV_ARGS(&mainBuffer));
		if (FAILED(hr)) {
			OutputDebugString("Swapchain Get Buffer Array Failed.\n");
		}
		SCBArray.push_back(mainBuffer);
	};

	return SCBArray;
}

ComPtr<ID3D11RenderTargetView> D3D11Renderer::CreateRTV(ID3D11Device* D3D11Device, ID3D11Texture2D* targetBuffer) {
	ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;

	hr = D3D11Device->CreateRenderTargetView(targetBuffer, nullptr, &renderTargetView);
	if (FAILED(hr)) {
		OutputDebugString("RTV Creation Failed. \n");
	}

	return renderTargetView;
}

//not for dx 11
std::vector <ComPtr<ID3D11RenderTargetView>> D3D11Renderer::CreateRTVArray(ID3D11Device* D3D11Device, IDXGISwapChain3* SwapChain, UINT Count) {

	std::vector<ComPtr<ID3D11RenderTargetView>> RTVArray_cp = {};

	for (int count = 0; count < Count; count++) {
		OutputDebugString((std::to_string(count) + "\n").c_str());
		ComPtr<ID3D11Texture2D> Buffer;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		hr = SwapChain->GetBuffer(count, IID_PPV_ARGS(&Buffer));
		if (FAILED(hr)) {
			OutputDebugString("Swapchain Get Buffer Failed.\n");
		}

		hr = D3D11Device->CreateRenderTargetView(Buffer.Get(), nullptr, &renderTargetView);
		if (FAILED(hr)) {
			OutputDebugString("RTV Creation Failed.\n");
		}
		else {
			OutputDebugString("RTV Creation succeeded.\n");
		}

		RTVArray_cp.push_back(renderTargetView);
	};

	return RTVArray_cp;
}



ComPtr<ID3D11PixelShader> D3D11Renderer::CreatePixelShader(ID3D11Device* D3D11Device, LPCWSTR FileName) {
	
	ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
	hr = D3DReadFileToBlob(FileName, &pixelShaderBlob);
	if (hr != S_OK) {
		Logger::log("Failed to read pixel shader file to blob");
	}

	ComPtr<ID3D11PixelShader> pixelShader;

	D3D11Device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr, &pixelShader);

	return pixelShader;
}

ComPtr<ID3D11VertexShader> D3D11Renderer::CreateVertexShader(ID3D11Device* D3D11Device, LPCWSTR FileName, ID3DBlob** VertexShaderBlobTempParam) {
	
	D3DReadFileToBlob(FileName, &VertexShaderBlobTemp);

	ComPtr<ID3D11VertexShader> vertexShader;

	D3D11Device->CreateVertexShader(
		VertexShaderBlobTemp->GetBufferPointer(),
		VertexShaderBlobTemp->GetBufferSize(),
		nullptr, &vertexShader);

	if (VertexShaderBlobTempParam) {
		*VertexShaderBlobTempParam = VertexShaderBlobTemp.Get();
		VertexShaderBlobTemp->AddRef();
	}

	return vertexShader;
}

ComPtr<ID3D11InputLayout> D3D11Renderer::CreateInputLayout(ID3D11Device* D3D11Device, D3D11_INPUT_ELEMENT_DESC* layout, UINT ArraySize) {

	ComPtr<ID3D11InputLayout> inputLayout;

	if (VertexShaderBlobTemp) {
		D3D11Device->CreateInputLayout(layout, ArraySize, VertexShaderBlobTemp->GetBufferPointer(),
			VertexShaderBlobTemp->GetBufferSize(), &inputLayout);
	}

	return inputLayout;
}


ComPtr<ID3D11Buffer> D3D11Renderer::CreateIndexBuffer(ID3D11Device* D3D11Device, const unsigned short(&Indices)[], UINT ArraySize, IndexBufferConfig BufferConfig) {
	
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * ArraySize;
	indexBufferDesc.Usage = BufferConfig.Usage;
	indexBufferDesc.BindFlags = BufferConfig.BindFlags;
	indexBufferDesc.CPUAccessFlags = BufferConfig.CPUAccessFlags;
	indexBufferDesc.MiscFlags = BufferConfig.MiscFlags;
	indexBufferDesc.StructureByteStride = BufferConfig.StructureByteStride;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = Indices;
	indexBufferData.SysMemPitch = BufferConfig.SysMemPitch;
	indexBufferData.SysMemSlicePitch = BufferConfig.SysMemSlicePitch;
	
	ComPtr<ID3D11Buffer> indexBuffer;
	D3D11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

	return indexBuffer;
}



void D3D11Renderer::SetViewPort(ID3D11DeviceContext* D3D11Context) {
	
}