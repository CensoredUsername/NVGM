#include <Arduino.h>
#include "ws2813b.pio.h"

/**
 * Triple-buffered DMA- & PIO-based WS2813b strand driver.
 */
template <uint32_t num_leds>
class WS2813b {
private:
    /**
     * GPIO index for this LED strand.
     */
    const uint8_t pin;

    /**
     * PIO program data.
     */
    PIOProgram prog {&ws2813b_program};

    /**
     * PIO instance used for this strand.
     */
    PIO pio = nullptr;

    /**
     * State machine index in PIO used for this strand.
     */
    int sm = 0;

    /**
     * Offset where the PIO program is loaded into instruction memory.
     */
    int offs = 0;

    /**
     * DMA channel index for this strand.
     */
    int dma = 0;

    /**
     * Number of framebuffers.
     */
    static constexpr uint32_t num_buffers = 3;

    /**
     * Index of the back framebuffer (what we render into).
     */
    int8_t back_idx = 0;

    /**
     * Index of the front framebuffer (what the DMA writes to the LEDs).
     */
    int8_t front_idx = 1;

    /**
     * Index of a buffer that's fully rendered but not yet picked up by the
     * DMA, or -1 if there is no such buffer.
     */
    int8_t waiting_idx = -1;

    /**
     * Memory for all the buffers.
     */
    uint32_t data[num_buffers][num_leds] = {};

    /**
     * Pointer to current back buffer (what we render into).
     */
    [[nodiscard]] uint32_t *back() { return data[back_idx]; }
    [[nodiscard]] const uint32_t *back() const { return data[back_idx]; }

    /**
     * Pointer to current front buffer (what the DMA writes to the LEDs).
     */
    [[nodiscard]] uint32_t *front() { return data[front_idx]; }
    [[nodiscard]] const uint32_t *front() const { return data[front_idx]; }

    /**
     * Index of buffer for which rendering most recently completed.
     */
    [[nodiscard]] int8_t latest_idx() const {
        return waiting_idx < 0 ? front_idx : waiting_idx;
    }

    /**
     * Pointer to buffer for which rendering most recently completed.
     */
    [[nodiscard]] uint32_t *latest() { return data[latest_idx()]; }
    [[nodiscard]] const uint32_t *latest() const { return data[latest_idx()]; }

public:
    explicit WS2813b(const uint8_t pin) : pin(pin) {}

    /**
     * Initializes peripherals for this strand.
     */
    void setup() {
        // Allocate a PIO and state machine and load the program into it.
        prog.prepare(&pio, &sm, &offs, pin, 1);

        // Configure PIO and GPIO for PIO control.
        pio_sm_set_set_pins(pio, sm, pin, 1);
        pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
        pio_gpio_init(pio, pin);

        // Configure the state machine.
        auto cfg = ws2813b_program_get_default_config(offs);
        sm_config_set_set_pins(&cfg, pin, 1);
        sm_config_set_out_pins(&cfg, pin, 1);
        sm_config_set_clkdiv(&cfg, F_CPU * 50e-9);
        sm_config_set_out_shift(&cfg, false, true, 24);
        pio_sm_init(pio, sm, offs, &cfg);
        pio_sm_set_enabled(pio, sm, true);

        // Allocate a DMA channel.
        dma = dma_claim_unused_channel(true);

        // Configure the DMA channel.
        dma_channel_config dma_cfg = dma_channel_get_default_config(dma);
        channel_config_set_dreq(&dma_cfg, pio_get_dreq(pio, sm, true));
        channel_config_set_write_increment(&dma_cfg, false);
        dma_channel_configure(dma, &dma_cfg, &pio->txf[sm], nullptr, num_leds, false);
    }

    /**
     * Writes a pixel into the current backbuffer. Color is 0xBBRRGG.
     */
    bool set_pixel(const uint32_t index, const uint32_t color) {
        if (index >= num_leds) return false;
        back()[index] = color << 8;
        return true;
    }

    /**
     * Mark the current backbuffer as complete. It will be sent to the screen
     * next frame. If retain is set, the finished frame is copied into the new
     * buffer, so partial updates work correctly; if false, all pixels must be
     * written before the next call to finish_frame().
     */
    void finish_frame(bool const retain = false) {
        waiting_idx = back_idx;
        for (int8_t free_idx = 0; free_idx < num_buffers; free_idx++) {
            if (free_idx == front_idx) continue;
            if (free_idx == back_idx) continue;
            back_idx = free_idx;
            break;
        }
        if (retain) {
            memcpy(back(), latest(), sizeof(data[0]));
        }
    }

    /**
     * Starts a DMA transaction to the LEDs using the latest completed
     * framebuffer.
     */
    void dma_start() {
        front_idx = latest_idx();
        waiting_idx = -1;
        dma_channel_set_read_addr(dma, front(), true);
    }

    /**
     * Returns whether the DMA is busy.
     */
    [[nodiscard]] bool dma_busy() const {
        return dma_channel_is_busy(dma);
    }
};

/**
 * Screen abstraction specific to the flexible Wacca screen thingy.
 */
class Screen {
private:
    static constexpr uint32_t num_leds_per_strip = 8;
    static constexpr uint32_t num_strips_per_panel = 20;
    static constexpr uint32_t num_leds_per_panel = num_leds_per_strip * num_strips_per_panel;
    static constexpr uint32_t num_panels_per_strand = 2;
    static constexpr uint32_t num_leds_per_strand = num_leds_per_panel * num_panels_per_strand;
    static constexpr uint32_t num_strands = 3;
    static constexpr uint32_t num_panels = num_panels_per_strand * num_strands;
    static constexpr uint32_t num_leds = num_leds_per_strand * num_strands;

    /**
     * The LED strands that make up the screen.
     */
    WS2813b<num_leds_per_strand> strands[num_strands] {
        WS2813b<num_leds_per_strand>(17),
        WS2813b<num_leds_per_strand>(21),
        WS2813b<num_leds_per_strand>(27)
    };

    /**
     * Target framerate in FPS.
     */
    uint32_t framerate = 60;

    /**
     * Display geometry.
     */
    uint32_t width_in_panels = 1;
    uint32_t height_in_panels = 1;
    bool vertical = false;
    bool mirror_x = false;
    bool mirror_y = false;

    /**
     * Arduino micros() when we last started a DMA transaction, used to create
     * a consistent framerate.
     */
    uint32_t last_update = 0;

public:
    /**
     * Configure peripherals for the strands.
     */
    void setup() {
        for (auto &strand : strands) {
            strand.setup();
        }
        set_geometry(1, 1, false);
        gradient();
    }

    /**
     * Call periodically in main loop to keep the LEDs updated.
     */
    void update() {
        const uint32_t now = micros();
        const uint32_t delta = now - last_update;
        if (delta >= 1000000 / framerate) {
            last_update = now;
            for (auto &strand : strands) {
                strand.dma_start();
            }
            Serial.write("!");
        }
    }

    /**
     * Sets the geometry of the screen.
     */
    bool set_geometry(
        const uint32_t new_width_in_panels = 1,
        uint32_t new_height_in_panels = 0,
        const bool new_vertical = false,
        bool new_mirror_x = false,
        const bool new_mirror_y = false
    ) {
        if (!new_width_in_panels) return false;
        if (!new_height_in_panels) new_height_in_panels = num_panels / new_width_in_panels;
        if (new_width_in_panels * new_height_in_panels > num_panels) return false;
        width_in_panels = new_width_in_panels;
        height_in_panels = new_height_in_panels;
        vertical = new_vertical;
        if (vertical) new_mirror_x = !new_mirror_x;
        mirror_x = new_mirror_x;
        mirror_y = new_mirror_y;
        return true;
    }

    /**
     * Configures the framerate.
     */
    bool set_framerate(uint32_t new_framerate) {
        if (!new_framerate) return false;
        if (new_framerate > 60) return false;
        framerate = new_framerate;
        return true;
    }

    /**
     * Returns width in LEDs of a single panel.
     */
    [[nodiscard]] uint32_t width_per_panel() const {
        return vertical ? num_leds_per_strip : num_strips_per_panel;
    }

    /**
     * Returns height in LEDs of a single panel.
     */
    [[nodiscard]] uint32_t height_per_panel() const {
        return vertical ? num_strips_per_panel : num_leds_per_strip;
    }

    /**
     * Returns the width of the screen in pixels.
     */
    [[nodiscard]] uint32_t width() const {
        return width_in_panels * width_per_panel();
    }

    /**
     * Returns the height of the screen in pixels.
     */
    [[nodiscard]] uint32_t height() const {
        return height_in_panels * height_per_panel();
    }

    /**
     * Writes the given color to the given pixel. Returns false if the
     * coordinate is out of range.
     */
    bool set_pixel(uint32_t x, uint32_t y, const uint32_t color) {
        if (mirror_x) x = width() - x - 1;
        if (mirror_y) y = height() - y - 1;
        if (vertical) std::swap(x, y);

        // Determine index of LED within the panel being addressed.
        const uint32_t led_x = x % num_strips_per_panel;
        const uint32_t led_y = y % num_leds_per_strip;
        const uint32_t led_in_panel = num_leds_per_strip * led_x + led_y;

        // Determine coordinate of panel being addressed.
        const uint32_t panel_x = x / num_strips_per_panel;
        const uint32_t panel_y = y / num_leds_per_strip;

        // Determine index of panel being addressed, failing on out-of-range.
        const uint32_t swapped_width_in_panels = vertical ? height_in_panels : width_in_panels;
        const uint32_t swapped_height_in_panels = vertical ? width_in_panels : height_in_panels;
        if (panel_x >= swapped_width_in_panels) return false;
        if (panel_y >= swapped_height_in_panels) return false;
        const uint32_t panel_index = panel_x + panel_y * swapped_width_in_panels;
        if (panel_index >= num_panels_per_strand * num_strands) return false;

        // Split panel index into strand index and panel in strand.
        const uint32_t strand_index = panel_index / num_panels_per_strand;
        const uint32_t panel_in_strand = panel_index % num_panels_per_strand;

        // Determine index of LED in the strand.
        const uint32_t led_in_strand = num_leds_per_panel * panel_in_strand + led_in_panel;

        // Set the pixel color.
        return strands[strand_index].set_pixel(led_in_strand, color);
    }

    /**
     * Writes the given color to the given pixel. Returns false if the
     * coordinate is out of range.
     */
    bool set_pixel(const uint32_t x, const uint32_t y, const uint8_t r, const uint8_t g, const uint8_t b) {
        return set_pixel(x, y, (g << 16) | (r << 8) | b);
    }

    /**
     * Writes a random color to all pixels.
     */
    void randomize(const bool finish = true) {
        for (uint32_t x = 0; x < width(); x++) {
            for (uint32_t y = 0; y < height(); y++) {
                set_pixel(x, y, random());
            }
        }
        if (finish) finish_frame();
    }

    /**
     * Writes a gradient to all pixels.
     */
    void gradient(const bool finish = true) {
        const uint32_t w = width();
        const uint32_t h = height();
        for (uint32_t x = 0; x < w; x++) {
            for (uint32_t y = 0; y < h; y++) {
                const uint8_t r = x * 255 / w;
                const uint8_t g = y * 255 / h;
                const uint8_t b = 255 - ((x + y) * 255 / (w + h));
                set_pixel(x, y, r, g, b);
            }
        }
        if (finish) finish_frame();
    }

    /**
     * Mark the current backbuffer as complete. It will be sent to the screen
     * next frame. If retain is set, the finished frame is copied into the new
     * buffer, so partial updates work correctly; if false, all pixels must be
     * written before the next call to finish_frame().
     */
    void finish_frame(bool const retain = false) {
        for (auto &strand : strands) {
            strand.finish_frame();
        }
    }
} screen {};

/**
 * Handles a single dimension of a cursor.
 */
struct Cursor {
    uint32_t pos = 0;
    uint32_t max = 0;
    uint32_t min = 0;

    /**
     * Increments cursor position. Returns whether the cursor wrapped around.
     */
    bool increment() {
        if (pos >= max) {
            pos = min;
            return true;
        } else {
            pos++;
            return false;
        }
    }
};

/**
 * decoding table for gamma-compressed data
 */
static const uint8_t gamma_decode_table[192] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,
     13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
     26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
     52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
     65,  66,  67,  68,  70,  71,  72,  73,  74,  75,  76,  77,  79,
     80,  81,  82,  83,  85,  86,  87,  88,  90,  91,  92,  93,  95,
     96,  97,  99, 100, 101, 103, 104, 105, 107, 108, 109, 111, 112,
    114, 115, 117, 118, 120, 121, 123, 124, 126, 127, 129, 130, 132,
    133, 135, 136, 138, 140, 141, 143, 144, 146, 148, 149, 151, 153,
    154, 156, 158, 160, 161, 163, 165, 167, 168, 170, 172, 174, 176,
    177, 179, 181, 183, 185, 187, 189, 190, 192, 194, 196, 198, 200,
    202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226,
    228, 231, 233, 235, 237, 239, 241, 243, 246, 248, 250, 252, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

/**
 * Helper class for rendering and yes, partial updates.
 */
class Renderer {
private:
    Cursor x, y;
    uint8_t r = 0, g = 0, b = 0;

    static constexpr uint8_t font_table[96][7] = {
       {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,},  // 0x20, Space
       {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04,},  // 0x21, !
       {0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00,},  // 0x22, "
       {0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a,},  // 0x23, #
       {0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04,},  // 0x24, $
       {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13,},  // 0x25, %
       {0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d,},  // 0x26, &
       {0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,},  // 0x27, '
       {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02,},  // 0x28, (
       {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08,},  // 0x29, )
       {0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04,},  // 0x2a, *
       {0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00,},  // 0x2b, +
       {0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08,},  // 0x2c, ,
       {0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,},  // 0x2d, -
       {0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c,},  // 0x2e, .
       {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10,},  // 0x2f, /
       {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e,},  // 0x30, 0
       {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e,},  // 0x31, 1
       {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f,},  // 0x32, 2
       {0x0e, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0e,},  // 0x33, 3
       {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02,},  // 0x34, 4
       {0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e,},  // 0x35, 5
       {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e,},  // 0x36, 6
       {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08,},  // 0x37, 7
       {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e,},  // 0x38, 8
       {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c,},  // 0x39, 9
       {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00,},  // 0x3a, :
       {0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08,},  // 0x3b, ;
       {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,},  // 0x3c, <
       {0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00,},  // 0x3d, =
       {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08,},  // 0x3e, >
       {0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04,},  // 0x3f, ?
       {0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f,},  // 0x40, @
       {0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11,},  // 0x41, A
       {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e,},  // 0x42, B
       {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e,},  // 0x43, C
       {0x1e, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1e,},  // 0x44, D
       {0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f,},  // 0x45, E
       {0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10,},  // 0x46, F
       {0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f,},  // 0x37, G
       {0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11,},  // 0x48, H
       {0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e,},  // 0x49, I
       {0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c,},  // 0x4a, J
       {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11,},  // 0x4b, K
       {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f,},  // 0x4c, L
       {0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11,},  // 0x4d, M
       {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11,},  // 0x4e, N
       {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e,},  // 0x4f, O
       {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10,},  // 0x50, P
       {0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d,},  // 0x51, Q
       {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11,},  // 0x52, R
       {0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e,},  // 0x53, S
       {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,},  // 0x54, T
       {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e,},  // 0x55, U
       {0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04,},  // 0x56, V
       {0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11,},  // 0x57, W
       {0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11,},  // 0x58, X
       {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04,},  // 0x59, Y
       {0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f,},  // 0x5a, Z
       {0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e,},  // 0x5b, [
       {0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01,},  // 0x5c, \.
       {0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e,},   // 0x5d, ]
       {0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00,},   // 0x5e, ^
       {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,},   // 0x5f, _
       {0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00,},   // 0x60, `
       {0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d,},   // 0x61, a
       {0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c,},   // 0x62, b
       {0x00, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x0e,},   // 0x63, c
       {0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07,},   // 0x64, d
       {0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f,},   // 0x65, e
       {0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08,},   // 0x66, f
       {0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e,},   // 0x67, g
       {0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11,},   // 0x68, h
       {0x00, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x0e,},   // 0x69, i
       {0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c,},   // 0x6a, j
       {0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12,},   // 0x6b, k
       {0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,},   // 0x6c, l
       {0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11,},   // 0x6d, m
       {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11,},   // 0x6e, n
       {0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e,},   // 0x6f, o
       {0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10,},   // 0x70, p
       {0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01,},   // 0x71, q
       {0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10,},   // 0x72, r
       {0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e,},   // 0x73, s
       {0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06,},   // 0x74, t
       {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d,},   // 0x75, u
       {0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04,},   // 0x76, v
       {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a,},   // 0x77, w
       {0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11,},   // 0x78, x
       {0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e,},   // 0x79, y
       {0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f,},   // 0x7a, z
       {0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06,},   // 0x7b, {
       {0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04,},   // 0x7c, |
       {0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c,},   // 0x7d, }
       {0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00,},   // 0x7e, ~
       {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,},   // 0x7f, DEL
   };

public:
    /**
     * Sets the cursor position.
     */
    void set_cursor(const uint32_t new_x, const uint32_t new_y) {
        x.pos = new_x;
        y.pos = new_y;
    }

    /**
     * Sets the rectangular window that the cursor will increment in, and set
     * the cursor position to the start of this window.
     */
    void set_window(const uint32_t x_min, const uint32_t y_min, const uint32_t x_max, const uint32_t y_max) {
        x.min = x_min;
        x.max = x_max;
        y.min = y_min;
        y.max = y_max;
        set_cursor(x_min, y_min);
    }

    /**
     * Resets the cursor window to the full screen.
     */
    void reset_window() {
        set_window(0, 0, screen.width() - 1, screen.height() - 1);
    }

    /**
     * Sets the (gamma-compressed) color that we're drawing pixels with.
     */
    void set_color(const uint8_t new_r, const uint8_t new_g, const uint8_t new_b) {
        r = new_r < 192 ? gamma_decode_table[new_r] : 255;
        g = new_g < 192 ? gamma_decode_table[new_g] : 255;
        b = new_b < 192 ? gamma_decode_table[new_b] : 255;
    }

    /**
     * Draws a single pixel and increments the cursor position.
     */
    void draw_pixel() {
        screen.set_pixel(x.pos, y.pos, r, g, b);
        if (x.increment()) y.increment();
    }

    /**
     * Fills the remainder of the window with the current color.
     */
    void fill_window() {
        do {
            screen.set_pixel(x.pos, y.pos, r, g, b);
        } while (!(x.increment() && y.increment()));
    }

    /**
     * Draws a character. This ignores the current window and just draws a
     * character with the top-left at the current cursor position.
     */
    void draw_character(char c) {
        const uint32_t x0 = x.pos;
        const uint32_t y0 = y.pos;
        if (c < 0x20 || c >= 0x80) c = '?';
        c -= 0x20;
        for (uint32_t dy = 0; dy < 7; dy++) {
            y.pos = y0 + dy;
            uint8_t d = font_table[c][dy];
            for (uint32_t dx = 0; dx < 5; dx++) {
                if (d & 0x10) {
                    x.pos = x0 + dx;
                    draw_pixel();
                }
                d <<= 1;
            }
        }
        x.pos = x0 + 6;
        y.pos = y0;
    }

    /**
     * Draws a string. This ignores the current window and just draws the
     * string with the top-left at the current cursor position.
     */
    void draw_string(const char *c) {
        while (*c) {
            draw_character(*c++);
        }
    }
} renderer {};

/**
 * Base class for command handlers.
 */
class CommandHandler {
public:
    virtual ~CommandHandler() = default;

    /**
     * Override to handle (the start of) a command. Should return the number of
     * bytes expected for the next argument (1..4), or 0 if no further
     * arguments are expected.
     */
    virtual uint8_t command(uint8_t command_byte) = 0;

    /**
     * Override to handle arguments for a command as they come in. Should
     * return the number of bytes expected for the next argument (1..4), or 0
     * if no further arguments are expected.
     */
    virtual uint8_t argument(uint8_t command_byte, uint32_t argument_index, uint32_t argument_data) { return 0; };

    /**
     * Override to handle streamed ddata after a command.
     * Data is ignored by default.
     */
    virtual void stream(uint8_t command_byte, uint8_t data) { };
};

/**
 * Protocol handler for serial connection.
 */
class Protocol {
private:
    uint8_t current_command = 0;
    uint8_t num_argument_bytes_remain = 0;
    uint32_t current_argument = 0;
    uint32_t argument_index = 0;
    CommandHandler *command_handlers[64] = {};

    /**
     * Handles a received byte.
     */
    void handle_byte(const uint8_t data) {
        if ((data & 0xC0) == 0xC0) {
            // command byte
            this->current_command = data & 0x3F;
            if (const auto handler = this->command_handlers[this->current_command]) {
                this->num_argument_bytes_remain = handler->command(this->current_command);
                if (this->num_argument_bytes_remain) {
                    this->argument_index = 0;
                    this->current_argument = 0;
                }
            } else {
                Serial.write("?");
            }

        } else {
            // data byte
            if (this->num_argument_bytes_remain) {
                // Store up to 28 bits per argument as requested by preceding
                // handle_command().
                this->current_argument <<= 7;
                this->current_argument |= data;
                this->num_argument_bytes_remain--;
                if (!this->num_argument_bytes_remain) {
                    if (const auto handler = this->command_handlers[this->current_command]) {
                        this->num_argument_bytes_remain = handler->argument(
                            this->current_command, this->argument_index, this->current_argument);
                        if (this->num_argument_bytes_remain) {
                            this->argument_index++;
                            this->current_argument = 0;
                        }
                    }
                }
            } else {
                // non-argument data stream
                if (const auto handler = this->command_handlers[this->current_command]) {
                    handler->stream(this->current_command, data);
                }
            }
        }
    }

public:

    /**
     * Initialize protocol handler.
     */
    void setup() {
        Serial.begin();
    }

    /**
     * Installs a command handler for the given command byte.
     */
    void set_handler(const uint8_t command_byte, CommandHandler *handler) {
        command_handlers[command_byte & 0x3F] = handler;
    }

    /**
     * Updates receive logic.
     */
    void update() {
        while (Serial.available()) {
            handle_byte(Serial.read());
        }
    }
} protocol {};

class GeometryCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 2;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        (void)argument_index;
        const uint8_t width = argument_data & 0x7;
        const uint8_t height = (argument_data >> 3) & 0x7;
        const bool vertical = ((argument_data >> 6) & 1) != 0;
        const bool mirror_x = ((argument_data >> 7) & 1) != 0;
        const bool mirror_y = ((argument_data >> 8) & 1) != 0;
        screen.set_geometry(width, height, vertical, mirror_x, mirror_y);
        Serial.printf("%d %d\n", screen.width(), screen.height());
        screen.gradient(false);
        return 0;
    }
} geometry_handler {};

class SetCursorCommandHandler : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 2;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        (void)argument_index;
        renderer.set_cursor(argument_data & 0x7F, argument_data >> 7);
        return 0;
    }
} set_cursor_handler {};

class SetCursorAndDrawCommandHandler final : public SetCursorCommandHandler {
public:
    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        SetCursorCommandHandler::argument(command_byte, argument_index, argument_data);
        renderer.draw_pixel();
        return 0;
    }
} set_cursor_and_draw_handler {};

class SetWindowCommandHandler : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 4;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        uint8_t x_min = argument_data & 0x7F;
        uint8_t y_min = (argument_data >> 7) & 0x7F;
        uint8_t x_max = (argument_data >> 14) & 0x7F;
        uint8_t y_max = (argument_data >> 21) & 0x7F;
        renderer.set_window(x_min, y_min, x_max, y_max);
        return 0;
    }
} set_window_handler {};

class SetWindowAndDrawCommandHandler final : public SetWindowCommandHandler {
public:
    uint8_t argument(uint8_t command_byte, uint32_t argument_index, uint32_t argument_data) override {
        const uint8_t ret = SetWindowCommandHandler::argument(command_byte, argument_index, argument_data);
        if (!ret) renderer.fill_window();
        return ret;
    }
} set_window_and_draw_handler {};

class ResetWindowCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        renderer.reset_window();
        return 0;
    }
} reset_window_handler {};

class ClearScreenCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        renderer.set_color(0, 0, 0);
        renderer.reset_window();
        renderer.fill_window();
        return 0;
    }
} clear_screen_handler {};

class SetColorCommandHandler : public CommandHandler {
    uint8_t color_buffer[3];
    uint8_t color_buffer_index;
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        color_buffer_index = 0;
        return 0;
    }

    void stream(const uint8_t command_byte, const uint8_t data) {
        (void)command_byte;
        this->color_buffer[this->color_buffer_index++] = data;
        if (this->color_buffer_index >= 3) {
            this->color_buffer_index = 0;
            renderer.set_color(
                this->color_buffer[0], this->color_buffer[1], this->color_buffer[2]);
        }
    }
} set_color_handler {};

class SetColorAndDrawCommandHandler final : public SetColorCommandHandler {
    uint8_t color_buffer[3];
    uint8_t color_buffer_index;
public:
    void stream(const uint8_t command_byte, const uint8_t data) {
        (void)command_byte;
        this->color_buffer[this->color_buffer_index++] = data;
        if (this->color_buffer_index == 3) {
            this->color_buffer_index = 0;
            renderer.set_color(
                this->color_buffer[0], this->color_buffer[1], this->color_buffer[2]);
            renderer.draw_pixel();
        }
    }

} set_color_and_draw_handler {};

class FinishFrameCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        screen.finish_frame();
        return 0;
    }
} finish_frame_handler {};

class FinishFrameAndRetainCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        screen.finish_frame(true);
        return 0;
    }
} finish_frame_and_retain_handler {};

class RandomizeCommandHandler final : public CommandHandler {
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        screen.randomize();
        return 0;
    }
} randomize_handler {};


void setup() {
    screen.setup();
    screen.set_geometry(3, 1);
    renderer.reset_window();
    renderer.set_color(0, 0, 0);
    renderer.fill_window();
    renderer.set_color(255, 128, 0);
    renderer.draw_string("duurt lang");
    screen.finish_frame();
    protocol.setup();
    protocol.set_handler(0x0, &geometry_handler);
    protocol.set_handler(0x1, &set_cursor_handler);
    protocol.set_handler(0x2, &set_cursor_and_draw_handler);
    protocol.set_handler(0x3, &set_window_handler);
    protocol.set_handler(0x4, &set_window_and_draw_handler);
    protocol.set_handler(0x5, &reset_window_handler);
    protocol.set_handler(0x6, &clear_screen_handler);
    protocol.set_handler(0x7, &set_color_handler);
    protocol.set_handler(0x8, &set_color_and_draw_handler);
    protocol.set_handler(0x9, &finish_frame_handler);
    protocol.set_handler(0xA, &finish_frame_and_retain_handler);
    protocol.set_handler(0xB, &randomize_handler);
}

void loop() {
    screen.update();
    protocol.update();
}
