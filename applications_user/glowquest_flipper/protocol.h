#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#define GQ_COMPANY 0x0183
#define GQ_PACKET_LEN 9
bool gq_fx(uint8_t color,uint8_t mask,uint8_t vibration,uint8_t time,uint8_t fade,bool extended,uint8_t out[GQ_PACKET_LEN]);
bool gq_ad(uint8_t const* payload,size_t len,uint8_t out[31],size_t* out_len);