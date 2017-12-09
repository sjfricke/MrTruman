document.addEventListener("keydown", testFunctions, false);

function testFunctions(e) {
	var keyCode = e.keyCode;
	switch(keyCode) {
		case 65: // A
			test0();
			break;
		case 83: // S
			test1();
			break;
		case 68: // D
			test2();
			break;

	}
}
