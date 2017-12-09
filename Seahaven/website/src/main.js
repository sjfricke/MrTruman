/**************************************************
*    HARD Assumption screen is 800 x 480          *
***************************************************/
$(window).ready(start());

var renderer;
var texLoaded = 0;
var currentWall = 0;
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
    
    (new PIXI.loaders.Loader()).add('lightOn', resPath.lightOn).
        load(function (loader, res) { lightOnTexture = res.lightOn.texture; });
    (new PIXI.loaders.Loader()).add('wall1', resPath.wall1).
        load(function (loader, res) { wallTextures[1] = res.wall1.texture; });
    (new PIXI.loaders.Loader()).add('wall2', resPath.wall2).
        load(function (loader, res) { wallTextures[2] = res.wall2.texture; });
    (new PIXI.loaders.Loader()).add('wall3', resPath.wall3).
        load(function (loader, res) { wallTextures[3] = res.wall3.texture; });
    (new PIXI.loaders.Loader()).add('wall4', resPath.wall4).
        load(function (loader, res) { wallTextures[4] = res.wall4.texture; });

    appData = [
        {
            type: 'addTexture',
            name: 'wall',
            path: resPath.wall0,
            pt: new PIXI.Point(0, 0)
        }, 
        {
            type: 'add',
            name: 'window',
            path: resPath.window,
            pt: new PIXI.Point(0.638125, 0.6083333333333333),
            scale: 1.0
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
            pt: new PIXI.Point(0.28125, 0.26),            
            anchorY: -.61,
            scale: .8
        },
        {
            type: 'add',
            name: 'frame',
            path: resPath.frame,
            pt: new PIXI.Point(0.28125, 0.26),
            anchorY: 0,
            scale: 1.0
        },        
        {           
            type: 'addSpritesheet',
            name: 'fireAnimated',
            path: resPath.fireAnimated,
            pt: new PIXI.Point(0.914375, 0.943),
            count: 14,
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
            pt: new PIXI.Point(0.28125, 1),
            scale: 1.5
        },
        {
            type: 'addSpritesheet',
            name: 'speaker1',
            path: resPath.speakerAnimated,
            pt: new PIXI.Point(0.4625, 1.5),
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
            pt: new PIXI.Point(0.1, 1.5),
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

    player = renderer.getElemByID("spineboy");
    
    couch = renderer.getElemByID("couch");
    picture = renderer.getElemByID("picture");
    frame = renderer.getElemByID("frame");

    speaker1 = renderer.getElemByID('speaker1');
    speaker2 = renderer.getElemByID('speaker2');
    // need to set start to know how far to bring back down in future
    speakerStartY = speaker1.position.y; // speaker2 should be same

    walkTicker.stop();
    walkTicker.add(walkAnimation);
    player.state.addListener({ 
        end: animationEnd
    })

    wallTextures[0] = renderer.getElemByID("wall").texture;
    renderer.getElemByID("wall").alpha = .65;

    run();
}

function run() {
    // Relayers to renderer
    for (var i = 0; i < appData.length; i++) {
        if (appData[i].hide) { continue; }
        renderer.displayLayerByID(appData[i].name);
    }

    player.position.x = 400; // make sure it starts in 400
    idleMode();
}
