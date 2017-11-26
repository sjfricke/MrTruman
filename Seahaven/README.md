[How to setup board for Seahaven](./docs/setup.md)

# Important Notes

- **HARD** Assumption screen is 800 x 480

# Seahaven mappings

Each event is given a index number to save from having to send a string. A future enum will be created in the host portion.

Each event has two parts `<type> <value>`

## Host-to-Web Calls
- `0` - General state of animation
  - `0` - Go to ideal mode
  - `1` - Get ready to ask question
- `1` - Light switch animation
  - `0` - Turn on
  - `1` - Turn off
  - `2` - Toggle Lights
- `2` - Change lights color animation
  - Value given as RGB with commas 
  - Ex.) `0,255,0` would be green
- `3` - Fireplace animation
  - `0` - Turn on
  - `1` - Turn off
  - `2` - Toggle Fire
- `4` - Camera animation
  - `0` - About to take picture
  - `1` - Take picture (flash)
  - `2` - Make GET request for photo and put it up
- `5` - Speakers animation
  - `0` - Bring out speakers
  - `1` - Music playing
  - `2` - Music stopped
  - `3` - Put away speakers
- `6` - Music BPM
  - Value int of BPM
- `7` - Gyro animation
  - Value is of rotation in degrees between 0 and 359 where 180 is level
- `8` - Chatting with Mr T.
  - Value could be random selection/chat to play back if we write some whimsical quips.

## Web-to-Host Calls
- `0` - Busy with Animation - try again later
- `1` - Light switch animation
  - `0` - Turn on lights
  - `1` - Turn off lights
- `2` - Fireplace (Fan) animation
  - `0` - Turn on fan
  - `1` - Turn off fan
- `3` - Camera animation
  - `0` - Ready to take picture (countdown)
  - `1` - Picture on wall
- `4` - Speaker animation
  - `0` - Speakers Up and ready
  - `1` - Speakers Down
- `5` - Gyro animation back to ideal mode
- `6` - Chat Bubble is Done
- `7` - Ready to record audio

## State Machine

![State Machine](Animation_State_Machine.png)
