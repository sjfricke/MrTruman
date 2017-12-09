/*    
    couchIdle
    couchJump
    couchOff
    fallEnd
    fallHold
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
    pictureChange
    pictureHold
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
var s_couchAnim     = false;
var s_fireOn        = false;
var s_fireAnim      = false;
var s_speakersUp    = false;
var s_speakersOn    = false;
var s_speakersAnim  = false;
var s_lightOn       = false; // default to lights on house off
var s_lightAnim     = false;
var s_tiltRight     = false;
var s_tiltRightLast = false;
var s_tiltWall      = false;
var s_tiltWallCouch = false;
var s_tiltAnim      = false;
var s_tiltGame      = false;
var s_fidgetAnim    = false;
var s_talkAnim      = false;
var s_pictureUp     = false;
var s_pictureTaken  = false;
var s_pictureAnim   = false;
var s_pictureIndex  = 2;

const walkRate = 2;
const fallRatePlayer = 3;
const fallRateCouch = 1.5;
const fallRatePic =  0.02;
const speakerRate = 0.00004;

var player;
const pScaleRight = 0.5;
const pScaleLeft = -0.5;

var speaker1, speaker2, speakerStartY;
var couch;
var tiltValue = 0;

const walkTicker = new PIXI.ticker.Ticker();

// No clean way of doing this then just taking animation and give it to section named
function animationEnd(entry) {
    // log("TEST","TEST", entry.animation.name);
    if (entry.animation.name == "walk") {
        walkComplete();
    } else if (entry.animation.name == "couchOff") {
        s_couchOn = s_couchAnim = false;
        if (s_idleMode && !s_fidgetAnim && !s_talkAnim) { idleMode(); }
    } else if (entry.animation.name == "speak") {
        document.getElementById("speech").style.visibility = "hidden";
        s_talkAnim = false;
        if (s_idleMode && !s_fidgetAnim) { idleMode(); }
    } else if (entry.animation.name == "fidget") {
        s_fidgetAnim = false;
        if (s_idleMode && !s_talkAnim) { idleMode(); }
    } else if (entry.animation.name == "pictureStart") {
        s_pictureUp = true;
        wsPictureReady();
    } else if (entry.animation.name == "pictureTake") {
        pictureTrigger();
    } else if (entry.animation.name == "pictureChange") {
        s_pictureAnim = s_animationOn = false;
        pictureChange();
        wsPictureDone();
        idleMode();
    } else if (entry.animation.name == "fireOn") {
        fireOn();
        s_fireAnim = s_animationOn = false;
        idleMode();
    } else if (entry.animation.name == "fireOff") {
        fireOff();
        s_fireAnim = s_animationOn = false;
        idleMode();
    } else if (entry.animation.name == "musicOff") {
        s_speakersAnim = s_animationOn = false;
        idleMode();
    } else if (entry.animation.name == "lightSwitch") {
        s_lightAnim = s_animationOn = false;
        idleMode();
    } else if (entry.animation.name == "fallEnd") {
        s_tiltAnim = s_animationOn = false;
        wsTiltDone();
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
            if (s_fidgetAnim) {
                walkTicker.stop();
                fidgetStart(); // fidget only happens out of idle mode 
            } else if (s_talkAnim) {
                walkTicker.stop();
                talkStart(); // talk only happens out of idle mode 
            }
            else {
                idleMode();
            }
        } else {
            // ugly, I know, timer and animation event callback can't synch
            walkTicker.stop();
            player.position.x = s_walkX;        
            player.state.clearTrack(0);
            // certain animation don't want the stand trasmission
            if (!s_speakersAnim && !s_pictureAnim && !s_tiltAnim) {
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
    } else if (s_fireAnim) {
        player.scale.x = pScaleRight;
        player.state.addAnimation(0, (s_fireOn) ? 'fireOff' : 'fireOn', false, 0);
        (s_fireOn) ? wsFireOffSound() : wsFireOnSound();
    } else if (s_pictureAnim) {
        (s_pictureTaken) ? pictureTrigger() : pictureAnimation();
    } else if (s_tiltAnim) {
        tiltAnimation();
    } else if (s_speakersAnim) {
        speakerAnimation();
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
            walkTicker.stop();
            couchAnimation(); 
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
    s_couchAnim = true;
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
    else if (s_fidgetAnim) { fidgetKill(); }
    else if (s_talkAnim) { talkKill(); }
    walk(0,0,525);
}

function toggleLightSwitch() {
    let lightSwitch = renderer.getElemByID('switch');
    renderer.getElemByID("wall").alpha = s_lightOn ? .65 : 1;
    if (s_lightOn) {
        lightOnTexture = lightSwitch.texture;
        lightSwitch.texture = lightOffTexture;
        s_lightOn = false;
        wsTurnLightsOff();
    }
    else {
        lightOffTexture = lightSwitch.texture;
        lightSwitch.texture = lightOnTexture;
        s_lightOn = true;
        wsTurnLightsOn();
    }
}

/*************************
*       Speakers         *
*************************/
function speakerAnimation() {
    s_speakersAnim = true;    
    s_animationOn = true;   
    if (s_couchOn) { couchKill(); }
    else if (s_fidgetAnim) { fidgetKill(); }
    else if (s_talkAnim) { talkKill(); }
    else if (s_idleMode) {
        s_idleMode = false;
        return; // need to finish walk animation to prevent stuck
    }
    s_idleMode = false; 
        
    // get out of speakers
    if (player.position.x <= 130 ) {
        walk(0,0,150);
    } else if (player.position.x >= 325 && player.position.x < 375) {
        walk(0,0,300);
    } else if (player.position.x >= 375 && player.position.x <= 425) {
        walk(0,0,450);
    } else {
	wsSpeakersReady();
        if (!s_speakersUp) {
            speakersOff(); // be safe, no guarntee
            renderer.app.ticker.add(speakersUp);
            player.state.addAnimation(0, "musicOn", false, 0);
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
function fireAnimation() {
    s_idleMode = false;
    s_animationOn = true;
    s_fireAnim = true;
    if (s_couchOn) { couchKill(); }
    else if (s_fidgetAnim) { fidgetKill(); }
    else if (s_talkAnim) { talkKill(); }
    walk(0,0, 680);
}

function fireOn() {
    renderer.getElemByID("fireAnimated").gotoAndPlay(0);    
    renderer.getElemByID("fireAnimated").alpha = 1;
    s_fireOn = true;
    wsFireOn();
}

function fireOff() {
    renderer.getElemByID("fireAnimated").stop();
    renderer.getElemByID("fireAnimated").alpha = 0;
    s_fireOn = false;
    wsFireOff();
}

/*************************
*       Picture          *
*************************/
function pictureAnimation() {
    s_pictureTaken = false; // need since picture has two post-walk animations
    s_pictureAnim = true;
    s_animationOn = true;   

    if (s_couchOn) { couchKill(); }
    else if (s_fidgetAnim) { fidgetKill(); }
    else if (s_talkAnim) { talkKill(); }
    else if (s_idleMode) {
        s_idleMode = false;
        return; // need to finish walk animation to prevent stuck
    }
    s_idleMode = false; 

    player.state.addAnimation(0, "pictureStart", false, 0);
    player.state.addAnimation(0, "pictureHold", false, 0); // purly cause listener won't trigger
}

function pictureFlash() {
    let flash = renderer.getElemByID("flashAnimated");
    flash.position.x = player.position.x + ((player.scale.x > 0) ? 31 : -31);
    flash.alpha = 1;
    flash.gotoAndPlay(0);

}

function pictureTrigger() {
    s_pictureTaken = true;

    if (s_pictureUp) {        
        s_pictureUp = false;
        renderer.getElemByID("flashAnimated").alpha = 0;
        player.state.setAnimation(0, "pictureTake", false);
        player.state.addAnimation(0, "stand", false, 0);
        return;
    }

    if (player.position.x < 250 || player.position.x > 300) {
        walk(0,0,275);
        return;
    }

    player.scale.x = pScaleLeft;
    player.state.setAnimation(0, "pictureChange", false);
    player.state.addAnimation(0, "stand", false, 0);
}

function pictureChange() { 

    (new PIXI.loaders.Loader()).add('newPicture', resPath.cameraImage + s_pictureIndex + ".jpg").
        load(function (loader, res) {
            picture.texture = res.newPicture.texture;
        });
}

/*************************
*      Tilt/Gyro         *
*************************/
function tiltAnimation() {
    s_tiltAnim = true;
    s_animationOn = true;
    s_tiltWall = s_tiltWallCouch = false;   

    if (s_couchOn) { couchKill(); }
    else if (s_fidgetAnim) { fidgetKill(); }  
    else if (s_talkAnim) { talkKill(); }
    else if (s_idleMode) {
        s_idleMode = false;
        return; // need to finish walk animation to prevent stuck
    }
    s_idleMode = false; 
    player.scale.x = s_tiltRight ? pScaleLeft : pScaleRight;
    player.state.addAnimation(0, "fallStart", false, 0);
    // setTimeout(function(){ renderer.app.ticker.add(tiltFall);}, 300);
    renderer.app.ticker.add(tiltFall);
}

// rather have a single boolean check per frame then a change direction fucntion
// beause that still needs a boolean check to know if there was a change... this works below well
function tiltFall(delta) {

    if (s_tiltRight) {
        // Truman
        player.scale.x = pScaleLeft;
        if (player.position.x < 720) {
            player.position.x += fallRatePlayer * delta;
        } else if (!s_tiltWall) {
            wsTiltTrumanWall();
            s_tiltWall = true;
            player.position.x = 720;
            player.state.setAnimation(0, "fallWall", false);
        }

        // Couch
        if (couch.position.x < 687) {
            couch.position.x += fallRateCouch * delta;
        } else if (!s_tiltWallCouch) {
            wsTiltCouchWall();
            s_tiltWallCouch = true;
            couch.position.x = 687;
        }

        // Picture Frame
        if (picture.rotation > -0.707) {
            picture.rotation = frame.rotation -= fallRatePic * delta;
        } else {
            picture.rotation = frame.rotation = -0.707;
        }

    } else {
        // Truman
        player.scale.x = pScaleRight;
        if (player.position.x > 80) {
            player.position.x -= fallRatePlayer * delta;
        } else if (!s_tiltWall) {
            wsTiltTrumanWall();
            s_tiltWall = true;
            player.position.x = 80;
            player.state.setAnimation(0, "fallWall", false);
        }

        // Couch
        if (couch.position.x > 114) {
            couch.position.x -= fallRateCouch * delta;
        } else if (!s_tiltWallCouch) {
            wsTiltCouchWall();
            s_tiltWallCouch = true;
            couch.position.x = 114;
        }

        // Picture Frame
        if (picture.rotation < 0.707) {
            picture.rotation = frame.rotation += fallRatePic * delta;
        } else {
            picture.rotation = frame.rotation = 0.707;
        }
    }
    // punish player for lots of tilts
    gameScore += Math.abs(startX - player.position.x)/(10*oppTiltCnt);

    if (s_tiltWall == true || s_tiltWallCouch == true) {
        oppTiltCnt = 0; // reset score
        gameScore = 0.000;
        nestTemp.innerHTML = "00";
    }
    // update score
    if (s_tiltGame) {
        gameScoreEl.innerHTML = "Score: " + gameScore.toFixed(3);
    }
}

function tiltRecovery() {
    //renderer.app.ticker.remove(s_tiltRight ? tiltFallRight : tiltFallLeft);
    renderer.app.ticker.remove(tiltFall);
    player.state.setAnimation(0, "fallEnd", false);
    player.state.addAnimation(0, "stand", false, 0);

    couch.position.x = 200;

    picture.rotation = frame.rotation = 0;

    // reset game nest_temp goes back to 88 degrees    
    oppTiltCnt = 0;
    prevTiltDir = null;
    startX = 0;
    nestTemp.style.left = "3.98%";
    nestTemp.innerHTML = "88&#176;";
    gameScoreEl.innerHTML = "";
    s_tiltGame = false;
}

/*************************
*       Fidget           *
*************************/
function fidgetAnimation() {
    s_fidgetAnim = true;
    if (s_couchOn) { 
        couchKill();
        fidgetStart();
    } else if (s_talkAnim) { 
        talkKill();
        fidgetStart();
    }
    // else walk animation will catch trigger
}

function fidgetStart() {
    player.state.addAnimation(0, "fidget", true, 0);
}

function fidgetKill() {
    player.state.addAnimation(0, "stand", false, 0);
}

/*************************
*          Talk          *
*************************/
function talkAnimation() {
    s_talkAnim = true;

    if (s_couchOn) { 
        couchKill();
        talkStart();
    } else if (s_fidgetAnim) { 
        fidgetKill();
        talkStart();
    }
    // else walk animation will catch trigger
}

function talkStart() {
    player.scale.x = (player.position.x < 400) ?  pScaleRight : pScaleLeft;

    player.state.addAnimation(0, "speak", true, 0);

    document.getElementById("speechImg").style.transform = "scaleX("+ ((player.scale.x > 0) ? 1 : -1) + ")";
    document.getElementById("speechImg").style.left = ((player.position.x) + ((player.scale.x > 0) ? 0 : -200)) + "px"
    document.getElementById("speechText").style.left = ((player.position.x) + ((player.scale.x > 0) ? 6 : -194)) + "px"
    document.getElementById("speech").style.visibility = "visible";

    setTimeout(function(){ if (s_talkAnim) {talkKill();} }, 3000);
}

function talkKill() {
    player.state.addAnimation(0, "stand", false, 0);
}

/*************************
*         Wall           *
*************************/
function changeWall() {
    let wall = renderer.getElemByID("wall");

    currentWall++;
    if (currentWall > 4) { currentWall = 0 }

    wall.texture = wallTextures[currentWall];
}
