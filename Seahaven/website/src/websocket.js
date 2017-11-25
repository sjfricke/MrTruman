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
        lightAnimation();
      }
      break;
  case 2:
      break;
  case 3:
      break;
  case 4:
      break;
  case 5:
    if (message.value == 0) { 
      renderer.app.ticker.add(speakersOn);
    } else if (message.value == 3) { 
      renderer.app.ticker.add(speakersOff);
    }
    break;
  default:
	  warn("WebSocket", "No case for data: %0", message);
  }
}

function wsTurnLightsOff() {
    webSocket.send("1:0");
}

function wsTurnLightsOn() {
    webSocket.send("1:1");
}

// function wsTurnSpeakersOff() {
//     webSocket.send("1:0");
// }

// function wsTurnSpeakersOn() {
//     webSocket.send("1:1");
// }




/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function startLightOnAnimation() {
    webSocket.send('{"type":1,"value":0}');
}

function startLightOffAnimation() {
    webSocket.send('{"type":1,"value":1}');
}

// function startSpeakerOnAnimation() {
//     webSocket.send('{"type":1,"value":0}');
// }

// function startSpeakerOffAnimation() {
//     webSocket.send('{"type":1,"value":1}');
// }
