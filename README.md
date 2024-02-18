# Steam Controller to DS4 Emulator #

This is a C++ Windows program which uses the [ViGEm](https://github.com/nefarius/ViGEmBus) emulation framework to create a virtual Dualshock 4 which can be handled by a Steam Controller via kolrabi/ltjax's [Steam Controller library](https://github.com/ltjax/steam_controller).

## Setup ##
To run the program, you will need to do the following:

- Install [ViGEmBus](https://github.com/nefarius/ViGEmBus/releases).
- Install [Microsoft Visual C++ Redistributable 2017](https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist) or newer.
- Unpack and "SteamControllerToDS4" from Releases or the bottom of the page and launch the executable.
- Connect your Steam Conrtroller using wired USB.

The program should then translate your Steam Controller inputs directly to the virtual DS4.

To build the program from source, you will need to do the following:

- Link the steam_controller and ViGEmClient libraries.
- Include [steam_controller](https://github.com/ltjax/steam_controller/tree/master/include/steam_controller) and [ViGEmClient](https://github.com/nefarius/ViGEmClient/tree/master/include/ViGEm) header files.
- Build using your method of choice. A Visual Studio project file is included but I suppose you could also build from CMake or other.


## Troubleshooting
If some inputs (gyro, trackpad) or vibration is not working properly, then there might be an issue with how Steam is handling your inputs. To fix this problem, you will need to adjust your controller's settings in Steam. This can be done by opening Steam -> Settings (upper left corner) -> Controller, then adjusting the following settings:

- Game Rumble: On
- Use Nintendo Button  Layout: Off
- Desktop Layout -> Edit -> Current Button Layout -> Edit Layout

All buttons, triggers, the joystick, trackpads, and the gyro should be mapped to something other than "None". It is reccomended to map these to standard gamepad inputs as opposed to mouse/keyboard so that unexpected keyboard shortcuts and mouse movement do not happen.

At this time, only one virtual Dualshock 4 is supported at a time. Adding more than one Steam Controller will cause one to be selected and used exclusively.

If you would like to use the Steam Button without opening the Steam window, close Steam via Task Manager.

In general, if the program picks up absolutely no input or cannot find any controllers, try quitting Steam then try relaunching the program.

If two controllers are detected by your program simultaneously, you can hide your gamepad using [HidHide](https://github.com/nefarius/HidHide).

## Accessing Trackpad ##
This program utilizes the unbound grip buttons (found on the back side of the Steam Controller) to toggle whether the virtual Dualshock 4 controller utilizes the Steam Controller's trackpads to emulate the d-pad and right joystick (default), or the trackpad. 

Pressing only the left grip will toggle the left trackpad only. The left trackpad will map to span the entire Dualshock 4 trackpad. The same goes for pressing only right grip with the right trackpad.

Pressing both grips simultaneously will toggle both trackpads to be used at once. This is useful for trackpad commands which require input with two fingers. In this mode, each trackpad will span only half of the Dualshock 4 trackpad. 

## Download ##
[Download](https://github.com/Jjajangioli/SteamControllerToDS4/releases)

---

## Contact ##
Please feel free to contact me at jjajangioli@gmail.com

I would love to answer your questions or hear your feedback!
