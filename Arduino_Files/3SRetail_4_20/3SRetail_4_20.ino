
#include <EEPROM.h>   // For read/write PICC's UIDs 
#include <SPI.h>      // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <Adafruit_SSD1306.h>
#include <WiFiEsp.h>
#include "SoftwareSerial.h"
#include <ArduinoJson.h>
#include <HttpClient.h>
#define SECRET_SSID "ur_wifi_SSID"
#define SECRET_PASS "ur_wifi_pw"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

String Item_Bought = "";
String Item_Price = "";
String Item_Location = "";
String Client_Name = "";
String Login_Time = "";
String Membership = "";
String encodeUID = "";
String newName = "";
String newEmail = "";
int price = "";
int redeem = "";
String buyitem = "";
String datetime = "";
int quan;
int anymode;
bool is_mem = false;
bool show_mode_select = true;

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
WiFiEspClient client;

// Allocate the JSON document
// Use arduinojson.org/v6/assistant to compute the capacity.
const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_ARRAY_SIZE(2) + 500;
DynamicJsonDocument doc(capacity);
DynamicJsonDocument doc_update(capacity);
DynamicJsonDocument doc_price(capacity);
DynamicJsonDocument doc_redeem(capacity);
DynamicJsonDocument doc_email(capacity);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create MFRC522 instance.
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Set Led Pins
int Blue_pin = 11; 
int Green_pin = 10;
int Red_pin = 9;

String mode = "";

//getID
bool successRead = false;
String content = "";
String currentUID = "";

//rewards
int rewards = 0;
String UID = "";
unsigned long balance = 0.0;

void(*restart)(void) = 0;

void setup() {
  pinMode(Blue_pin, OUTPUT);
  pinMode(Green_pin, OUTPUT);
  pinMode(Red_pin, OUTPUT);
  offLed();


  //Protocol Configuration
  Serial.begin(115200);  // Initialize serial communications with PC
  Serial2.begin(115200);
  WiFi.init(&Serial2);
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network
      Serial.print(".");
      delay(3000);
    }
    Serial.println("\n Wifi Connected");
  }

  SPI.begin();                           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();                    // Initialize MFRC522 Hardware
  Serial.println(F(" Apr 13 Version"));  // For debugging purposes

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
}

void loop() {
  if (show_mode_select) {
    display_mode_select();
  }
  mode_select();
}

///////////////////////////////////// LED SET UP ///////////////////////////////////
void RGB_color(int red_value, int green_value, int blue_value) {
  analogWrite(Blue_pin, blue_value);
  analogWrite(Green_pin, green_value);
  analogWrite(Red_pin, red_value);
}

void blinkLed() {
  RGB_color(0, 0, 255);  //Blue led blinking
  delay(200);
  RGB_color(0, 0, 0);
  delay(200);
}

void offLed() {
  RGB_color(0, 0, 0);  //Turn off all LED
}

///////////////////////////////////////// OLED Display function /////////////////////////////////////////
void display_mode_select() {
  Serial.println(F("Please input A,B,C or D for different mode"));
  Serial.println(F("A - Initializing Membership Card"));
  Serial.println(F("B - Login "));
  Serial.println(F("C - Pay Cashier"));
  Serial.println(F("D - COVID-19 Patient Alert"));
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 8);
  display.println("Select mode:");
  display.setCursor(25, 24);
  display.println("Member");
  display.setCursor(87, 24);
  display.println("Login");
  display.setCursor(25, 47);
  display.println("Cashier");
  display.setCursor(87, 47);
  display.println("Alert");
  display.fillCircle(14, 28, 8, SSD1306_WHITE);
  display.fillCircle(76, 28, 8, SSD1306_WHITE);
  display.fillCircle(14, 50, 8, SSD1306_WHITE);
  display.fillCircle(76, 50, 8, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(12, 24);
  display.println("A");
  display.setCursor(74, 24);
  display.println("B");
  display.setCursor(12, 47);
  display.println("C");
  display.setCursor(74, 47);
  display.println("D");
  display.display();
  show_mode_select = false;
  return;
}

void mode_reset() {
  mode = "";
  show_mode_select = true;
}

//////////////////////////////// Select Mode /////////////////////////////////////////////
void mode_select() {
  if ((mode == "") && (Serial.available() > 0)) {
    mode = Serial.readStringUntil('\n');

    Serial.print("Input mode: ");
    Serial.println(mode);

    if (mode == "A" || mode == "a") {
      Serial.println(F("Entering to Sign-up/Update Membership Card Mode"));
      //write_rfid();
      //read_rfid();
      successRead = getID();
      if (successRead) {
        ///////////// If the customer tap a valid RFID card //////////////
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 10);
        memUpdate(isMember(content));
      } else {
        //read not success
        Serial.println(F("Can not read your card. Please try again"));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(21, 14);
      }
      delay(2000);  //wait for 5 seconds for another reading
      content = "";
      Membership = "";      
    }

    else if (mode == "B" || mode == "b") {
      Serial.println(F("Entering to Arrival Mode"));
      successRead = getID();
      if (successRead) {
        ///////////// If the customer tap a valid RFID card //////////////
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 10);
        if(isMember(content)){
          granted();
        }else{
          denied();
        }
      } else {
        //read not success
        Serial.println(F("Can not read your card. Please try again"));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(21, 14);
      }
      delay(2000);  //wait for 5 seconds for another reading
      content = "";
    } else if (mode == "C" || mode == "c") {
      if (Membership == "true") {
        Serial.println(F("Entering to Pay Cashier mode"));
        checkOut();
      } else {
        Serial.println(F("Please login first."));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(10, 28);
        display.println(F("Please login first."));
        display.display();
        delay(2000);

      }
    } else if (mode == "D" || mode == "d") {
      Serial.println(F("Entering to COVID-19 Patient Alert Mode"));
      send_email();
    } else {
      Serial.println(F("Invalid input. Please try again"));
    }
    mode_reset();
  }
}

///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
bool getID() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 20);
  display.println(F("Please tap your"));  // Greeting message
  display.setCursor(30, 30);
  display.println(F("member card"));
  display.display();
  Serial.println(F("Reading... Please do not move your card"));  // Getting ready for Reading PICCs
  //LED
  RGB_color(0, 0, 255);  //Turn on blue LED
  delay(2000);           // Hold LED on for 2 seconds to completely red the card
  //RFID
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }
  Serial.print("Found UID tag :");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  offLed();
  return true;
}

////////////////////////// Update member info mode /////////////////////
void memUpdate(bool isMember) {
  do{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(18, 20);
    display.println(F("Please input"));
    display.setCursor(18, 32);
    display.println(F("member's name:"));
    display.display();
  
    Serial.end();
    Serial.begin(115200);
    newName = "";
    Serial.println(F("Please input the member's name"));
    while ((Serial.available() == 0) && (newName == "")) {
    }
    newName = Serial.readStringUntil('\n');
    newName.replace(' ','_');
  }while(newName == "");
  do{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(18, 20);
    display.println(F("Please input"));
    display.setCursor(18, 32);
    display.println(F("member's email:"));
    display.display();
  
    Serial.end();
    Serial.begin(115200);
    newEmail = "";
    Serial.println(F("Please input the member's email before '@'(Support gmail only for now)"));
    while ((Serial.available() == 0) && (newEmail == "")) {
    }
    newEmail = Serial.readStringUntil('\n');
    newEmail.trim();
  }while(newEmail == "");
  newEmail += "@gmail.com";
  Serial.println(newName); //debug
  Serial.println(newEmail); //debug
  postUpdate(content,newName,newEmail);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(46,20);
  if(isMember){
    display.println(F("Update"));
  }else{
    display.println(F("Sign up"));
  }
  display.setCursor(26,30);
  display.println(F("Successfully!"));
  for (int x = 46,y = 54; x < 52;x++,y++) {
    display.drawLine(96, x, 104, y, SSD1306_WHITE);
  }
  for (int x = 54,y = 30; x < 60;x++,y++) {
    display.drawLine(104, x, 120, y, SSD1306_WHITE);
  }
  display.display();
  return;
}

bool isMember(String UID) {
  Serial.println(" *******************");  //DEBUG
  Serial.println(" Go to isMember now");
  Serial.println(" *******************");
  doc = getInfo(UID);
  Membership = doc["is_member"].as<String>();
  Item_Bought = doc["Item"].as<String>();
  Item_Price = doc["Price"].as<String>();
  Item_Location = doc["Location"].as<String>();
  Login_Time = doc["last_login"].as<String>();
  Login_Time = Login_Time.substring(11, 20);
  Client_Name = doc["name"].as<String>();
  if (Membership == String("true")) {
    return true;
  } else {
    return false;
  }
}

////////////////////////////////////////////Send HTTP Request///////////////////////
void connect_localHost(String url) {
  client.setTimeout(10000);
  if (!client.connect("(ur server ip)", 8080)) {
    Serial.println(F("Connection failed"));
    Serial.println(F("Restarting!"));
    delay(5000);
    restart();
    return;
  }
  client.println(url);  
  Serial.print(F("Connecting to "));
  Serial.println(url);
  client.println(F("Host: (ur server ip)"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    client.stop();
    Serial.println(F("Restarting!"));
    delay(5000);
    restart();
    return;
  } else {
    Serial.println(F("Sending request..."));
  }
  // Check HTTP status
  char status[32] = { 0 };
  client.readBytesUntil('\r', status, sizeof(status));
  /*
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }
  */
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    Serial.println(F("Restarting!"));
    delay(5000);
    restart();
  }
  return;
}

DynamicJsonDocument getInfo(String UID) {
  // Send HTTP request
  String url = "GET /membership?UID=";  
  encodeUID = urlencode(UID);           
  url = url + encodeUID + " HTTP/1.0";  
  connect_localHost(url);
  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //client.stop();
    //return;
  }
  // Disconnect wifi
  client.stop();
  return doc;
}

DynamicJsonDocument postUpdate(String UID, String memName, String memEmail) {
  String url = "GET /UpdateOrSignup?UID=";  //!!!!!!!!!
  encodeUID = urlencode(UID);
  url = url + encodeUID + "&Email=" + memEmail +"&U_name="+memName+" HTTP/1.0";   
  connect_localHost(url);
  DeserializationError error = deserializeJson(doc_update, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //client.stop();
    //return;
  }
  client.stop();
  return doc_update;
}

DynamicJsonDocument getPrice(String buyitem) {
  String url_item = "GET /buyitem&UID?Name=";                        
  url_item = url_item + buyitem + "&UID=" + encodeUID + " HTTP/1.0"; 
  connect_localHost(url_item);
  DeserializationError error = deserializeJson(doc_price, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //client.stop();
    //return;
  }
  client.stop();
  return doc_price;
}

DynamicJsonDocument getRedeem(int rewards) {
  String url_redeem = "GET /redeem&UID?Redeem=";                          
  url_redeem = url_redeem + rewards + "&UID=" + encodeUID + " HTTP/1.0";  
  connect_localHost(url_redeem);
  DeserializationError error = deserializeJson(doc_redeem, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //client.stop();
    //return;
  }
  client.stop();
  return doc_redeem;
}

DynamicJsonDocument getEmail(String datetime) {
  String url = "GET /email?dateTime=";  
  url = url + datetime + " HTTP/1.0";   
  connect_localHost(url);
  DeserializationError error = deserializeJson(doc_email, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    //client.stop();
    //return;
  }
  client.stop();
  return doc_email;
}

///////////////////////////////////////// Encode URL ///////////////////////////////////
String urlencode(String uid)  
{
    String encodedUID="";
    char c;
    char partA;
    char partB;
    char partC;

    for (int i =0; i < uid.length(); i++){
      c = uid.charAt(i);
      if (isSpace(c) && i !=0)
      {
        encodedUID+= '-';
      } else if (isalnum(c))
      {
        encodedUID += c;
      } else{
        partB = (c & 0xf)+'0';
        if ((c & 0xf) >9)
        {
            partB = (c & 0xf) - 10 + 'A';
        }
        c = (c>>4)& 0xf;
        partA = c+'0';
        if (c > 9)
        {
            partA = c - 10 + 'A';
        }
        partC = '\0';
        encodedUID += '%';
        encodedUID += partA;
        encodedUID += partB;
      }
      yield(); //pass control
    }
    return encodedUID;
    //input: FC C8 13 31
    //output: FC-C8-13-31
}

///////////////////////////////////////// Access Granted  ///////////////////////////////////
void granted() {
  //LED
  RGB_color(0, 255, 0);  //Turn on green LED
  //delay(1000);         // Hold LED on for a second

  Serial.println(F("Connect success!"));
  Serial.print(F("Welcome! Time: "));
  Serial.println(Login_Time);
  Serial.println(F(""));  
  //OLED
  display.clearDisplay();
  display.println(F("Welcome!"));
  display.setTextSize(1);
  display.setCursor(20, 36);
  display.println(F("Time: "));
  display.setCursor(48, 36);
  display.println(Login_Time);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 4);
  display.println(F("Recently Bought:"));
  display.setCursor(48, 18);
  display.println(Item_Bought);
  display.setCursor(18, 30);
  display.println(F("Price today: "));
  display.setCursor(100, 30);
  display.println(Item_Price);
  display.setCursor(16, 44);
  display.println(F("It's in Area: "));
  display.setCursor(100, 44);
  display.println(Item_Location);
  display.display();
  delay(4000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 8);
  display.println("Select mode:");
  display.setCursor(25, 24);
  display.println("Member");
  display.setCursor(87, 24);
  display.println("Login");
  display.setCursor(25, 47);
  display.println("Cashier");
  display.setCursor(87, 47);
  display.println("Alert");
  display.fillCircle(14, 28, 8, SSD1306_WHITE);
  display.fillCircle(76, 28, 8, SSD1306_WHITE);
  display.fillCircle(14, 50, 8, SSD1306_WHITE);
  display.fillCircle(76, 50, 8, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(12, 24);
  display.println("A");
  display.setCursor(74, 24);
  display.println("B");
  display.setCursor(12, 47);
  display.println("C");
  display.setCursor(74, 47);
  display.println("D");
  display.display();

  return;
}

///////////////////////////////////////// Access Deniead  ///////////////////////////////////
void denied() {
  //LED
  RGB_color(255, 0, 0);  //Turn on red LED
  delay(1000);           // Hold LED on for a second
  //OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 14);
  display.println(F("This is not a"));  // Non-member card prompt message
  display.setCursor(2, 24);
  display.println(F("registered membership"));
  display.setCursor(12, 34);
  display.println(F("card. Please check"));
  display.setCursor(20, 44);
  display.println(F("and try again."));
  display.display();
  //DEBUG
  Serial.println(F("This is not a registered membership card. Please check and try again."));
  Serial.println(F(""));
  delay(5000);
  content = "";
  return;
}

///////////////////////////////////////// Reward ///////////////////////////////////
void reward(double Balance) {
  //for every $500 purphase, add 1 reward point into account
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 16);
  display.println(F("Please wait..."));
  for (int x = 20; x < 110; x++) {
    display.fillCircle(x, 36, 8, SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(2000);
  if (Balance >= 500) {
    int(rewards) = Balance / 500;

    doc_redeem = getRedeem(rewards);
    redeem = doc_redeem["Redeem"].as<int>();

    //OLED
    display.clearDisplay();
    display.setCursor(16, 5);
    display.println(F("You have earned "));
    display.setCursor(60, 15);
    display.println(rewards);
    display.setCursor(20, 25);
    display.println(F(" redeem points, "));
    display.setCursor(59, 35);
    display.println(redeem);
    display.setCursor(16, 45);
    display.println(F(" in total now."));
    display.display();
    delay(5000);
    Serial.print(F("You have earned "));
    Serial.print(rewards);
    Serial.print(F (" points, "));
    Serial.print(redeem);
    Serial.print(F(" in total now."));
    return;
  } else {
    
    Serial.println(F("You did not earn any redeem point this time."));
    display.clearDisplay();
    display.setCursor(16, 15);
    display.println(F("You did not earn "));
    display.setCursor(12, 25);
    display.println(F("any redeem points "));
    display.setCursor(22, 35);
    display.println(F("for this time."));
    display.display();
    delay(5000);
    Serial.println(F("Thank you for coming!"));
    return;
  }
}

////////////////////////////////Check out mode/////////////////////////////////////////////

void checkOut(){
  do{
    do{ 
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(28, 20);
      display.println(F("Please input"));
      display.setCursor(38, 32);
      display.println(F("the item:"));
      display.display();
    
      Serial.end();
      Serial.begin(115200);
      buyitem = "";
    
      Serial.println(F("Please input the item that you want to buy:"));
      while ((Serial.available() == 0) && (buyitem == "")) {
      }
      buyitem = Serial.readStringUntil('\n');
      buyitem.toUpperCase();
      buyitem.trim();
      buyitem.replace(' ','_');
      Serial.println(buyitem);
      doc_price = getPrice(buyitem);
      price = doc_price["Price"].as<int>();
      Serial.println(price); //debug
    }while(!inputCheck(price, 0));
    Serial.print(F("Its price is: "));
    Serial.println(price);
    do{
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(40, 28);
      display.println(F("Quantity?"));
      display.display();
      Serial.end();
      Serial.begin(115200);
      Serial.println(F("Please input the quantity:"));
      while (Serial.available() == 0) {
      }
      quan = Serial.parseInt();
      Serial.println(quan);
    }while(!inputCheck(quan, 1));
    
    balance = balance + price * quan;
    long num = balance;
    int digit = 0;
    while (num != 0) {
      digit++;
      num /= 10;
    }
    
    Serial.print("The total price is: ");
    Serial.println(balance);
    display.clearDisplay();
    display.setCursor(9, 14);
    display.println(F("Total price now is:"));
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    if (digit == 1) {
      display.setCursor(110, 36);
    } else if (digit == 2) {
      display.setCursor(96, 36);
    } else if (digit == 3) {
      display.setCursor(84, 36);
    } else if (digit == 4) {
      display.setCursor(72, 36);
    } else {
      display.setCursor(10, 36);
    }
    display.println(balance);
    display.display();
    delay(3000);
  }while(buyMore());
  return;
}
bool inputCheck(int input,int checkmode){
  switch(checkmode){
    //check product
    case 0:
      if (input == 0){
        Serial.println(F("Item not found, please try again."));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 20);
        display.println(F("Item not found!"));
        display.setCursor(16, 32);
        display.println(F("Please try again."));
        display.display();
        delay(3000);
        return false;
      }else{
        return true;
      }
    //check quan
    case 1:
      if (input <= 0 || input > 500){
        Serial.println(F("Invalid input, please try again."));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(24, 20);
        display.println(F("Invalid input!"));
        display.setCursor(16, 32);
        display.println(F("Please try again."));
        display.display();
        delay(3000);
        return false;  
      }else{
        return true;
      }
    default:
      break;
  }
  return false;
}

bool buyMore(){
  bool invalid = false;
  do{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(24, 10);
    display.println(F("Anything else"));
    display.setCursor(22, 22);
    display.println(F("you wanna buy?"));
    display.fillCircle(16, 46, 8, SSD1306_WHITE);
    display.fillCircle(66, 46, 8, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(14, 43);
    display.setTextSize(1);
    display.println(F("1"));
    display.setCursor(64, 43);
    display.println(F("2"));
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(28, 43);
    display.println(F("YES"));
    display.setCursor(78, 38);
    display.println(F("NO,CHECK"));
    display.setCursor(78, 50);
    display.println(F("OUT Plz"));
    display.display(); 
    Serial.end();
    Serial.begin(115200);
    Serial.println(F("Anything else you wanna buy?"));
    Serial.println(F("If yes, enter 1. If no, enter 2:"));
    while (Serial.available() == 0);
    anymode = Serial.parseInt();
    Serial.println(anymode);
    switch(anymode){
      case 1:
        return true;
      case 2:
        Serial.println(F("Please wait..."));
        reward(balance);
        balance == 0;
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(2, 10);
        display.println(F("Thank you for coming!"));
        display.setCursor(12, 22);
        display.println(F("See you next time."));
        display.fillCircle(64, 46, 12, SSD1306_WHITE);
        display.fillCircle(59, 41, 2, SSD1306_BLACK);
        display.fillCircle(69, 41, 2, SSD1306_BLACK);
        display.fillCircle(64, 50, 4, SSD1306_BLACK);
        display.fillCircle(64, 47, 5, SSD1306_WHITE);
        display.display();
        delay(5000);
        return false;
      default:
        Serial.println(F("Invalid input, please try again"));
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(24, 20);
        display.println(F("Invalid input!"));
        display.setCursor(16, 32);
        display.println(F("Please try again."));
        display.display();
        delay(3000);
        invalid = true;
        break;
    }
  }while(invalid);
  return true;
}

////////////////////////////////Mode D: COVID-19 Email Alert///////////////////////////////
void send_email() {
  //TO-DO check datetime is vaild for the format.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 20);
  display.println(F("Please input"));
  display.setCursor(18, 32);
  display.println(F("Date and Time:"));
  display.display();

  Serial.end();
  Serial.begin(115200);
  datetime = "";
  Serial.println(F("Please input the datetime (format:YYYYMMDDHHmmSS)"));
  while ((Serial.available() == 0) && (datetime == "")) {
  }
  datetime = Serial.readStringUntil('\n');
  if (datetime.length() != 14) {
    Serial.println(F("Invalid Input. Please Input again."));
    send_email();
  } else {
    doc_email = getEmail(datetime);
  }

  return;
}
