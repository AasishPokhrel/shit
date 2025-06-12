#ifndef GFX_H
#define GFX_H

#include <limine.h>
#include <stdint.h>

typedef struct gfx_device {
    void *address;
    uint64_t width_px;
    uint64_t height_px;
    uint64_t pitch;
    uint16_t bpp;

    void (*put_pixel)(struct gfx_device *dev, uint64_t x, uint64_t y,
                      uint32_t color);
    void (*clear_screen)(struct gfx_device *dev, uint32_t color);

    void *private_data; // Device-specific internal data
} gfx_device_t;

/**
 * @brief Initializes a graphics device using a Limine framebuffer
 *
 * @param dev Pointer to the gfx_device_t structure to initialize.
 * @param fb Pointer to the limine_framebuffer provided by the bootloader.
 * @return 0 on success, -1 on error
 */
int gfx_init_from_limine_fb(gfx_device_t *dev, struct limine_framebuffer *fb);

static inline void gfx_put_pixel(gfx_device_t *dev, uint64_t x, uint64_t y,
                                 uint32_t color) {
    if (dev && dev->put_pixel)
        dev->put_pixel(dev, x, y, color);
}

static inline void gfx_clear_screen(gfx_device_t *dev, uint32_t color) {
    if (dev && dev->clear_screen)
        dev->clear_screen(dev, color);
}

static inline uint64_t gfx_get_width(gfx_device_t *dev) {
    return dev ? dev->width_px : 0;
}

static inline uint64_t gfx_get_height(gfx_device_t *dev) {
    return dev ? dev->height_px : 0;
}

#endif // GFX_H