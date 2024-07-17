#pragma once
#include "LeanWindows.h"
#include "WinException.h"

class Window {
    public:
        class Exception : public WinException {
        public:
            Exception(int line, const char* file, HRESULT hr) noexcept;
            const char* what() const noexcept override;
            virtual const char* getType() const noexcept;
            static std::string tranlsateErrorCode(HRESULT hr) noexcept;
            HRESULT getErrorCode() const noexcept;
            std::string getErrorString() const noexcept;
        private:
            HRESULT hr;
        };

    private:
        class WindowClass {
        public:
            static const char* GetName() noexcept;
            static HINSTANCE GetInstance() noexcept;
        private:
            WindowClass() noexcept;
            ~WindowClass();
            WindowClass( const WindowClass& ) = delete;
            WindowClass& operator=( const WindowClass& ) = delete;
            static constexpr const char*  wndClassName = "Direct3D Engine Window";
            static WindowClass wndClass;
            HINSTANCE hInst;
        };
    public:
        Window(int wifth, int height, const char* name);
        ~Window();
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
    private:
        static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
        static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

        LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    private:
        int width;
        int height;
        HWND hWnd;
};

// Error exception helper macros
#define WND_EXCEPT( hr ) Window::Exception( __LINE__, __FILE__, hr )
#define WND_LAST_EXCEPT() Window::Exception( __LINE__, __FILE__, GetLastError() )
