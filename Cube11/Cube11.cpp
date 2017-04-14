// Modified from:
// https://code.msdn.microsoft.com/windowsapps/Cube11-DirectX-9-to-3b16f777
// https://msdn.microsoft.com/en-us/library/windows/desktop/dn643743.aspx

#include "Cube11.h"
#include <fstream>
#include <vector>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Cube11::Cube11() : m_frameCount(0) {
	CreateDeviceResources();
}

Cube11::~Cube11() { }

//-----------------------------------------------------------------------------
// Create device-dependent resources.
//-----------------------------------------------------------------------------
void Cube11::CreateDeviceResources() {
	CreateDevice();
	CreateShaders();
	CreateCube();
	CreateViewMatrix();
}

//-----------------------------------------------------------------------------
// Create the Direct3D 11 device and device context.
//-----------------------------------------------------------------------------
void Cube11::CreateDevice() {
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
		featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, nullptr, &context);

	device.As(&m_d3dDevice);
	context.As(&m_d3dContext);
}

//-----------------------------------------------------------------------------
// Load compiled shader objects.
//-----------------------------------------------------------------------------
void Cube11::CreateShaders() {
	const std::istreambuf_iterator<char> EOS;

	// Process path.
	wchar_t path_buffer[_MAX_PATH];
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	GetModuleFileName(nullptr, path_buffer, _MAX_PATH);
	_wsplitpath_s(path_buffer, drive, dir, fname, ext);

	// Load vertex shader.
	std::ifstream vertexShaderFile;
	_wmakepath_s(path_buffer, drive, dir, L"CubeVertexShader", L"cso");
	vertexShaderFile.open(path_buffer, std::ios::binary);
	std::vector<char> vertexShaderData(std::istreambuf_iterator<char>(vertexShaderFile), EOS);
	vertexShaderFile.close();
	m_d3dDevice->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &m_vertexShader);
	
	// Create input layout.
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_d3dDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), 
		vertexShaderData.data(), vertexShaderData.size(), &m_inputLayout);

	// Create constant buffer.
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer);

	// Load pixel shader.
	std::ifstream pixelShaderFile;
	_wmakepath_s(path_buffer, drive, dir, L"CubePixelShader", L"cso");
	pixelShaderFile.open(path_buffer, std::ios::binary);
	std::vector<char> pixelShaderData(std::istreambuf_iterator<char>(pixelShaderFile), EOS);
	pixelShaderFile.close();
	m_d3dDevice->CreatePixelShader(pixelShaderData.data(), pixelShaderData.size(), nullptr, &m_pixelShader);
}

//-----------------------------------------------------------------------------
// Creates the vertex buffer and index buffer.
//-----------------------------------------------------------------------------
void Cube11::CreateCube() {
	// Create vertex buffer to store cube geometry.
	VertexPositionColor CubeVertices[] = {
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) },

		{ DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ DirectX::XMFLOAT3(0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ DirectX::XMFLOAT3(0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) },
		{ DirectX::XMFLOAT3(0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};
	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = CubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(CubeVertices), D3D11_BIND_VERTEX_BUFFER);
	m_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);

	// Create index buffer.
	unsigned short cubeIndices[] = {
		// -x face
		0, 2, 1, 1, 2, 3,
		// +x face
		4, 5, 6, 5, 7, 6,
		// -y face
		0, 1, 5, 0, 5, 4,
		// +y face
		2, 6, 7, 2, 7, 3,
		// -z face
		0, 4, 6, 0, 6, 2,
		// +z face
		1, 3, 7, 1, 7, 5,
	};
	m_indexCount = ARRAYSIZE(cubeIndices);
	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	m_d3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
}

//-----------------------------------------------------------------------------
// Create the view matrix.
//-----------------------------------------------------------------------------
void Cube11::CreateViewMatrix() {
	// Create the constant buffer data in system memory.
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMStoreFloat4x4(&m_constantBufferData.view, 
		DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));
}

//-----------------------------------------------------------------------------
// Create the swap chain, back buffer and viewport.
//-----------------------------------------------------------------------------
void Cube11::CreateWindowSizeDependentResources(HWND window) {
	if (m_swapChain != nullptr) {
		// If the swap chain already exists, resize it.
		m_renderTargetView = nullptr;
		m_swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	}
	else {
		// Otherwise, create a new one.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = window;

		ComPtr<IDXGIDevice> dxgiDevice;
		m_d3dDevice.As(&dxgiDevice);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		dxgiDevice->GetAdapter(&dxgiAdapter);

		ComPtr<IDXGIFactory> dxgiFactory;
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		ComPtr<IDXGISwapChain> swapChain;
		dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, &swapChain);
		swapChain.As(&m_swapChain);
	}

	// Get the back buffer resource.
	ComPtr<ID3D11Texture2D> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	// Create a render target view on the back buffer.
	m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);

	// Set the rendering viewport to target the entire window.
	D3D11_TEXTURE2D_DESC backBufferDesc = {};
	backBuffer->GetDesc(&backBufferDesc);
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, 
		static_cast<float>(backBufferDesc.Width), static_cast<float>(backBufferDesc.Height));
	m_d3dContext->RSSetViewports(1, &viewport);

	// The perspective matrix depends on the viewport aspect ratio.
	CreatePerspectiveMatrix(static_cast<float>(backBufferDesc.Width), static_cast<float>(backBufferDesc.Height));
}

//-----------------------------------------------------------------------------
// Create the perspective matrix.
//-----------------------------------------------------------------------------
void Cube11::CreatePerspectiveMatrix(float width, float height) {
	float aspectRatio = width / height;
	float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;

	DirectX::XMStoreFloat4x4(&m_constantBufferData.projection,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.01f, 100.0f)));
}

//-----------------------------------------------------------------------------
// Render the cube.
//-----------------------------------------------------------------------------
void Cube11::RenderFrame() {
	// Clear the back buffer.
	const float midnightBlue[] = { 0.098f, 0.098f, 0.439f, 1.000f };
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), midnightBlue);

	// Set the render target. This starts the drawing operation.
	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

	// Rotate the cube 1 degree per frame.
	XMStoreFloat4x4(&m_constantBufferData.model, 
		DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(m_frameCount++ * DirectX::XM_PI / 180.f)));

	// Copy the updated constant buffer from system memory to video memory.
	m_d3dContext->UpdateSubresource(m_constantBuffer.Get(),	0, nullptr,
		&m_constantBufferData, 0, 0);

	// Send vertex data to the Input Assembler stage.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_d3dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	// Set the vertex shader.
	m_d3dContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Set the vertex shader constant buffer data.
	m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Set the pixel shader.
	m_d3dContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the cube.
	m_d3dContext->DrawIndexed(m_indexCount, 0, 0);

	// Present the frame by swapping the back buffer to the screen.
	m_swapChain->Present(1, 0);
}

Cube11 cube;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZE:
		cube.CreateWindowSizeDependentResources(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASSNAME[] = L"Cube11";

	WNDCLASS wc = {};
	wc.lpszClassName = CLASSNAME;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(CLASSNAME, L"Cube11", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	MSG msg = {};
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			cube.RenderFrame();
		}
	}

	return 0;
}
