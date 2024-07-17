#include "WinException.h"
#include <sstream>

WinException::WinException(int line, const char* file) noexcept
    :
    line(line),
    file(file)
{}

const char* WinException::what() const noexcept {
    std::ostringstream oss;
    oss << getType() << std::endl << getOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* WinException::getType() const noexcept {
    return "WinException";
}

int WinException::getLine() const noexcept {
    return line;
}

const std::string& WinException::getFile() const noexcept {
    return file;
}

std::string WinException::getOriginString() const noexcept {
    std::ostringstream oss;
    oss << "[File] " << file << std::endl << "[Line] " << line;
    return oss.str();
}
