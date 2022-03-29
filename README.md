# EMMS_Code_UserInterface
MPLAB X Project
- Chip - PIC24FV32KA302
- Compiler - XC16 1.61

# ToDo
- nothing at this time

# Changelog
### NEXT
- none yet
- When password change is about to be saved the "Save" was not on the far right and the previous character showed


### v2.1.3
March 26, 2022<br />
Tom Austin
#### Changes
- increased MAX_PARAMETER_LENGTH to account for ModInfo and NULL_CHAR
- ModInfo menu - right justified right info elements
- Alarms - rewrote whole alarm system since old one was not working quite right
  - remove many global variables as well
- main loop oneshots might not run as expected
  - if a previous oneshot lined up with a subsequent one it might prevent the subsequent one from firing if the runtime took too long
  - change to use explicit time to run so the function is never skipped
- cleaned up extra global variables and functions for showing time / date on main screens
- statistics menu was showing wrong values
  - communications was reading parameter[2] twice - now reads [2] and [3]
  - menu text was misleading - now fixed to Yesterday & Lifetime
- some general cleanup
  - removed some communication items which were not used (like all Read commands)
  - cleaned up a few vestigial items

### v2.1.2
March 20, 2022<br />
Tom Austin
#### Changes
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
- Fix - Menu now times out separate from backlight timeout
  - this allows the menu to reset so the parameters can be collected again
  - by design parameters are only collected when on the main screens so they do not interfere with setting them in the menus
- Feature - Meter Name
  - Updated About menu to remove outdated version info and show Meter Name
  - Update communications to handle Meter Name (MName)
- Cleanup - removed some items no longer used
  - PSVersion
- Moved ModInfo menu to be under the Admin
  - It could contain some sensitive info


### v2.1.1
March 19, 2022<br />
Tom Austin
#### Changes
- New versioning and changelog scheme
## 
### Pre v2.1.1
#### Previous changes are not currently listed
