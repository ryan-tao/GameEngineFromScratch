#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <d2d1.h>

ID2D1Factory *pFactory = nullptr;
ID2D1HwndRenderTarget *pRenderTarget = nullptr;
ID2D1SolidColorBrush *pLightSlateGrayBrush = nullptr;
ID2D1SolidColorBrush *pCornFlowerBlueBrush = nullptr;

template<class T> inline void SafeRelease(T **ppInterfaceToRelease)
{
	if ((*ppInterfaceToRelease) != nullptr)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}

HRESULT CreateGraphicResources(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (pRenderTarget == nullptr)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), &pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &pLightSlateGrayBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &pCornFlowerBlueBrush);
		}
	}

	return hr;
}

void DiscardGraphicResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pLightSlateGrayBrush);
	SafeRelease(&pCornFlowerBlueBrush);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		return -1;
	}

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(0, _T("WindowClass1"), _T("Hello, Engine![Direct2D]"), WS_OVERLAPPEDWINDOW, 100, 100, 960, 540, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	bool wasHandled = false;

	switch (message)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			result = -1;
			return result;
		}

		wasHandled = true;
		break;

	case WM_PAINT:
	{
		HRESULT hr = CreateGraphicResources(hWnd);
		if (SUCCEEDED(hr))
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			pRenderTarget->BeginDraw();
			pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
			int width = static_cast<int>(rtSize.width);
			int height = static_cast<int>(rtSize.height);

			for (int i = 0; i < width; i += 10)
			{
				pRenderTarget->DrawLine(D2D1::Point2F(static_cast<float>(i), 0.0f), D2D1::Point2F(static_cast<float>(i), rtSize.height), pLightSlateGrayBrush, 0.5f);
			}

			for (int i = 0; i < height; i += 10)
			{
				pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<float>(i)), D2D1::Point2F(rtSize.width, static_cast<float>(i)), pLightSlateGrayBrush, 0.5f);
			}

			D2D1_RECT_F rectangle1 = D2D1::RectF(rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f, rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f);
			D2D1_RECT_F rectangle2 = D2D1::RectF(rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f, rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f);
			pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);
			pRenderTarget->DrawRectangle(&rectangle2, pCornFlowerBlueBrush);

			hr = pRenderTarget->EndDraw();

			if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
			{
				DiscardGraphicResources();
			}

			EndPaint(hWnd, &ps);
		}
	}
		wasHandled = true;
		break;
	
	case WM_SIZE:
		if (pRenderTarget != nullptr)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
			pRenderTarget->Resize(&size);
		}

		wasHandled = true;
		break;

	case WM_DESTROY:
		DiscardGraphicResources();
		if (pFactory != nullptr)
		{
			pFactory->Release();
			pFactory = nullptr;
		}

		PostQuitMessage(0);
		result = 0;
		wasHandled = true;
		break;

	case WM_DISPLAYCHANGE:
		InvalidateRect(hWnd, nullptr, false);
		wasHandled = true;
		break;
	}

	if (!wasHandled)
	{
		result = DefWindowProc(hWnd, message, wParam, lParam);
	}

	return result;
}
