#include "Keyboard.h"

bool Keyboard::keyIsPressed(unsigned char keycode) const noexcept {
    return keystates[keycode];
}

Keyboard::Event Keyboard::readKey() noexcept {
    if (keybuffer.size() > 0u) {
        Keyboard::Event e = keybuffer.front();
        keybuffer.pop();
        return e;
    }
    else {
        // default constructor represents an Invalid key
        return Keyboard::Event();
    }
}

bool Keyboard::keyIsEmpty() const noexcept {
    return keybuffer.empty();
}

char Keyboard::readChar() noexcept {
    if (charbuffer.size() > 0u) {
        unsigned char charcode = charbuffer.front();
        keybuffer.pop();
        return charcode;
    }
    else {
        return 0;
    }
}

bool Keyboard::charIsEmpty() const noexcept {
    return charbuffer.empty();
}

void Keyboard::flushKey() noexcept {
    keybuffer = std::queue<Event>();
}

void Keyboard::flushChar() noexcept {
    charbuffer = std::queue<char>();
}

void Keyboard::flush() noexcept
{
    flushKey();
    flushChar();
}

void Keyboard::enableAutoRepeat() noexcept {
    autoRepeatEnabled = true;
}

void Keyboard::disableAutoRepeat() noexcept {
    autoRepeatEnabled = false;
}

bool Keyboard::autoRepeatIsEnabled() const noexcept
{
    return autoRepeatEnabled;
}

void Keyboard::onKeyPressed(unsigned char keycode) noexcept {
    keystates[keycode] = true;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    trimBuffer(keybuffer);
}

void Keyboard::onKeyReleased(unsigned char keycode) noexcept {
    keystates[keycode] = false;
    keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    trimBuffer(keybuffer);
}

void Keyboard::onChar(char character) noexcept {
    charbuffer.push(character);
    trimBuffer(charbuffer);
}

void Keyboard::clearState() noexcept {
    keystates.reset();
}

template<typename T>
void Keyboard::trimBuffer(std::queue<T>& buffer) noexcept {
    while (buffer.size() > bufferSize) {
        buffer.pop();
    }
}
