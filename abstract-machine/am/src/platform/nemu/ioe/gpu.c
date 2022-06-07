#include <am.h>
#include <nemu.h>
#include <string.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = 400;  // TODO: get the correct width
  int h = 300;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t data = inl(VGACTL_ADDR);
  uint32_t w = (data >> 16) & 0xFFFF;
  uint32_t h = (data & 0xFFFF);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w * h * sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t size = inl(VGACTL_ADDR);
  uint32_t width = (size >> 16) & 0xFFFF;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (w == 0 || h == 0) return;
  uint32_t* data = (uint32_t*)ctl->pixels;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
      outl(FB_ADDR + sizeof(uint32_t) * (width * (y + i) + x + j), data[i * w + j]);
      // fb[width * (y + i) + x + j] = data[i * w + j];

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
