/*
   COVID 19 Summary Display
   ========================

   Display COVID-19 confirmed, deaths, and recovered cases on the LCD of Wio Terminal.

   2021.05.02 Takayuki YANO
*/
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#define ARDUINOJSON_USE_DOUBLE 1
#define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e10
#include <Arduino.h>
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <rpcWiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "Free_Fonts.h"
#include "TFT_eSPI.h"
TFT_eSPI tft;

const char* ssid     = "Your WiFi SSID"; // WiFi SSID
const char* password = "Your Password";  // WiFi pass
const time_t TZ = 3600 * 9; // Timezone: UTC+9
static const char *wd[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};

// ROOT CA
const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
                           "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
                           "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
                           "ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
                           "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
                           "LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
                           "RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
                           "+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
                           "PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
                           "xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
                           "Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
                           "hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
                           "EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
                           "MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
                           "FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
                           "nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
                           "eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
                           "hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
                           "Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
                           "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
                           "+OkuE6N36B9K\n" \
                           "-----END CERTIFICATE-----\n";

// Fetch data from
// https://covid-19-data.unstatshub.org/datasets/1cb306b5331945548745a5ccd290188e_2/geoservice?geometry=-120.498%2C22.999%2C36.826%2C47.695&orderBy=Country_Region
String url = "https://services1.arcgis.com/"\
             "0MSEUqKaxRlEPj5g/arcgis/rest/services/ncov_cases2_v1/FeatureServer/2/"\
             "query?where=&objectIds=88&time=&geometry=&geometryType=esriGeometryEnvelope&inSR="\
             "&spatialRel=esriSpatialRelIntersects&resultType=none&distance=0.0&units=esriSRUnit_Meter&returnGeodetic=false"\
             "&outFields=OBJECTID%2CRecovered%2CCountry_Region%2CDeaths%2CConfirmed%2CLast_Update%2CActive"\
             "&returnGeometry=false&featureEncoding=esriDefault&multipatchOption=none&maxAllowableOffset="\
             "&geometryPrecision=&outSR=4326&datumTransformation=&applyVCSProjection=false&returnIdsOnly=false"\
             "&returnUniqueIdsOnly=false&returnCountOnly=false&returnExtentOnly=false&returnQueryGeometry=false&"\
             "returnDistinctValues=false&cacheHint=false&orderByFields=&groupByFieldsForStatistics=&outStatistics="\
             "&having=&resultOffset=&resultRecordCount=&returnZ=false&returnM=false&returnExceededLimitFeatures=true"\
             "&quantizationParameters=&sqlFormat=none&f=pjson&token=";

WiFiClientSecure client;

void setup() {

  Serial.begin(115200);
  //while (!Serial); // Enable this line only when DEBUG.
  delay(1000);

  Serial.println();
  Serial.println("=======================");
  Serial.println(" COVID19 Summary Start ");
  Serial.println("=======================");

  // WiFi connection
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(500);
  WiFi.begin(ssid, password);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(FMB12);
  tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi..")) / 2, 120);
  tft.print("Connecting to Wi-Fi..");

  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected to");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  tft.fillScreen(TFT_BLACK);
  tft.setCursor((320 - tft.textWidth("WiFi Connected!")) / 2, 120);
  tft.print("WiFi Connected!");
  delay(1000);

  Serial.print("Fetch data from");
  Serial.println(url);
  Serial.println("set CA Cert:");
  Serial.println(test_root_ca);
  client.setCACert(test_root_ca);
}

void loop() {
  if (&client) {
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor((320 - tft.textWidth("Conectting to Server..")) / 2, 120);
      tft.print("Connecting to Server..");

      if (https.begin(client, url)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor((320 - tft.textWidth("Connected to Server!")) / 2, 120);
            tft.print("Connected to Server!");

            // Get Confirmed, Deaths, and Recovered.
            String payload = https.getString();
            Serial.println(payload);
            DynamicJsonDocument doc(1024 * 5);
            deserializeJson(doc, payload);
            String region = doc["features"][0]["attributes"]["Country_Region"].as<String>();
            unsigned long confirmed = doc["features"][0]["attributes"]["Confirmed"].as<unsigned long>();
            unsigned long deaths = doc["features"][0]["attributes"]["Deaths"].as<unsigned long>();
            unsigned long recovered = doc["features"][0]["attributes"]["Recovered"].as<unsigned long>();
            unsigned long lastUpdatedStrIdx = payload.lastIndexOf("Last_Update") + 15;
            time_t lastUpdatedEpoch = payload.substring(lastUpdatedStrIdx, lastUpdatedStrIdx + 10).toInt() + TZ;
            char lastUpdated[24];
            sprintf(lastUpdated,
                    "%04d-%02d-%02d(%s) %02d:%02d:%02d",
                    year(lastUpdatedEpoch),
                    month(lastUpdatedEpoch),
                    day(lastUpdatedEpoch),
                    wd[weekday(lastUpdatedEpoch) - 1],
                    hour(lastUpdatedEpoch),
                    minute(lastUpdatedEpoch),
                    second(lastUpdatedEpoch)
                   );

            Serial.println();
            Serial.println("[Data]");
            Serial.print("Region    : ");
            Serial.println(region);
            Serial.print("Confirmed : ");
            Serial.println(confirmed);
            Serial.print("Deaths    : ");
            Serial.println(deaths);
            Serial.print("Recovered : ");
            Serial.println(recovered);
            Serial.print("Updated   : ");
            Serial.println(lastUpdated);
            Serial.println();

            // LCD
            tft.fillScreen(tft.color565(24, 15, 60));
            tft.setFreeFont(FF17);
            tft.setTextColor(tft.color565(224, 225, 232));
            tft.drawString("COVID-19 Dashboard/" + region, 20, 10);

            tft.fillRoundRect(10, 35, 300, 55, 5, tft.color565(40, 40, 86));
            tft.fillRoundRect(10, 95, 300, 55, 5, tft.color565(40, 40, 86));
            tft.fillRoundRect(10, 155, 300, 55, 5, tft.color565(40, 40, 86));

            tft.setFreeFont(FM9);
            tft.drawString("Total Confirmed", 75, 40);
            tft.drawString("Total Deaths", 95, 100);
            tft.drawString("Total Recovered", 75, 160);

            tft.setFreeFont(FMB12);
            tft.setTextColor(TFT_RED);
            tft.drawNumber(confirmed, 110, 65);
            tft.setTextColor(tft.color565(224, 225, 232));
            tft.drawNumber(deaths, 120, 125);
            tft.setTextColor(TFT_GREEN);
            tft.drawNumber(recovered, 110, 185);

            tft.setFreeFont(FM9);
            tft.setTextColor(tft.color565(224, 225, 232));
            tft.drawString(lastUpdated, 40, 220);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
          tft.fillScreen(TFT_BLACK);
          tft.setCursor((320 - tft.textWidth("Connection failed!")) / 2, 120);
          tft.print("Connection failed!");
        }

        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor((320 - tft.textWidth("Unable to connect!")) / 2, 120);
        tft.print("Unable to connect!");
      }
      // End extra scoping block
    }

  } else {
    Serial.println("Unable to create client");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor((320 - tft.textWidth("Unable to")) / 2, 50);
    tft.print("Unable to");
    tft.setCursor((320 - tft.textWidth("create Client!")) / 2, 170);
    tft.print("create Client!");
  }

  Serial.println();
  Serial.println("Interval: 30min");
  delay(1800000);
}
