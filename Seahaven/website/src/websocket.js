// global WebSocket pointer
var webSocket;
var oppTiltCnt = 0;
var startX = 400;
var gameScore = 0.000;
var gameScoreLoseTime;
var gameScoreLoseAmt;
var chatCallback = 0;

const nestTemp = document.getElementById("temp");
const gameScoreEl = document.getElementById("gameScore");
const gameScoreLose = document.getElementById("gameScoreLose");
const gameHighScore = document.getElementById("gameHighScore");
gameHighScore.innerHTML = "High Score: " + (highscore.toFixed(3) || "250.000");
const speechText = document.getElementById("speechText");

// decides what do when message arrives
function wsOnMessage(event) {

   // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);

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
  case 2:
    talkKill();
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
    if (message.value == 0) { 
      (s_animationOn) ? wsBusy() : pictureAnimation();
    } else if (message.value == 1) { 
      pictureFlash();
    } else if (message.value >= 2) { 
      s_pictureIndex = message.value;
      pictureTrigger();      
    }
      break;
  case 5:
log("websocket", "Key - Value", message.type, message.value);
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
    } else {      
      s_tiltRight = (message.value == -0.707) ? true : false;
      tiltValue = message.value;     

      if (!s_tiltAnim) { tiltAnimation(); } //only one starting of tilt
      else if (!s_tiltGame) {
        s_tiltGame = true;
        nestTemp.style.left = "4.3%";
        oppTiltCnt = 1;
        nestTemp.innerHTML = "01";
        gameHighScore.style.visibility = "visible";
      }  
      else if (s_tiltRightLast != s_tiltRight) { 
        s_tiltWall = s_tiltWallCouch = false;
        player.state.setAnimation(0, "fallHold", false);
        oppTiltCnt++;
        nestTemp.innerHTML = (oppTiltCnt < 10) ? "0" + oppTiltCnt : oppTiltCnt;
        tiltReduceScore();
        startX = player.position.x;
      }
      s_tiltRightLast = s_tiltRight;
    }
    break;
  case 8:
    if (s_animationOn || s_talkAnim) { wsBusy(); }
    else {
      chatCallback = message.value;
      speechText.style.top = (message.chat.indexOf("<br>") >= 0) ? "56%" : "58.8%";
      speechText.innerHTML = message.chat;

      if (message.value == 4) {
        speechText.style.fontSize = "12px";
        speechText.style.lineHeight = "19px";
    	  setTimeout(function(){speechText.innerHTML = "skrrrahh pap pap ka-ka-ka<br>Skidiki-pap-pap<br>"},4000);
    	  setTimeout(function(){speechText.innerHTML = "skrrrahh pap pap ka-ka-ka<br>Skidiki-pap-pap<br>pu-pu-pudrrrr-boom"},6000);
	    }

      talkAnimation();
    }
    
    break;
  case 9:
      if (s_animationOn || !s_idleMode || s_fidgetAnim ||
	  (s_couchOn && message.value != 1)) { wsBusy(); }
      //else { (s_fidgetAnim) ? fidgetKill() : fidgetAnimation(); }
      else {
	  fidgetAnimation();
	  setTimeout(function(){fidgetKill();}, 3000);
      }
      break;

  case 10:
    player.filters = [ glowFilter ]  
    setTimeout(function(){ player.filters = [];}, 5000);
    break;

  case 11:
    displayHelp();
    setTimeout(function(){ helpBoxClose();}, 10000);
    break;

  default:
	  warn("WebSocket", "No case for data: %0", message);
  }
}

function wsBusy() {
  // webSocket.send("0:0");
}
function wsAllReadyToStart() {
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
function wsChatReady() {
  webSocket.send("6:" + chatCallback);
}
function wsVolume(value) {
  webSocket.send("8:" + value);
}
function wsPowerOff() {
  webSocket.send("9:0");
}
function wsReboot() {
  webSocket.send("9:1");
}
function wsUpdateHighScore() {
    webSocket.send("10:" + highscore.toFixed(3));
}

/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function test0() {
    webSocket.send('{"type":4,"value":0}');
}

function test1() {
    webSocket.send('{"type":4,"value":1}');
}

function test2() {
    webSocket.send('{"type":4,"value":2}');
}
