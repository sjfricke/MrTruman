// creates connection first
var ws = new WebSocket('ws://' + location.host);

// Set values on page
var speakerStatus = document.getElementById("speakerStatus");
var rotationLevel = document.getElementById("rotationLevel");

// decides what do when message arrives
ws.onmessage = function(event) {

  var message = JSON.parse(event.data);
  console.log(message);    
    switch(parseInt(message.type)) {
    case 0:
	document.body.style.backgroundColor = message.value;
	setTimeout(function(){ws.send("0:0");}, 2000);
	break;
    case 2:
	speakerStatus.innerHTML = message.value;
	break;
    case 3:
	console.log("3.5 mm jack: " + message.value);
	break;
    case 4:
	rotationLevel.innerHTML = message.value;
	break;
    default:
	console.log("No case for data");
	console.log(message);
  }

}

// sends song back when button pressed
function simulateData(type, value) {
    ws.send(type + ":" + value);
}

// sends end of animation after 3 second delay
function simulateLightAnimation() {
    ws.send("0:0");
    setTimeout(function(){ws.send("0:1");}, 3000);
}

