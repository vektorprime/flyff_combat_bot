# Flyff_Combat_Bot

Flyff Combat Bot is a free, advanced, and automatic bot for Flyff Universe.

The bot has been tested on Windows 10 and 11 x64.


Features:
* Doesn't trigger captcha
* Giant avoiding
* Healer or fighter mode
* Fighter always prioritizes monsters attacking healer
* Automatically finds monsters regardless of distance


Restrictions:
Running the bot takes over some mouse and keyboard actions, so you can't run more than one instance per PC.




**HOW TO START**

1. Download the release 
    - https://github.com/vektorprime/flyff_combat_bot/releases
2. Download and install Visual C++ redistributable files from Microsoft 
    - https://aka.ms/vs/17/release/vc_redist.x64.exe
3. Launch Flyff Universe in Chrome and keep the window up (don't minimize)
     - If you minimize the window the bot won't find it, and the bot will close instantly
4. Launch flyff_combat_bot.exe
    - If it closes instantly, it can't find the Chrome window with Flyff Universe running
5. Select your mode (fight or heal)
6. The default settings supp


**In-game requirements**

The Chrome window should be at the top left of your screen. Or optionally, you can have chrome taking up the full window (maximize). 
The character preview screen that
#1 in the screenshot -  The character screen, which shows your character's image, Lvl, HP, etc, should be on the top left.
#2 in the screenshot - The map should be at the top right.
Use the following in game options

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/afa87303-7601-4f0f-8610-3c4a888c8ec6)

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/9cce45ad-ab40-447b-a640-6b26cf43b3af)



**CONFIG**

The bot can be configured from both the config file and the GUI. The config file is called flyffcombat.cfg, and it's in the root directory with the bot executable.
The GUI always loads from the config file. Changes in the GUI are currently not saved back in the config file.

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/fcfc73b3-9d31-4bcb-83b7-b1cc710eb427)

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



**Giant Avoiding**

Giant avoiding is done by finding the giant on the map. This requires the following map/navigator settings.

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/a61af687-1e25-4faa-b17a-06830321dc2f)

I had to use the above method because distinguishing giants from normal monsters via memory was difficult. 

**Pick-up pets**

Pick-up pets should be out BEFORE you start the bot.


Technical overview of bot:
The bot utilizes both image matching, text recognition (OCR), and memory reading to operate.

**Compile The Bot**

The below information is for anyone who wants to compile the bot and modify it.

* C++ 20
* Visual studio 2022 v143 toolset
* OpenCV 4.7
* The bot utilizes Tessarct for OCR of red text. 

