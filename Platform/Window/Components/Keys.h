//
// Created by IDKTHIS on 28.07.2025.
//

#pragma once

// Enum values are ordered and commented to describe each key's purpose
enum class KeyCode {
    KEY_UNKNOWN = 0,

    // Alphabet keys A-Z
    KEY_A = 0x41, // 'A' key
    KEY_B,        // 'B' key
    KEY_C,        // 'C' key
    KEY_D,        // 'D' key
    KEY_E,        // 'E' key
    KEY_F,        // 'F' key
    KEY_G,        // 'G' key
    KEY_H,        // 'H' key
    KEY_I,        // 'I' key
    KEY_J,        // 'J' key
    KEY_K,        // 'K' key
    KEY_L,        // 'L' key
    KEY_M,        // 'M' key
    KEY_N,        // 'N' key
    KEY_O,        // 'O' key
    KEY_P,        // 'P' key
    KEY_Q,        // 'Q' key
    KEY_R,        // 'R' key
    KEY_S,        // 'S' key
    KEY_T,        // 'T' key
    KEY_U,        // 'U' key
    KEY_V,        // 'V' key
    KEY_W,        // 'W' key
    KEY_X,        // 'X' key
    KEY_Y,        // 'Y' key
    KEY_Z,        // 'Z' key

    // Number keys 0-9, located above the alphabet keys
    KEY_0 = 0x30, // '0' key (top row)
    KEY_1,        // '1' key (top row)
    KEY_2,        // '2' key (top row)
    KEY_3,        // '3' key (top row)
    KEY_4,        // '4' key (top row)
    KEY_5,        // '5' key (top row)
    KEY_6,        // '6' key (top row)
    KEY_7,        // '7' key (top row)
    KEY_8,        // '8' key (top row)
    KEY_9,        // '9' key (top row)

    // Function keys F1-F24
    KEY_F1 = 0x70,  // F1 key
    KEY_F2,         // F2 key
    KEY_F3,         // F3 key
    KEY_F4,         // F4 key
    KEY_F5,         // F5 key
    KEY_F6,         // F6 key
    KEY_F7,         // F7 key
    KEY_F8,         // F8 key
    KEY_F9,         // F9 key
    KEY_F10,        // F10 key
    KEY_F11,        // F11 key
    KEY_F12,        // F12 key
    KEY_F13,        // F13 key
    KEY_F14,        // F14 key
    KEY_F15,        // F15 key
    KEY_F16,        // F16 key
    KEY_F17,        // F17 key
    KEY_F18,        // F18 key
    KEY_F19,        // F19 key
    KEY_F20,        // F20 key
    KEY_F21,        // F21 key
    KEY_F22,        // F22 key
    KEY_F23,        // F23 key
    KEY_F24,        // F24 key

    // Modifier and control keys
    KEY_ESCAPE        = 0x1B, // Escape key
    KEY_TAB           = 0x09, // Tab key
    KEY_CAPS_LOCK     = 0x14, // Caps Lock key
    KEY_LEFT_SHIFT    = 0xA0, // Left Shift key
    KEY_RIGHT_SHIFT   = 0xA1, // Right Shift key
    KEY_LEFT_CONTROL  = 0xA2, // Left Control key
    KEY_RIGHT_CONTROL = 0xA3, // Right Control key
    KEY_LEFT_ALT      = 0xA4, // Left Alt key
    KEY_RIGHT_ALT     = 0xA5, // Right Alt key
    KEY_LEFT_SUPER    = 0x5B, // Left Windows/Command key
    KEY_RIGHT_SUPER   = 0x5C, // Right Windows/Command key
    KEY_MENU          = 0x5D, // Menu/Application key

    // Symbol keys (usually on US QWERTY layout)
    KEY_GRAVE         = 0xC0, // '`' (tilde/grave accent) key
    KEY_MINUS         = 0xBD, // '-' (minus/underscore) key
    KEY_EQUALS        = 0xBB, // '=' (equals/plus) key
    KEY_LEFT_BRACKET  = 0xDB, // '[' key
    KEY_RIGHT_BRACKET = 0xDD, // ']' key
    KEY_BACKSLASH     = 0xDC, // '\' key
    KEY_SEMICOLON     = 0xBA, // ';' key
    KEY_APOSTROPHE    = 0xDE, // '\'' (apostrophe/quote) key
    KEY_COMMA         = 0xBC, // ',' key
    KEY_PERIOD        = 0xBE, // '.' key
    KEY_SLASH         = 0xBF, // '/' key

    // Special keys
    KEY_SPACE         = 0x20, // Space bar
    KEY_ENTER         = 0x0D, // Enter/Return key
    KEY_BACKSPACE     = 0x08, // Backspace key
    KEY_INSERT        = 0x2D, // Insert key
    KEY_DELETE        = 0x2E, // Delete key
    KEY_HOME          = 0x24, // Home key
    KEY_END           = 0x23, // End key
    KEY_PAGE_UP       = 0x21, // Page Up key
    KEY_PAGE_DOWN     = 0x22, // Page Down key
    KEY_PRINT_SCREEN  = 0x2C, // Print Screen key
    KEY_SCROLL_LOCK   = 0x91, // Scroll Lock key
    KEY_PAUSE         = 0x13, // Pause/Break key

    // Arrow keys
    KEY_LEFT          = 0x25, // Left Arrow key
    KEY_UP            = 0x26, // Up Arrow key
    KEY_RIGHT         = 0x27, // Right Arrow key
    KEY_DOWN          = 0x28, // Down Arrow key

    // Numpad keys (numeric keypad)
    KEY_NUM_LOCK      = 0x90, // Num Lock key
    KEY_NUMPAD_0      = 0x60, // Numpad 0 key
    KEY_NUMPAD_1      = 0x61, // Numpad 1 key
    KEY_NUMPAD_2      = 0x62, // Numpad 2 key
    KEY_NUMPAD_3      = 0x63, // Numpad 3 key
    KEY_NUMPAD_4      = 0x64, // Numpad 4 key
    KEY_NUMPAD_5      = 0x65, // Numpad 5 key
    KEY_NUMPAD_6      = 0x66, // Numpad 6 key
    KEY_NUMPAD_7      = 0x67, // Numpad 7 key
    KEY_NUMPAD_8      = 0x68, // Numpad 8 key
    KEY_NUMPAD_9      = 0x69, // Numpad 9 key
    KEY_NUMPAD_DECIMAL   = 0x6E, // Numpad '.' (decimal point) key
    KEY_NUMPAD_DIVIDE    = 0x6F, // Numpad '/' (divide) key
    KEY_NUMPAD_MULTIPLY  = 0x6A, // Numpad '*' (multiply) key
    KEY_NUMPAD_SUBTRACT  = 0x6D, // Numpad '-' (subtract) key
    KEY_NUMPAD_ADD       = 0x6B, // Numpad '+' (add) key
    KEY_NUMPAD_ENTER     = 0x0D, // Numpad Enter key (sometimes same as main Enter)

    // Multimedia and other special keys
    KEY_VOLUME_UP        = 0xAF, // Volume Up key
    KEY_VOLUME_DOWN      = 0xAE, // Volume Down key
    KEY_MUTE             = 0xAD, // Mute key
    KEY_MEDIA_NEXT       = 0xB0, // Next Track key
    KEY_MEDIA_PREV       = 0xB1, // Previous Track key
    KEY_MEDIA_STOP       = 0xB2, // Stop Media key
    KEY_MEDIA_PLAY_PAUSE = 0xB3  // Play/Pause Media key
};

enum class KeyState {
    Up,
    Down,
    Pressed,
    Released
};

struct MouseState {
    int x = 0;
    int y = 0;
    int deltaX = 0;
    int deltaY = 0;
    bool buttons[5] = { false };
};
