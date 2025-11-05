# AstrOs Screen
This project is for a portable touch screen remote to trigger animations for AstrOs (https://github.com/battlesloth/AstrOs.Server).

It syncs with the configurations on the AstrOs Remote web page.



## Squareline Studio
The UI is designed using Squareline Studio (https://squareline.io/). Open /.squareline/AstrOsScreen.spj in squareline studio to make changes to the UI screens. 

You will need to update your "Project Export Root" and "UI Files Export Path" in your Squareline Project Settings.

![Squareline Studio Settings](https://github.com/battlesloth/AstrOs.Screen/blob/main/.readme_files/squareline.jpg)

## Elecrow ESP32 WZ8048C050 ESP-IDF
This poject is currently set up to work on Elecrow 5" LCD display WZ8048C050. You may be able to get it working with other displays.


## How to run the example
Only ESP-IDF v5.2 was tested.
Using ESP-IDF as Visual Studio Code extension is recommended. When it is installed, please open folder containing this repository in Visual Studio Code and press "ESP-IDF Build, Flash and Monitor" button. It should build and flash the application.

## Important notes
The lvgl_macros.h header is used from the root CMakeLists.txt file to setup LVGL custom macros needed to work with ESP-IDF