/* Run the real tile renderer against an in-memory LCD, not a mock layout. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __attribute__(x)
static uint16_t screen[240*240];
static unsigned fills,blits,last_w,last_h;
static void lcd_sync(void){}
static void lcd_fill(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint16_t c){
 if(x+w>240||y+h>240)exit(3);fills++;for(uint32_t j=y;j<y+h;j++)for(uint32_t i=x;i<x+w;i++)screen[j*240+i]=c;
}
static void lcd_blit(uint32_t x,uint32_t y,uint32_t w,uint32_t h,const uint16_t*p){
 if(x+w>240||y+h>240)exit(4);blits++;last_w=w;last_h=h;
 for(uint32_t j=0;j<h;j++)for(uint32_t i=0;i<w;i++){uint16_t v=p[j*w+i];screen[(j+y)*240+i+x]=(uint16_t)((v>>8)|(v<<8));}
}
#include "../firmware/src/gfx.c"
static void fmt_int(char*b,int32_t v){sprintf(b,"%d",v);}
static unsigned str_len(const char*s){return (unsigned)strlen(s);}
static void str_cpy(char*d,const char*s,unsigned max){unsigned i=0;while(s[i]&&i+1<max){d[i]=s[i];i++;}d[i]=0;}
static uint32_t ytempo=132,ydensity=100,ymutation=35,yplaying,ym_overload,audio_cpu_lifetime_peak=80,audio_xruns;
static int32_t ytranspose;
static uint32_t ypsg=1,yinstrument,ylevels[12]={100,100,100,100,100,100,100,100,100,100,100,100},ydrums_mix=100;
static uint32_t current_bar=12,current_step=7;
static uint32_t ym_bar(void){return current_bar;}
static uint32_t ym_step(void){return current_step;}
#include "../firmware/src/ym/ym_ui.c"
static void check(int v,const char*s){printf("%s: %s\n",s,v?"PASS":"FAIL");if(!v)exit(2);}
static void image(const char*path){FILE*f=fopen(path,"wb");if(!f)exit(5);fprintf(f,"P6\n240 240\n255\n");for(int i=0;i<240*240;i++){uint16_t c=screen[i];uint8_t p[3]={(uint8_t)(((c>>11)&31)*255/31),(uint8_t)(((c>>5)&63)*255/63),(uint8_t)((c&31)*255/31)};fwrite(p,1,3,f);}fclose(f);}
int main(void){
 palette_set(0);ydisplay();lcd_sync();image("build/host/ui-paused.ppm");unsigned initial=blits,clears=fills;
 for(int i=0;i<100;i++)ydisplay();check(blits==initial&&fills==clears,"idle: zero repeated LCD writes");
 ytempo=145;ydisplay();check(blits==initial+1&&last_w==108&&last_h==52,"one knob: exactly one atomic tile");
 initial=blits;yplaying=1;ydisplay();check(blits==initial+2,"transport change: transport + steps only");
 initial=blits;current_step=8;ydisplay();check(blits==initial+1&&last_h==12,"one step: small strip only");
 image("build/host/ui-playing.ppm");ymutation=0;ytranspose=-7;ydisplay();image("build/host/ui-varied.ppm");
 initial=blits;yinstrument=9;ydisplay();check(blits==initial+1&&last_w==148,"SELECT selects an instrument: one tile only");
 initial=blits;ypsg=0;ydisplay();check(blits==initial+1&&last_h==34,"PSG switch updates selected voice tile");
 initial=blits;ylevels[yinstrument]=50;ydisplay();check(blits==initial+1&&last_w==148,"ALGORITHM updates only selected instrument level");
 initial=blits;ydrums_mix=0;ydisplay();check(blits==initial+1&&last_w==72,"PRESETS level: percussion bus tile only");
 initial=blits;audio_cpu_lifetime_peak=95;audio_xruns=42;ydisplay();check(blits==initial,"USB diagnostics never trigger screen writes");
 ypsg=1;ydisplay();image("build/host/ui-mixer.ppm");
 ym_overload=8;yplaying=0;ydisplay();image("build/host/ui-overload.ppm");
 check(fills==clears,"no visible clears while playing or updating");
 ydisplay_invalidate();ydisplay();check(fills==clears+1,"full redraw only on explicit invalidation");
 printf("Renderer bounds and redraw checks passed.\n");return 0;
}
