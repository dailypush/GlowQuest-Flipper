#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <input/input.h>
#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "radio.h"
/* Native extra beacon radio; stop transmission on exit and page changes. */
typedef enum {Home,Colors,LedMixer,FxStudio,Fireworks,RangeLab,BandScout,Settings} Page;
typedef struct {
 FuriMessageQueue* queue; ViewPort* viewport; Gui* gui; Page page;
 uint8_t selection,color,mask,vibe,packet[9],cue;
 bool running; GlowRadio radio; uint32_t next_tick,stop_tick; char status[32];
} Glow;
static const char* menu[]={"Colors","LED Mixer","FX Studio","Fireworks","Walk-by Range","Band Scout","Settings"};
static const char* colors[]={"Cyan","Purple","Blue","Yellow","Orange","Red","Green","White","Off"};
static const uint8_t ids[]={0,1,2,16,19,21,25,27,29};
static void draw(Canvas* c,void* ctx){
 Glow* a=ctx; canvas_clear(c); canvas_set_font(c,FontPrimary);
 canvas_draw_str(c,2,11,"GlowQuest Flipper");canvas_draw_line(c,0,14,127,14);
 canvas_set_font(c,FontSecondary);
 if(a->page==Home){
  for(int i=0;i<3;i++){int n=(a->selection/3)*3+i;if(n<7){
   if(n==a->selection)canvas_draw_box(c,0,18+i*12,127,12);
   canvas_set_color(c,n==a->selection?ColorWhite:ColorBlack);
   canvas_draw_str(c,5,27+i*12,menu[n]);canvas_set_color(c,ColorBlack);
  }}
 }else{
  const char* name=a->page==Colors?"Colors":a->page==LedMixer?"LED Mixer":a->page==FxStudio?"FX Studio":a->page==Fireworks?"Fireworks":a->page==RangeLab?"Range Lab":a->page==BandScout?"Band Scout":"Settings";
  canvas_draw_str(c,3,25,name);
  if(a->page==Colors)canvas_draw_str(c,3,37,colors[a->selection%9]);
  else if(a->page==LedMixer){char s[30];snprintf(s,sizeof(s),"Mask: %u Color: %u",a->mask,a->color);canvas_draw_str(c,3,37,s);}
  else if(a->page==FxStudio){char s[30];snprintf(s,sizeof(s),"Vibe: %u Color: %u",a->vibe,a->color);canvas_draw_str(c,3,37,s);}
  else if(a->page==Fireworks){char s[30];snprintf(s,sizeof(s),"Cue %u/12 %s",a->cue,a->running?"RUN":"READY");canvas_draw_str(c,3,37,s);}
  else if(a->page==RangeLab)canvas_draw_str(c,3,37,"Power: unverified");
  else if(a->page==BandScout)canvas_draw_str(c,3,37,"Scanner unavailable");
  else canvas_draw_str(c,3,37,"Radio: extra BLE beacon");
 }
 canvas_draw_line(c,0,49,127,49);canvas_draw_str(c,2,60,a->status);
}
static void input_cb(InputEvent* e,void* ctx){Glow* a=ctx;furi_message_queue_put(a->queue,e,0);}
static void stop(Glow* a){glow_radio_stop(&a->radio);a->running=false;a->cue=0;snprintf(a->status,sizeof(a->status),"Stopped - radio idle");}
int32_t glowquest_flipper_app(void* p){
 UNUSED(p);Glow a={0};a.queue=furi_message_queue_alloc(8,sizeof(InputEvent));
 a.viewport=view_port_alloc();a.gui=furi_record_open(RECORD_GUI);
 strcpy(a.status,"OK: send / Back: stop");
 view_port_draw_callback_set(a.viewport,draw,&a);
 view_port_input_callback_set(a.viewport,input_cb,&a);
 gui_add_view_port(a.gui,a.viewport,GuiLayerFullscreen);
 bool alive=true;InputEvent e;
 while(alive){
  if(furi_message_queue_get(a.queue,&e,50)==FuriStatusOk && e.type==InputTypeShort){
   if(e.key==InputKeyBack){if(a.running)stop(&a);else if(a.page!=Home){glow_radio_stop(&a.radio);a.page=Home;a.selection=0;}else alive=false;}
   else if(a.page==Home){if(e.key==InputKeyDown)a.selection=(a.selection+1)%7;
    if(e.key==InputKeyUp)a.selection=(a.selection+6)%7;
    if(e.key==InputKeyOk){a.page=(Page)(a.selection+1);a.selection=0;}}
   else if(a.page==Colors){if(e.key==InputKeyDown)a.selection=(a.selection+1)%9;
    if(e.key==InputKeyUp)a.selection=(a.selection+8)%9;
    if(e.key==InputKeyOk){a.color=ids[a.selection];gq_fx(a.color,0,0,9,0,false,a.packet);strcpy(a.status,glow_radio_start(&a.radio,a.packet,9,1800)?"BLE broadcasting":"BLE start failed");}}
   else if(a.page==LedMixer||a.page==FxStudio){if(e.key==InputKeyUp)a.color=(a.color+1)%32;
    if(e.key==InputKeyDown)a.color=(a.color+31)%32;
    if(e.key==InputKeyLeft){if(a.page==LedMixer)a.mask=(a.mask+4)%5;else a.vibe=(a.vibe+15)%16;}
    if(e.key==InputKeyRight){if(a.page==LedMixer)a.mask=(a.mask+1)%5;else a.vibe=(a.vibe+1)%16;}
    if(e.key==InputKeyOk){gq_fx(a.color,a.mask,a.vibe,9,0,false,a.packet);strcpy(a.status,"Packet ready; TX gated");}}
   else if(a.page==Fireworks){if(e.key==InputKeyOk){if(a.running)stop(&a);else{a.running=true;a.cue=0;a.next_tick=furi_get_tick();a.stop_tick=a.next_tick+60000;strcpy(a.status,"Show simulation only");}}}
   else if(e.key==InputKeyOk)strcpy(a.status,"Not implemented yet");
  }
  if(a.running){uint32_t now=furi_get_tick();if((int32_t)(now-a.stop_tick)>=0){stop(&a);strcpy(a.status,"Show completed");}
   else if((int32_t)(now-a.next_tick)>=0){a.cue++;a.color=ids[a.cue%9];gq_fx(a.color,0,0,9,0,false,a.packet);glow_radio_start(&a.radio,a.packet,9,1800);a.next_tick+=5000;}}
  glow_radio_poll(&a.radio);
  view_port_update(a.viewport);
 }
 stop(&a);gui_remove_view_port(a.gui,a.viewport);view_port_free(a.viewport);
 furi_record_close(RECORD_GUI);furi_message_queue_free(a.queue);return 0;
}