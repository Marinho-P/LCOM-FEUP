#include <lcom/lcf.h>
#include "graphics.h"
#include "math.h"
#include "stdio.h"


static uint8_t* video_mem_first;
static uint8_t* video_mem_secondary;
static bool isSecondBuffer = false;
static vbe_mode_info_t modeInfo;
static u32_t vram_size;
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int changeMode(uint16_t mode){
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));
  r86.intno = 0x10;
  r86.bx = mode | BIT(14);
  r86.al = 0x02;
  r86.ah = 0x4f;

  if (sys_int86(&r86) != OK) return 1;

  if (r86.al != 0x4f) {
    printf("Function is not supported\n");
    return 1;
  }
  if (r86.ah != 0x00) {
    printf("Function was not sucessfull. Check AH value to know what is wrong\n");
    return 1;
  }
  return 0;
}


void* getVideoMem(){
  return video_mem_first;
}

int draw_pixel(uint16_t x, uint16_t y, uint32_t color){

  if (x >= h_res || y >=v_res) {
    return 0;
  }


  unsigned bytesPerPixel = (bits_per_pixel + 7) / 8;
  unsigned index = (x + y * h_res) * bytesPerPixel;
  if (memcpy(video_mem_first + index, &color, bytesPerPixel) == NULL) return 1;
  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for (size_t i = 0; i < len; i++){
    if (draw_pixel(x + i, y, color)) return 1;
  }
  return 0;
}


int vg_draw_vline(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  
  for (size_t i = 0; i < len; i++){
    if (draw_pixel(x, y + i, color)) return 1;
  }
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){

  for (size_t row = 0; row < height; row++){
    if (vg_draw_hline(x, y+row, width, color)) return 1;
  }
  return 0;
}

uint8_t R(uint32_t color){
  return (color >> (modeInfo.RedFieldPosition)) & ~(0xffffff << (modeInfo.RedMaskSize));
}

uint8_t G(uint32_t color){
  return (color >> (modeInfo.GreenFieldPosition)) & ~(0xffffff << (modeInfo.GreenMaskSize));
}

uint8_t B(uint32_t color){
  return (color >> (modeInfo.BlueFieldPosition)) & ~(0xffffff << (modeInfo.BlueMaskSize));
}

int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step) {

  unsigned Xrect_res = h_res / no_rectangles;
  unsigned Yrect_res = v_res / no_rectangles;

  for (size_t col = 0; col < no_rectangles; col++){

    for (size_t line=0; line<no_rectangles; line++){

      int32_t color = (first + (line * no_rectangles + col) * step) % (1 << bits_per_pixel) ;

      uint8_t red = (R(first) + col * step) % (1 << modeInfo.RedMaskSize);
	    uint8_t green = (G(first) + line * step) % (1 << modeInfo.GreenMaskSize);
	    uint8_t blue = (B(first) + (col + line) * step) % (1 << modeInfo.BlueMaskSize);
      if (modeInfo.RedMaskSize){
        color = (red << modeInfo.RedFieldPosition) | (green << modeInfo.GreenFieldPosition) | (blue << modeInfo.BlueFieldPosition);
      }

      if (vg_draw_rectangle(col * Xrect_res, line * Yrect_res, Xrect_res, Yrect_res, color)) return 1;
    }
  }
  return 0;
}

int (cleanCanvas)(){
  memset(video_mem_first, 0, vram_size);
  return 0;
}


int drawXPMImage(xpm_image_t img, double x, double y, double angle){
  uint32_t transparentColor = xpm_transparency_color(img.type);

    double cosseno = cos(angle);
    double seno = sin(angle);


  for (size_t col = 0 ; col < img.width; col++){
    for (size_t row = 0 ; row < img.height; row ++){


      unsigned index = (col + row * img.width) * ((bits_per_pixel + 7) / 8);
      uint32_t color = 0;
      memcpy(&color, img.bytes +index, ((bits_per_pixel + 7) / 8));

      if (color != transparentColor){

      double imageX = col - img.width/2.0;
      double imageY = row - img.height/2.0;


      double changedX = cosseno*imageX - seno*imageY;
      double changedY = seno*imageX + cosseno*imageY;


      uint16_t posX = x + changedX;
      uint16_t posY = y + changedY;
      if (draw_pixel(posX , posY, color)) return 1;
      }
    }
  }
  return 0;
}
int(mapMemory)(uint16_t mode){
  /* Use VBE function 0x01 to initialize vram_base and vram_size */
  if(vbe_get_mode_info(mode,&modeInfo)) return 1;

  struct minix_mem_range mr_first,mr_secondary;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_base_secondary;
  unsigned int vram_size;  /* VRAM's size, but you can use the frame-buffer size, instead */
  int r;

  h_res = modeInfo.XResolution;
  v_res = modeInfo.YResolution;

  bits_per_pixel = modeInfo.BitsPerPixel;

  vram_size = h_res * v_res * ((bits_per_pixel+7) / 8);
  vram_base = modeInfo.PhysBasePtr;

  vram_base_secondary = modeInfo.PhysBasePtr + vram_size;

  /* Allow memory mapping */
  mr_first.mr_base = (phys_bytes) vram_base;
  mr_secondary.mr_base = (phys_bytes)vram_base_secondary;    

  mr_first.mr_limit = mr_first.mr_base + vram_size * 2;


  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr_first))) return 1;

  /* Map memory */

  video_mem_first = vm_map_phys(SELF, (void *)mr_first.mr_base, vram_size);

  video_mem_secondary = vm_map_phys(SELF, (void *)mr_secondary.mr_base, vram_size);

  if(video_mem_first == MAP_FAILED || video_mem_secondary == MAP_FAILED) return 1;
  
  /*Initialize memory to be a black screen*/
  memset(video_mem_first, 0, vram_size);
  memset(video_mem_secondary, 0, vram_size);

  return 0;
}
int (swap_buffers)(){
  reg86_t r;
  memset(&r, 0, sizeof(struct reg86));
  u16_t new_dx;

  if(!isSecondBuffer){ /*Caso de ser o main Buffer*/
    new_dx = v_res;
  }else{  /*Caso de ser o secondary*/
    new_dx = 0;
  }
  isSecondBuffer = !isSecondBuffer;

  r.ax = 0x4F07;
  r.bh = 0x00;
  r.bl = 0x80; 
  r.dx = new_dx;
  r.intno = 0x10;
  if(sys_int86(&r)) return 1;

  void *temp = video_mem_first;
  video_mem_first = video_mem_secondary;
  video_mem_secondary = temp;
  return 0;
}
