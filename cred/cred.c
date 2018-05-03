#include "opendefs.h"
#include "cred.h"
#include "opencoap.h"
#include "packetfunctions.h"
#include "openqueue.h"
#include "leds.h"
#include "Button.h"
#include "stdio.h"
#include "string.h"

#include "idmanager.h"
#include "IEEE802154E.h"


static const uint8_t ipAddr_Server[] = { 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
//////////////////////// push

cred_vars_t cred_vars;

const uint8_t cred_path0[] = "red";

static unsigned long int next = 1;

int btn_cnt = 0;

owerror_t cred_receive(OpenQueueEntry_t* msg, coap_header_iht* coap_header, coap_option_iht* coap_options);

void cred_sendDone(OpenQueueEntry_t* msg, owerror_t error);

void cb_btn(void);

int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned  int)(next / 65536) % 32768;
}

// initializes the module
void cred_init() {

    cred_vars.desc.path0len = sizeof(cred_path0) - 1;
    cred_vars.desc.path0val = (uint8_t*)(&cred_path0);
    cred_vars.desc.path1len = 0;
    cred_vars.desc.path1val = NULL;
    cred_vars.desc.componentID = COMPONENT_CRED;
    cred_vars.desc.callbackRx = &cred_receive;
    cred_vars.desc.callbackSendDone = &cred_sendDone;

    opencoap_register(&cred_vars.desc);

    /*
    cred_vars.timerId = opentimers_start (
	  3000,
	  TIMER_PERIODIC,
	  TIME_MS,
	  cred_push
    );
    *//////push

    // button 인터럽트의 콜백 함수 설정
    btn_setCallbacks(cb_btn);
}


void cred_push() {
    OpenQueueEntry_t* pkt;
    owerror_t outcome;
    uint8_t numOptions;

    // don't run if not synch
    if (ieee154e_isSynch() == FALSE) return;

    // don't run on dagroot
    if (idmanager_getIsDAGroot()) {
	 opentimers_stop(cred_vars.timerId);
	 return;
    }

    //=== request a packetBuffer

    // create a CoAP RD packet
    pkt = openqueue_getFreePacketBuffer(COMPONENT_CRED);
    if (pkt == NULL) {
	 openserial_printError(
	     COMPONENT_CRED,
	     ERR_NO_FREE_PACKET_BUFFER,
	     (errorparameter_t)0,
	     (errorparameter_t)0
	 );
	 openqueue_freePacketBuffer(pkt);
	 return;
    }
    // take ownership over that packet
    pkt->creator = COMPONENT_CRED;
    pkt->owner = COMPONENT_CRED;

    // CoAP payload   sync가 켜졌는지에 대한 메세지
    numOptions = 0;
    if (leds_sync_isOn()) {
	 packetfunctions_reserveHeaderSize(pkt, 10);
	 pkt->payload[0] = 'T';
	 pkt->payload[1] = 'O';
	 pkt->payload[2] = 'G';
	 pkt->payload[3] = 'G';
	 pkt->payload[4] = 'L';
	 pkt->payload[5] = 'E';
	 pkt->payload[6] = (btn_cnt + '0');
	 pkt->payload[7] = ' ';
	 pkt->payload[8] = 'O';
	 pkt->payload[9] = 'N';
    }
    else {
	 packetfunctions_reserveHeaderSize(pkt, 11);
	 pkt->payload[0] = 'T';
	 pkt->payload[1] = 'O';
	 pkt->payload[2] = 'G';
	 pkt->payload[3] = 'G';
	 pkt->payload[4] = 'L';
	 pkt->payload[5] = 'E';
	 pkt->payload[6] = (btn_cnt + '0');
	 pkt->payload[7] = ' ';
	 pkt->payload[8] = 'O';
	 pkt->payload[9] = 'F';
	 pkt->payload[10] = 'F';
    }




    // payload marker
    packetfunctions_reserveHeaderSize(pkt, 1);
    pkt->payload[0] = COAP_PAYLOAD_MARKER;

    // URI-path
    packetfunctions_reserveHeaderSize(pkt, sizeof(cred_path0) - 1);
    memcpy(pkt->payload, cred_path0, sizeof(cred_path0) - 1);
    packetfunctions_reserveHeaderSize(pkt, 1);
    pkt->payload[0] = (COAP_OPTION_NUM_URIPATH) << 4 | 3;
    numOptions++;

    //=== send CoAP message
    // metadata
    pkt->l4_destination_port = 4444; /////////////////////////////7654
    pkt->l3_destinationAdd.type = ADDR_128B;
    memcpy(&pkt->l3_destinationAdd.addr_128b[0], &ipAddr_Server, 16);

    // send
    outcome = opencoap_send(
	 pkt,
	 COAP_TYPE_NON,
	 COAP_CODE_REQ_POST,
	 numOptions,
	 &cred_vars.desc);
    if (outcome == E_FAIL) {
	 openqueue_freePacketBuffer(pkt);
    }
    return;
}

void cred_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
    openqueue_freePacketBuffer(msg);
}

owerror_t cred_receive(OpenQueueEntry_t* msg, coap_header_iht* coap_header, coap_option_iht* coap_options) {

    owerror_t outcome;
    int num = -1;
    int random = 0;
    volatile uint16_t delay;

    switch (coap_header->Code) {

    case COAP_CODE_REQ_PUT:

	 // 메세지에 따른 특정 행동 수행
	 if (strncmp(msg->payload, "blink", 5)==0) {

	     //x, red, green, red green, blue, red blue,  green blue, red green blue
	     num = 0;
	     switch (msg->payload[5]) {
	     case 'R':
		  leds_error_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  break;
	     case 'G':
		  leds_radio_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  break;
	     case 'B':
		  leds_sync_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  break;
	     default:
		  leds_all_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_all_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_all_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_all_toggle();
		  break;
	     }


	 }
	 else if (strncmp(msg->payload , "twinkle",7)==0) {
	     
	     // red와  blue green이 
	     num = 1;
	     
	     leds_error_on();
	     leds_radio_off();
	     leds_sync_on();
	     for (delay = 0xffff; delay > 0; delay--);

	     leds_error_off();
	     leds_radio_on();
	     leds_sync_off();
	     for (delay = 0xffff; delay > 0; delay--);

	     leds_error_on();
	     leds_radio_off();
	     leds_sync_on();
	     for (delay = 0xffff; delay > 0; delay--);

	     leds_error_off();
	     leds_radio_on();
	     leds_sync_off();
	     for (delay = 0xffff; delay > 0; delay--);

	     leds_error_on();
	     leds_radio_off();
	     leds_sync_on();
	     for (delay = 0xffff; delay > 0; delay--);

	     leds_error_off();
	     leds_radio_on();
	     leds_sync_off();
	     for (delay = 0xffff; delay > 0; delay--);


	 }
	 //random한 수로 led키고 두번 깜빡
	 else if (strncmp(msg->payload,"random",6)==0) {
	     num = 2;

	     random = rand() % 3;

	     switch (random) {
	     case 0:
		  leds_error_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_error_toggle();
		  break;

	     case 1:
		  leds_radio_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_radio_toggle();
		  break;

	     case 2:
		  leds_sync_on();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  for (delay = 0xffff; delay>0; delay--);
		  leds_sync_toggle();
		  break;
	     }

	 }
	 else if (strncmp(msg->payload,"shift",5)==0) {
		  num = 3;
	         //led circular shift
		  // leds_all_off();
		  //leds_error_on();
		  //for (delay = 0xffff; delay > 0; delay--);
	         
		  leds_circular_shift();
		  for (delay = 0xffff; delay > 0; delay--);
	  
		  leds_circular_shift();
		  for (delay = 0xffff; delay > 0; delay--);

		  leds_circular_shift();
		  for (delay = 0xffff; delay > 0; delay--);

		  leds_circular_shift();
		  for (delay = 0xffff; delay > 0; delay--);

		  leds_circular_shift();
		  for (delay = 0xffff; delay > 0; delay--);
	 }
	 else {
	     leds_error_toggle();
	 }



	 // reset packet payload
	 msg->payload = &(msg->packet[127]);
	 msg->length = 0;

	 if (num == 0) {

	     packetfunctions_reserveHeaderSize(msg, 16);
	     uint8_t msg_name[16] = "SERVER DEPENDENT";
	     memcpy(msg->payload, msg_name, 16);
	     
	 }
	 else if (num == 1) {
	     packetfunctions_reserveHeaderSize(msg, 7);
	     msg->payload[0] = 'T';
	     msg->payload[1] = 'W';
	     msg->payload[2] = 'I';
	     msg->payload[3] = 'N';
	     msg->payload[4] = 'K';
	     msg->payload[5] = 'L';
	     msg->payload[6] = 'E';

	 }
	 //random
	 else if (num == 2) {
	     if (random == 0) {
		  packetfunctions_reserveHeaderSize(msg, 5);
		  msg->payload[0] = (random + '0');
		  msg->payload[1] = ' ';
		  msg->payload[2] = 'R';
		  msg->payload[3] = 'E';
		  msg->payload[4] = 'D';
	     }
	     else if (random == 1) {
		  packetfunctions_reserveHeaderSize(msg, 7); 
		  msg->payload[0] = (random + '0');
		  msg->payload[1] = ' ';
		  msg->payload[2] = 'G';
		  msg->payload[3] = 'R';
		  msg->payload[4] = 'E';
		  msg->payload[5] = 'E';
		  msg->payload[6] = 'N';
	     }
	     else if (random == 2) {
		  packetfunctions_reserveHeaderSize(msg, 6);
		  msg->payload[0] = (random + '0');
		  msg->payload[1] = ' ';
		  msg->payload[2] = 'B';
		  msg->payload[3] = 'L';
		  msg->payload[4] = 'U';
		  msg->payload[5] = 'E';
	     }

	 }
	 //circular shift
	 else if (num == 3) {
	     packetfunctions_reserveHeaderSize(msg, 8);
	     msg->payload[0] = 'C';
	     msg->payload[1] = 'I';
	     msg->payload[2] = 'R';
	     msg->payload[3] = 'C';
	     msg->payload[4] = 'U';
	     msg->payload[5] = 'L';
	     msg->payload[6] = 'A';
	     msg->payload[7] = 'R';
	 }
	 else {
	     packetfunctions_reserveHeaderSize(msg, 15);
	     uint8_t msg_name[15] = "INVALID MESSAGE";
	     memcpy(msg->payload, msg_name, 15);
	 }

	 // payload marker
	 packetfunctions_reserveHeaderSize(msg, 1);
	 msg->payload[0] = COAP_PAYLOAD_MARKER;

	 // set the CoAP header
	 coap_header->Code = COAP_CODE_RESP_CHANGED;

	 outcome = E_SUCCESS;
	 break;

    default:
	 outcome = E_FAIL;
	 break;

    }

    return outcome;
}


void cb_btn(void) {
    //  toggle LED
    btn_cnt += 1;
    leds_sync_toggle(); 
    cred_push();

    /*
    cred_vars.timerId = opentimers_start(
	 3000,
	 TIMER_PERIODIC,
	 TIME_MS,
	 cred_push
    );
    */
}

