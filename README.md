# EMMS_Code_UserInterface
MPLAB X Project
- Chip - PIC24FV32KA302
- Compiler - XC16 1.61

# ToDo
- nothing at this time

# Changelog
### NEXT
- Bugfix - Alarm Menu
  - Alarm threshold would show 0 instead of "Off"
  - fixed by removing the AlarmXEnabled variable since it was directly derived from the AlarmXThreshold variable
- Cleanup - removed AlarmXEnabled from the communication commands
- Tweak - On Module Menu moved the module index to the lower right since that space is used the least
- Tweak - On the Detail menu changed "Used" to "Left" - better meaning of what is shown
- Tweak - adjusted the detail menu top line to avoid formating issues
- Fix - Reset Time Menu would display the reset time incorrectly at times
  - it will display the reset time as currently set. It is OK to have non 30 minute mark time 7:45
  - but if this menu changes the reset time then it will be restricted to the 30 minute mark
  - updated the math on time "Up" and "Down" to handle non 30 minute start time


### v2.1.1
March 19, 2022<br />
Tom Austin
#### Changes
- New versioning and changelog scheme
## 
### Pre v2.1.1
#### Previous changes are not currently listed
