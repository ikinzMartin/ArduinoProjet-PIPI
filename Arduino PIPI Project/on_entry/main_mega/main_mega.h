#ifndef MAIN_MEGA
#define MAIN_MEGA

#define RELAY1 42
#define RELAY2 44
#define RELAY3 46
#define RELAY4 48

void relay_on(int relay);
void relay_off(int relay);


#define MVT_CAPT 400
#define DO_IDENTIFY 401
#define MVT_IDENTIFYED 402

void send_sig(int sig);

#endif
