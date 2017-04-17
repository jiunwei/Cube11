#pragma once
#pragma comment(lib, "d3d11.lib")

#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

struct ModelViewProjectionConstantBuffer {
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct VertexPositionColor {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

class Cube11 {
private:
	Microsoft::WRL::ComPtr<ID3D11Device>			m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            m_constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	ModelViewProjectionConstantBuffer m_constantBufferData;
	UINT32 m_frameCount;
	UINT32 m_indexCount;
public:
	Cube11();
	virtual ~Cube11();
	void CreateDevice();
	void CreateWindowSizeDependentResources(HWND window);
	void CreateShaders();
	void CreateCube();
	void CreateViewMatrix();
	void CreatePerspectiveMatrix(float width, float height);
	void CreateDeviceResources();
	void RenderFrame();
};
