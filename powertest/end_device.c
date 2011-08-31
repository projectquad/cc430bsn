/** @file end_device.c
*
* @brief Network Discovery End Device
*
* @author Alvaro Prieto
*/
#include "common.h"
#include "intrinsics.h"
#include "leds.h"
#include "uart.h"
#include "timers.h"
#include "cc2500.h"
#include "radio_cc2500.h"
#include "network.h"

#include <string.h>

#ifndef DEVICE_ADDRESS
#define DEVICE_ADDRESS 0xFF
#endif

static uint8_t scheduler (void);
static uint8_t rx_callback( uint8_t*, uint8_t );

static uint8_t p_radio_tx_buffer[RADIO_BUFFER_SIZE];

static volatile uint8_t counter = TIMER_CYCLES;
static volatile uint8_t ack_required = 0;
static volatile uint8_t ack_time = 0;

// Array to store all RSSIs from other devices
static volatile uint8_t rssi_table[MAX_DEVICES+1];
static volatile uint8_t final_rssi_table[MAX_DEVICES+1];

void debug_status();

int main( void )
{   
  
  /* Init watchdog timer to off */
  WDTCTL = WDTPW|WDTHOLD;
     
  // Setup oscillator for 12MHz operation
  BCSCTL1 = CALBC1_12MHZ;
  DCOCTL = CALDCO_12MHZ; 
  
  // Wait for changes to take effect
  __delay_cycles(4000);
      
  setup_uart();
  
  setup_spi();
  
  setup_cc2500( rx_callback );
  
  cc2500_set_address( DEVICE_ADDRESS );
       
  setup_leds();
           
  for (;;) 
  {        
    __bis_SR_register( LPM1_bits + GIE );   // Sleep
    __no_operation();
    
  } 
  
  return 0;
}

/*******************************************************************************
 * @fn     void rx_callback( void )
 * @brief  rx callback function, called when packet has been received
 * ****************************************************************************/
static uint8_t rx_callback( uint8_t* p_buffer, uint8_t size )
{    
  packet_header_t* p_rx_packet;
  
  p_rx_packet = (packet_header_t*)p_buffer;
  
  //led2_toggle();
  
  // TODO Packet handling is temporary for testing purposes
  // Later implementation will use function pointers to avoid all the if-else
  // and have constant time to start processing each packet
  
  if ( PACKET_POLL == p_rx_packet->type )
  {
   
    packet_header_t* p_tx_packet;  

    led1_on();

    p_tx_packet = (packet_header_t*)p_radio_tx_buffer;
    
    // Setup ack packet
    p_tx_packet->destination = p_rx_packet->source;
    p_tx_packet->source = DEVICE_ADDRESS;
    p_tx_packet->type = PACKET_POLL | FLAG_ACK;
    p_tx_packet->tx_power = 0x7f;
    p_tx_packet->rx_power = p_buffer[size];
       
    __delay_cycles(1000);
       
    cc2500_tx_packet(&p_radio_tx_buffer[1], (sizeof(packet_header_t) ),
                                                p_tx_packet->destination );
                                                
    led1_off();   
    
  }
  
  return 0;
}

