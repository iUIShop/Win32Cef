#include "OsrRenderHandler.h"

#pragma comment (lib, "Msimg32.lib")

void COsrRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 50;
    rect.y = 50;
    rect.width = 300;
    rect.height = 300;
}

bool g_bPopupOsr = false;
void RenderBitmapWithAlpha(HWND hwnd, const void* pBuffer, int nWidth, int nHeight)
{
	HDC hdcScreen = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight; // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32; // 32bpp
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	void* pBits = NULL;
	HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	memcpy(pBits, pBuffer, nWidth * nHeight * 4);

	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);

	SIZE sizeWindow = { nWidth, nHeight };
	POINT ptSrc = { 0, 0 };

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// ¥Ú”°µΩ∆¡ƒª…œ
	if (!g_bPopupOsr)
	{
		::AlphaBlend(hdcScreen, 0, 0, nWidth, nHeight, hdcMem, 0, 0, nWidth, nHeight, blend);
	}

	//BOOL bRet = ::UpdateLayeredWindow(hwnd, hdcScreen, NULL, &sizeWindow, hdcMem, &ptSrc, 0, &blend, 2);

	// Clean
	ReleaseDC(NULL, hdcScreen);
	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbm);
	DeleteDC(hdcMem);
}

void COsrRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList& dirtyRects,
    const void* buffer,
    int width,
    int height)
{
	HWND h = browser->GetHost()->GetWindowHandle();
	RenderBitmapWithAlpha(h, buffer, width, height);
    int m = 0;
}
