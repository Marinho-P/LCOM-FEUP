#include "menuViewer.h"
#include "../labs/graphics.h"
int drawMenuBackground(xpm_image_t img){
  if(drawXPMImage(img,512,381,0)) return 1;
  return 0;
}
int drawMenuButton(Button* button){
    if(button->selected){
        if(drawXPMImage(button->imgSelected,button->pos.x+100,button->pos.y+25,0)) return 1;
    }
    else{
        if(drawXPMImage(button->img,button->pos.x+100,button->pos.y+25,0)) return 1;
    }
    return 0;
}
int drawMenuMouse(Mouse* mouse, xpm_image_t mouseImg, xpm_image_t imgMouseHover, bool isHovering){
    if(isHovering){
        if(drawXPMImage(imgMouseHover,mouse->pos.x,mouse->pos.y,0)) return 1;
    }
    else{
        if(drawXPMImage(mouseImg,mouse->pos.x,mouse->pos.y,0)) return 1;
    }
    return 0;
}
