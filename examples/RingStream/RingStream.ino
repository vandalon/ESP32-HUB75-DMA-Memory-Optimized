// Moving white bar on a 64x64 HUB75 panel, ring streaming mode. Pin map: edit for your wiring (default pins are the library's).
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
  Serial.begin(115200);
  HUB75_I2S_CFG mxconfig(64, 64, 1);
  mxconfig.double_buff = true;           // two compact framebuffers
  mxconfig.ring_rows = 8;                // ring mode: 8 rows of bit planes are kept in memory (0 = classic driver)
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;
  mxconfig.setPixelColorDepthBits(8);    // ring mode supports up to 8 bits
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  Serial.printf("free heap after begin: %u\n", (unsigned)ESP.getFreeHeap());
}

void loop() {
  static int x = 0, dir = 1;
  dma_display->fillScreenRGB888(0, 0, 0);
  for (int dx = -1; dx <= 1; ++dx)
    for (int y = 0; y < 64; ++y) dma_display->drawPixelRGB888(x + dx, y, 255, 255, 255);
  dma_display->flipDMABuffer();          // the lower half switches one sweep later (see README); the next draw may wait for that
  x += dir;
  if (x <= 0 || x >= 63) dir = -dir;
  delay(20);
}
