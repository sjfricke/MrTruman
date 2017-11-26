/*
    changePicture
    couchIdle
    couchJump
    couchOff
    fallEnd
    fallStart
    fallWall
    fidget
    fireOff
    fireOn
    jump
    lightSwitch
    musicOff
    musicOn
    musicPlay
    pictureStart
    pictureTake
    speak
    walk
*/
var s_animationOn   = false;
var s_request       = false;

var s_idleMode      = false;
var s_walkX         = 400;
var s_couchOn       = false;
var s_fireOn        = false;
var s_speakersUp    = false;
var s_speakersOn    = false;
var s_speakersAnim  = false;
var s_lightOn       = false; // default to lights on house off
var s_lightAnim     = false;
var s_tiltOn        = false;
var s_fidgetOn      = false;
var s_speakOn       = false;
var s_pictureUp     = false;
var s_pictureOn     = false;
var s_playerRight   = true;

const walkRate = 2;
const speakerRate = 0.00004;
var speaker1, speaker2, speakerStartY;

var player;
const pScaleRight = 0.5;
const pScaleLeft = -0.5;

const walkTicker = new PIXI.ticker.Ticker();

// No clean way of doing this then just taking animation and give it to section named
function animationEnd(entry) {
    if (entry.animation.name == "walk") {
        walkComplete();
    } else if (entry.animation.name == "couchOff") {
        s_couchOn = false;
        if (s_idleMode) { idleMode(); }
        // else if ()
    } else if (entry.animation.name == "musicOff") {
        s_speakersAnim = false;
        s_animationOn = false;
        idleMode();
    } else if (entry.animation.name == "lightSwitch") {
        s_lightAnim = false;
        s_animationOn = false;
        idleMode();
    }
}

/*************************
*          Walk          *
*************************/

// direction [ -1 == left, 1 == right, 0 == depends on toPos]
// distance to walk, 0 == ignore
// toPos is x distance to walk too
function walk(direction, distance, toPos) {

    if (distance > 0) {
        player.scale.x = (direction > 0) ?  pScaleRight : pScaleLeft;
        s_walkX = player.position.x + (distance * (direction > 0 ? 1 : -1));
    } else {
        player.scale.x = (player.position.x - toPos < 0) ?  pScaleRight : pScaleLeft;
        s_walkX = toPos;
    }

    if (player.state.tracks.length == 0 || 
        ( player.state.tracks[0] == null || player.state.tracks[0].animation.name != "walk")) {
        player.state.setAnimation(0, 'walk', true, 0); 
        walkTicker.start();    
    }
}

function walkAnimation(delta) {

    player.position.x += walkRate * delta * (player.scale.x > 0 ? 1 : -1);

    if (Math.abs(player.position.x - s_walkX) < 5) {
        // This is done here instead of callback of animation since
        // Idle can't start and stop ticker and no callback of ticker seemed to work
        if (s_idleMode) {            
            player.position.x = s_walkX;        
            player.state.clearTrack(0);
            player.state.addAnimation(0, 'stand', false, 0);  
            if (s_couchOn) { 
                walkTicker.stop();
                couchAnimation(); 
            } else {
                idleMode();
            }
        } else {
            // ugly, I know, timer and animation event callback can't synch
            walkTicker.stop();
            player.position.x = s_walkX;        
            player.state.clearTrack(0);
            // certain animation don't want the stand trasmission
            if (!s_speakersAnim) {
                player.state.addAnimation(0, 'stand', false, 0); 
            }
        }           
    }
}

function walkComplete() {
    if (s_lightAnim) {
        player.scale.x = pScaleRight;
        player.state.addAnimation(0, 'lightSwitch', false, 0);  
        toggleLightSwitch();
    } else if (s_speakersAnim) {
        speakerAnimation();
    } else if (s_idleMode) {
        //idleMode();
    }
}
/*************************
*          Idle          *
*************************/
function idleMode() {
    s_idleMode = true;

    // logic to move around randomly
    if (player.position.x == 200) {
        if (Math.random() < 0.6) {
            s_couchOn = true;
            walk(0, 0, 175);
        } else { 
            walk(1, 200);
        }
    }
    else if (player.position.x == 400) {
        Math.random() < 0.5 ? walk(1, 200) : walk(-1, 200);
    }
    else if (player.position.x == 600) {
        walk(-1, 200);
    }
    else {
        // get back to middle for idle
        walk(0, 0, 400);
    }
}

function couchAnimation() {
    player.scale.x = pScaleRight;
    player.state.addAnimation(0, "couchJump", false, 0);
    player.state.addAnimation(0, "couchIdle", false, 0);
    player.state.addAnimation(0, "couchIdle", false, 0);
    player.state.addAnimation(0, "couchOff", false, 0);
    player.state.addAnimation(0, "stand", false, 0);
}

function couchKill() {      
    player.state.clearTrack(0);
    player.state.addAnimation(0, "couchOff", false, 0);
    player.state.addAnimation(0, "stand", false, 0);
}

/*************************
*         Lights         *
*************************/
function lightAnimation() {
    s_idleMode = false;
    s_animationOn = true;
    s_lightAnim = true;
    if (s_couchOn) { couchKill(); }
    walk(0,0,525);
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

/*************************
*       Speakers         *
*************************/
function speakerAnimation() {
    s_speakersAnim = true;    
    s_animationOn = true;   
    if (s_idleMode) {
        s_idleMode = false;
        return; // need to finish walk animation to prevent stuck
    }
    s_idleMode = false; 
    if (s_couchOn) { couchKill(); }
    
    // get out of speakers
    if (player.position.x < 100 ) {
        walk(0,0,150);
    } else if (player.position.x > 300 && player.position.x < 420) {
        walk(0,0,250);
    } else {
        if (!s_speakersUp) {
            speakersOff(); // be safe, no guarntee
            renderer.app.ticker.add(speakersUp);
            player.state.setAnimation(0, "musicOn", false);
            player.state.addAnimation(0, "musicPlay", true, 0);            
        } else {
            renderer.app.ticker.add(speakersDown);
            player.state.addAnimation(0, "musicOff", false, 0);
            player.state.addAnimation(0, 'stand', false, 0); 
        }
        
    }
}

function speakersUp(delta) {
    if (speaker1.position.y > window.outerHeight) {
        speaker1.position.y = speaker2.position.y -= speakerRate * speaker1.position.y * speaker1.position.y;
    } else {
        renderer.app.ticker.remove(speakersUp);
        speaker1.position.y = speaker2.position.y = window.outerHeight;
        s_speakersUp = true;
        wsSpeakersUp();
    }
}

function speakersOn() {
    speaker1.play();
    speaker2.play();
    s_speakersOn = true;
}

function speakersOff() {
	speaker1.gotoAndStop(0);
    speaker2.gotoAndStop(0);
    s_speakersOn = false;
}

function speakersDown(delta) {
    if (speaker1.position.y < speakerStartY) {
        speaker1.position.y = speaker2.position.y += speakerRate * speaker1.position.y * speaker1.position.y;
    } else {
        renderer.app.ticker.remove(speakersDown);
        speaker1.position.y = speaker2.position.y = speakerStartY;
        s_speakersUp = false;
        wsSpeakersDown();
    }
}

/*************************
*          Fire          *
*************************/

function fireOn() {
    // at x = 655
    renderer.app.stage.addChild(renderer.getElemByID("fireAnimated"));
    renderer.getElemByID("fireAnimated").gotoAndPlay(0);
}

function fireOff() {
    // at x = 650
    renderer.getElemByID("fireAnimated").stop();
    renderer.app.stage.removeChild(renderer.getElemByID("fireAnimated"));
}

/*************************
*       Picture          *
*************************/
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


/*************************
*      Tilt/Gyro         *
*************************/

// Falling +X at x=15 then x=35
// Falling -X at x=785 then x=765


/*************************
*       Fidget           *
*************************/

/*************************
*         Wall           *
*************************/
function changeWall() {
    let wall = renderer.getElemByID("wall" + startingWall);

    currentWall++;
    if (currentWall > 5) { currentWall = 0 }

    wall.texture = renderer.textures["wall" + currentWall];
}
