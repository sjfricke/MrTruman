var speaker1Ready = false;
var speaker2Ready = false;
var speaker1StartY = 0;
var speaker2StartY = 0;

var speakerRate = 0.00004;

var playerMovingForward = false;
var playerMovingBackward = false;

// renderer.app.ticker.add(speakersOn);
function speakersOn(delta) {

	let speaker1 = renderer.getElemByID('speaker1');
    let speaker2 = renderer.getElemByID('speaker2');

    if (speaker1Ready && speaker2Ready) {
		speaker1.play();
		speaker2.play();
		log("animation", "done with speaker on");
	        renderer.app.ticker.remove(speakersOn);
	wsTurnSpeakersOn();
	}

    if (speaker1.position.y > window.outerHeight) {
        speaker1.position.y -= speakerRate * speaker1.position.y * speaker1.position.y;
    } else {
    	speaker1Ready = true;
    	speaker1.position.y = window.outerHeight;
    }

    if (speaker2.position.y > window.outerHeight) {
        speaker2.position.y -= speakerRate * speaker2.position.y * speaker2.position.y;
    }
    else { 
    	speaker2Ready = true;
    	speaker2.position.y = window.outerHeight ;       
    }
}

// renderer.app.ticker.add(speakersOff);
function speakersOff() {
	let speaker1 = renderer.getElemByID('speaker1');
    let speaker2 = renderer.getElemByID('speaker2');

	speaker1.gotoAndStop(0);
    speaker2.gotoAndStop(0);

	if (!speaker1Ready && !speaker2Ready) {
		log("animation", "done with speaker off");
		renderer.app.ticker.remove(speakersOff);
	}

    if (speaker1.position.y < speaker1StartY) {
        speaker1.position.y += speakerRate * speaker1.position.y * speaker1.position.y;
    } else {
    	speaker1Ready = false;
    	speaker1.position.y = speaker1StartY;
    }

    if (speaker2.position.y < speaker2StartY) {
        speaker2.position.y += speakerRate * speaker2.position.y * speaker2.position.y;
    }
    else { 
    	speaker2Ready = false;
    	speaker2.position.y = speaker2StartY;       
    }
}

function fireOn() {
    renderer.app.stage.addChild(renderer.getElemByID("fireAnimated"));
    renderer.getElemByID("fireAnimated").gotoAndPlay(0);
}

function fireOff() {
    renderer.getElemByID("fireAnimated").stop();
    renderer.app.stage.removeChild(renderer.getElemByID("fireAnimated"));
}


function animateSpineBoy() {
    if (playerMovingForward == true || playerMovingBackward == true) {
        // Currently performing action, so cannot stop
        return;
    }
    playerMovingForward = true;
    renderer.getElemByID('spineboy').state.addAnimation(0, 'walk', true, 0);
    renderer.app.ticker.add(walkSpineBoy);
}

function walkSpineBoy(delta) {

    let player = renderer.getElemByID('spineboy');
    let lightSwitch = renderer.getElemByID('switch');

    if (playerMovingForward) {
        player.position.x += 2 * delta;
        if (Math.abs(player.position.x - lightSwitch.position.x) < 20) {
            playerMovingForward = false;
            player.scale.x = -player.scale.x;
            playerMovingBackward = true;
            toggleLightSwitch();
        }
    }
    if (playerMovingBackward) {
        player.position.x -= 2 * delta;
        if (player.position.x < 150) {
            playerMovingBackward = false;
            player.scale.x = -player.scale.x;
            player.state.addAnimation(0, 'walk', false, 0);
            renderer.app.ticker.remove(walkSpineBoy);
        }
    }
}

function toggleLightSwitch() {
    let lightSwitch = renderer.getElemByID('switch');
    renderer.editorFilter.uniforms.mode = renderer.editorFilter.uniforms.mode ^ 0x1;
    if (renderer.editorFilter.uniforms.mode == 0) {
        lightOnTexture = lightSwitch.texture;
        lightSwitch.texture = lightOffTexture;
        wsTurnLightsOff();
    }
    else {
        lightOffTexture = lightSwitch.texture;
        lightSwitch.texture = lightOnTexture;
        wsTurnLightsOn();
    }
}

function getNewPhoto() { 
    renderer.app.stage.removeChild(renderer.getElemByID("picture"));
    delete renderer.elems["picture"];

    renderer.add({
            name: 'picture',
            path: resPath.cameraImage,
            pt: new PIXI.Point(0.25, 0.56),
            scale: 1.0
        }, function() {
            renderer.app.stage.addChild(renderer.getElemByID("picture"));
        })
}

function changeWall() {
    let wall = renderer.getElemByID("wall" + startingWall);

    currentWall++;
    if (currentWall > 5) { currentWall = 0 }

    wall.texture = renderer.textures["wall" + currentWall];
}