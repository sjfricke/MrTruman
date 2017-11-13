// global WebSocket pointer
var webSocket;

// decides what do when message arrives
function wsOnMessage(event) {

  console.dir(event);
 // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);

  switch(parseInt(message.type)) {
    case 0:
        animateSpineBoy();
      break;
    case 1:
        if (message.value == 0) { 
          renderer.app.ticker.add(speakersOff);
          wsTurnSpeakersOff()
        } else if (message.value == 1) { 
          renderer.app.ticker.add(speakersOn);
        }
      break;
    case 2:
      animateRain();
      break;
  case 3:
      animateLightning();
      break;
    default:
  	  warn("WebSocket", "No case for data: %0", message);
  }
}

function wsTurnLightsOff() {
    webSocket.send("0:0");
}

function wsTurnLightsOn() {
    webSocket.send("0:1");
}

function wsTurnSpeakersOff() {
    webSocket.send("1:0");
}

function wsTurnSpeakersOn() {
    webSocket.send("1:1");
}




/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function startLightOnAnimation() {
    webSocket.send('{"type":0,"value":0}');
}

function startLightOffAnimation() {
    webSocket.send('{"type":0,"value":1}');
}

function startSpeakerOnAnimation() {
    webSocket.send('{"type":1,"value":0}');
}

function startSpeakerOffAnimation() {
    webSocket.send('{"type":1,"value":1}');
}
