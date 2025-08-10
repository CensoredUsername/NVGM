from typing import Sequence
import struct
from serial import Serial


class Screen:
    """Driver for Wacca-meets-Maurits pixel art bendable screen thingy 9000"""

    def __init__(self, device: str, *args, **kwargs):
        self._width = 0
        self._height = 0
        self._serial = Serial(device, timeout=0.1)
        self._sync()
        self.set_geometry(*args, **kwargs)
        self.clear_screen()
        self.finish_frame()

    def __enter__(self) -> "Screen":
        return self

    def __exit__(self, *_) -> None:
        self._serial.close()

    @staticmethod
    def _encode_cursor(pos: tuple[int, int]) -> bytes:
        argument = pos[0] & 0xFFF
        argument |= (pos[1] & 0xFFF) << 12
        return struct.pack(">I", argument)[-3:]

    @staticmethod
    def _encode_color(pos: tuple[int, int, int]) -> bytes:
        return bytes(reversed(pos))

    def _write(self, data: bytes) -> None:
        # print(data)
        self._serial.write(data)

    def _sync(self) -> None:
        self._write(b"UuDdLrLrAb")
        for _ in range(1000):
            while True:
                x = self._serial.read(1)
                if x == b"$":
                    return
                if x == b"":
                    break
        raise ValueError("Communication error")

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
        argument = width_in_panels & 0xFF
        argument |= (height_in_panels & 0xFF) << 8
        if vertical:
            argument |= 1 << 16
        if mirror_x:
            argument |= 1 << 17
        if mirror_y:
            argument |= 1 << 18
        self._write(b"g" + struct.pack(">I", argument)[-3:])
        self._width, self._height = map(
            int, self._serial.readline().decode("ascii").strip().split()
        )

    def set_cursor(self, pos: tuple[int, int], and_draw: bool = False) -> None:
        """Sets the cursor position. If and_draw is set, also set the pixel to
        the current color and increment the cursor position."""
        self._write((b"C" if and_draw else b"c") + self._encode_cursor(pos))

    def set_window(
        self, min_pos: tuple[int, int], max_pos: tuple[int, int], and_draw: bool = False
    ) -> None:
        """Sets the rectangular window that the cursor uses for wrapping.
        Coordinates are inclusive. If and_draw is set, all pixels in the window
        are set to the current color."""
        self._write(
            (b"W" if and_draw else b"w")
            + self._encode_cursor(min_pos)
            + self._encode_cursor(max_pos),
        )

    def reset_window(self) -> None:
        """Resets the rectangular window that the cursor uses for wrapping to
        the full screen."""
        self._write(b"r")

    def clear_screen(self) -> None:
        """Clears the whole screen, color, and cursor position."""
        self._write(b"R")

    def set_color(self, color: tuple[int, int, int], and_draw: bool = False) -> None:
        """Sets the current color. If and_draw is set, the color is also
        written to the cursor position, and the position is incremented."""
        self._write((b"K" if and_draw else b"k") + self._encode_color(color))

    def set_pixel(self, position: tuple[int, int], color: tuple[int, int, int]):
        """Combination of setting cursor position, setting color, and drawing
        a pixel."""
        self.set_cursor(position)
        self.set_color(color, and_draw=True)

    def draw_pixels(self, data: Sequence[tuple[int, int, int]]) -> None:
        """Same as set_color(..., and_draw=True) in a loop, but sent more
        efficiently."""
        self._write(
            b"B"
            + struct.pack("<H", len(data))
            + b"".join(self._encode_color(color) for color in data)
        )

    def finish_frame(self, retain: bool = True) -> None:
        """Finish the current frame -- it will be sent to the LEDs at the next
        update cycle. If retain is true (the default), the finished frame is
        copied to the buffer for the next frame in hardware, allowing for
        proper partial updates. If it's set to false, the contents of the new
        buffer are undefined and the buffer should be fully overwritten."""
        self._write(b"F" if retain else b"f")

    def randomize(self) -> None:
        """Randomize the screen for testing. This includes finishing the
        frame."""
        self._write(b"?")

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
