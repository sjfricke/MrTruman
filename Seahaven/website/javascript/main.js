
//Create a container object called the `stage`
var stage = new PIXI.Container();

//Create the renderer
var renderer = PIXI.autoDetectRenderer(
  800, 480,
  {antialias: false, transparent: false, resolution: 1}
);

// var wh = window.document.documentElement.clientHeight;

// var ww = document.body.clientWidth;

// renderer.view.style.position = "absolute";
// renderer.view.style.display = "block";
// renderer.autoResize = true;
// renderer.resize(window.innerWidth, window.innerHeight);
// renderer.resize(ww, wh)	;

//Add the canvas to the HTML document
document.body.appendChild(renderer.view);


//Use Pixi's built-in `loader` object to load an image
PIXI.loader
  .add("images/house_background.png")
  .load(setup);

//This `setup` function will run when the image has loaded
function setup() {

  //Create the `background` sprite from the texture
  var background = new PIXI.Sprite(
    PIXI.loader.resources["images/background_poc.png"].texture
  );

  //Add the background to the stage
  stage.addChild(background);
  
  //Render the stage   
  renderer.render(stage);
}