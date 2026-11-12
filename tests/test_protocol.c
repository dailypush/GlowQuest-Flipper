#include <assert.h>
#include <string.h>
#include "../applications_user/glowquest_flipper/protocol.h"
int main(void){
 uint8_t p[9],ad[31];size_t n=0;
 const uint8_t blue[]={0xE1,0,0xE9,5,0,9,0x0E,2,0xB0};
 assert(gq_fx(2,0,0,9,0,false,p));assert(!memcmp(p,blue,9));
 assert(gq_ad(p,9,ad,&n)&&n==13&&ad[0]==12&&ad[1]==0xFF&&ad[2]==0x83&&ad[3]==1);
 assert(!gq_fx(32,0,0,0,0,false,p));assert(!gq_fx(0,5,0,0,0,false,p));
 assert(!gq_fx(0,0,16,0,0,false,p));assert(!gq_fx(0,0,0,16,0,false,p));
 assert(!gq_fx(0,0,0,0,4,false,p));assert(!gq_fx(0,0,0,0,0,false,0));
 assert(!gq_ad(p,28,ad,&n));return 0;
}