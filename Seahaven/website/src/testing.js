document.addEventListener("keydown", testFunctions, false);

function testFunctions(e) {
	var keyCode = e.keyCode;
	switch(keyCode) {
		case 81: // Q
			webSocket.send('{"type":8,"value":2,"chat":"asdfasdf"}');
			break;
		case 87: // W
			webSocket.send('{"type":2,"value":2}');
			break;
		case 69: // E
			webSocket.send('{"type":1,"value":0}');
			break;
      	case 82: // R
            webSocket.send('{"type":1,"value":1}');
			break;
		case 65: // A
			webSocket.send('{"type":7,"value":0}');
			break;
		case 83: // S
			webSocket.send('{"type":7,"value":0.707}');
			break;
		case 68: // D
			webSocket.send('{"type":7,"value":-0.707}');
			break;
		case 70: // F
			webSocket.send('{"type":11,"value":3}');
			break;

	}
}
