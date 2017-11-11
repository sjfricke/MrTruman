
# Setup

Here are the things needed to setup board to run everything we need

First good step is to run
	
`sudo apt update`
`sudo apt upgrade`

- [Wi-Fi](#wi-fi)
- [Audio](#audio)
- [PocketSphinx](#pocketsphinx)
- [Camera](#camera)
- [Chrome on Boot](#chrome-on-boot)
- [Rebuilding and booting Linux](#rebuilding-and-booting-linux)

## Wi-Fi

The two main commands you need are `nmcli` to check your WiFi and `nmtui` to set it.

The board doesn't come with `ifconfig` and more details about WiFi can [be found here](https://developer.qualcomm.com/mlh)

## Audio

More info about the Audio system on the dragonboard can be found [here](https://developer.qualcomm.com/qfile/29468/lm80-p0436-43_stereocontaudioroutappnote.pdf)

We are using ALSA for sound which requires a few things to set up:

- A series of calls to configure ALSA are needed, we might right a script, but otherwise here are what they should be
- Mic Input
	- `amixer -c 0 cset iface=MIXER,name='DEC1 MUX' 'ADC2'`
	- `amixer -c 0 cset iface=MIXER,name='ADC2 MUX' 'INP3'`
	- `amixer -c 0 cset iface=MIXER,name='ADC2 Volume' 4`
		- The input volume is set between 0 - 8 and each value adds 6db to the input mic gain
	- `arecord -D plughw:0,2 -r 48000 -f S16_LE output.wav`
- `amixer -c 0 cset iface=MIXER,name='RX3 MIX1 INP1' 'RX1'`
- `amixer -c 0 cset iface=MIXER,name='SPK DAC Switch' 1`
- `amixer -c 0 cset iface=MIXER,name='RX3 Digital Volume' 128`
	- `aplay -D plughw:0,1 /usr/share/sounds/alsa/Front_Left.wav`

- To load anything with `<alsa/asoundlib.h>` libraries you need to run `sudo apt install libasound2-dev`

## PocketSphinx

[Check out the PocketSphinx doc](./PocketSphinx)

## Camera

`sudo apt install v4l-utils`

## Chrome on Boot

- Clear `/etc/X11/default-display-manager` file
	- Should have `/usr/bin/sddm` inside
		- sddm == Simple Desktop Display Manager
	- Will need to reboot to see effects
- Edit Chromium configure settings to match screen:
	- **NOTE:** You need to have open Chromium once prior as root to see preferences
		- `chromium --no-sandbox`
	- Opne `~/.config/chromium/Default/Preferences`
	- replace `window_placement` with:
		- `window_placement":{"bottom":480,"left":0,"maximized":true,"right":800,"top":0,"work_area_bottom":480,"work_area_left":0,"work_area_right":800,"work_area_top":0}`
		- This really just needs to match the screen size being used
- Have linaro login without password 
	- Open `/lib/systemd/system/getty@.service`
		- Change:
			- `ExecStart=-/sbin/agetty --noclear %I $TERM`
		- TO 
			- `ExecStart=-/sbin/agetty --noclear -a root %I $TERM`
- Launch Chrome to full screen
	- `xinit /usr/bin/chromium --kiosk http://localhost:6419 --no-sandbox`
- Setup bash script on startup 

## Rebuilding and booting Linux

This was needed to get the **ov5647** camera sensor to work on the dragonboard since it is not by default

