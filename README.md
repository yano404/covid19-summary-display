COVID19 Summary Display
=======================

Display COVID-19 confirmed, deaths, and recovered cases on the LCD of Wio Terminal.

<div align="center">
    <img src="https://i.imgur.com/C7ilaAu.jpg" alt="image" width="400"/>
</div>

## Requirements
- [Seeed_Arduino_LCD](https://github.com/Seeed-Studio/Seeed_Arduino_LCD)
- [Seeed_Arduino_rpcWiFi](https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [TimeLib](https://github.com/PaulStoffregen/Time)
## Installation
1. Change the ssid and password to your WiFi Network.
   ```c
   const char* ssid     = "Your WiFi SSID"; // WiFi SSID
   const char* password = "Your Password";  // WiFi pass
   ```
2. If necessary, change the default region.
    Region ID is shown in [the table below](#list-of-regions).
   ```c
   /* Default Region: Japan */
   int regionID = 10;
   ```
3. If necessary, change the refresh interval (milli seconds).
   ```c
   const unsigned long REFRESH_INTERVAL = 1800000; // 30min
   ```
4. Upload sketch to the Wio Terminal.

## Usage
### Turn verbose mode ON/OFF
Press `C` button.
There are 3 buttons on the upper side of Wio Terminal, and `C` button is the left one.

### Change the region
1. Press `B` button of Wio Terminal.
   Then the list of regions will be displayed.
2. Choose the region.
   | Button                  | When Pressed            |
   | ----------------------- | ----------------------- |
   | `UP/DOWN` (5way switch) | Scroll list of regions. |
   | `PRESS` (5way switch)   | Select the region.      |
   | `B` (upper side button) | Cancel.                 |

### When unable to connect the WiFi or server
Reset Wio Terminal.
Almost all errors are solved by reset.

## Data Sources
Fetch the information from [UN COVID-19 Data Hub Cases Country](https://covid-19-data.unstatshub.org/datasets/1cb306b5331945548745a5ccd290188e_2).

## List of Regions
| ID  | Region         | ObjectID |
| --- | -------------- | -------- |
| 0   | Australia      | 9        |
| 1   | Brazil         | 24       |
| 2   | Myanmar(Burma) | 28       |
| 3   | Canada         | 33       |
| 4   | China          | 37       |
| 5   | Egypt          | 54       |
| 6   | France         | 63       |
| 7   | Germany        | 67       |
| 8   | India          | 80       |
| 9   | Italy          | 86       |
| 10  | Japan          | 88       |
| 11  | South Korea    | 92       |
| 12  | Malaysia       | 108      |
| 13  | Mexico         | 115      |
| 14  | New Zealand    | 126      |
| 15  | Pakistan       | 133      |
| 16  | Philippines    | 138      |
| 17  | Poland         | 139      |
| 18  | Portugal       | 140      |
| 19  | Russia         | 143      |
| 20  | Singapore      | 156      |
| 21  | South Africa   | 161      |
| 22  | Spain          | 163      |
| 23  | Sweden         | 167      |
| 24  | Switzerland    | 168      |
| 25  | Taiwan         | 170      |
| 26  | Thailand       | 173      |
| 27  | Turkey         | 178      |
| 28  | USA            | 179      |
| 29  | United Kingdom | 183      |
| 30  | Vietnam        | 188      |
