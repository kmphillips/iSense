#include "Arduino.h"
#include "WiFi.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <LittleFS.h>
#include <FS.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Replace with your network credentials
const char* ssid = "wahoo";
const char* password = "";
// This is GandiStandardSSLCA2.pem, the root Certificate Authority that signed 
// the server certifcate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Sep 11 23:59:59 2024 GMT
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIF6TCCA9GgAwIBAgIQBeTcO5Q4qzuFl8umoZhQ4zANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQw\n" \
"OTEyMDAwMDAwWhcNMjQwOTExMjM1OTU5WjBfMQswCQYDVQQGEwJGUjEOMAwGA1UE\n" \
"CBMFUGFyaXMxDjAMBgNVBAcTBVBhcmlzMQ4wDAYDVQQKEwVHYW5kaTEgMB4GA1UE\n" \
"AxMXR2FuZGkgU3RhbmRhcmQgU1NMIENBIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQCUBC2meZV0/9UAPPWu2JSxKXzAjwsLibmCg5duNyj1ohrP0pIL\n" \
"m6jTh5RzhBCf3DXLwi2SrCG5yzv8QMHBgyHwv/j2nPqcghDA0I5O5Q1MsJFckLSk\n" \
"QFEW2uSEEi0FXKEfFxkkUap66uEHG4aNAXLy59SDIzme4OFMH2sio7QQZrDtgpbX\n" \
"bmq08j+1QvzdirWrui0dOnWbMdw+naxb00ENbLAb9Tr1eeohovj0M1JLJC0epJmx\n" \
"bUi8uBL+cnB89/sCdfSN3tbawKAyGlLfOGsuRTg/PwSWAP2h9KK71RfWJ3wbWFmV\n" \
"XooS/ZyrgT5SKEhRhWvzkbKGPym1bgNi7tYFAgMBAAGjggF1MIIBcTAfBgNVHSME\n" \
"GDAWgBRTeb9aqitKz1SA4dibwJ3ysgNmyzAdBgNVHQ4EFgQUs5Cn2MmvTs1hPJ98\n" \
"rV1/Qf1pMOowDgYDVR0PAQH/BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYD\n" \
"VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMCIGA1UdIAQbMBkwDQYLKwYBBAGy\n" \
"MQECAhowCAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNl\n" \
"cnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNy\n" \
"bDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRy\n" \
"dXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZ\n" \
"aHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAWGf9\n" \
"crJq13xhlhl+2UNG0SZ9yFP6ZrBrLafTqlb3OojQO3LJUP33WbKqaPWMcwO7lWUX\n" \
"zi8c3ZgTopHJ7qFAbjyY1lzzsiI8Le4bpOHeICQW8owRc5E69vrOJAKHypPstLbI\n" \
"FhfFcvwnQPYT/pOmnVHvPCvYd1ebjGU6NSU2t7WKY28HJ5OxYI2A25bUeo8tqxyI\n" \
"yW5+1mUfr13KFj8oRtygNeX56eXVlogMT8a3d2dIhCe2H7Bo26y/d7CQuKLJHDJd\n" \
"ArolQ4FCR7vY4Y8MDEZf7kYzawMUgtN+zY+vkNaOJH1AQrRqahfGlZfh8jjNp+20\n" \
"J0CT33KpuMZmYzc4ZCIwojvxuch7yPspOqsactIGEk72gtQjbz7Dk+XYtsDe3CMW\n" \
"1hMwt6CaDixVBgBwAc/qOR2A24j3pSC4W/0xJmmPLQphgzpHphNULB7j7UTKvGof\n" \
"KA5R2d4On3XNDgOVyvnFqSot/kGkoUeuDcL5OWYzSlvhhChZbH2UF3bkRYKtcCD9\n" \
"0m9jqNf6oDP6N8v3smWe2lBvP+Sn845dWDKXcCMu5/3EFZucJ48y7RetWIExKREa\n" \
"m9T8bJUox04FB6b9HbwZ4ui3uRGKLXASUoWNjDNKD/yZkuBjcNqllEdjB+dYxzFf\n" \
"BT02Vf6Dsuimrdfp5gJ0iHRc2jTbkNJtUQoj1iM=\n" \
"-----END CERTIFICATE-----\n";

// Insert Firebase project API Key
#define API_KEY "AIzaSyDrUpeCd7QSKp2NdxpMXhqGbYMpQ8c5YO0 "


// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "kyle.mit.phillips@gmail.com"
#define USER_PASSWORD "Teamyellow"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "esppractice-17e5a.appspot.com"
// For example:
//#define STORAGE_BUCKET_ID "esp-iot-app.appspot.com"

// Photo File Name to save in LittleFS
#define FILE_PHOTO_PATH "/photo.jpg"
int photoCounter = 0;


// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

boolean takeNewPhoto = true;

//Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

void fcsUploadCallback(FCS_UploadStatusInfo info);

bool taskCompleted = false;

char* setClock() {
  configTime(-4 * 3600, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }
  char* buffer = (char*)malloc(80);
  if (buffer == nullptr) {
    Serial.println(F("Failed to allocate memory for time buffer"));
    return nullptr;
  }

  // Format the current time and copy it to the buffer
  struct tm timeinfo;
  localtime_r(&nowSecs, &timeinfo);
  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);

  return buffer;
}
WiFiMulti WiFiMulti;
// Capture Photo and Save it to LittleFS
void capturePhotoSaveLittleFS( void ) {
  // Dispose first pictures because of bad quality
  camera_fb_t* fb = NULL;
  // Skip first 3 frames (increase/decrease number as needed).
  for (int i = 0; i < 4; i++) {
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    fb = NULL;
  }
    
  // Take a new photo
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }  

  // Photo file name
  Serial.printf("Picture file name: %s\n", FILE_PHOTO_PATH);
  File file = LittleFS.open(FILE_PHOTO_PATH, FILE_WRITE);

  // Insert the data in the photo file
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.print("The picture has been saved in ");
    Serial.print(FILE_PHOTO_PATH);
    Serial.print(" - Size: ");
    Serial.print(fb->len);
    Serial.println(" bytes");
  }
  // Close the file
  file.close();
  esp_camera_fb_return(fb);
}

void initWiFi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void initLittleFS(){
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("LittleFS mounted successfully");
  }
}

void initCamera(){
 // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  } 
}

int drawerState;
int pastDrawerState;


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  initWiFi();
  initLittleFS();
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  initCamera();

  //Firebase
  // Assign the api key
  configF.api_key = API_KEY;
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  //Assign the callback function for the long running token generation task
  configF.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&configF, &auth);
  Firebase.reconnectWiFi(true);
  pinMode(13, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  pastDrawerState = digitalRead(13);
  setClock();
}

void loop() {
  char bucketPath[64];
  drawerState = digitalRead(13);
  if (drawerState == LOW || pastDrawerState == HIGH) {
    Serial.println("DRAWER IS CLOSED");
    //digitalWrite(14, LOW);
    pastDrawerState = drawerState;
    delay(10000);
  }
  else if (drawerState == HIGH && pastDrawerState == LOW) {
    Serial.println("OPEN!!!");
    //digitalWrite(14, HIGH);
    pastDrawerState = HIGH;
    photoCounter++;
    digitalWrite(4, HIGH);
    delay(500);
    digitalWrite(4, LOW);
    snprintf(bucketPath, sizeof(bucketPath), "/data/photo_%d.jpg", photoCounter);
    capturePhotoSaveLittleFS();
    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
      client -> setCACert(rootCACertificate);

      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
        HTTPClient https;
        https.begin("https://drawerapp.pythonanywhere.com/api/get_armed_drawer_status");
        int httpCode = https.GET();
  
        if (httpCode == HTTP_CODE_OK) {
          String jsonResponse = https.getString();
          Serial.println("Server response: " + jsonResponse);
    
          // Parse JSON
          DynamicJsonDocument jsonDoc(1024); // Adjust the size as needed
          DeserializationError error = deserializeJson(jsonDoc, jsonResponse);
    
          if (error) {
            Serial.print("JSON parsing failed: ");
            Serial.println(error.c_str());
            return;
          }
    
          // Extract data from JSON
          int status = jsonDoc["armed_status"]; // Assuming JSON key is "status"
          bool isArmed = (status == 1);
          if (isArmed) Serial.println("Armed");
          else Serial.println("Unarmed.");
        } 
        else {
          Serial.printf("Failed to connect to server (HTTP error code: %d)\n", httpCode);
        }
        https.end();
        delay(1);
        if (Firebase.ready() && !taskCompleted){
          Serial.print("Uploading picture... ");

          //MIME type should be valid to avoid the download problem.
          //The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h.
          if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO_PATH /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, bucketPath /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */,fcsUploadCallback)){
            Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
          }
          else{
            Serial.println(fbdo.errorReason());
          }
        }
      // 
      Serial.print("[HTTPS] begin...\n");
      if (https.begin("https://dc7643e2b0405b57638de45a234fd0ec.serveo.net/api/add_new_activity")) {  // HTTPS
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        JsonDocument doc;
        char* timeString = setClock();
        doc["MACAddress"] = WiFi.macAddress();
        if (timeString != nullptr) {
          doc["time"] = timeString;
          free(timeString);
        }
        doc["data"] = "OPEN";
        doc["url"] = (String) fbdo.downloadURL().c_str();
        String jsonString;
        serializeJson(doc, jsonString);
        Serial.printf(jsonString.c_str());
        Serial.println();
        Serial.println("....");
        Serial.println();
        int httpCode = https.POST(jsonString);
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } 
        else {
          Serial.printf("[HTTPS] POST FAILED... code: %d\n", httpCode);
        }
      https.end();
      }
  Serial.printf("DELAY....");
  delay(5000);
  LittleFS.format();
  }
  }
  }
}


// The Firebase Storage upload callback function
void fcsUploadCallback(FCS_UploadStatusInfo info){
    String downloadURL = fbdo.downloadURL();
    if (info.status == firebase_fcs_upload_status_init){
        Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.fileSize, info.remoteFileName.c_str());
    }
    else if (info.status == firebase_fcs_upload_status_upload)
    {
        Serial.printf("Uploaded %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
        delay(250);
    }
    else if (info.status == firebase_fcs_upload_status_complete)
    {
        Serial.println("Upload completed\n");
        FileMetaInfo meta = fbdo.metaData();
        Serial.printf("Name: %s\n", meta.name.c_str());
        Serial.printf(downloadURL.c_str());
        Serial.printf("Bucket: %s\n", meta.bucket.c_str());
        Serial.printf("contentType: %s\n", meta.contentType.c_str());
        Serial.printf("Size: %d\n", meta.size);
        Serial.printf("Generation: %lu\n", meta.generation);
        Serial.printf("Metageneration: %lu\n", meta.metageneration);
        Serial.printf("ETag: %s\n", meta.etag.c_str());
        Serial.printf("CRC32: %s\n", meta.crc32.c_str());
        Serial.printf("Tokens: %s\n", meta.downloadTokens.c_str());
        Serial.printf("Download URL: %s\n\n", fbdo.downloadURL().c_str());
    }
    else if (info.status == firebase_fcs_upload_status_error){
        Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
    }
}