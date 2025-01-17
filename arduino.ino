#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>
#include <SD.h>  


#define WIFI_SSID "D-Link"
#define WIFI_PASSWORD "123456789"

#define API_KEY "AIzaSyAhd41Pj9pHoNSxpWU1dW47rgdZEd5621k"
#define DATABASE_URL "https://machine-failure-3c644-default-rtdb.asia-southeast1.firebasedatabase.app/"


#define USER_EMAIL "priya@gmail.com"
#define USER_PASSWORD "12345678"


FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConfig;

#define DHTPIN 4  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SW420_PIN_DO 5    
#define SW420_PIN_AO 34   


String tempPath = "/users/rJFoaso9u1XGBEf3aN3Jm5ngLKp2/temperature";
String humPath = "/users/rJFoaso9u1XGBEf3aN3Jm5ngLKp2/humidity";
String vibrationPath = "/users/rJFoaso9u1XGBEf3aN3Jm5ngLKp2/vibration";
String aoPath = "/users/rJFoaso9u1XGBEf3aN3Jm5ngLKp2/ao_data";

void setup() {
  Serial.begin(115200);

 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  fbConfig.api_key = API_KEY;        
  fbConfig.database_url = DATABASE_URL;  
  fbAuth.user.email = USER_EMAIL;
  fbAuth.user.password = USER_PASSWORD;

 
  Firebase.begin(&fbConfig, &fbAuth);


  dht.begin();


  pinMode(SW420_PIN_DO, INPUT);
  pinMode(SW420_PIN_AO, INPUT);
}

void loop() {
  
  float temperature = dht.readTemperature(); 
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  int vibrationState = digitalRead(SW420_PIN_DO);  
  
  int aoValue = analogRead(SW420_PIN_AO); 

  if (Firebase.setFloat(fbData, tempPath.c_str(), temperature)) {
    Serial.println("Temperature updated successfully");
  } else {
    Serial.println(fbData.errorReason());
  }


  if (Firebase.setFloat(fbData, humPath.c_str(), humidity)) {
    Serial.println("Humidity updated successfully");
  } else {
    Serial.println(fbData.errorReason());
  }

 
  if (Firebase.setInt(fbData, vibrationPath.c_str(), vibrationState)) {
    if (vibrationState == HIGH) {
      Serial.println("Vibration detected and sent to Firebase");
    } else {
      Serial.println("No vibration detected");
    }
  } else {
    Serial.println(fbData.errorReason());
  }

  if (Firebase.setInt(fbData, aoPath.c_str(), aoValue)) {
    Serial.print("AO data sent to Firebase: ");
    Serial.println(aoValue);
  } else {
    Serial.println(fbData.errorReason());
  }


  delay(5000);
}

