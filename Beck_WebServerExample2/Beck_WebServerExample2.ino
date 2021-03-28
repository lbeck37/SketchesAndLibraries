const String szSketchName  = "Beck_WebServerExample2.ino";
const String szFileDate    = "April 18, 2019q";

#include <BeckMiniLib.h>
#include <BeckWiFiLib.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer WebServer(80);    // Create a webserver object that listens for HTTP request on port 80

//Function protos
void HandleTestRoot   ();
void HandleLoginRoot  ();
void HandleTestFunc   ();
void HandleLoginFunc  ();
void HandleNotFound   ();

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(100);
  //Serial.println('\n');
  Serial << endl << LOG0 << "setup(): Sketch: " << szSketchName << ", " << szFileDate << endl;
  SetupWiFi();

  //WebServer.on("/login", HTTP_POST, HandleLoginFunc); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  WebServer.on("/"            , HTTP_GET,   HandleTestRoot);
  WebServer.on("/login"       , HTTP_GET,   HandleLoginRoot);
  WebServer.on("/handlelogin" , HTTP_POST,  HandleLoginFunc);
  WebServer.on("/handletest"  , HTTP_POST,  HandleTestFunc);

  WebServer.onNotFound(HandleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  WebServer.begin();                            // Actually start the server
  Serial << endl << LOG0 << "setup(): HTTP Web Server started" << endl;
  return;
} //setup


void loop(void){
  WebServer.handleClient();                     // Listen for HTTP requests from clients
  return;
} //loop


void HandleTestRoot() {
  WebServer.send(200, "text/html", "<form action=\"/handletest\" method=\"POST\"><input type=\"text\" name=\"word1\" placeholder=\"Word #1\"></br><input type=\"text\" name=\"word2\" placeholder=\"Word #2\"></br><input type=\"submit\" value=\"Next\"></form><p>Enter words for test</p>");
  return;
} //HandleTestRoot


void HandleLoginRoot() {                          // When URI / is requested, send a web page with a button to toggle the LED
  //WebServer.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'dude' and '111' ...</p>");
  WebServer.send(200, "text/html", "<form action=\"/handlelogin\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'dude' and '111' ...</p>");
  return;
} //HandleLoginRoot


void HandleTestFunc() {
  Serial << endl << LOG0 << "HandleTestFunc(): Received word1= " << WebServer.arg("word1") <<
      ", password= " << WebServer.arg("word2") << endl;

  WebServer.send(200, "text/html", "<h1>Thanks for entering <i>" + WebServer.arg("word1") + "</i> <i>" + WebServer.arg("word2") + "</i>, You Rock!</h1><p>Please proceed to the next fork in the road and take it. <i>YB</i></p>");
  return;
} //HandleTestFunc


void HandleLoginFunc() {                         // If a POST request is made to URI /login
  if( ! WebServer.hasArg("username") || ! WebServer.hasArg("password")
      || WebServer.arg("username") == NULL || WebServer.arg("password") == NULL) { // If the POST request doesn't have username and password data
    WebServer.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }

  if(WebServer.arg("username") == "dude" && WebServer.arg("password") == "111") { // If both the username and the password are correct
    WebServer.send(200, "text/html", "<h1>Welcome, " + WebServer.arg("username") + "!</h1><p>Login successful</p>");
  } else {                                                                              // Username and password don't match
    WebServer.send(401, "text/plain", "401: Unauthorized");
  }
  return;
} //HandleLoginFunc


void HandleNotFound(){
  WebServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
} //HandleNotFound

/*
<form action="/login" method="POST">
    <input type="text" name="username" placeholder="Username"></br>
    <input type="password" name="password" placeholder="Password"></br>
    <input type="submit" value="Login">
</form>
<p>
    Try 'dude' and '111' ...
</p>
*/
