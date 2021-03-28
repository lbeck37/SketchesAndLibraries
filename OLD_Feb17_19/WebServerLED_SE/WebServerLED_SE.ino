//08/04/15 Port to ESP8266 from Starting Electronics
/*The Arduino sketch in this example reads the HTTP request header and checks for the text LED2=2 and if found, the Arduino will toggle the LED from off to on or on to off.

Both of the above requests contain the LED2=2 text although in different places. When checking the box, the text is part of the GET request line. When unchecking the box, the text is part of the Referer: header.

With this background information, we can now see how the Arduino sketch works.
LED Web Server Sketch

The Arduino sketch for the LED web server is shown below.*/
/*--------------------------------------------------------------
  Program:      eth_websrv_LED
  Hardware:     LED and resistor in series connected between
                  Arduino pin 2 and GND
  Date:         11 January 2013
  Author:       W.A. Smith, http://startingelectronics.org
--------------------------------------------------------------*/
#include <SPI.h>
#include <Ethernet.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 20); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(2, OUTPUT);       // LED on pin 2
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Arduino LED Control</title>");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<h1>LED</h1>");
                    client.println("<p>Click to switch LED on and off.</p>");
                    client.println("<form method=\"get\">");
                    ProcessCheckbox(client);
                    client.println("</form>");
                    client.println("</body>");
                    client.println("</html>");
                    Serial.print(HTTP_req);
                    HTTP_req = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// switch LED and send back HTML for LED checkbox
void ProcessCheckbox(EthernetClient cl)
{
    if (HTTP_req.indexOf("LED2=2") > -1) {  // see if checkbox was clicked
        // the checkbox was clicked, toggle the LED
        if (LED_status) {
            LED_status = 0;
        }
        else {
            LED_status = 1;
        }
    }

    if (LED_status) {    // switch LED on
        digitalWrite(2, HIGH);
        // checkbox is checked
        cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\" checked>LED2");
    }
    else {              // switch LED off
        digitalWrite(2, LOW);
        // checkbox is unchecked
        cl.println("<input type=\"checkbox\" name=\"LED2\" value=\"2\" \
        onclick=\"submit();\">LED2");
    }
}

/*Modification to Sketch

This sketch is a modified version of the eth_websrv_page sketch from the basic Arduino web server.

The sketch creates the HTML page as usual, but calls the ProcessCheckbox() function to take care of the line that draws the checkbox.

The ProcessCheckbox() function checks to see if the HTTP request contains the text LED2=2. If the HTTP request does contain this text, then the LED will be toggled (switched from on to off or from off to on) and the web page is sent again with the checkbox control also toggled to reflect the state of the LED.
Improvements

The sketch has been kept simple for learning purposes, but some improvements can be made to this sketch to make it more reliable.

The sketch currently only checks for the presence of the text LED2=2 in the HTTP request to see if the checkbox was clicked. It would be more reliable to check where the LED2=2 text is in the HTTP message to determine whether the checkbox is being checked or unchecked. This would then make it impossible for the state of the LED and the state of the checkbox to become unsynchronized.*/

//Last line.