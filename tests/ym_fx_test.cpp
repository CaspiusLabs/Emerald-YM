/* Actual shared-delay/control implementation, independent impulse tests. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../firmware/src/ym/ym_fx.h"
static void check(int v,const char*s){printf("%s: %s\n",s,v?"PASS":"FAIL");if(!v)exit(2);}
int main(){
 fx_init();float l=123,r=-456;fx_echo(100,100,&l,&r);
 check(l==123&&r==-456,"all effects off: echo is transparent");
 fx_requested[2][0]=100;fx_requested[2][1]=60;fx_requested[2][2]=80;fx_dirty=4;fx_sync();
 check(fx_gate[2]==64&&fx_values[2][1]==60&&emerald_echo_gain[2]>0,"three effect parameters coexist on one source");
 check(fx_gate[1]==256&&fx_values[1][1]==0&&emerald_echo_gain[1]==0,"other source retains independent neutral parameters");
 fx_requested[2][2]=0;fx_dirty=4;fx_sync();check(fx_values[2][0]==100&&fx_values[2][1]==60,"editing echo retains decay and color");
 fx_init();fx_echo_any=1;int first=-1,right_isolated=1;float peak=0;
 for(int i=0;i<17000;i++){l=r=0;fx_echo(i==0?1000.0f:0.0f,0,&l,&r);if(l>peak)peak=l;if(l!=0&&first<0)first=i;if(r!=0)right_isolated=0;}
 check(right_isolated,"echo send does not cross into other stereo channel");
 check(first==8193&&peak==375,"stereo impulse returns after 4096 half-rate frames at bounded gain");
 fx_echo_any=0;int tail_seen=0,late_silent=1;
 for(int i=0;i<100000;i++){l=r=0;fx_echo(0,0,&l,&r);if(l!=0||r!=0){tail_seen=1;if(i>98000)late_silent=0;}}
 check(tail_seen&&late_silent&&fx_tail==0,"real echo tail decays after sends are disabled");
 fx_init();fx_echo_any=1;l=r=0;fx_echo(1000,0,&l,&r);l=r=0;fx_echo(0,0,&l,&r);
 fx_reset_audio();int reset_silent=1;
 for(int i=0;i<10000;i++){l=r=0;fx_echo(0,0,&l,&r);if(l!=0||r!=0)reset_silent=0;}
 check(reset_silent,"epoch reset discards a previously written impulse over a full delay cycle");
 return 0;
}
