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
        uint32_t new_width_in_panels = 1,
        uint32_t new_height_in_panels = 0,
        const bool new_vertical = false,
        bool new_mirror_x = false,
        bool new_mirror_y = false
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
        if (width_in_panels == 0) return false;
        if (panel_x >= width_in_panels) return false;
        if (panel_y >= height_in_panels) return false;
        const uint32_t panel_index = panel_x + panel_y * width_in_panels;
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
 * Helper class for rendering and yes, partial updates.
 */
class Renderer {
private:
    Cursor x, y;
    uint8_t r = 0, g = 0, b = 0;

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
     * Sets the color that we're drawing pixels with.
     */
    void set_color(const uint8_t new_r, const uint8_t new_g, const uint8_t new_b) {
        r = new_r;
        g = new_g;
        b = new_b;
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
};

/**
 * Protocol handler for serial connection.
 */
class Protocol {
private:
    static constexpr uint8_t reset_sequence[11] = "UuDdLrLrAb";
    uint8_t reset_index = 0;
    uint8_t current_command = 0;
    uint8_t num_argument_bytes_remain = 0;
    uint32_t current_argument = 0;
    uint32_t argument_index = 0;
    CommandHandler *command_handlers[256] = {};

    /**
     * Handles a received byte.
     */
    void handle_byte(const uint8_t data) {

        // Handle protocol reset sequence.
        if (data == reset_sequence[reset_index]) {
            reset_index++;
            if (!reset_sequence[reset_index]) {
                reset_index = 0;
                num_argument_bytes_remain = 0;
                Serial.write("$");
                return;
            }
        }

        if (num_argument_bytes_remain) {
            // Store up to 4 bytes per argument as requested by preceding
            // handle_command().
            current_argument <<= 8;
            current_argument |= data;
            num_argument_bytes_remain--;
            if (!num_argument_bytes_remain) {
                if (const auto handler = command_handlers[current_command]) {
                    num_argument_bytes_remain = handler->argument(current_command, argument_index, current_argument);
                    if (num_argument_bytes_remain) {
                        argument_index++;
                        current_argument = 0;
                    }
                }
            }
        } else {
            // If we're not waiting for an argument, the next byte is a
            // command.
            current_command = data;
            if (const auto handler = command_handlers[current_command]) {
                num_argument_bytes_remain = handler->command(current_command);
                if (num_argument_bytes_remain) {
                    argument_index = 0;
                    current_argument = 0;
                }
            } else {
                Serial.write("?");
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
        command_handlers[command_byte] = handler;
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
        return 3;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        (void)argument_index;
        const uint8_t width = argument_data & 0xFF;
        const uint8_t height = (argument_data >> 8) & 0xFF;
        const bool vertical = ((argument_data >> 16) & 1) != 0;
        const bool mirror_x = ((argument_data >> 17) & 1) != 0;
        const bool mirror_y = ((argument_data >> 18) & 1) != 0;
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
        return 3;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        (void)argument_index;
        renderer.set_cursor(argument_data & 0xFFF, argument_data >> 12);
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
private:
    uint32_t arg0 = 0;
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 3;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        if (argument_index == 0) {
            arg0 = argument_data;
            return 3;
        }
        renderer.set_window(arg0 & 0xFFF, arg0 >> 12, argument_data & 0xFFF, argument_data >> 12);
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
private:
    uint32_t arg0 = 0;
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        renderer.reset_window();
        return 0;
    }
} reset_window_handler {};

class ClearScreenCommandHandler final : public CommandHandler {
private:
    uint32_t arg0 = 0;
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
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 3;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        renderer.set_color(argument_data & 0xFF, (argument_data >> 8) & 0xFF, argument_data >> 16);
        return 0;
    }
} set_color_handler {};

class SetColorAndDrawCommandHandler final : public SetColorCommandHandler {
public:
    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        SetColorCommandHandler::argument(command_byte, argument_index, argument_data);
        renderer.draw_pixel();
        return 0;
    }
} set_color_and_draw_handler {};

class BulkDrawCommandHandler final : public CommandHandler {
private:
    uint16_t pixels_remain = 0;
public:
    uint8_t command(const uint8_t command_byte) override {
        (void)command_byte;
        return 2;
    }

    uint8_t argument(const uint8_t command_byte, const uint32_t argument_index, const uint32_t argument_data) override {
        (void)command_byte;
        if (argument_index == 0) {
            pixels_remain = argument_data;
        } else {
            renderer.set_color(argument_data & 0xFF, (argument_data >> 8) & 0xFF, argument_data >> 16);
            renderer.draw_pixel();
        }
        return pixels_remain > 0 ? 3 : 0;
    }
} bulk_draw_handler {};

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
    protocol.setup();
    protocol.set_handler('g', &geometry_handler);
    protocol.set_handler('c', &set_cursor_handler);
    protocol.set_handler('C', &set_cursor_and_draw_handler);
    protocol.set_handler('w', &set_window_handler);
    protocol.set_handler('W', &set_window_and_draw_handler);
    protocol.set_handler('r', &reset_window_handler);
    protocol.set_handler('R', &clear_screen_handler);
    protocol.set_handler('k', &set_color_handler);
    protocol.set_handler('K', &set_color_and_draw_handler);
    protocol.set_handler('B', &bulk_draw_handler);
    protocol.set_handler('f', &finish_frame_handler);
    protocol.set_handler('F', &finish_frame_and_retain_handler);
    protocol.set_handler('?', &randomize_handler);
}

void loop() {
    screen.update();
    protocol.update();
}
