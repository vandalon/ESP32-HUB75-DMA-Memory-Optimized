# ESP32 HUB75 Ring DMA

A fork of [ESP32-HUB75-MatrixPanel-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA) (MIT, 3.0.14) for the **original ESP32**. It keeps the Adafruit GFX API and the upstream configuration; the upstream README is in `UPSTREAM_README.md`.

## What is different

- **Ring streaming mode (`ring_rows`).** Instead of storing every bit plane of every row, the row-end interrupt builds the bit planes of the next rows from a compact framebuffer. Set `mxconfig.ring_rows = 8` (0 keeps the classic driver).
- **Dense framebuffer.** Six bytes per column and row pair (two 24-bit groups, six-bit lanes `R1 G1 B1 R2 G2 B2`, no padding). A 64x64 panel at 8-bit colour needs 2 x 12 KiB framebuffers + 8 KiB ring + 1 KiB template = **33 KiB** (the classic driver needs about 128 KiB).
- **Fast row generation.** The 7/8-bit refill reads each pixel pair once and reuses it for all bit planes: about 23-25 us per 228 us row at 20 MHz (about 12% of one core).
- **Seam fix.** A scanned panel updates its two halves a full sweep apart at the middle (rows 31 and 32 are scanned one sweep apart), so moving content shows the lower half ahead. At 7/8-bit the lower half now switches to each new frame one sweep after the upper half, so the update runs smoothly from top to bottom.
- **Draw ownership.** `flipDMABuffer()` does not wait, but the first draw after a flip waits (at most about 14 ms, once per flip) until the driver stops reading that buffer. Drawing into a buffer that is still on screen can no longer show half-drawn frames.
- `setRingMode(own, lag)` overrides the defaults; `getRingStats()` reports refill time, missed rows and flip/wait counters.

## Limits

- Original ESP32 only (not S2/S3), no PSRAM DMA buffers, panel height up to 64, colour depth up to 8 bits, even panel width, 1/16 or 1/32 scan with a 138 decoder. Other configurations fall back to the classic driver with a warning in the log.
- The row-generation fast path and the seam fix cover 7 and 8 bit depth. Other depths work but use the slower path and keep the seam.
- Pixel drawing is slightly slower than a padded layout (a 64x64 frame of single `drawPixelRGB888` calls takes about 6.5 ms); clears and fills are much faster. Use line and fill calls where you can.
- A face that draws immediately after every flip at more than about 50 fps can stall by up to one flip wait. Check `getRingStats().waits` / `waitMaxUs` on your own sketch.

## Measured (64x64, 8-bit, 20 MHz, 137 Hz, ring of 8 rows)

Free heap after `begin()` is about 8 KiB higher than with a padded 32 KiB framebuffer; refill 23-25 us; clear 30 us; no allocation failures. Seam checked by eye on one panel only.

## Licence

MIT. Copyright upstream authors (Faptastic, MrCodetastic and contributors) and Joris Vandalon for the ring mode. See `LICENSE.txt`.
