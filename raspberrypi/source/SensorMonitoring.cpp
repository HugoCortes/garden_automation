#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "../libraries/RF24_rpi/RF24.h"
#include <mysql/mysql.h> // I added include /usr/include/mysql/ to ld.so.conf which is why that works

#include <stdio.h>
#include <time.h>

void setupSQL(void);

using namespace std;

MYSQL     *connection, mysql;
MYSQL_RES *result;
MYSQL_ROW row;
int query_state;

#define HOST "localhost" // you must keep the quotes on all four items,  
#define USER "root" // the function "mysql_real_connect" is looking for a char datatype,
#define PASSWD "PASS" // without the quotes they're just an int.
#define DB "Test"



// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

const uint8_t pipes[][6] = {"1Node","2Node"};


int main(int argc, char** argv){
    setupSQL();


  bool role_ping_out = true, role_pong_back = false;
  bool role = role_pong_back;

  // Print preamble:
  printf("RF24/examples/pingtest/\n");

  // Setup and configure rf radio
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();


  printf("\n ************ Role Setup ***********\n");
    cout << "Role: Pong Back, awaiting transmission " << endl << endl;
    

    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();
	
	// forever loop
	while (1)
	{
		if ( role == role_pong_back )
		{
			
			// if there is data ready
			//printf("Check available...\n");

			if ( radio.available() )
			{
				// Dump the payloads until we've gotten everything
				int i;

				// Fetch the payload, and see if this was the last one.
				radio.read( &i, sizeof(int) );
				printf("Received: %i\n", i);
				radio.stopListening();
				
				radio.write( &i, sizeof(int) );
				printf("Writing: %i\n", i);

				// Now, resume listening so we catch the next packets.
				radio.startListening();

				printf("Got payload(%d) %i...\n",sizeof(int), i);

                
                
                struct tm *timeInfo;
                time_t rawtime;
                char strResponse[128];
                rawtime = time(NULL);
                timeInfo = localtime(&rawtime);
                strftime(strResponse,128,"%Y-%m-%d %H:%M:%S",timeInfo);  
                
                string stringI = static_cast<ostringstream*>( & (ostringstream() << i ) )->str();
                //string stringDate(strResponse);
                
                // INSERT INTO TempHumid VALUES(unix_timestamp(now()),%5.1f,%5.1f)",(temp / 10.0),(rh / 10.0);
                //string query = "INSERT INTO Table1 VALUES(unix_timestamp(now()),%5i,%5.1f)",(i),(10));
                
                string query = "INSERT INTO Table1 VALUES(unix_timestamp(now()), " + stringI + ", 5.5 )";
                
                //string query = "INSERT INTO Table1 (testInt, date) VALUES ( " + stringI + ",'" + stringDate + "' )";
                mysql_query(connection, query.c_str());

                //Send query to database
                query_state = mysql_query(connection, "select * from Table1");
                // store result
                result = mysql_store_result(connection);
    
                if( result != NULL){
                    int num_rows = mysql_num_rows(result);
                    int num_fields = mysql_num_fields(result);
    
                    cout << "Number of rows: " << num_rows << "\n";
                    MYSQL_ROW row;
                    while(row = mysql_fetch_row(result))
                    {
                        char *value_int = row[0];
                        printf("Got value: ");
                        cout << value_int << "\n";
                    }
                }
                
			}
		
		}


	} // forever loop

  return 0;
}

void setupSQL(void)
{
    mysql_init(&mysql);
    connection = mysql_real_connect(&mysql,HOST,USER,PASSWD,DB,0,0,0); 
    if (connection == NULL) 
    {
        cout << mysql_error(&mysql) << endl;
    }
    else
    {
        cout << "Connection established...\n";
    }
    
    string query = "DELETE FROM Table1";
    mysql_query(connection, query.c_str());
}
