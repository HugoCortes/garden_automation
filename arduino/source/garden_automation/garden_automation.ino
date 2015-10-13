#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8
RF24 radio(7,8);

byte addresses[][6] = {"1Node","2Node"};        //Unsure about this?


// Set up roles to simplify testing
boolean role;                        // The main role variable, holds the current role identifier
boolean transmit = 1, receive = 0;   // The two different roles.

int i;
int countSecs;


void setup(void)
{
    Serial.begin(57600);
    printf_begin();
    printf("\n\rWill send an incremented i value every second.\n\r");
    
    i = 0;
    countSecs = 0;
    role = transmit;
    
    
    setupRfRadio();
    setupInterrupts();
    
    
}

void loop(void)
{
    
}

void transmitting(void){
    if (role == transmit)  {
        radio.stopListening();                                    // First, stop listening so we can talk.
        
        printf("Now sending: %i \n\r", i);

        //unsigned long time = micros();                             // Take the time, and send it.  This will block until complete
        if (!radio.write( &i, sizeof(int) ))
        {
            printf("failed.\n\r");
        }
        
        radio.startListening();                                    // Now, continue listening
        
        unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
        boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
        
        printf("Started : %ul\n\r" ,started_waiting_at);
        
        while ( ! radio.available() ){                             // While nothing is received
            if (signed(micros() - started_waiting_at) > 200000){          // If waited longer than 200ms, indicate timeout and exit while loop
                Serial.println(micros()-started_waiting_at);
                printf("Micros: %ul\n\r", micros());
                timeout = true;
                break;
            }
        }
        
        if ( timeout ){                                             // Describe the results
            printf("Failed, response timed out.\n\r");
        }
        else{
            int got_i;                                 // Grab the response, compare, and send to debugging spew
            radio.read( &got_i, sizeof(int) );

            // Spew it
            printf("Sent %i, Got response %i \n\r",i,got_i);
        }
    }
}

// Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
    countSecs++;
    if(countSecs == 10)
    {
        i = random(0, 50);
        transmitting();
        countSecs = 0;
    }
}

void setupRfRadio(void){
    radio.begin();                          // Start up the radio
    radio.setAutoAck(1);                    // Ensure autoACK is enabled
    radio.setRetries(15,15);                // Max delay between retries & number of retries
    
    // 0 1 for transmit
    radio.openWritingPipe(addresses[0]);            // 0 for transmit, 1 for receive
    radio.openReadingPipe(1,addresses[1]);          // 1 for transmit, 0 for receive
    
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
    TCNT1  = 0; // Timer Counter
    
    OCR1A = 125000;                                     // = 16MHz/256 * 1Hz (1 second timer)
    TCCR1B |= (1 << WGM12);                             // Turn on CTC mode: Clear Timer on Compare Match
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);  // Set CS12 bit for 256 prescaler
    TIMSK1 |= (1 << OCIE1A);                            // Enable Timer Compare interrupt
}