/* Production settings codec/scheduler + actual A/B storage on simulated NOR. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __attribute__(x)
#define OBJ_PROJ 0u
#define OBJ_PANEL 1u
static uint8_t nor[0x100000];
static int fail_after=-1;
static unsigned erases,programs,applied;
static int st_read(uint32_t off,void*p,uint32_t n){if(off+n>sizeof nor)exit(8);memcpy(p,nor+off,n);return 0;}
static int st_erase(uint32_t off){if(off!=0xfc000u&&off!=0xfd000u)exit(9);erases++;memset(nor+off,255,4096);return 0;}
static int st_prog(uint32_t off,const void*p,uint32_t n){
 if(off<0xfc000u||off+n>0xfe000u||(off&255u)+n>256u)exit(10);
 if(fail_after==0)return -9;if(fail_after>0)fail_after--;programs++;
 for(uint32_t i=0;i<n;i++)nor[off+i]&=((const uint8_t*)p)[i];return 0;
}
#include "../firmware/src/storage.c"
static uint32_t ylevels[12],yfx_values[12][3],ytempo=132,ydensity=100,ymutation=35;
static int32_t ytranspose;
static int plat_store_load(uint32_t o,void*p,uint32_t n){return st_load(o,p,n);}
static int plat_store_save(uint32_t o,const void*p,uint32_t n){return st_save(o,p,n);}
static void ym_instrument_level(uint32_t i,uint32_t v){if(i>=12||v>100)exit(6);applied++;}
static void ym_effect(uint32_t i,uint32_t e,uint32_t v){if(i>=12||e>=3||v>100)exit(7);applied++;}
#include "../firmware/src/ym/ym_settings.c"
static void check(int ok,const char*s){printf("%s: %s\n",s,ok?"PASS":"FAIL");if(!ok)exit(2);}
static void reboot(void){ytempo=132;ydensity=100;ymutation=35;ytranspose=0;for(unsigned i=0;i<12;i++){ylevels[i]=100;for(unsigned e=0;e<3;e++)yfx_values[i][e]=0;}ys_load();}
int main(void){
 memset(nor,255,sizeof nor);reboot();check(!ys_dirty&&ylevels[0]==100,"empty flash uses defaults without saving");
 for(unsigned i=0;i<12;i++){ylevels[i]=i*7;for(unsigned e=0;e<3;e++)yfx_values[i][e]=(i*13+e*29)%101;}
 ytempo=147;ydensity=64;ymutation=20;ytranspose=-7;
 ysettings_t wanted;ys_pack(&wanted);ys_changed(100);
 check(!ys_due(9000,1)&&erases==0,"playback/held keys defer all flash writes");
 check(!ys_due(11999,0)&&ys_due(12000,0),"three-second quiet and edit debounce");
 ys_commit(12000);check(ys_state==YS_SAVED&&!ys_dirty&&erases==1,"commit completes once with visible saved state");
 check(!ys_due(14000,0)&&ys_state==YS_CLEAN&&erases==1,"saved state expires without another write");
 reboot();ysettings_t got;ys_pack(&got);check(!memcmp(&got,&wanted,sizeof got)&&applied==48,"reboot restores twelve levels, 36 effects and all four knobs");
 ylevels[0]=99;ys_changed(15000);ylevels[0]=wanted.levels[0];ys_changed(15010);check(!ys_dirty&&!ys_due(19000,0),"returning to saved values cancels pending write");
 for(int fail=0;fail<2;fail++){
  ylevels[0]=55;ys_changed(20000);fail_after=fail;ys_commit(24000);fail_after=-1;
  check(ys_state==YS_ERROR&&ys_dirty&&!ys_due(50000,0),"failed payload/header save does not retry in a loop");
  reboot();ys_pack(&got);check(!memcmp(&got,&wanted,sizeof got),"power loss before commit preserves previous complete snapshot");
 }
 ylevels[0]=55;ys_changed(60000);fail_after=0;ys_commit(64000);fail_after=-1;ys_retry(65000);
 check(ys_due(65000,0),"SAVE retries a failed save explicitly");ys_commit(65000);reboot();check(ylevels[0]==55,"second committed slot restores after reboot");
 st_hdr_t h;int current=st_current(OBJ_PROJ,&h);nor[st_sector(OBJ_PROJ,(uint32_t)current)+ST_PAYLOAD_OFF+8]^=1;
 reboot();ys_pack(&got);check(!memcmp(&got,&wanted,sizeof got),"corrupt newest CRC falls back to previous valid snapshot");
 ysettings_t bad=wanted;bad.schema=3;st_save(OBJ_PROJ,&bad,sizeof bad);reboot();check(ylevels[0]==100&&!ys_dirty,"unknown schema rejected without rewriting flash");
 bad=wanted;bad.effects[11][2]=101;st_save(OBJ_PROJ,&bad,sizeof bad);reboot();check(yfx_values[11][2]==0,"out-of-range effects rejected atomically");
 uint8_t oversized[sizeof wanted+1];memcpy(oversized,&wanted,sizeof wanted);oversized[sizeof wanted]=0;
 st_save(OBJ_PROJ,oversized,sizeof oversized);reboot();check(ylevels[0]==100,"oversized record cannot pass as a truncated valid schema");
 /* Boundary validation must reject the whole record, not clamp individual fields. */
 for(unsigned k=0;k<6;k++){
  bad=wanted;if(k==0)bad.tempo=79;if(k==1)bad.tempo=181;if(k==2)bad.density=101;if(k==3)bad.mutation=101;if(k==4)bad.transpose=-13;if(k==5)bad.transpose=13;
  st_save(OBJ_PROJ,&bad,sizeof bad);reboot();check(ytempo==132&&ydensity==100&&ymutation==35&&ytranspose==0&&ylevels[0]==100,"invalid knob field rejects the full settings record");
 }
 for(unsigned k=0;k<2;k++){
  bad=wanted;bad.tempo=k?180:80;bad.density=bad.mutation=k?100:0;bad.transpose=k?12:-12;
  st_save(OBJ_PROJ,&bad,sizeof bad);reboot();ys_pack(&got);check(!memcmp(&got,&bad,sizeof got),"knob extrema survive reboot including signed transpose");
 }
 ysettings_v1_t legacy;memcpy(&legacy,&wanted,sizeof legacy);legacy.schema=1;
 st_save(OBJ_PROJ,&legacy,sizeof legacy);unsigned before_migration=erases;reboot();
 ys_pack(&got);got.schema=1;
 check(!memcmp(&got,&legacy,sizeof legacy),"schema 1 migration retains every old level and effect");
 check(ytempo==132&&ydensity==100&&ymutation==35&&ytranspose==0&&!ys_dirty&&erases==before_migration,"schema 1 migration keeps knob defaults without a boot-time write");
 ytempo=155;ys_changed(70000);check(ys_dirty,"K1-only change marks snapshot dirty");ys_commit(74000);reboot();check(ytempo==155&&ylevels[11]==77,"first knob edit saves migrated mixer in schema 2");
 st_save(OBJ_PROJ,"DUB project",11);reboot();check(ylevels[0]==100,"foreign project does not load as Emerald settings");
 ys_quiet_at=0xfffffff0u;ylevels[1]=25;ys_changed(0xfffffff0u);
 check(!ys_due(0x100u,0)&&ys_due(0x1000u,0),"millisecond wrap preserves debounce");
 for(unsigned i=0;i<0xfc000u;i++)if(nor[i]!=255)exit(11);
 for(unsigned i=0xfe000u;i<sizeof nor;i++)if(nor[i]!=255)exit(12);
 check(1,"all writes confined to existing FC000/FD000 pair; panel/OTA/code untouched");
 printf("Settings tests passed (%u erases, %u page writes).\n",erases,programs);return 0;
}
