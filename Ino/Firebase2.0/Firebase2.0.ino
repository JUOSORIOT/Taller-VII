/*
  Rui Santos
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-esp8266-firebase-authentication/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based in the Authenticatiions Examples by Firebase-ESP-Client library by mobizt: https://github.com/mobizt/Firebase-ESP-Client/tree/main/examples/Authentications
*/

//Servo
#include <ESP32Servo.h>

#define SERVO_PIN 13 // ESP32 pin GIOP26 connected to servo motor

Servo servoMotor;

//Firebase
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <FirebaseESP32.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "JAOT_2.4"
#define WIFI_PASSWORD "juan1234"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCxmDjTQ3L2UlzKfd1DBc8-qJ95Aiy834Q"

#define DATABASE_URL "https://krunch-abf73-default-rtdb.firebaseio.com"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "krunch@gmail.com"
#define USER_PASSWORD "Password"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

// Variable to save USER UID
String uid;

String pathPeso = "/Peso";
String pathStates = "/States";
String jsonStr;
//Celda de carga

#include "HX711.h"
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
HX711 balanza;
//locales

String state = "0";
//Firebase
// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

}

void setup() {
  Serial.begin(115200);

  //Celda de carga
  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  //delay(250);

  balanza.set_scale(423);
  balanza.tare(10);  // Hacer 10 lecturas, el promedio es la tara

  //Firebase

  // Initialize WiFi
  initWiFi();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;


  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.print(uid);
  servoMotor.attach(SERVO_PIN);  // attaches the servo on ESP32 pin
  Firebase.setInt(fbdo, pathStates, 1);
  delay(1000);
  state = "0";
}

void loop()
{
  if (state == "0")
  {

    state = Firebase.getInt(fbdo, pathStates) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str();
    Serial.println(state);
  }
  else if (state == "1")
  {
    if (balanza.is_ready())
    { float reading = balanza.get_units(10);
      if (reading < 0)
      {
        reading = 0;
      }
      Serial.println(reading);
      reading = reading;
      Firebase.setFloat(fbdo, pathPeso, reading);
    }
    else {
      Serial.println("HX711 not found.");
    }
    delay(1000);
    Serial.printf("Get peso... %s\n", Firebase.getFloat(fbdo, pathPeso) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());

    Serial.println();
    //Firebase.setInt(fbdo, pathStates, 1);
    state = "0";


  }
  else if (state == "2")
  {
    servoMotor.write(180);
    delay(5000);
    servoMotor.write(0);
    delay(5000);
    servoMotor.detach();
    Firebase.setInt(fbdo, pathStates, 1);
    //delay(1000);
    state = "0";

  }




}
