# Flyff_Combat_Bot

Flyff Combat Bot is a free bot for Flyff Universe running on Chrome.

The bot features a fighter and healer mode. Running the bot takes over some mouse and keyboard actions, so you can't run more than one instance per PC.

if you want to compile this from source, go to the bottom of the page.

**REQUIREMENTS**

Visual C++ redistributable files

https://aka.ms/vs/17/release/vc_redist.x64.exe


**CONFIG**

The bot can be configured from both the config file and the GUI. The config file is called flyffcombat.cfg
The GUI always loads from the config file. Changes in the GUI are currently not saved back in the config file.

![Config file](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/2031fed0-da45-4320-bca9-8fa18ac7da03)




Here's an image that explains which config file sections correlate to which GUI items.


**Fighter GUI Config Sections**
![FIGHTER-GUI-CONFIG-FILE-EXPLANATION](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/44f8e5c0-caca-42ae-b806-87c7dc332aaf)


**Healer GUI Config Sections**
![HEALER-GUI-CONFIG-FILE-EXPLANATION](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/2187ddb8-5042-43ec-8c4c-f16ae4f2d65d)

**Logging**
A log file called flyffcombat.log is created automatically on startup. The log is appended to on every run.
2 new lines are added to differentiate the runs
_"Log file started 
config loaded"_
The default logging level is "Error." Error means the bot will stop and crash. Verbose is for troubleshooting certain actions.

![GUI-LOGGING](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/57b045b3-5e43-4a5b-ab4d-e0f67b429097)

**Compile The Bot**
The bot utilizes Tessarct for OCR of red text. It also heavily utilizes OpenCV for certain image matching and logic.
