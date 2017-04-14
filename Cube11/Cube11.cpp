#include "Cube11.h"

Cube11::Cube11() : m_frameCount(0) { }

Cube11::~Cube11() { }

void Cube11::CreateDevice() {

}

void Cube11::CreateWindowSizeDependentResources(HWND window) {

}

void Cube11::CreateShaders() {

}

void Cube11::CreateCube() {

}

void Cube11::CreateViewMatrix() {

}

void Cube11::CreatePerspectiveMatrix(float width, float height) {

}

void Cube11::CreateDeviceResources() {

}

void Cube11::RenderFrame() {

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	const wchar_t CLASSNAME[] = L"Cube11";

	WNDCLASS wc = {};
	wc.lpszClassName = CLASSNAME;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
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

		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
