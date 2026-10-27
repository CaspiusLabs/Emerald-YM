/* SPDX-License-Identifier: GPL-3.0-only
 * Main-loop-only settings. Existing project A/B sectors, header-last commit + CRC.
 * Never write during playback, held live keys, or the three-second release window. */
#define YSET_MAGIC 0x31534d45u /* "EMS1", distinct from inherited projects */
#define YSET_QUIET_MS 3000u
typedef struct {uint32_t magic,schema;uint8_t levels[12],effects[12][3];} ysettings_v1_t;
typedef struct {uint32_t magic,schema;uint8_t levels[12],effects[12][3];uint32_t tempo,density,mutation;int32_t transpose;} ysettings_t;
enum {YS_CLEAN,YS_DIRTY,YS_WRITING,YS_SAVED,YS_ERROR};
static ysettings_t ys_saved;
static uint32_t ys_dirty,ys_state,ys_changed_at,ys_quiet_at,ys_feedback_at,ys_failed;
static void ys_pack(ysettings_t*p){
 p->magic=YSET_MAGIC;p->schema=2;
 p->tempo=ytempo;p->density=ydensity;p->mutation=ymutation;p->transpose=ytranspose;
 for(uint32_t i=0;i<12u;i++){p->levels[i]=(uint8_t)ylevels[i];for(uint32_t e=0;e<3u;e++)p->effects[i][e]=(uint8_t)yfx_values[i][e];}
}
static int ys_valid(const ysettings_t*p){
 if(p->magic!=YSET_MAGIC||p->schema!=2u)return 0;
 if(p->tempo<80u||p->tempo>180u||p->density>100u||p->mutation>100u||p->transpose< -12||p->transpose>12)return 0;
 for(uint32_t i=0;i<12u;i++){if(p->levels[i]>100u)return 0;for(uint32_t e=0;e<3u;e++)if(p->effects[i][e]>100u)return 0;}
 return 1;
}
static void ys_load(void){
 /* Extra byte detects oversized/truncated records from another schema. */
 uint8_t raw[sizeof(ysettings_t)+1u];ysettings_t p;
 ys_pack(&ys_saved);ys_dirty=0;ys_state=YS_CLEAN;ys_failed=0;
 int n=plat_store_load(OBJ_PROJ,raw,sizeof raw);
 if(n==(int)sizeof(ysettings_v1_t)){
  /* Legacy mixer-only snapshot: preserve it and keep current knob defaults.
   * Migration is RAM-only; first actual edit commits schema 2. */
  ys_pack(&p);memcpy(&p,raw,sizeof(ysettings_v1_t));if(p.schema!=1u)return;p.schema=2;
 }else if(n==(int)sizeof p)memcpy(&p,raw,sizeof p);else return;
 if(!ys_valid(&p))return;
 ys_saved=p;ytempo=p.tempo;ydensity=p.density;ymutation=p.mutation;ytranspose=p.transpose;
 for(uint32_t i=0;i<12u;i++){
  ylevels[i]=p.levels[i];ym_instrument_level(i,ylevels[i]);
  for(uint32_t e=0;e<3u;e++){yfx_values[i][e]=p.effects[i][e];ym_effect(i,e,yfx_values[i][e]);}
 }
}
static void ys_changed(uint32_t now){
 ysettings_t p;ys_pack(&p);ys_dirty=memcmp(&p,&ys_saved,sizeof p)!=0;
 ys_state=ys_dirty?YS_DIRTY:YS_CLEAN;ys_changed_at=now;ys_failed=0;
}
static void ys_retry(uint32_t now){
 ys_failed=0;
 if(ys_dirty)ys_state=YS_DIRTY;else {ys_state=YS_SAVED;ys_feedback_at=now;}
}
static int ys_due(uint32_t now,uint32_t active){
 if(active)ys_quiet_at=now;
 if(ys_state==YS_SAVED&&now-ys_feedback_at>=1500u)ys_state=YS_CLEAN;
 return ys_dirty&&!ys_failed&&!active&&now-ys_changed_at>=YSET_QUIET_MS&&now-ys_quiet_at>=YSET_QUIET_MS;
}
static void ys_commit(uint32_t now){
 ysettings_t p;ys_pack(&p);
 if(plat_store_save(OBJ_PROJ,&p,sizeof p)==0){ys_saved=p;ys_dirty=0;ys_state=YS_SAVED;ys_feedback_at=now;}
 else {ys_failed=1;ys_state=YS_ERROR;} /* No erase/retry loop on a failing part. */
}
