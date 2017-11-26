// global WebSocket pointer
var webSocket;

// decides what do when message arrives
function wsOnMessage(event) {

   // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);
//console.dir(message);
  switch(parseInt(message.type)) {
  case 0:
      break;
  case 1:
      if ((message.value == 0 && !s_lightOn) ||
          (message.value == 1 && s_lightOn)  ||
          (message.value == 2)) {
        (s_animationOn) ? wsBusy() : lightAnimation();
      }
      break;
  case 2:
      break;
  case 3:
      if ((message.value == 0 && !s_fireOn) ||
          (message.value == 1 && s_fireOn)  ||
          (message.value == 2)) {
        (s_animationOn) ? wsBusy() : fireAnimation();
      }
      break;
  case 4:
    if (message.value == 0 && !s_pictureUp) { 
      (s_animationOn) ? wsBusy() : pictureAnimation();
    } else if (message.value == 1 && s_pictureUp) { 
      pictureFlash();
    } else if (message.value == 2) { 
      pictureTrigger();
    }
      break;
  case 5:
    if (message.value == 0 && !s_speakersUp) { 
      (s_animationOn) ? wsBusy() : speakerAnimation();
    } else if (message.value == 1 && s_speakersUp) { 
      speakersOn();
    } else if (message.value == 2) { 
      speakersOff();
    } else if (message.value == 3 && s_speakersUp) {
      if (s_speakersOn) { speakersOff(); }
      speakerAnimation();
    }
    break;
  case 6:
      break;
  case 7:
      if (s_animationOn && !s_tiltAnim) {
        wsBusy();
      } else if (message.value == 180) {
        tiltRecovery();
      } else {
        s_tiltRight = (message.value > 180) ? true : false;
        tiltAnimation();
      }
      break;
  case 8:
      break;
  case 9:
      break;
  default:
	  warn("WebSocket", "No case for data: %0", message);
  }
}

function wsBusy() {
  webSocket.send("0:0");
}
function wsTurnLightsOff() {
  webSocket.send("1:0");
}
function wsTurnLightsOn() {
  webSocket.send("1:1");
}
function wsFireOn() {
  webSocket.send("2:0");
}
function wsFireOff() {
  webSocket.send("2:1");
}
function wsPictureReady() {
  webSocket.send("3:0");
}
function wsPictureDone() {
  webSocket.send("3:1");
}
function wsSpeakersUp() {
  webSocket.send("4:0");
}
function wsSpeakersDown() {
  webSocket.send("4:1");
}
function wsTiltDone() {
  webSocket.send("5:0");
}


/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function test0() {
    webSocket.send('{"type":7,"value":190}');
}

function test1() {
    webSocket.send('{"type":7,"value":0}');
}

function test2() {
    webSocket.send('{"type":7,"value":180}');
}