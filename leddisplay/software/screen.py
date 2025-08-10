from typing import Sequence
import struct
from serial import Serial

COMPRESSED_GAMMA_ENCODING = [
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,
      1,   1,   1,   1,   2,   2,   2,   2,   2,   3,   3,   3,   3,
      3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   7,
      7,   7,   7,   8,   8,   8,   9,   9,  10,  10,  10,  11,  11,
     12,  12,  12,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,
     18,  18,  19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,
     25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,  32,  33,
     34,  35,  35,  36,  37,  38,  38,  39,  40,  41,  42,  43,  43,
     44,  45,  46,  47,  48,  49,  50,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,
     69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,
     82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,
     95,  96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
    134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146,
    147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
    173, 174, 175, 176, 177, 178, 179, 180, 181
]

class Screen:
    """Driver for Wacca-meets-Maurits pixel art bendable screen thingy 9000"""

    def __init__(self, device: str, *args, **kwargs):
        self._width = 0
        self._height = 0
        self._serial = Serial(device, timeout=0.1)
        self.set_geometry(*args, **kwargs)
        self.clear_screen()
        self.finish_frame()

    def __enter__(self) -> "Screen":
        return self

    def __exit__(self, *_) -> None:
        self._serial.close()

    @staticmethod
    def _encode_cursor(pos: tuple[int, int]) -> bytes:
        return bytes((pos[1] & 0x7F, pos[0] & 0x7F))

    @staticmethod
    def _encode_color(pos: tuple[int, int, int]) -> bytes:
        return bytes((
            COMPRESSED_GAMMA_ENCODING[pos[0]],
            COMPRESSED_GAMMA_ENCODING[pos[1]],
            COMPRESSED_GAMMA_ENCODING[pos[2]],
        ))

    def _write(self, data: bytes) -> None:
        # print(data)
        self._serial.write(data)

    def set_geometry(
        self,
        width_in_panels: int = 1,
        height_in_panels: int = 1,
        vertical: bool = False,
        mirror_x: bool = False,
        mirror_y: bool = False,
    ) -> None:
        """Sets the screen's geometry. Hardware will load a gradient into
        the backbuffer with blue in the top-left corner, yellow in the
        bottom-right, red tints toward the top-right, and green tints
        toward the bottom-left for testing, but to avoid unnecessary
        startup flicker under normal operating conditions it does not
        finish the frame. So, to test with the gradient, call
        finish_frame() after setting the display geometry."""
        argument = width_in_panels & 0x7
        argument |= (height_in_panels & 0x7) << 3
        if vertical:
            argument |= 1 << 6
        if mirror_x:
            argument |= 1 << 7
        if mirror_y:
            argument |= 1 << 8
        self._write(bytes((0xC0, (argument >> 7) & 0x7F, argument & 0x7F)))
        self._width, self._height = map(
            int, self._serial.readline().decode("ascii").strip().split()
        )

    def set_cursor(self, pos: tuple[int, int], and_draw: bool = False) -> None:
        """Sets the cursor position. If and_draw is set, also set the pixel to
        the current color and increment the cursor position."""
        self._write((b"\xC2" if and_draw else b"\xC1") + self._encode_cursor(pos))

    def set_window(
        self, min_pos: tuple[int, int], max_pos: tuple[int, int], and_draw: bool = False
    ) -> None:
        """Sets the rectangular window that the cursor uses for wrapping.
        Coordinates are inclusive. If and_draw is set, all pixels in the window
        are set to the current color."""
        self._write(
            (b"\xC4" if and_draw else b"\xC3")
            + self._encode_cursor(max_pos)
            + self._encode_cursor(min_pos),
        )

    def reset_window(self) -> None:
        """Resets the rectangular window that the cursor uses for wrapping to
        the full screen."""
        self._write(b"\xC5")

    def clear_screen(self) -> None:
        """Clears the whole screen, color, and cursor position."""
        self._write(b"\xC6")

    def set_color(self, color: tuple[int, int, int], and_draw: bool = False) -> None:
        """Sets the current color. If and_draw is set, the color is also
        written to the cursor position, and the position is incremented."""
        self._write((b"\xC8" if and_draw else b"\xC7") + self._encode_color(color))

    def set_pixel(self, position: tuple[int, int], color: tuple[int, int, int]):
        """Combination of setting cursor position, setting color, and drawing
        a pixel."""
        self.set_cursor(position)
        self.set_color(color, and_draw=True)

    def draw_pixels(self, data: Sequence[tuple[int, int, int]]) -> None:
        """Same as set_color(..., and_draw=True) in a loop, but sent more
        efficiently."""
        self._write(b"\xC8" + b"".join(self._encode_color(color) for color in data))

    def finish_frame(self, retain: bool = True) -> None:
        """Finish the current frame -- it will be sent to the LEDs at the next
        update cycle. If retain is true (the default), the finished frame is
        copied to the buffer for the next frame in hardware, allowing for
        proper partial updates. If it's set to false, the contents of the new
        buffer are undefined and the buffer should be fully overwritten."""
        self._write(b"\xCA" if retain else b"\xC9")

    def randomize(self) -> None:
        """Randomize the screen for testing. This includes finishing the
        frame."""
        self._write(b"\xCB")

    @property
    def width(self) -> int:
        """The width of the screen in pixels."""
        return self._width

    @property
    def height(self) -> int:
        """The height of the screen in pixels."""
        return self._height


if __name__ == "__main__":

    import time

    with Screen("/dev/ttyACM1", 1, 3) as s:
        # draw a mandelbrot fractal?

        start = time.time()
        last_update = time.time()
        while True:
            t = time.time() - start
            for x in range(s.width):
                re = (x / s.width - 0.5) * 2.5 * 0.7**t + 0  # 0.743643887037151
                for y in range(s.height):
                    im = (y / s.width - 0.5) * 2.5 * 0.7**t + 1  # 0.131825904205330
                    c = re + 1j * im
                    a = 0.0
                    i = 0
                    for i in range(256):
                        a = a**2 + c
                        if abs(a) > 2:
                            break
                    s.set_pixel((x, y), (i, 0, 0))
            s.finish_frame()
            while True:
                now = time.time()
                delta = now - last_update
                if delta > 1 / 60:
                    last_update += 1 / 16
                    break
                time.sleep(0.001)
