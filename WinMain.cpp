#include "Window.h"


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
) {

	try {
		// create window instance
		Window wnd(800, 600, "Direct3D Engine Window");

		// process messages from Windows
		MSG msg;
		BOOL gResult;

		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// handle exit code
		if (gResult == -1) {
			return -1;
		}

		return (int)msg.wParam;
	}
	catch (const WinException& e) {
		MessageBox(nullptr, e.what(), e.getType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch ( ... ) {
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
