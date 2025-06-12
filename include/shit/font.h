#ifndef FONT_H
#define FONT_H

#include <stddef.h>
#include <stdint.h>

typedef struct font_glyph {
    const unsigned char *bitmap;
    unsigned int width_px;
    unsigned int height_px;
    unsigned int bytes_per_row;
} font_glyph_t;

typedef struct kernel_font {
    char name[64];
    unsigned int char_cell_width_px;
    unsigned int char_cell_height_px;
    unsigned int num_glyphs;

    int (*get_glyph)(struct kernel_font *font, unsigned char c,
                     font_glyph_t *out_glyph);

    void *font_private_data;
} kernel_font_t;

/**
 * @brief Initializes a kernel_font structure for an embedded PSF font.
 *
 * @param font Pointer to the kernel_font_t structure to initialize
 * @param psf_binary_start Pointer to the start of embedded PSF binary data
 * @param psf_binary_size Size of the embedded PSF binary data
 * @return 0 on success, -1 on error.
 */
int font_init_psf(kernel_font_t *font, const void *psf_binary_start,
                  size_t psf_binary_size);

static inline int font_get_glyph(kernel_font_t *font, unsigned char c,
                                 font_glyph_t *out_glyph) {
    if (font && font->get_glyph)
        return font->get_glyph(font, c, out_glyph);
    return -1;
}

static inline unsigned int font_get_char_width(kernel_font_t *font) {
    return font ? font->char_cell_width_px : 0;
}

static inline unsigned int font_get_char_height(kernel_font_t *font) {
    return font ? font->char_cell_height_px : 0;
}

#endif // FONT_H