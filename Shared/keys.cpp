#include "keys.h"

static unsigned char scan_code[0x80] =
{
    0, //0x00
    0, //0x01
    0, //0x02
    0, //0x03
    0, //0x04
    0, //0x05
    0, //0x06
    0, //0x07
    0, //0x08
    0, //0x09
    KEY_NUM_1, //0x0A
    KEY_NUM_2, //0x0B
    KEY_NUM_3, //0x0C
    KEY_NUM_4, //0x0D
    KEY_NUM_5, //0x0E
    KEY_NUM_6, //0x0F

    KEY_NUM_7, //0x10
    KEY_NUM_8, //0x11
    KEY_NUM_9, //0x12
    KEY_NUM_0, //0x13
    0, //0x14
    0, //0x15
    0, //0x16
    0, //0x17
    KEY_Q, //0x18
    KEY_W, //0x19
    KEY_E, //0x1A
    KEY_R, //0x1B
    KEY_T, //0x1C
    KEY_Y, //0x1D
    KEY_U, //0x1E
    KEY_I, //0x1F

    KEY_O, //0x20
    KEY_P, //0x21
    0, //0x22
    0, //0x23
    KEY_RETURN, //0x24
    KEY_LCONTROL, //0x25
    KEY_A, //0x26
    KEY_S, //0x27
    KEY_D, //0x28
    KEY_F, //0x29
    KEY_G, //0x2A
    KEY_H, //0x2B
    KEY_J, //0x2C
    KEY_K, //0x2D
    KEY_L, //0x2E
    0, //0x2F

    0, //0x30
    0, //0x31
    KEY_LSHIFT, //0x32
    0, //0x33
    KEY_Z, //0x34
    KEY_X, //0x35
    KEY_C, //0x36
    KEY_V, //0x37
    KEY_B, //0x38
    KEY_N, //0x39
    KEY_M, //0x3A
    KEY_BC, //0x3B
    KEY_BE, //0x3C
    KEY_BF, //0x3D
    KEY_RSHIFT, //0x3E
    0, //0x3F


    0, //0x40
    KEY_SPACE, //0x41
    0, //0x42
    KEY_F1, //0x43
    0, //0x44
    KEY_F3, //0x45
    KEY_F4, //0x46
    KEY_F5, //0x47
    KEY_F6, //0x48
    KEY_F7, //0x49
    KEY_F8, //0x4A
    KEY_F9, //0x4B
    KEY_F10, //0x4C
    0, //0x4D
    0, //0x4E
    KEY_NUMPAD_7, //0x4F


    KEY_NUMPAD_8, //0x50
    KEY_NUMPAD_9, //0x51
    0, //0x52
    KEY_NUMPAD_4, //0x53
    KEY_NUMPAD_5, //0x54
    KEY_NUMPAD_6, //0x55
    0, //0x56
    KEY_NUMPAD_1, //0x57
    KEY_NUMPAD_2, //0x58
    KEY_NUMPAD_3, //0x59
    KEY_NUMPAD_0, //0x5A
    0, //0x5B
    0, //0x5C
    0, //0x5D
    0, //0x5E
    KEY_F11, //0x5F

    KEY_F12, //0x60
    0, //0x61
    0, //0x62
    0, //0x63
    0, //0x64
    0, //0x65
    0, //0x66
    0, //0x67
    0, //0x68
    KEY_RCONTROL, //0x69
    0, //0x6A
    0, //0x6B
    0, //0x6C
    0, //0x6D
    0, //0x6E
    KEY_UP, //0x6F

    0, //0x70
    KEY_LEFT, //0x71
    KEY_RIGHT, //0x72
    0, //0x73
    KEY_DOWN, //0x74
    0, //0x75
    0, //0x76
    KEY_DELETE, //0x77
    0, //0x78
    0, //0x79
    0, //0x7A
    0, //0x7B
    0, //0x7C
    0, //0x7D
    0, //0x7E
    0, //0x7F
/*
    0, //0x80
    0, //0x81
    0, //0x82
    0, //0x83
    0, //0x84
    0, //0x85
    0, //0x86
    0, //0x87
    0, //0x88
    0, //0x89
    0, //0x8A
    0, //0x8B
    0, //0x8C
    0, //0x8D
    0, //0x8E
    0, //0x8F

    0, //0x90
    0, //0x91
    0, //0x92
    0, //0x93
    0, //0x94
    0, //0x95
    0, //0x96
    0, //0x97
    0, //0x98
    0, //0x99
    0, //0x9A
    0, //0x9B
    0, //0x9C
    0, //0x9D
    0, //0x9E
    0, //0x9F

    0, //0xA0
    0, //0xA1
    0, //0xA2
    0, //0xA3
    0, //0xA4
    0, //0xA5
    0, //0xA6
    0, //0xA7
    0, //0xA8
    0, //0xA9
    0, //0xAA
    0, //0xAB
    0, //0xAC
    0, //0xAD
    0, //0xAE
    0, //0xAF

    0, //0xB0
    0, //0xB1
    0, //0xB2
    0, //0xB3
    0, //0xB4
    0, //0xB5
    0, //0xB6
    0, //0xB7
    0, //0xB8
    0, //0xB9
    0, //0xBA
    0, //0xBB
    0, //0xBC
    0, //0xBD
    0, //0xBE
    0, //0xBF

    0, //0xC0
    0, //0xC1
    0, //0xC2
    0, //0xC3
    0, //0xC4
    0, //0xC5
    0, //0xC6
    0, //0xC7
    0, //0xC8
    0, //0xC9
    0, //0xCA
    0, //0xCB
    0, //0xCC
    0, //0xCD
    0, //0xCE
    0, //0xCF

    0, //0xE0
    0, //0xE1
    0, //0xE2
    0, //0xE3
    0, //0xE4
    0, //0xE5
    0, //0xE6
    0, //0xE7
    0, //0xE8
    0, //0xE9
    0, //0xEA
    0, //0xEB
    0, //0xEC
    0, //0xED
    0, //0xEE
    0, //0xEF

    0, //0xF0
    0, //0xF1
    0, //0xF2
    0, //0xF3
    0, //0xF4
    0, //0xF5
    0, //0xF6
    0, //0xF7
    0, //0xF8
    0, //0xF9
    0, //0xFA
    0, //0xFB
    0, //0xFC
    0, //0xFD
    0, //0xFE
    0, //0xFF
    */
};


uint8_t scan_code_to_key(uint16_t c)
{
    if (c >= 0x80)
        return 0;

    return scan_code[c];
}
