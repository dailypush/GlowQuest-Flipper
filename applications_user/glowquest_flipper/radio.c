#include "radio.h"
#include "protocol.h"
#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_random.h>
#include <extra_beacon.h>
#include <string.h>

/* Non-connectable extra beacon; independent of Flipper's paired Bluetooth profile.
   All effects use BLE manufacturer-specific AD, company ID 0x0183. */
void glow_radio_stop(GlowRadio* radio) {
    if(!radio)return;
    if(radio->active) {
        furi_hal_bt_extra_beacon_stop();
        radio->active=false;
    }
}
bool glow_radio_start(GlowRadio* radio,const uint8_t* payload,uint8_t len,uint32_t duration_ms) {
    if(!radio || !payload || !duration_ms)return false;
    uint8_t ad[31];size_t ad_len=0;
    if(!gq_ad(payload,len,ad,&ad_len))return false;
    glow_radio_stop(radio);
    GapExtraBeaconConfig cfg={
        .min_adv_interval_ms=20,
        .max_adv_interval_ms=25,
        .adv_channel_map=GapAdvChannelMapAll,
        .adv_power_level=GapAdvPowerLevel_0dBm,
        .address_type=GapAddressTypeRandom,
    };
    furi_hal_random_fill_buf(cfg.address,sizeof(cfg.address));
    cfg.address[5]=(cfg.address[5]&0x3F)|0xC0; /* valid static random address */
    if(!furi_hal_bt_extra_beacon_set_config(&cfg))return false;
    if(!furi_hal_bt_extra_beacon_set_data(ad,(uint8_t)ad_len))return false;
    if(!furi_hal_bt_extra_beacon_start())return false;
    radio->active=true;radio->until_tick=furi_get_tick()+duration_ms;
    return true;
}
void glow_radio_poll(GlowRadio* radio) {
    if(radio && radio->active && (int32_t)(furi_get_tick()-radio->until_tick)>=0)
        glow_radio_stop(radio);
}