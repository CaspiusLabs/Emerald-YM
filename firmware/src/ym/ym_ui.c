/* SPDX-License-Identifier: GPL-3.0-only
 * Atomic offscreen tiles: never clear the visible screen during normal refresh.
 * Uses the platform's existing canvas and LCD DMA; no new framebuffer. */
#define Y_BG RGB(5,9,20)
#define Y_CARD RGB(13,23,42)
#define Y_EDGE RGB(28,48,77)
#define Y_BLUE RGB(50,197,255)
#define Y_GREEN RGB(96,241,169)
#define Y_WHITE RGB(224,239,255)
#define Y_DIM RGB(121,149,181)
#define Y_AMBER RGB(255,176,64)
static uint8_t yui_ready;
static struct {uint32_t tempo,density,mutation,bar,playing,overload,step,psg,mix,melody,drums;int32_t transpose;} yui_old;
static void yui_header(void){
 cv_begin(240,44,Y_BG);
 /* Original faceted jewel + striped 16-bit badge, drawn natively. */
 for(int i=0;i<3;i++){
  int r=15-i*4;uint16_t c=i==1?Y_WHITE:Y_BLUE;
  cv_line(22,5+i*4,22+r,20,c);cv_line(22+r,20,22,35-i*4,c);
  cv_line(22,35-i*4,22-r,20,c);cv_line(22-r,20,22,5+i*4,c);
 }
 cv_line(7,20,37,20,Y_GREEN);cv_line(22,5,22,35,Y_GREEN);
 cv_text(45,2,&FONT_M,"EMERALD",RGB(14,68,122));
 cv_text(44,1,&FONT_M,"EMERALD",Y_WHITE);
 cv_text(45,28,&FONT_XS,"YM + PSG / LIVE",Y_BLUE);
 cv_rect(171,5,62,27,Y_EDGE);cv_rect(173,7,58,23,Y_CARD);
 cv_text(184,10,&FONT_XS,"16-BIT",Y_WHITE);
 for(int k=0;k<3;k++)cv_rect(178,24+k*2,48,1,k==1?Y_BLUE:Y_DIM);
 cv_rect(8,42,224,1,Y_EDGE);cv_blit(0,0);
}
static void yui_transport(uint32_t bar){
 char b[16];uint16_t accent=ym_overload>=8?Y_AMBER:yplaying?Y_GREEN:Y_DIM;
 cv_begin(224,24,Y_CARD);
 if(yplaying){for(int x=0;x<9;x++)cv_rect(8+x,5+x/2,1,13-x,accent);}
 else {cv_rect(8,6,3,12,accent);cv_rect(14,6,3,12,accent);}
 cv_text(25,4,&FONT_S,ym_overload>=8?"OVERLOAD":yplaying?"PLAYING":"PAUSED",accent);
 cv_text(142,6,&FONT_XS,"BAR",Y_DIM);fmt_int(b,(int32_t)(bar%100000u));
 cv_text(166,4,&FONT_S,b,Y_WHITE);cv_blit(8,48);
}
static void yui_card(uint32_t index,int32_t value){
 static const char*const label[]={"K1 TEMPO","K2 DENSITY","K3 MUTATION","K4 TRANSPOSE"};
 char b[16];uint16_t accent=index==0||index==3?Y_BLUE:Y_GREEN;
 cv_begin(108,52,Y_CARD);
 cv_rect(0,0,108,1,Y_EDGE);cv_rect(0,51,108,1,Y_EDGE);
 cv_text(6,4,&FONT_XS,label[index],Y_DIM);
 if(index==3&&value>0){b[0]='+';fmt_int(b+1,value);}else fmt_int(b,value);
 cv_text(6,18,&FONT_M,b,Y_WHITE);
 cv_text(77,27,&FONT_XS,index==0?"BPM":index==3?"ST":"%",accent);
 cv_rect(6,46,96,3,Y_EDGE);
 if(index==3){
  int x=54+value*4;
  cv_rect(value<0?x:54,46,value<0?54-x:x-54,3,accent);cv_rect(53,44,1,7,Y_DIM);cv_rect(x-1,44,2,7,Y_WHITE);
 }else{
  uint32_t fill=index==0?(uint32_t)(value-80)*96u/100u:(uint32_t)value*96u/100u;
  if(fill>96u)fill=96u;cv_rect(6,46,(int32_t)fill,3,accent);
 }
 cv_blit(index&1u?124u:8u,index>=2u?134u:76u);
}
static void yui_mixer(uint32_t index,uint32_t value,int enabled){
 static const char*const labels[]={"SEL MELO","ALG PSG","PRE DRUM"};
 char b[16];uint16_t accent=enabled?(index==1?Y_GREEN:Y_BLUE):Y_DIM;
 uint32_t limit=index==1?400u:100u;
 cv_begin(72,34,Y_CARD);cv_rect(0,0,72,1,Y_EDGE);
 cv_text(4,2,&FONT_XS,labels[index],Y_DIM);
 if(enabled){fmt_int(b,(int32_t)value);str_cpy(b+str_len(b),"%",sizeof b-str_len(b));}
 else str_cpy(b,"OFF",sizeof b);
 cv_text(4,15,&FONT_S,b,accent);cv_rect(4,31,64,2,Y_EDGE);
 if(enabled)cv_rect(4,31,(int32_t)(value*64u/limit),2,accent);
 cv_blit(8u+index*76u,190);
}
static void yui_steps(uint32_t step){
 cv_begin(224,12,Y_BG);
 for(uint32_t k=0;k<16;k++)cv_rect((int32_t)(k*14u),4,10,4,yplaying&&k==step?Y_GREEN:k%4u==0u?Y_DIM:Y_EDGE);
 cv_blit(8,228);
}
static void ydisplay_invalidate(void){yui_ready=0;}
static void ydisplay(void){
 uint32_t bar=ym_bar(),step=ym_step();
 uint32_t overloaded=ym_overload>=8u,step_key=step|(yplaying<<4)|(overloaded<<5);
 if(!yui_ready){
  lcd_fill(0,0,240,240,Y_BG);yui_header();
  memset(&yui_old,255,sizeof yui_old);yui_old.transpose=1000;yui_ready=1;
 }
 if(yui_old.playing!=yplaying||yui_old.bar!=bar||yui_old.overload!=overloaded){yui_transport(bar);yui_old.playing=yplaying;yui_old.bar=bar;yui_old.overload=overloaded;}
 if(yui_old.tempo!=ytempo){yui_card(0,(int32_t)ytempo);yui_old.tempo=ytempo;}
 if(yui_old.density!=ydensity){yui_card(1,(int32_t)ydensity);yui_old.density=ydensity;}
 if(yui_old.mutation!=ymutation){yui_card(2,(int32_t)ymutation);yui_old.mutation=ymutation;}
 if(yui_old.transpose!=ytranspose){yui_card(3,ytranspose);yui_old.transpose=ytranspose;}
 if(yui_old.step!=step_key){yui_steps(step);yui_old.step=step_key;}
 if(yui_old.psg!=ypsg||yui_old.mix!=ypsg_mix){yui_mixer(1,ypsg_mix,ypsg);yui_old.psg=ypsg;yui_old.mix=ypsg_mix;}
 if(yui_old.melody!=ymelody_mix){yui_mixer(0,ymelody_mix,1);yui_old.melody=ymelody_mix;}
 if(yui_old.drums!=ydrums_mix){yui_mixer(2,ydrums_mix,1);yui_old.drums=ydrums_mix;}
}
