/* Production routing: USB receives full mix, physical output alone fades/mutes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FELUCCA_UAC 1
enum {B_GLO=5,B_PLAY=10};
static uint32_t yusb_only,ym_overload,renders,taps;
static int32_t usb[512],input[512];
static void ym_render(int32_t*out,uint32_t n,uint32_t master){(void)master;renders++;memcpy(out,input,n*2*sizeof(int32_t));}
static void uac_tap(const int32_t*out,uint32_t n){taps++;memcpy(usb,out,n*2*sizeof(int32_t));}
#include "../firmware/src/ym/ym_output.h"
static void check(int ok,const char*s){printf("%s: %s\n",s,ok?"PASS":"FAIL");if(!ok)exit(2);}
static int zero(const int32_t*p,uint32_t n){for(uint32_t i=0;i<n;i++)if(p[i])return 0;return 1;}
int main(void){
 struct {uint32_t pre;int32_t out[512];uint32_t post;} guarded={0x12345678,{0},0xabcdef01};
 int32_t *out=guarded.out;
 for(uint32_t i=0;i<256;i++){input[2*i]=8388607;input[2*i+1]=-8388608;}
 youtput_block(out,256,4096,0);
 check(!memcmp(out,input,sizeof input)&&!memcmp(usb,input,sizeof input),"normal mode preserves every PCM bit on both outputs");
 youtput_controls(1u<<B_PLAY);check(!yusb_only,"unrelated key does not toggle output routing");
 youtput_controls(1u<<B_GLO);check(yusb_only==1,"GLO edge enables USB ONLY");
 youtput_block(out,256,4096,yusb_only);
 check(!memcmp(usb,input,sizeof input)&&ylocal_gain==0,"USB mix stays unchanged during local mute ramp");
 int fade=1;for(unsigned i=1;i<256;i++)if(out[2*i]>out[2*(i-1)]||out[2*i+1]<out[2*(i-1)+1])fade=0;
 check(fade&&out[510]==0&&out[511]==0,"both physical channels fade monotonically to silence");
 youtput_block(out,256,4096,yusb_only);
 check(zero(out,512)&&!memcmp(usb,input,sizeof input),"USB keeps signal while physical buffer is entirely silent");
 youtput_controls(1u<<B_GLO);check(!yusb_only,"next GLO edge restores physical output");
 youtput_block(out,63,4096,yusb_only);check(ylocal_gain==63,"ramp carries smoothly across short render blocks");
 youtput_block(out,256,4096,yusb_only);
 check(ylocal_gain==256&&out[510]==input[510]&&out[511]==input[511],"unmute reaches exact unity without altering USB");
 unsigned count=renders;ym_overload=8;youtput_block(out,256,4096,0);
 check(renders==count&&zero(out,512)&&zero(usb,512),"overload still silences USB and physical outputs without rendering");
 check(guarded.pre==0x12345678&&guarded.post==0xabcdef01,"output routing stays within the DMA buffer");
 check(taps==renders+1,"USB tap remains active including protected silence");
 return 0;
}
