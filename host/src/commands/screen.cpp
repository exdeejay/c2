#include "commands.h"

#include <vector>
#include <lodepng.h>
#include <Windows.h>


int screenshot(Controller* ctrl) {
	HDC screenDC = GetDC(nullptr);
	HDC bitmapDC = CreateCompatibleDC(screenDC);
	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int width = x2 - x;
	int height = y2 - y;
	HBITMAP hBmp = CreateCompatibleBitmap(screenDC, width, height);
	SelectObject(bitmapDC, hBmp);
	BitBlt(bitmapDC, 0, 0, width, height, screenDC, x, y, SRCCOPY);

	BITMAPINFOHEADER bi = {};
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	GetDIBits(screenDC, hBmp, 0, height, nullptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	unsigned char* pixels = new unsigned char[bi.biSizeImage];
	GetDIBits(screenDC, hBmp, 0, height, pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	DeleteObject(hBmp);
	DeleteObject(bitmapDC);
	ReleaseDC(nullptr, screenDC);

	int bpp = bi.biBitCount / 8;
	int numPixels = width * height;
	for (int i = 0; i < numPixels; i += 1) {
		int tmp = pixels[4*i];
		pixels[4*i] = pixels[bpp*i + 2];
		pixels[4*i + 2] = tmp;
	}

	unsigned char* png = nullptr;
	size_t pngSize;
	lodepng_encode32(&png, &pngSize, pixels, width, height);

	//ctrl->sendBuffer((char*) png, pngSize);
	free(png);

	return 0;
}