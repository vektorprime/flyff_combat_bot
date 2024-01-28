# Flyff_Combat_Bot

Flyff Combat Bot is a free, advanced, and fully automatic bot for Flyff Universe. I made this to learn C++.

The bot has been tested on Windows 10 and 11 x64.


1. Download and install Visual C++ redistributable files from Microsoft 
    - https://aka.ms/vs/17/release/vc_redist.x64.exe
2. Download the bot
    - [https://github.com/vektorprime/flyff_combat_bot/releases](https://github.com/vektorprime/flyff_combat_bot/releases/download/v1.0.0/Flyff_Combat_1_0_0_WIN.zip)
      
![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/593e0107-f18d-4cc9-82a8-6dc31f42acb6)


**Demo of the bot in action**

https://github.com/vektorprime/flyff_combat_bot/assets/9269666/b329d3fb-c3b9-4a1b-9af0-8c24811f5313



Features:

* Healer or fighter mode
* Monitor hp/mp/fp and use potions to restore
* Tracks all monsters nearby and prioritizes the ones closest to the player
* Fighter always prioritizes monsters attacking healer, then monsters attacking the fighter, then monsters that are not in combat
* Doesn't trigger captcha
* Restores operation if game disconnects
* Giant avoiding
* Player returns to original position after fighting for a while

Restrictions:
Running the bot takes over some mouse and keyboard actions, so you can't run more than one instance per PC.



**BASIC HOW TO START**


1. Download and install Visual C++ redistributable files from Microsoft 
    - https://aka.ms/vs/17/release/vc_redist.x64.exe
2. Download the bot
    - [https://github.com/vektorprime/flyff_combat_bot/releases](https://github.com/vektorprime/flyff_combat_bot/releases/download/v1.0.0/Flyff_Combat_1_0_0_WIN.zip)
3. Create a folder somewhere and unzip all of the contents into that folder
4. Launch Flyff Universe in Chrome and keep the window up (don't minimize)
     - If you minimize the window the bot won't find it, and the bot will close instantly
5. Launch flyff_combat_bot.exe
    - If it closes instantly, it can't find the Chrome window with Flyff Universe running
6. Select your mode (fight or heal)
7. Fill out your player name under Setup Players, ommit any titles or guild name.
8. The default settings assume you have a healer with you, uncheck "Enable monitor and defend healer" if you do not. Otherwise, the bot will crash.
9. The Self Buffs section assumes all they are in the F2 skill bar (second skill bar)
10. You MUST have the "Attack selected target" action set as the #6 skill in the F1 (first) skillbar like below.
    - If you don't know where that is, press "o" on your keyboard to open the Motion menu and find it here.

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/df466c0c-e85d-47bc-bd18-d595f4ccbc71)

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/bb1a375e-3201-4fab-9b45-9201b1c0ca74)

11. Make sure your chrome window is Top left of the monitor window if it's not full screen.

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/78042bcd-bcdf-41bf-9c4e-d2386bea3376)

12. Click start on the bot GUI.

13. You may need to mess with the zoom of the character, I generally zoom out and a little high.

If you ever need to close the bot, you can click stop in the GUI. You can also close the windows console window which runs the bot. I usually just alt F4 on that.

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






Here's an image that explains which config file sections correlate to which GUI items.


**GUI Config Sections**

![image](https://github.com/vektorprime/flyff_combat_bot/assets/9269666/c622df99-cf7e-4ae5-bcdd-a7cdf442f9e5)



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

**General tips**

Try to fight monsters your level or 1 level below.
I usually hide special effects and player names, but this isn't required.
Zoom out a bit and face the camera a little higher, it helps when the bot auto-focuses.

**Troubleshooting**
If you get any strange behavior, please restart Chrome and the bot. Things like character position not returning, or character no longer fighting are cleared with this.
The bot is set to crash/close if it cannot set itself up. I do this so that I can detect errors instead of just proceeding.
If you want to send me logs, set the log to "verbose" in the GUI. Then, send me the flyffcombat.log file. 

**EVERYTHING BELOW THIS LINE IS TECHNICAL DETAILS FOR ANYONE INTERESTED IN THE SOURCE CODE AND COMPILING**


Technical overview of bot:
The bot utilizes both image matching, text recognition (OCR), and memory reading to operate.

**Compile The Bot**

The below information is for anyone who wants to compile the bot and modify it.

* C++ 20
* Visual studio 2022 v143 toolset (others will probably work too)
* OpenCV 4.7 or latest version from VCPKG
* Latest version of Tesseract from VCPKG

