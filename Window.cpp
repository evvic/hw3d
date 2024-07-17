#include "Window.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
    : hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = static_cast<HICON>(
        LoadImage(
            GetInstance(),
            MAKEINTRESOURCE(IDI_ICON1),
            IMAGE_ICON,
            32, 32, 0
        )
    );
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = static_cast<HICON>(
        LoadImage(
            GetInstance(),
            MAKEINTRESOURCE(IDI_ICON1),
            IMAGE_ICON,
            16, 16, 0
        )
    );
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

Window::Window(int width, int height, const char* name) {
    // calculate window size based on desired client region size
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;

    if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))) {
        throw WND_LAST_EXCEPT();
    }

    // create window & get hWnd
    hWnd = CreateWindow(
        WindowClass::GetName(),
        name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        WindowClass::GetInstance(),
        this
    );
    // check if error making window
    if (hWnd == nullptr) {
        throw WND_LAST_EXCEPT();
    }

    // show window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window() {
    DestroyWindow(hWnd);
}

// callback function passed into WindowClass constructor to setup window
// function wraps around DefWindowProc and has extra logic for WM_NCCREATE message
// only used for setting up pointer of the windows instance to the Win32 API
LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    // WM_NCCREATE is a special message that is sent before window creation
    // for WM_NCCREATE, WPARAM is empty but LPARAM has 
    if (msg == WM_NCCREATE) {
        // extract ptr to window class from creation data (lparam contains ptr to CREATESTRUCT struct)
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window class handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCCREATE message, handle with default handler
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// callback function used instead of HandleMsgSetup after window has been created
// wrapper for HandleMsg that retrieves ptr to window class and calls HandleMsg
LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    // retrieve ptr to window class
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window class handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

// actual message handler for the window class
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    switch (msg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        // return here to handle program exiting
        // Window destructor will run and clean up window
        return 0;
        break;
    case WM_KILLFOCUS:
        kbd.clearState();
        break;
    /*** Keyboard messages ***/
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: // syskey commands should also be handled (ALT key)
        if (!(lParam & 0x40000000) || kbd.autoRepeatIsEnabled()) {
            kbd.onKeyPressed(static_cast<unsigned char>(wParam));
        }
        if (wParam == 'F')
        {
            SetWindowText(hWnd, "Sonia");
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        kbd.onKeyReleased(static_cast<unsigned char>(wParam));
        if (wParam == 'F')
        {
            SetWindowText(hWnd, "Sucks");
        }
        break;
    case WM_CHAR:
        kbd.onChar(static_cast<unsigned char>(wParam));
        break;
    }
    /*** End Keyboard messages ***/
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


// Windows Exceptions
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
    :
    WinException(line, file),
    hr(hr)
{}

const char* Window::Exception::what() const noexcept {
    std::ostringstream oss;
    oss << getType() << std::endl << "[Error Code] " << getErrorCode() << std::endl
        << "[Description] " << getErrorString() << std::endl;

    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::getType() const noexcept {
    return "Window Exception";
}

std::string Window::Exception::tranlsateErrorCode(HRESULT hr) noexcept {
    char* pMsgBuf = nullptr;

    // windows will allocate memory for err string and make our pointer point to it
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0, nullptr
    );

    // string length of 0 means it failed
    if (nMsgLen == 0) {
        return "Unidentified error code";
    }

    // copy error string from windows-allocated buffer to std::string
    std::string errorString = pMsgBuf;

    // free windows buffer
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT Window::Exception::getErrorCode() const noexcept {
    return hr;
}

std::string Window::Exception::getErrorString() const noexcept {
    return tranlsateErrorCode(hr);
}
