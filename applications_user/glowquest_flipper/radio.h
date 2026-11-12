#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct {bool active; uint32_t until_tick;} GlowRadio;
bool glow_radio_start(GlowRadio* radio,const uint8_t* payload,uint8_t len,uint32_t duration_ms);
void glow_radio_poll(GlowRadio* radio);
void glow_radio_stop(GlowRadio* radio);