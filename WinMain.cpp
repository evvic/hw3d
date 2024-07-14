#include "Window.h"


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
) {

	// create window instance
	Window wnd(800, 600, "Direct3D Engine Window");

	// process messages from Windows
	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// handle exit code
	if (gResult == -1) {
		return -1;
	}

	return (int)msg.wParam;
}
