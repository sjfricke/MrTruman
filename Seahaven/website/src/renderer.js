
class Renderer {
    constructor(parentNode) {
        parentNode = $(parentNode);
        if (!parentNode[0]) parentNode = $('body');
        if (parentNode[0] === undefined) {
            log('Renderer', 'Could not correctly initialize renderer.');
        }
        else {
            this.$container = parentNode;
            this.app = new PIXI.Application(window.outerWidth, window.outerHeight, { backgroundColor: 0x1099bb });
            this.$container.append(this.app.view);
            this.elems = {};
            this.app.stage.interactive = true;
            this.editorFilter = new PIXI.Filter(null, `
                    precision mediump float;
                    varying vec2 vTextureCoord;
                    uniform sampler2D uSampler;
                    uniform float mode;
                    // Brightness-Saturation-Contrast Filter
                    vec3 applyBSC(vec4 color, float brightness, float saturation, float contrast)
                    {
	                    vec3 color_B  = color.rgb * (1.0 +color.a * (brightness -1.0));
	                    vec3 intensity = vec3(dot(color_B, vec3(0.2125, 0.7154, 0.0721)));
	                    vec3 color_BS  = mix(intensity, color_B, 1.0 + color.a * (saturation -1.0));
	                    vec3 color_BSC  = mix(vec3(0.5, 0.5, 0.5), color_BS, 1.0 + color.a * (contrast - 1.0));
	                    return color_BSC;
                    }
                    void main(void)
                    {
                        float x = vTextureCoord.x;
                        float y = vTextureCoord.y;
                        vec4 pixel = texture2D(uSampler, vTextureCoord.xy);
                        if (mode == 1.0
                            || (((x < 0.192 && x > 0.1382) || (x < 0.2605 && x > 0.205))
                            && ((y < 0.45996 && y > 0.3232) || (y < 0.603 && y > 0.4718)))) {
                            gl_FragColor = pixel;
                        }
                        else {
	                        vec3 color = applyBSC(pixel, 0.55, 1.1, 3.0);
	                        gl_FragColor = vec4(color, pixel.a);
                        }
                    }`
                );
            this.editorFilter.uniforms.mode = 0.0;
            this.app.stage.filters = [this.editorFilter];

        }
    }

    doStuff() {
        // load spine data
        PIXI.loader
            .add('spineboy', 'res/json/spineboy.json')
            .load(onAssetsLoaded);

        var that = this;
        function onAssetsLoaded(loader, res) {
            // create a spine boy
            var spineBoy = new PIXI.spine.Spine(res.spineboy.spineData);

            // set the position
            spineBoy.x = that.app.renderer.width / 2;
            spineBoy.y = that.app.renderer.height;

            spineBoy.scale.set(1.5);

            // set up the mixes!
            spineBoy.stateData.setMix('walk', 'jump', 0.2);
            spineBoy.stateData.setMix('jump', 'walk', 0.4);

            // play animation
            spineBoy.state.setAnimation(0, 'walk', true);

            that.app.stage.on('pointerdown', function () {
                spineBoy.state.setAnimation(0, 'jump', false);
                spineBoy.state.addAnimation(0, 'walk', true, 0);
            });
            that.app.stage.on('pointermove', function (e) {
                spineBoy.state.addAnimation(0, 'walk', true, 0);
                spineBoy.x = (spineBoy.x + 5) % 1650;
            });
        }
    }

    addSpine(data, onload) {
        if (data === undefined || typeof data !== 'object') {
            log('Renderer', 'Could not add because data (= %O) is invalid.', data);
            return;
        }
        let id = data.name,
            jsonPath = data.path,
            pos = data.pt,
            containerID = data.containerID,
            scale = data.scale;
        if (typeof id !== 'string' || this.testID(id)) {
            log('Renderer', 'Could not add because ID (= %O) is already in use.', id);
            return;
        }
        if (typeof jsonPath !== 'string') {
            log('Renderer', 'Could not add because jsonPath (= %O) is not of a string.', jsonPath);
            return;
        }
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not add because position (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        var container = this.getElemByID(containerID);
        if (container == null || !(container instanceof PIXI.Container)) {
            container = this.app.stage;
        }
        PIXI.loader
            .add(id, jsonPath)
            .load(onAssetsLoaded);

        var that = this;
        function onAssetsLoaded(loader, res) {
            that.elems[id] = new PIXI.spine.Spine(res[id].spineData);

            that.elems[id].x = pos.x;
            that.elems[id].y = pos.y;

            if (scale !== undefined && typeof scale === 'number' && scale > 0) {
                that.elems[id].scale.x = scale;
                that.elems[id].scale.y = scale;
            }
            if (typeof onload === 'function') {
                onload(id, that.elems[id]);
            }
        }
    }

    add(data, onload) {
        if (data === undefined || typeof data !== 'object') {
            log('Renderer', 'Could not add because data (= %O) is invalid.', data);
            return;
        }
        let id = data.name,
            imgPath = data.path,
            pos = data.pt,
            containerID = data.containerID,
            scale = data.scale;
        if (typeof id !== 'string' || this.testID(id)) {
            log('Renderer', 'Could not add because ID (= %O) is already in use.', id);
            return;
        }
        if (typeof imgPath !== 'string') {
            log('Renderer', 'Could not add because imgPath (= %O) is not of a string.', imgPath);
            return;
        }
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not add because position (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        let container = this.getElemByID(containerID);
        if (container == null || !(container instanceof PIXI.Container)) {
            container = this.app.stage;
        }
        let loader = new PIXI.loaders.Loader();
        loader.add(id, imgPath);
        var that = this;
        loader.load((loader, res) => {
            that.elems[id] = new PIXI.Sprite(res[id].texture);
            that.elems[id].anchor.x = 0.5;
            that.elems[id].anchor.y = 1;
            that.elems[id].position.x = pos.x * window.outerWidth;
            that.elems[id].position.y = pos.y * window.outerHeight;
            if (scale !== undefined && typeof scale === 'number' && scale > 0) {
                that.elems[id].scale.x = scale;
                that.elems[id].scale.y = scale;
            }
            if (typeof onload === 'function') {
                onload(id, that.elems[id]);
            }
        });
    }

    addTile(data, onload) {
        if (data === undefined || typeof data !== 'object') {
            log('Renderer', 'Could not add because data (= %O) is invalid.', data);
            return;
        }
        let id = data.name,
            imgPath = data.path,
            pos = data.pt,
            containerID = data.containerID;
        if (typeof id !== 'string' || this.testID(id)) {
            log('Renderer', 'Could not add because ID (= %O) is already in use.', id);
            return;
        }
        if (typeof imgPath !== 'string') {
            log('Renderer', 'Could not add because imgPath (= %O) is not of a string.', imgPath);
            return;
        }
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not add because position (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        let loader = new PIXI.loaders.Loader();
        loader.add(id, imgPath);
        var that = this;
        loader.load((loader, res) => {
            that.elems[id] = new PIXI.extras.TilingSprite(
                res[id].texture,
                that.app.renderer.width,
                that.app.renderer.height
            );
            if (typeof onload === 'function') {
                onload(id, that.elems[id]);
            }
        });
    }

    addSpritesheet(data, onload) {
        if (data === undefined || typeof data !== 'object') {
            log('Renderer', 'Could not add because data (= %O) is invalid.', data);
            return;
        }
        let id = data.name,
            jsonPath = data.path,
            pos = data.pt,
            containerID = data.containerID,
            count = data.count,
            framePrefix = data.framePrefix,
            scale = data.scale || 1,
            start = data.start || true;
        if (typeof id !== 'string' || this.testID(id)) {
            log('Renderer', 'Could not add because ID (= %O) is already in use.', id);
            return;
        }
        if (typeof jsonPath !== 'string') {
            log('Renderer', 'Could not add because jsonPath (= %O) is not of a string.', jsonPath);
            return;
        }
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not add because position (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        var container = this.getElemByID(containerID);
        if (container == null || !(container instanceof PIXI.Container)) {
            container = this.app.stage;
        }
        (new PIXI.loaders.Loader())
            .add(id, jsonPath)
            .load(onAssetsLoaded);

        var that = this;
        function onAssetsLoaded(loader, res) {
           
           var animtedTextures = [];

            // pass sprite size in code
            for (var i = 0; i < count; i++) {
                 var texture = PIXI.Texture.fromFrame(framePrefix + 'Sequence_' + (i+1) + '.png');
                 animtedTextures.push(texture);
            }

            that.elems[id] = new PIXI.extras.AnimatedSprite(animtedTextures);

            that.elems[id].anchor.x = 0.5;
            that.elems[id].anchor.y = 1;
            that.elems[id].position.x = pos.x * window.outerWidth;
            that.elems[id].position.y = pos.y * window.outerHeight;

            that.elems[id].animationSpeed = 0.17;

            if (data.start) {                
                that.elems[id].gotoAndPlay(0);
            } else {
                that.elems[id].gotoAndStop(0);
            }

            if (scale !== undefined && typeof scale === 'number' && scale > 0) {
                that.elems[id].scale.x = scale;
                that.elems[id].scale.y = scale;
            }
            if (typeof onload === 'function') {
                onload(id, that.elems[id]);
            }
        }
    }

    testID(id) {
        return this.getElemByID(id) != null;
    }

    getElemByID(id) {
        if (typeof id !== 'string') {
            return null;
        }
        let found = this.elems[id];
        return (found !== undefined) ? found : null;
    }

    getW() {
        return this.app.renderer.width;
    }

    getH() {
        return this.app.renderer.height;
    }

    setPos(id, pos) {
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not set position because pos (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        let elem = this.getElemByID(id);
        if (elem == null) {
            log('Renderer', 'Could not set position because no element with ID (= %O) was found.', id);
            return;
        }
        elem.position = pos;
    }

    setPosByPercent(id, pos) {
        if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not set position because pos (= %O) is not of type PIXI.Point.', pos);
            return;
        }
        let elem = this.getElemByID(id);
        if (elem == null) {
            log('Renderer', 'Could not set position because no element with ID (= %O) was found.', id);
            return;
        }
        let w = elem.parent.width, h = elem.parent.height;
        elem.x = pos.x * w;
        elem.y = pos.y * h;
    }

    displayLayerByID(id) {
        let layer = this.getElemByID(id);
        if (layer == null) {
            log('Renderer', 'Could not display layer because  ID (= %O) was not found.', id);
            return;
        }
        this.app.stage.addChild(layer);
    }
}
