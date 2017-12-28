#include <SPI.h>
#include  <Ethernet.h>

// Led pin, used for the internet connection
#define LEDPIN 7

//MAC adress of controller
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
//IP address of controler
IPAddress ip(192, 168, 1, 177);
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// This will store the request that a client has sent
// in order to then determine what the client desires
// For now the possibliites are:
//  - He wants to turn on the led
//  - He wants to turn off the led
//  - He wants to read the sensor values (default)
String requestRecieved = String(200);

void setupShield(){
  Ethernet.begin(mac,ip);
  server.begin();
}

void listenForEthernatClients(){
  // listen for incoming clients
  //Gets a client that is connected to the server and has data available for reading. 
  EthernetClient client = server.available();
  if (client){
    Serial.println("Got a client.");
    boolean currentLineIsBlank = true;
    while(client.connected())
    //Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data. 
    {
      if (client.available()){
        char c = client.read();
        if (requestRecieved.length() < 200) // not to overflow the array in the case of a big request
          requestRecieved.concat(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c=='\n' && currentLineIsBlank) {
          // Print the recieved request
          Serial.println(requestRecieved);

          // Here we will check whether the led variable was recieved
          if (requestRecieved.indexOf("led=")!=-1) {
            int value = requestRecieved.indexOf("led=") + 4;
            if (requestRecieved.charAt(value) == 't') // Meaning that it's led=true
              digitalWrite(LEDPIN,HIGH);
            else // Meaning thats it's led!=true (false works)
              digitalWrite(LEDPIN,LOW);
          }
          
          //send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          //Send sensor data under suiatble format to client; code in sensors.ino file
          char message[333];
          //get sensor values and print them
          htmlEncodeSensorsData(message);
          client.println(message);
          Serial.println("Message sent");
          // Reset the string containing the client request
          requestRecieved = "";
          break;
        }
        if (c=='\n'){
          //you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r'){
          //you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
  
}




