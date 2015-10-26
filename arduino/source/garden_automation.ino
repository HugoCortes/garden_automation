/*
 * moisture_sensor.ino
 *
 * Author: Hugo Cortes
 * Description: Controls garden's sensors and water pump(s).
 */ 

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// ******************************************************************************************
// User Config
// ******************************************************************************************

// RF Definitions
#define RF_CSN       8
#define RF_CE        7
#define RF_trans     1
#define RF_rec       0
#define RF_radio_num 0

// Moisture Sensor Definitions
#define moist_sens_0 0
#define moist_sens_1 1

// ******************************************************************************************
// End User Config
// ******************************************************************************************


RF24 radio(RF_CE, RF_CSN);
byte addresses[][6] = {"1Node","2Node"};        //Unsure about this?

int countSecs;
int moisture_0;

void setup(void)
{
    Serial.begin(57600);
    printf_begin();
    printf("\n\rWelcome to garden_automation! Arduino will communicate with Raspberry Pi!\n\r");
    
    setupRfRadio();
    setupInterrupts();
    
    countSecs  = 0;
    moisture_0 = 0;
}

void loop(void)
{
    
}

void transmitting(void)
{    
    radio.stopListening();                                      // First, stop listening so we can talk.
    printf("Now sending: %i \n\r", moisture_0);

    if (!radio.write( &moisture_0, sizeof(int) ))
    {
        printf("Failed to send.\n\r");
    }
        
    radio.startListening();                                     // Now, continue listening
    
    boolean timeout = false;                                    // Set up a variable to indicate if a response was received or not
    unsigned long started_waiting_at = micros();                // Set up a timeout period, get the current microseconds
    
    while ( ! radio.available() )                               // While nothing is received
    {                             
        if (signed(micros() - started_waiting_at) > 200000)     // If waited longer than 200ms, indicate timeout and exit while loop
        {    
            timeout = true;
            break;
        }
    }
        
    if ( timeout )
    {
        printf("Failed, response timed out.\n\r");
    }
    else                                                        // Grab the response, compare, and send to debugging spew
    {
        int got_res;                                 
        radio.read( &got_res, sizeof(int) );
        printf("Sent %i, Got response %i \n\r",moisture_0,got_res);
    }
}

// Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
    countSecs++;
    printf("\nSeconds: %i", countSecs);
    if(countSecs == 60)
    {
        moisture_0 = analogRead(moist_sens_0);
        transmitting();
        countSecs = 0;
    }
}

void setupRfRadio(void){
    radio.begin();                          // Start up the radio
    radio.setAutoAck(1);                    // Ensure autoACK is enabled
    radio.setRetries(15,15);                // Max delay between retries & number of retries
    
    // Open a writing and reading pipe on each radio, with opposite addresses
    radio.openWritingPipe(addresses[RF_radio_num]);
    radio.openReadingPipe(!RF_radio_num, addresses[!RF_radio_num]);
    
    radio.startListening();                 // Start listening
    radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void setupInterrupts(void){
    cli();  // Stop interrupts
    setupTimer1();
    sei();  // Start Interrupts
}

void setupTimer1(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;                                         // Timer Counter
    
    OCR1A = 125000;                                     // = 16MHz/256 * 1Hz (1 second timer)
    TCCR1B |= (1 << WGM12);                             // Turn on CTC mode: Clear Timer on Compare Match
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);  // Set CS12 bit for 256 prescaler
    TIMSK1 |= (1 << OCIE1A);                            // Enable Timer Compare interrupt
}