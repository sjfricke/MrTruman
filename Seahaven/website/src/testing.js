document.addEventListener("keydown", testFunctions, false);

function testFunctions(e) {
	var keyCode = e.keyCode;
	switch(keyCode) {
		case 81: // Q
			wsTurnLightsOn();
			break;
		case 87: // W
			wsTurnLightsOff();
			break;
		case 69: // E
			break;
          	case 82: // R
	                animateRain();
			break;
		case 65: // A
			startLightOnAnimation();
			break;
		case 83: // S
			startLightOffAnimation();
			break;
		case 68: // D
			break;
		case 70: // F
			break;

	}
}
