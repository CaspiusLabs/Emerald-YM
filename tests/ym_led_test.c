/* Exercise the production LED publisher with the documented FM-1 wiring. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define FM1_NCOL 11
enum {B_FX=0,B_SAVE=7,B_ARP=8,B_SEQ=9,B_PLAY=10,B_REC=11,B_OCTDN=12,B_OCTUP=13};
static const int8_t FM1_KEYMAP[6][11]={
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
 {5,11,4,10,3,9,2,8,-1,-1,-1},
 {34,35,36,37,38,40,39,13,7,6,12},
 {23,24,25,26,27,28,29,30,31,32,33},
 {0,1,15,14,17,16,19,18,20,21,22},
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};
static struct {uint8_t btn[14];}panel={{2,3,4,5,6,7,8,9,10,11,12,13,0,1}};
static struct {uint32_t buttons;}fm1_in;
static uint8_t fm1_led[11],fm1_led_dim[11],fm1_led_dim_div;
static uint32_t yinstrument,yfx_values[12][3];
static uint32_t fm1_ms,yplaying,ym_overload,ymutation=35,ypsg=1,live,sequence;
static uint32_t ym_live_active_keys(void){return live;}
static uint32_t ym_sequence_keys(void){return sequence;}
enum {YS_CLEAN,YS_DIRTY,YS_WRITING,YS_SAVED,YS_ERROR};
static uint32_t ys_state;
#include "../firmware/src/ym/ym_leds.c"
static void check(int v,const char*s){printf("%s: %s\n",s,v?"PASS":"FAIL");if(!v)exit(2);}
int main(void){
 yled_init();yled_update();check((fm1_led_dim[5]&2)!=0,"SAVE discoverable when clean");ys_state=YS_DIRTY;yled_update();check((fm1_led[5]&2)!=0,"SAVE bright with unsaved settings");ys_state=YS_CLEAN;yled_update();check((fm1_led_dim[6]&2)!=0,"FX discoverable dim when dry");yfx_values[0][2]=50;yled_update();check((fm1_led[6]&2)!=0&&!(fm1_led_dim[6]&2),"FX bright when any parameter is active");yfx_values[0][2]=0;live=1;sequence=1u<<12;yled_update();
 check((fm1_led[3]&16)!=0,"F3 live key: physical LED bright");
 check((fm1_led_dim[3]&8)!=0&&!(fm1_led[3]&8),"F4 sequencer key: physical LED dim");
 sequence|=live;yled_update();check(!(fm1_led_dim[3]&16),"live brightness wins over sequencer");
 live=sequence=0;yled_update();check(!(fm1_led[3]&16)&&!(fm1_led_dim[3]&8),"released notes clear without stale LED bits");
 live=1;sequence=1u<<12;ym_overload=8;yled_update();
 check(!(fm1_led[3]&16)&&!(fm1_led_dim[3]&8),"overload clears note indication");
 check((fm1_led[10]&4)!=0,"overload keeps red PLAY indicator");
 check(fm1_led_dim_div==4,"dim scanning brightness unchanged");
 return 0;
}
