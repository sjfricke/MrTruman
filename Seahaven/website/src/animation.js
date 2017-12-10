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
    pictureWalk
    speak
    walk
*/
var s_animationOn   = false;
var s_request       = false;

var s_idleMode      = false;
var s_walkX         = 400;
var s_couchOn       = false;
var s_couchLast     = false;
var s_couchAnim     = false;
var s_couchKilling  = false;
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
var s_pictureAnim   = false;
var s_pictureIndex  = 2;

const walkRate = 2;
const fallRatePlayer = 3;
const fallRateCouch = 1.5;
const fallRatePic =  0.02;
const speakerRate = 0.00004;
const picRate = 0.04;

var player;
const pScaleRight = 0.5;
const pScaleLeft = -0.5;

var speaker1, speaker2, speakerStartY;
var couch;
var tiltValue = 0;

var maxScore = 0;

const walkTicker = new PIXI.ticker.Ticker();

// No clean way of doing this then just taking animation and give it to section named
function animationEnd(entry) {
    // log("TEST","TEST", entry.animation.name);
    if (entry.animation.name == "walk") {
        walkComplete();
    } else if (entry.animation.name == "couchOff") {
        s_couchOn = s_couchAnim = s_couchKilling = false;
        if (s_lightAnim) { walk(0,0,525); }
        else if (s_fireAnim) { walk(0,0,680); }
        else if (s_tiltAnim) { }
        else if (s_talkAnim) { talkStart(); }
        else if (s_fidgetAnim) { fidgetStart(); }
        else if (s_pictureAnim) { pictureStart(); }
        else if (s_idleMode) { idleMode(); }
    } else if (entry.animation.name == "speak") {
        document.getElementById("speech").style.visibility = "hidden";
        speechText.style.fontSize = "18px";
        speechText.style.lineHeight = "25px";
        s_talkAnim = s_animationOn = false;
        s_couchOn = s_couchAnim = false; // dirty fix, not solved, patching bug of double speak
        idleMode();
    } else if (entry.animation.name == "fidget") {
        s_fidgetAnim = false;
        if (s_idleMode) { idleMode(); }
    } else if (entry.animation.name == "pictureStart") {
        wsPictureReady();
    } else if (entry.animation.name == "pictureTake") {
        player.state.setAnimation(0, 'pictureWalk', true);
        renderer.app.ticker.add(pictureWalk);
    } else if (entry.animation.name == "pictureChange") {
        s_pictureAnim = s_animationOn = false;                      
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
            idleMode();
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
    } else if (s_fireAnim) {
        player.scale.x = pScaleRight;
        player.state.addAnimation(0, (s_fireOn) ? 'fireOff' : 'fireOn', false, 0);
        (s_fireOn) ? wsFireOffSound() : wsFireOnSound();
    }
     else if (s_speakersAnim) {
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
        if (s_couchLast && Math.random() < 0.7) {
            s_couchOn = true;
	    s_couchLast = true;
            walkTicker.stop();
            couchAnimation(); 
        } else {
	    s_couchLast = false;
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
        // get back to main 3 for idle
        if (player.position.x < 300) {
            walk(0, 0, 200);
        } else if (player.position.x > 500) {
            walk(0, 0, 600);
        } else {
            walk(0, 0, 400);
        }
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
    s_couchKilling = true;
    // player.state.clearTrack(0);
    player.state.setAnimation(0, "couchOff", false, 0);
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
    else {
        s_fidgetAnim = false; // force check
        walk(0,0,525);
    }
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
    else if (s_fidgetAnim) {
        s_idleMode = s_fidgetAnim = false; // force check 
    }
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
    else {
        s_fidgetAnim = false; // force check
        walk(0,0, 680);
    }   
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
    s_pictureAnim = true;
    s_animationOn = true;    
    s_idleMode = false; 

    s_fidgetAnim = false; // force check
    if (s_couchOn) { couchKill(); }
    else {
        walkTicker.stop();
        pictureStart();
    }
}

function pictureStart() {
    player.state.setAnimation(0, "pictureStart", false);
    player.state.addAnimation(0, "pictureHold", false, 0); // purly cause listener won't trigger
}

function pictureFlash() {
    let flash = renderer.getElemByID("flashAnimated");
    flash.position.x = player.position.x + ((player.scale.x > 0) ? 31 : -31);
    flash.alpha = 1;
    flash.gotoAndPlay(0);
}

function pictureTrigger() {
    pictureChange();
    renderer.getElemByID("flashAnimated").alpha = 0;
    player.state.setAnimation(0, "pictureTake", false);
    player.state.addAnimation(0, "stand", false, 0);
    player.scale.x = (player.position.x - 275 < 0) ?  pScaleRight : pScaleLeft;
}    
 

function pictureWalk(delta) {
    player.position.x += walkRate * delta * (player.scale.x > 0 ? 1 : -1);

    if (Math.abs(player.position.x - 275) < 5) {
        player.position.x = 275;
        renderer.app.ticker.remove(pictureWalk);
        pictureReplace()
    }
}

function pictureChange() { 

    (new PIXI.loaders.Loader()).add('newPicture', resPath.cameraImage + s_pictureIndex + ".jpg").
        load(function (loader, res) {
            newPicture.texture = res.newPicture.texture;
            newPicture.position.x = 235;
            newPicture.position.y = 322;
            newPicture.scale.x = newPicture.scale.y = .16;
        });
}

function pictureReplace() {
    player.scale.x = pScaleLeft;
    player.state.setAnimation(0, "pictureChange", false);
    setTimeout(function(){
        newPicture.alpha = 1;
        renderer.app.ticker.add(pictureScale);
    }, 1000);
    player.state.addAnimation(0, "stand", false, 0);
}

function pictureScale(delta) {
    if (newPicture.scale.x < .57 && newPicture.position.y > 260) {
        newPicture.scale.x = newPicture.scale.y += .29 * picRate;
        newPicture.position.y -= 22 * picRate;
        newPicture.position.x -= 12 * picRate;
    } else {
        newPicture.alpha = 0;
        picture.texture = newPicture.texture;  
        renderer.app.ticker.remove(pictureScale);
    }
}

/*************************
*      Tilt/Gyro         *
*************************/
function tiltAnimation() {
    s_tiltAnim = s_animationOn = true;
    s_tiltWall = s_tiltWallCouch = false;   

    s_fidgetAnim = false;
    s_idleMode = false;
    if (s_couchOn) { couchKill(); }
    walkTicker.stop();
   
    player.scale.x = s_tiltRight ? pScaleLeft : pScaleRight;
    player.state.setAnimation(0, "fallStart", false, 0);
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
            s_tiltWall = false;
        } else if (!s_tiltWall) {
            wsTiltTrumanWall();
            s_tiltWall = true;
            player.position.x = 720;
            player.state.setAnimation(0, "fallWall", false);
        }

        // Couch
        if (couch.position.x < 687) {
            couch.position.x += fallRateCouch * delta;
            s_tiltWallCouch = false;
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
            s_tiltWall = false;
        } else if (!s_tiltWall) {
            wsTiltTrumanWall();
            s_tiltWall = true;
            player.position.x = 80;
            player.state.setAnimation(0, "fallWall", false);
        }

        // Couch
        if (couch.position.x > 114) {
            couch.position.x -= fallRateCouch * delta;
            s_tiltWallCouch = false;
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
    
    // update score
    if (s_tiltGame) {
        // punish player for lots of tilts
        gameScore += Math.abs(startX - player.position.x) / 100;
        maxScore = Math.max(gameScore, maxScore);

        if (s_tiltWall == true || s_tiltWallCouch == true) {
            oppTiltCnt = 0; // reset score
            gameScore = 0.000;
            nestTemp.innerHTML = "00";
        }
        gameScoreEl.style.display = "inherit";
        gameScoreEl.innerHTML = "Score: " + gameScore.toFixed(3);
        gameHighScore.innerHTML = "High Score: " + maxScore.toFixed(3);
    }
}

function tiltRecovery() {
    renderer.app.ticker.remove(tiltFall);
    player.state.setAnimation(0, "fallEnd", false);
    player.state.addAnimation(0, "stand", false, 0);

    // reset furniture
    couch.position.x = 200;
    picture.rotation = frame.rotation = 0;

    // reset game nest_temp goes back to 88 degrees    
    oppTiltCnt = 0;
    s_tiltRightLast = null;
    startX = 0;
    gameScore = 0;
    nestTemp.style.left = "3.98%";
    nestTemp.innerHTML = "88&#176;";
    gameScoreEl.style.display = "none";
    gameScoreLose.style.display = "none";
    gameHighScore.style.display = "none";
    s_tiltGame = false;

    // update score
    wsUpdateHighScore(maxScore.toFixed(3));
}

// used to punish quick tilters
function tiltReduceScore() {
    clearTimeout(gameScoreLoseTime);
    gameScoreLoseAmt = Math.min(50, (oppTiltCnt-1) * 10);
    gameScore -= gameScoreLoseAmt;
    gameScoreLose.style.display = "inherit";
    gameScoreLose.classList.add("move");
    gameScoreLose.innerHTML = "-" + gameScoreLoseAmt + " points";
    gameScoreLoseTime = setTimeout(function(){
        gameScoreLose.style.display = "none";
        gameScoreLose.classList.remove("move");
    }
    , 1000);
}

/*************************
*       Fidget           *
*************************/
function fidgetAnimation() {
    s_fidgetAnim = true;
    if (s_couchOn) { 
        couchKill();
    } else {
        // walking
        walkTicker.stop();
        fidgetStart(); 
    }
}

function fidgetStart() {
    player.state.setAnimation(0, "fidget", true, 0);
}

function fidgetKill() {
    if (s_fidgetAnim) {
	player.state.addAnimation(0, "stand", false, 0);
    }
}

/*************************
*          Talk          *
*************************/
function talkAnimation() {
    s_talkAnim = true;

    if (s_couchOn) { 
        couchKill();
    } else if (s_fidgetAnim) { 
        s_fidgetAnim = false; // force check
        talkStart();
    } else { // in walk
        s_fidgetAnim = false; // force check
        walkTicker.stop();
        talkStart();
    }
}

function talkStart() {
    s_animationOn = true;

    player.scale.x = (player.position.x < 400) ?  pScaleRight : pScaleLeft;

    player.state.setAnimation(0, "speak", true, 0);
    
    wsChatReady(); // Might move if timing is off stiil

    document.getElementById("speechImg").style.transform = "scaleX("+ ((player.scale.x > 0) ? 1 : -1) + ")";
    document.getElementById("speechImg").style.left = ((player.position.x) + ((player.scale.x > 0) ? 0 : -200)) + "px"
    speechText.style.left = ((player.position.x) + ((player.scale.x > 0) ? 6 : -194)) + "px"
    document.getElementById("speech").style.visibility = "visible";
}

function talkKill() {
    player.state.setAnimation(0, "stand", false, 0);
}
