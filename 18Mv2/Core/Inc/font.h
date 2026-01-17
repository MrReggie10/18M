/*
 * draw_chars.h
 *
 *  Created on: Jan 16, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_DRAW_CHARS_H_
#define INC_DRAW_CHARS_H_

#include <stdint.h>

// Font structure definition
typedef struct {
    const uint8_t *data;      // Pointer to font data
    uint8_t width;            // Character width in pixels
    uint8_t height;           // Character height in pixels
    uint8_t first_char;       // First character in font (usually 32 for space)
    uint8_t last_char;        // Last character in font
} Font;

// Example 16x16 font data for larger text
extern const Font Font24;

#endif /* INC_DRAW_CHARS_H_ */
