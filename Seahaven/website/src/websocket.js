// global WebSocket pointer
var webSocket;
var prevTiltDir = null;
var oppTiltCnt = 0;
var startX = 400;
var gameScore = 0.000;

const nestTemp = document.getElementById("temp");
const gameScoreEl = document.getElementById("gameScore");

// decides what do when message arrives
function wsOnMessage(event) {

   // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);
  log("websocket", "Key - Value", message.type, message.value);
  switch(parseInt(message.type)) {
  case 1:
      if ((message.value == 0 && !s_lightOn) ||
          (message.value == 1 && s_lightOn)  ||
          (message.value == 2)) {
        (s_animationOn) ? wsBusy() : lightAnimation();
      } else {
	  (s_lightOn) ? wsTurnLightsOn() :  wsTurnLightsOff();
      }
      break;
  case 3:
      if ((message.value == 0 && !s_fireOn) ||
          (message.value == 1 && s_fireOn)  ||
          (message.value == 2)) {
        (s_animationOn) ? wsBusy() : fireAnimation();
      } else {
	  (s_fireOn) ? wsFireOn() :  wsFireOff();
      }
      break;
  case 4:
    if (message.value == 0 && !s_pictureUp) { 
      (s_animationOn) ? wsBusy() : pictureAnimation();
    } else if (message.value == 1 && s_pictureUp) { 
      pictureFlash();
    } else if (message.value >= 2) { 
      s_pictureIndex = message.value;
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
    if (!s_tiltAnim) {
      nestTemp.innerHTML = message.value + "&#176;";
    }
    break;
  case 7:
    if (s_animationOn && !s_tiltAnim) {
	(message.value == 0) ? wsTiltDone() : wsBusy(); // prevent noice from host side
    } else if (message.value == 0) {
      tiltRecovery();
      // reset game nest_temp goes back to 88 degrees
      oppTiltCnt = 0;
      prevTiltDir = null;
      nestTemp.innerHTML = "88&#176;";
      gameScoreEl.innerHTML = "";
    } else {      
      s_tiltRight = (message.value < 0) ? true : false;
      // see if we are tilting in opp dir
      if (prevTiltDir != s_tiltRight) {
          oppTiltCnt++;
          startX = player.position.x;
      }
      prevTiltDir = s_tiltRight;

      // see if we should start game or update
      if (oppTiltCnt > 1 || nestTemp.innerHTML.includes("&nbsp;")) {
        nestTemp.innerHTML = "&nbsp;" + oppTiltCnt;
      }
      tiltValue = message.value;
      if (!s_tiltAnim) { tiltAnimation(); } //only one starting of tilt
      else if (s_tiltRightLast != s_tiltRight) { 
          s_tiltWall = s_tiltWallCouch = false;
          player.state.setAnimation(0, "fallHold", false);
      }
      s_tiltRightLast = s_tiltRight;
    }
    break;
  case 8:
    if (s_animationOn || s_talkAnim) { wsBusy(); }
    else {
      document.getElementById("speechText").style.top = (message.value.indexOf("<br>") >= 0) ? "56%" : "58.8%";
      document.getElementById("speechText").innerHTML = message.value;
      talkAnimation();
    }
    break;
  case 9:
    if (s_animationOn) { wsBusy(); }
    else { (s_fidgetAnim) ? fidgetKill() : fidgetAnimation(); }
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
function wsFireOnSound() {
  webSocket.send("2:2");
}
function wsFireOffSound() {
  webSocket.send("2:3");
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
function wsSpeakersReady() {
    webSocket.send("4:2");
}
function wsTiltDone() {
  webSocket.send("5:0");
}
function wsTiltTrumanWall() {
  webSocket.send("5:1");
}
function wsTiltCouchWall() {
  webSocket.send("5:2");
}
function wsVolume(value) {
  webSocket.send("8:" + value);
}
function wsPowerOff() {
  webSocket.send("9:0");
}

/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function test0() {
    webSocket.send('{"type":7,"value":0.707}');
}

function test1() {
    webSocket.send('{"type":7,"value":-0.707}');
}

function test2() {
    webSocket.send('{"type":7,"value":0.0}');
}
