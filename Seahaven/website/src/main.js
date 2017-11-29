/**************************************************
*    HARD Assumption screen is 800 x 480          *
***************************************************/
$(window).ready(start());

var renderer;
var texLoaded = 0;
var currentWall, startingWall;
var appData;

// textures
var lightOnTexture, lightOffTexture;
var wallTextures = new Array(5);

function start() {
    setWebSocket();
    setup();
}

function setWebSocket() {
    // Attempts to just reload webpage if it was not able to get websocket
    // Will cause loop if not connect, but app is useless anyways without WS
    try {
	   webSocket = new WebSocket('ws://' + location.host);
	   webSocket.onmessage = wsOnMessage;
    } catch (e) {
	   location.reload();
    }
}

function setup() {
    renderer = new Renderer();

    setMenuEvents();
    
    // pick random texture between 0 and 6
    currentWall = Math.round(Math.random()*5);
    startingWall = currentWall;

    (new PIXI.loaders.Loader()).add('lightOn', resPath.lightOn).
        load(function (loader, res) { lightOnTexture = res.lightOn.texture; });

    appData = [
        {
            type: 'addTexture',
            name: 'wall0',
            path: resPath.wall0,
            pt: new PIXI.Point(0, 0),
            tile: (0 == currentWall) ? true : false,
            hide: (0 == currentWall) ? false : true
        },
        {
            type: 'addTexture',
            name: 'wall1',
            path: resPath.wall1,
            pt: new PIXI.Point(0, 0),
            tile: (1 == currentWall) ? true : false,
            hide: (1 == currentWall) ? false : true
        },
        {
            type: 'addTexture',
            name: 'wall2',
            path: resPath.wall2,
            pt: new PIXI.Point(0, 0),
            tile: (2 == currentWall) ? true : false,
            hide: (2 == currentWall) ? false : true
        },
        {
            type: 'addTexture',
            name: 'wall3',
            path: resPath.wall3,
            pt: new PIXI.Point(0, 0),
            tile: (3 == currentWall) ? true : false,
            hide: (3 == currentWall) ? false : true
        },
        {
            type: 'addTexture',
            name: 'wall4',
            path: resPath.wall4,
            pt: new PIXI.Point(0, 0),
            tile: (4 == currentWall) ? true : false,
            hide: (4 == currentWall) ? false : true
        },
        {
            type: 'addTexture',
            name: 'wall5',
            path: resPath.wall5,
            pt: new PIXI.Point(0, 0),
            tile: (5 == currentWall) ? true : false,
            hide: (5 == currentWall) ? false : true
        },
        {
            type: 'addTile',
            name: 'baseboard',
            path: resPath.baseboard,
            pt: new PIXI.Point(0, .948),
            height: 25
        },
        {
            type: 'add',
            name: 'scene',
            path: resPath.scene,
            pt: new PIXI.Point(0.65, 0.62),
            scale: 1.1
        },
        {
            type: 'add',
            name: 'window',
            path: resPath.window,
            pt: new PIXI.Point(0.65, 0.62),
            scale: 1.1
        },
        {
            type: 'add',
            name: 'switch',
            path: resPath.lightOff,
            pt: new PIXI.Point(0.705, 0.854)
        },
        {
            type: 'add',
            name: 'picture',
            path: resPath.picture,
            pt: new PIXI.Point(0.25, 0.56),
            scale: 1.0
        },
        {
            type: 'add',
            name: 'frame',
            path: resPath.frame,
            pt: new PIXI.Point(0.25, 0.6),
            scale: 1.0
        },        
        {
            type: 'add',
            name: 'firePlace',
            path: resPath.firePlace,
            pt: new PIXI.Point(0.88, 1),
            scale: 1.0
        },
        {           
            type: 'addSpritesheet',
            name: 'fireAnimated',
            path: resPath.fireAnimated,
            pt: new PIXI.Point(0.878, 0.943),
            count: 15,
            framePrefix : "fire",            
            loop: true,  
            start: false,
            hideAlpha: true,
            scale: 0.8
        },
        {
            type: 'add',
            name: 'couch',
            path: resPath.couch,
            pt: new PIXI.Point(0.25, 1),
            scale: 1.5
        },
        {
            type: 'addSpritesheet',
            name: 'speaker1',
            path: resPath.speakerAnimated,
            pt: new PIXI.Point(0.45, 1.5),
            count: 6,
            loop: true,  
            framePrefix : "speaker1",
            scale: 0.75,
            start : false
        },
        {
            type: 'addSpritesheet',
            name: 'speaker2',
            path: resPath.speakerAnimated,
            pt: new PIXI.Point(0.05, 1.5),
            count: 6,
            loop: true,            
            framePrefix : "speaker1",
            scale: 0.75,
            start : false
        },	        
        {
            type: 'addSpine',
            name: 'spineboy',
            path: 'res/json/spineboy.json',
            pt: new PIXI.Point(renderer.getW() / 2, renderer.getH()),
            scale: pScaleRight
        },
        {
            type: 'add',
            name: 'speechBubble',
            path: resPath.speechBubble,
            pt: new PIXI.Point(0.5, 0.75),
            anchorX : 0.0,
            hideAlpha: true
        },
        {           
            type: 'addSpritesheet',
            name: 'flashAnimated',
            path: resPath.flashAnimated,
            pt: new PIXI.Point(0.5, 0.87),
            count: 8,
            framePrefix : "flash",
            start: false,
            loop: false,
            speed: .5,
            hideAlpha: true,
            scale: 2
        },
    ];

    // Load all app data async
    for (var i = 0; i < appData.length; i++) {
        renderer[appData[i].type](appData[i], loadedTex);
    }
}

function loadedTex() {
    // onload
    texLoaded++;
    if (texLoaded >= appData.length) loadedSetup();
}

// Setup that needs to be done after loading
function loadedSetup() {

    // only doing spineboy since its used most often
    player = renderer.getElemByID("spineboy");

    speaker1 = renderer.getElemByID('speaker1');
    speaker2 = renderer.getElemByID('speaker2');
    // need to set start to know how far to bring back down in future
    speakerStartY = speaker1.position.y; // speaker2 should be same

    walkTicker.stop();
    walkTicker.add(walkAnimation);
    player.state.addListener({ 
        end: animationEnd
    })

    run();
}

function run() {
    // Relayers to renderer
    for (var i = 0; i < appData.length; i++) {
        if (appData[i].hide) { continue; }
        renderer.displayLayerByID(appData[i].name);
    }
    idleMode();
}
