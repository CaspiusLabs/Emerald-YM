/* Run the real tile renderer against an in-memory LCD, not a mock layout. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __attribute__(x)
static uint16_t screen[240*240];
static unsigned fills,blits,last_w,last_h,last_x,last_y;
static void lcd_sync(void){}
static void lcd_fill(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint16_t c){
 if(x+w>240||y+h>240)exit(3);fills++;for(uint32_t j=y;j<y+h;j++)for(uint32_t i=x;i<x+w;i++)screen[j*240+i]=c;
}
static void lcd_blit(uint32_t x,uint32_t y,uint32_t w,uint32_t h,const uint16_t*p){
 if(x+w>240||y+h>240||h>144)exit(4);blits++;last_w=w;last_h=h;last_x=x;last_y=y;
 for(uint32_t j=0;j<h;j++)for(uint32_t i=0;i<w;i++){uint16_t v=p[j*w+i];screen[(j+y)*240+i+x]=(uint16_t)((v>>8)|(v<<8));}
}
#include "../firmware/src/gfx.c"
#include "../firmware/src/ym/ym_clock.h"
static struct ym_clock yclock={0,0,0,0,0,132,0,0,0,0};
static void fmt_int(char*b,int32_t v){sprintf(b,"%d",v);}
static unsigned str_len(const char*s){return (unsigned)strlen(s);}
static void str_cpy(char*d,const char*s,unsigned max){unsigned i=0;while(s[i]&&i+1<max){d[i]=s[i];i++;}d[i]=0;}
static uint32_t yusb_only,ymaster_adc=512;
static uint32_t ytempo=132,ydensity=100,ymutation=35,yplaying,ym_overload,audio_cpu_lifetime_peak=80,audio_xruns;
static int32_t ytranspose;
static uint32_t ypsg=1,yinstrument,ylevels[12]={100,100,100,100,100,100,100,100,100,100,100,100};
static uint32_t yfx_kind[12],yfx_values[12][3];
static uint32_t current_bar=12,current_step=7;
static uint32_t ym_bar(void){return current_bar;}
static uint32_t ym_step(void){return current_step;}
static char test_version[16]="TEST";
#define EN_VERSION test_version
enum {YS_CLEAN,YS_DIRTY,YS_WRITING,YS_SAVED,YS_ERROR};
static uint32_t ys_state;
#include "../firmware/src/ym/ym_ui.c"
static void check(int v,const char*s){printf("%s: %s\n",s,v?"PASS":"FAIL");if(!v)exit(2);}
static void image(const char*path){FILE*f=fopen(path,"wb");if(!f)exit(5);fprintf(f,"P6\n240 240\n255\n");for(int i=0;i<240*240;i++){uint16_t c=screen[i];uint8_t p[3]={(uint8_t)(((c>>11)&31)*255/31),(uint8_t)(((c>>5)&63)*255/63),(uint8_t)((c&31)*255/31)};fwrite(p,1,3,f);}fclose(f);}
int main(void){
 FILE*vf=fopen("VERSION","r");if(!vf||fscanf(vf,"%15s",test_version)!=1)exit(6);fclose(vf);
 check(40+text_w(&FONT_M,"EMERALD YM")<185,"large uppercase header fits before the smaller badge");
 palette_set(0);yui_boot();lcd_sync();image("build/host/ui-boot.ppm");ydisplay();lcd_sync();image("build/host/ui-paused.ppm");unsigned initial=blits,clears=fills;
 for(int i=0;i<100;i++)ydisplay();check(blits==initial&&fills==clears,"idle: zero repeated LCD writes");

 {static uint16_t before[240*240];memcpy(before,screen,sizeof screen);
  initial=blits;ymaster_adc=1023;ydisplay();
  check(blits==initial+1&&last_x==10&&last_y==70&&last_w==22&&last_h==108&&yui_old.master==90,"MASTER maximum redraws only the slider tile");
  check(screen[78*240+12]==Y_RED,"MASTER maximum puts red marker at top");image("build/host/ui-master-max.ppm");
  ymaster_adc=0;ydisplay();check(yui_old.master==0&&screen[168*240+12]==Y_RED,"MASTER minimum puts red marker at bottom");image("build/host/ui-master-min.ppm");
  int outside=1;for(int y=0;y<240;y++)for(int x=0;x<240;x++)if(!(x>=10&&x<32&&y>=70&&y<178)&&screen[y*240+x]!=before[y*240+x])outside=0;
  check(outside,"MASTER movement preserves all surrounding pixels");
  uint32_t previous=0;int monotonic=1;for(uint32_t raw=0;raw<=1023;raw++){
   ymaster_adc=raw;ydisplay();if(yui_old.master<previous||yui_old.master>90)monotonic=0;previous=yui_old.master;}
  check(monotonic&&previous==90,"full ADC sweep moves slider monotonically from 0 to 10");
  ymaster_adc=512;ydisplay();image("build/host/ui-master-mid.ppm");initial=blits;
  for(int i=0;i<100;i++){ymaster_adc=(uint32_t)(512+(i%3)-1);ydisplay();}
  check(blits==initial,"filtered MASTER jitter does not repaint the slider");
  check(yui_master_position(519,45)==45&&yui_master_position(520,45)==46&&yui_master_position(516,46)==46&&yui_master_position(515,46)==45,"visual hysteresis holds near a pixel boundary in both directions");
  check(ymaster_adc>=511&&ymaster_adc<=513,"slider does not write back to MASTER input");
 }
 initial=blits;
 ytempo=145;ydisplay();check(blits==initial+1&&last_w==86&&last_h==44,"one knob: exactly one atomic tile");
 initial=blits;yplaying=1;ydisplay();check(blits==initial+2,"transport change: transport + steps only");
 initial=blits;current_step=8;ydisplay();check(blits==initial+1&&last_h==12,"one step: small strip only");
 image("build/host/ui-playing.ppm");ymutation=0;ytranspose=-7;ydisplay();image("build/host/ui-varied.ppm");
 initial=blits;yusb_only=1;ydisplay();check(blits==initial+1&&last_x==8&&last_y==184&&last_w==40&&last_h==32,"GLO redraws only the USB ONLY mode tile");image("build/host/ui-usb-only.ppm");
 initial=blits;ydisplay();check(blits==initial,"stable USB ONLY mode never redraws");yusb_only=0;ydisplay();
 initial=blits;yinstrument=9;ydisplay();check(blits==initial+2,"SELECT updates instrument and its effect tile");
 initial=blits;ypsg=0;ydisplay();check(blits==initial+1&&last_h==38,"PSG switch updates selected voice tile");
 initial=blits;ylevels[yinstrument]=50;ydisplay();check(blits==initial+1&&last_w==116,"PRESETS updates only selected instrument level");
 initial=blits;yfx_kind[yinstrument]=2;ydisplay();check(blits==initial+1&&last_w==60,"FX button changes only effect tile");
 initial=blits;yfx_values[yinstrument][2]=65;ydisplay();check(blits==initial+1&&last_w==60,"ALGORITHM changes only selected effect tile");
 initial=blits;ys_state=YS_DIRTY;ydisplay();check(blits==initial+1&&last_h==24,"pending save updates only transport tile");
 ys_state=YS_WRITING;ydisplay();image("build/host/ui-saving.ppm");ys_state=YS_SAVED;ydisplay();image("build/host/ui-saved.ppm");ys_state=YS_ERROR;ydisplay();image("build/host/ui-save-error.ppm");ys_state=YS_CLEAN;ydisplay();
 initial=blits;audio_cpu_lifetime_peak=95;audio_xruns=42;ydisplay();check(blits==initial,"USB diagnostics never trigger screen writes");
 ypsg=1;ydisplay();image("build/host/ui-mixer.ppm");
 initial=blits;yclock.mode=yclock.running=1;yclock.bpm=120;ydisplay();
 check(blits==initial+1&&last_w==86&&last_h==44,"USB clock updates only the tempo tile");image("build/host/ui-usb-clock.ppm");
 initial=blits;for(int i=0;i<100;i++)ydisplay();check(blits==initial,"stable external clock never redraws");
 yclock.bpm=30;ydisplay();yclock.bpm=300;ydisplay();
 initial=blits;yclock.lost=1;yclock.running=0;ydisplay();check(blits==initial+1,"clock loss redraws its label without a whole-screen clear");
 image("build/host/ui-usb-clock-lost.ppm");yc_internal(&yclock);ydisplay();
 ym_overload=8;yplaying=0;ydisplay();image("build/host/ui-overload.ppm");
 check(fills==clears,"no visible clears while playing or updating");
 ydisplay_invalidate();ydisplay();check(fills==clears+1,"full redraw only on explicit invalidation");
 printf("Renderer bounds and redraw checks passed.\n");return 0;
}
