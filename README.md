# AstrOs Screen
This project is for a portable touch screen remote to trigger animations for [AstrOs](https://github.com/battlesloth/AstrOs.Server). It syncs with the configurations on the AstrOs Remote web page.

## Main Screen

![AstrOs Screen Main](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/AstrOsMain.jpg)

- The Arrow keys at the top change the current page of buttons.
- The "Panic Stop" Button tiggers all animations to hault immedaitely.
- The round indicator to the left of the "Panic Stop" indicates WiFi connection status (green for connected, red for disconnected).
- The gear button to the right of the "Panic Stop" opens the setting screen.
- The 9 buttons in the grid correspond to the AstrOs animation scripts configured in the AstrOs Server Screen Configuration page.

## Settings Screen

![AstrOs Screen Settings](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/AstrOsSettings.jpg)

- Click on the WiFi Scan button. The SSID drop down will populate with available access points
- Password field is for the WiFi password.
- If you are connecting directly to the AstrOs Server as a WiFi Access Point, leave "Server IP" blank and "Using WiFi Router" unchecked.
- If you are using a WiFi router instead, check "Using WiFi Router" and enter the AstrOs Server's IP in the "Server IP" field.
- The API Key comes from the AstrOs Server Utility page. This ensures the connection to the AstrOs Server is "secure" (for some defintions of secure).
- Click on the "Connect" button to connect to the WiFi.
- Click on the "Sync" button to sync the configuration from the AstrOs Screen Configuration page.
- Click on the "Close" button to return to the main screen.


## AstrOs Server Screen Configuration Page

![AstrOs Server Screen Configuration Page](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/AstrOsScreenPage.jpg)

## AstrOs Server Utility Page

![AstrOs Server Utility Page](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/AstrOsUtilityPage.jpg)

## Squareline Studio
The UI is designed using Squareline Studio (https://squareline.io/). Open /.squareline/AstrOsScreen.spj in squareline studio to make changes to the UI screens. 

You will need to update your "Project Export Root" and "UI Files Export Path" in your Squareline Project Settings.

![Squareline Studio Settings](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/squareline.jpg)

## Elecrow ESP32 WZ8048C050 ESP-IDF
This poject is currently set up to work on Elecrow 5" LCD display WZ8048C050. You may be able to get it working with other displays.


## How to run the project
Only ESP-IDF v5.2 was tested.
Using ESP-IDF via the Visual Studio Code extension is recommended. When it is installed, open folder containing this repository in Visual Studio Code, set your screen into bootloader mode, and press "ESP-IDF Build, Flash and Monitor" button. It should build and flash the application.

## Important notes
The lvgl_macros.h header is used from the root CMakeLists.txt file to setup LVGL custom macros needed to work with ESP-IDF