#include "wl_def.h"

#ifndef SPEAR

const unsigned char gamepal[] =
{ '\0', '\0', '\0', '\0', '\0', 0x2a, '\0', 0x2a, '\0', '\0', 0x2a,
 0x2a, 0x2a, '\0', '\0', 0x2a, '\0', 0x2a, 0x2a, 0x15, '\0', 0x2a,
 0x2a, 0x2a, 0x15, 0x15, 0x15, 0x15, 0x15, '?', 0x15, '?', 0x15,
 0x15, '?', '?', '?', 0x15, 0x15, '?', 0x15, '?', '?', '?', 0x15,
 '?', '?', '?', ';', ';', ';', '7', '7', '7', '4', '4', '4', '0',
 '0', '0', 0x2d, 0x2d, 0x2d, 0x2a, 0x2a, 0x2a, 0x26, 0x26, 0x26,
 0x23, 0x23, 0x23, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c, 0x1c, 0x19, 0x19,
 0x19, 0x15, 0x15, 0x15, 0x12, 0x12, 0x12, 0x0e, 0x0e, 0x0e, 0x0b,
 0x0b, 0x0b, 0x08, 0x08, 0x08, '?', '\0', '\0', ';', '\0', '\0',
 '8', '\0', '\0', '5', '\0', '\0', '2', '\0', '\0', 0x2f, '\0',
 '\0', 0x2c, '\0', '\0', 0x29, '\0', '\0', 0x26, '\0', '\0', 0x22,
 '\0', '\0', 0x1f, '\0', '\0', 0x1c, '\0', '\0', 0x19, '\0', '\0',
 0x16, '\0', '\0', 0x13, '\0', '\0', 0x10, '\0', '\0', '?', '6',
 '6', '?', 0x2e, 0x2e, '?', 0x27, 0x27, '?', 0x1f, 0x1f, '?',
 0x17, 0x17, '?', 0x10, 0x10, '?', 0x08, 0x08, '?', '\0', '\0',
 '?', 0x2a, 0x17, '?', 0x26, 0x10, '?', 0x22, 0x08, '?', 0x1e,
 '\0', '9', 0x1b, '\0', '3', 0x18, '\0', 0x2d, 0x15, '\0', 0x27,
 0x13, '\0', '?', '?', '6', '?', '?', 0x2e, '?', '?', 0x27, '?',
 '?', 0x1f, '?', '>', 0x17, '?', '=', 0x10, '?', '=', 0x08, '?',
 '=', '\0', '9', '6', '\0', '3', '1', '\0', 0x2d, 0x2b, '\0',
 0x27, 0x27, '\0', 0x21, 0x21, '\0', 0x1c, 0x1b, '\0', 0x16, 0x15,
 '\0', 0x10, 0x10, '\0', '4', '?', 0x17, '1', '?', 0x10, 0x2d,
 '?', 0x08, 0x28, '?', '\0', 0x24, '9', '\0', 0x20, '3', '\0',
 0x1d, 0x2d, '\0', 0x18, 0x27, '\0', '6', '?', '6', 0x2f, '?',
 0x2e, 0x27, '?', 0x27, 0x20, '?', 0x1f, 0x18, '?', 0x17, 0x10,
 '?', 0x10, 0x08, '?', 0x08, '\0', '?', '\0', '\0', '?', '\0',
 '\0', ';', '\0', '\0', '8', '\0', '\0', '5', '\0', 0x01, '2',
 '\0', 0x01, 0x2f, '\0', 0x01, 0x2c, '\0', 0x01, 0x29, '\0', 0x01,
 0x26, '\0', 0x01, 0x22, '\0', 0x01, 0x1f, '\0', 0x01, 0x1c, '\0',
 0x01, 0x19, '\0', 0x01, 0x16, '\0', 0x01, 0x13, '\0', 0x01, 0x10,
 '\0', '6', '?', '?', 0x2e, '?', '?', 0x27, '?', '?', 0x1f, '?',
 '>', 0x17, '?', '?', 0x10, '?', '?', 0x08, '?', '?', '\0', '?',
 '?', '\0', '9', '9', '\0', '3', '3', '\0', 0x2d, 0x2d, '\0',
 0x27, 0x27, '\0', 0x21, 0x21, '\0', 0x1c, 0x1c, '\0', 0x16, 0x16,
 '\0', 0x10, 0x10, 0x17, 0x2f, '?', 0x10, 0x2c, '?', 0x08, 0x2a,
 '?', '\0', 0x27, '?', '\0', 0x23, '9', '\0', 0x1f, '3', '\0',
 0x1b, 0x2d, '\0', 0x17, 0x27, '6', '6', '?', 0x2e, 0x2f, '?',
 0x27, 0x27, '?', 0x1f, 0x20, '?', 0x17, 0x18, '?', 0x10, 0x10,
 '?', 0x08, 0x09, '?', '\0', 0x01, '?', '\0', '\0', '?', '\0',
 '\0', ';', '\0', '\0', '8', '\0', '\0', '5', '\0', '\0', '2',
 '\0', '\0', 0x2f, '\0', '\0', 0x2c, '\0', '\0', 0x29, '\0', '\0',
 0x26, '\0', '\0', 0x22, '\0', '\0', 0x1f, '\0', '\0', 0x1c, '\0',
 '\0', 0x19, '\0', '\0', 0x16, '\0', '\0', 0x13, '\0', '\0', 0x10,
 '\n', '\n', '\n', '?', '8', '\r', '?', '5', 0x09, '?', '3', 0x06,
 '?', '0', 0x02, '?', 0x2d, '\0', 0x2d, 0x08, '?', 0x2a, '\0',
 '?', 0x26, '\0', '9', 0x20, '\0', '3', 0x1d, '\0', 0x2d, 0x18,
 '\0', 0x27, 0x14, '\0', 0x21, 0x11, '\0', 0x1c, '\r', '\0', 0x16,
 '\n', '\0', 0x10, '?', '6', '?', '?', 0x2e, '?', '?', 0x27, '?',
 '?', 0x1f, '?', '?', 0x17, '?', '?', 0x10, '?', '?', 0x08, '?',
 '?', '\0', '?', '8', '\0', '9', '2', '\0', '3', 0x2d, '\0', 0x2d,
 0x27, '\0', 0x27, 0x21, '\0', 0x21, 0x1b, '\0', 0x1c, 0x16, '\0',
 0x16, 0x10, '\0', 0x10, '?', ':', '7', '?', '8', '4', '?', '6',
 '1', '?', '5', 0x2f, '?', '3', 0x2c, '?', '1', 0x29, '?', 0x2f,
 0x27, '?', 0x2e, 0x24, '?', 0x2c, 0x20, '?', 0x29, 0x1c, '?',
 0x27, 0x18, '<', 0x25, 0x17, ':', 0x23, 0x16, '7', 0x22, 0x15,
 '4', 0x20, 0x14, '2', 0x1f, 0x13, 0x2f, 0x1e, 0x12, 0x2d, 0x1c,
 0x11, 0x2a, 0x1a, 0x10, 0x28, 0x19, 0x0f, 0x27, 0x18, 0x0e, 0x24,
 0x17, '\r', 0x22, 0x16, 0x0c, 0x20, 0x14, 0x0b, 0x1d, 0x13, '\n',
 0x1b, 0x12, 0x09, 0x17, 0x10, 0x08, 0x15, 0x0f, 0x07, 0x12, 0x0e,
 0x06, 0x10, 0x0c, 0x06, 0x0e, 0x0b, 0x05, '\n', 0x08, 0x03, 0x18,
 '\0', 0x19, '\0', 0x19, 0x19, '\0', 0x18, 0x18, '\0', '\0', 0x07,
 '\0', '\0', 0x0b, 0x0c, 0x09, 0x04, 0x12, '\0', 0x12, 0x14, '\0',
 0x14, '\0', '\0', '\r', 0x07, 0x07, 0x07, 0x13, 0x13, 0x13, 0x17,
 0x17, 0x17, 0x10, 0x10, 0x10, 0x0c, 0x0c, 0x0c, '\r', '\r', '\r',
 '6', '=', '=', 0x2e, ':', ':', 0x27, '7', '7', 0x1d, '2', '2',
 0x12, '0', '0', 0x08, 0x2d, 0x2d, 0x08, 0x2c, 0x2c, '\0', 0x29,
 0x29, '\0', 0x26, 0x26, '\0', 0x23, 0x23, '\0', 0x21, 0x21, '\0',
 0x1f, 0x1f, '\0', 0x1e, 0x1e, '\0', 0x1d, 0x1d, '\0', 0x1c, 0x1c,
 '\0', 0x1b, 0x1b, 0x26, '\0', 0x22,'\0'
};

#else

const unsigned char gamepal[] =
{ '\0', '\0', '\0', '\0', '\0', 0x2a, '\0', 0x2a, '\0', '\0', 0x2a,
 0x2a, 0x2a, '\0', '\0', 0x2a, '\0', 0x2a, 0x2a, 0x15, '\0', 0x2a,
 0x2a, 0x2a, 0x15, 0x15, 0x15, 0x15, 0x15, '?', 0x15, '?', 0x15,
 0x15, '?', '?', '?', 0x15, 0x15, '?', 0x15, '?', '?', '?', 0x15,
 '?', '?', '?', ';', ';', ';', '7', '7', '7', '4', '4', '4', '0',
 '0', '0', 0x2d, 0x2d, 0x2d, 0x2a, 0x2a, 0x2a, 0x26, 0x26, 0x26,
 0x23, 0x23, 0x23, 0x1f, 0x1f, 0x1f, 0x1c, 0x1c, 0x1c, 0x19, 0x19,
 0x19, 0x15, 0x15, 0x15, 0x12, 0x12, 0x12, 0x0e, 0x0e, 0x0e, 0x0b,
 0x0b, 0x0b, 0x08, 0x08, 0x08, '?', '\0', '\0', ';', '\0', '\0',
 '8', '\0', '\0', '5', '\0', '\0', '2', '\0', '\0', 0x2f, '\0',
 '\0', 0x2c, '\0', '\0', 0x29, '\0', '\0', 0x26, '\0', '\0', 0x22,
 '\0', '\0', 0x1f, '\0', '\0', 0x1c, '\0', '\0', 0x19, '\0', '\0',
 0x16, '\0', '\0', 0x13, '\0', '\0', 0x10, '\0', '\0', '?', '6',
 '6', '?', 0x2e, 0x2e, '?', 0x27, 0x27, '?', 0x1f, 0x1f, '?',
 0x17, 0x17, '?', 0x10, 0x10, '?', 0x08, 0x08, '?', '\0', '\0',
 '?', 0x2a, 0x17, '?', 0x26, 0x10, '?', 0x22, 0x08, '?', 0x1e,
 '\0', '9', 0x1b, '\0', '3', 0x18, '\0', 0x2d, 0x15, '\0', 0x27,
 0x13, '\0', '?', '?', '6', '?', '?', 0x2e, '?', '?', 0x27, '?',
 '?', 0x1f, '?', '>', 0x17, '?', '=', 0x10, '?', '=', 0x08, '?',
 '=', '\0', '9', '6', '\0', '3', '1', '\0', 0x2d, 0x2b, '\0',
 0x27, 0x27, '\0', 0x21, 0x21, '\0', 0x1c, 0x1b, '\0', 0x16, 0x15,
 '\0', 0x10, 0x10, '\0', '4', '?', 0x17, '1', '?', 0x10, 0x2d,
 '?', 0x08, 0x28, '?', '\0', 0x24, '9', '\0', 0x20, '3', '\0',
 0x1d, 0x2d, '\0', 0x18, 0x27, '\0', '6', '?', '6', 0x2f, '?',
 0x2e, 0x27, '?', 0x27, 0x20, '?', 0x1f, 0x18, '?', 0x17, 0x10,
 '?', 0x10, 0x08, '?', 0x08, '\0', '?', '\0', '\0', '?', '\0',
 '\0', ';', '\0', '\0', '8', '\0', '\0', '5', '\0', 0x01, '2',
 '\0', 0x01, 0x2f, '\0', 0x01, 0x2c, '\0', 0x01, 0x29, '\0', 0x01,
 0x26, '\0', 0x01, 0x22, '\0', 0x01, 0x1f, '\0', 0x01, 0x1c, '\0',
 0x01, 0x19, '\0', 0x01, 0x16, '\0', 0x01, 0x13, '\0', 0x01, 0x10,
 '\0', '6', '?', '?', 0x2e, '?', '?', 0x27, '?', '?', 0x1f, '?',
 '>', 0x17, '?', '?', 0x10, '?', '?', 0x08, '?', '?', '\0', '?',
 '?', '\0', '9', '9', '\0', '3', '3', '\0', 0x2d, 0x2d, '\0',
 0x27, 0x27, '\0', 0x21, 0x21, '\0', 0x1c, 0x1c, '\0', 0x16, 0x16,
 '\0', 0x10, 0x10, 0x17, 0x2f, '?', 0x10, 0x2c, '?', 0x08, 0x2a,
 '?', '\0', 0x27, '?', '\0', 0x23, '9', '\0', 0x1f, '3', '\0',
 0x1b, 0x2d, '\0', 0x17, 0x27, '6', '6', '?', 0x2e, 0x2f, '?',
 0x27, 0x27, '?', 0x1f, 0x20, '?', 0x17, 0x18, '?', 0x10, 0x10,
 '?', 0x08, 0x09, '?', '\0', 0x01, '?', '\0', '\0', '?', '\0',
 '\0', ';', '\0', '\0', '8', '\0', '\0', '5', '\0', '\0', '2',
 '\0', '\0', 0x2f, '\0', '\0', 0x2c, '\0', '\0', 0x29, '\0', '\0',
 0x26, '\0', '\0', 0x22, '\0', '\0', 0x1f, '\0', '\0', 0x1c, '\0',
 '\0', 0x19, '\0', '\0', 0x16, '\0', '\0', 0x13, '\0', '\0', 0x10,
 '\n', '\n', '\n', '?', '8', '\r', '?', '5', 0x09, '?', '3', 0x06,
 '?', '0', 0x02, '?', 0x2d, '\0', '\0', 0x0e, '\0', '\0', '\n',
 '\0', 0x26, '\0', '9', 0x20, '\0', '3', 0x1d, '\0', 0x2d, 0x18,
 '\0', 0x27, 0x14, '\0', 0x21, 0x11, '\0', 0x1c, '\r', '\0', 0x16,
 '\n', '\0', 0x10, '?', '6', '?', '?', 0x2e, '?', '?', 0x27, '?',
 '?', 0x1f, '?', '?', 0x17, '?', '?', 0x10, '?', '?', 0x08, '?',
 '?', '\0', '?', '8', '\0', '9', '2', '\0', '3', 0x2d, '\0', 0x2d,
 0x27, '\0', 0x27, 0x21, '\0', 0x21, 0x1b, '\0', 0x1c, 0x16, '\0',
 0x16, 0x10, '\0', 0x10, '?', ':', '7', '?', '8', '4', '?', '6',
 '1', '?', '5', 0x2f, '?', '3', 0x2c, '?', '1', 0x29, '?', 0x2f,
 0x27, '?', 0x2e, 0x24, '?', 0x2c, 0x20, '?', 0x29, 0x1c, '?',
 0x27, 0x18, '<', 0x25, 0x17, ':', 0x23, 0x16, '7', 0x22, 0x15,
 '4', 0x20, 0x14, '2', 0x1f, 0x13, 0x2f, 0x1e, 0x12, 0x2d, 0x1c,
 0x11, 0x2a, 0x1a, 0x10, 0x28, 0x19, 0x0f, 0x27, 0x18, 0x0e, 0x24,
 0x17, '\r', 0x22, 0x16, 0x0c, 0x20, 0x14, 0x0b, 0x1d, 0x13, '\n',
 0x1b, 0x12, 0x09, 0x17, 0x10, 0x08, 0x15, 0x0f, 0x07, 0x12, 0x0e,
 0x06, 0x10, 0x0c, 0x06, 0x0e, 0x0b, 0x05, '\n', 0x08, 0x03, 0x18,
 '\0', 0x19, '\0', 0x19, 0x19, '\0', 0x18, 0x18, '\0', '\0', 0x07,
 '\0', '\0', 0x0b, 0x0c, 0x09, 0x04, 0x12, '\0', 0x12, 0x14, '\0',
 0x14, '\0', '\0', '\r', 0x07, 0x07, 0x07, 0x13, 0x13, 0x13, 0x17,
 0x17, 0x17, 0x10, 0x10, 0x10, 0x0c, 0x0c, 0x0c, '\r', '\r', '\r',
 '6', '=', '=', 0x2e, ':', ':', 0x27, '7', '7', 0x1d, '2', '2',
 0x12, '0', '0', 0x08, 0x2d, 0x2d, 0x08, 0x2c, 0x2c, '\0', 0x29,
 0x29, '\0', 0x26, 0x26, '\0', 0x23, 0x23, '\0', 0x21, 0x21, '\0',
 0x1f, 0x1f, '\0', 0x1e, 0x1e, '\0', 0x1d, 0x1d, '\0', 0x1c, 0x1c,
 '\0', 0x1b, 0x1b, 0x26, '\0', 0x22,'\0'
};

#endif
