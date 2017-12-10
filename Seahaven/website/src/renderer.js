
class Renderer {
    constructor(parentNode) {
        parentNode = $(parentNode);
        if (!parentNode[0]) parentNode = $('body');
        if (parentNode[0] === undefined) {
            log('Renderer', 'Could not correctly initialize renderer.');
        }
        else {
            this.$container = parentNode;
            this.app = new PIXI.Application(window.outerWidth, window.outerHeight, { backgroundColor: 0xffffff });
            this.$container.append(this.app.view);
            this.elems = {};
            this.textures = {};
            this.app.stage.filters = [];
            this.app.renderer.clearBeforeRender = false;
        }
    }

  
    checkload(id, path, pos) {
        if (typeof id !== 'string' || this.testID(id)) {
            log('Renderer', 'Could not add because ID (= %O) is already in use.', id);
            return false;
        }
        else if (typeof path !== 'string') {
            log('Renderer', 'Could not add because jsonPath (= %O) is not of a string.', path);
            return false;
        } 
        else if (!(pos instanceof PIXI.Point)) {
            log('Renderer', 'Could not add because position (= %O) is not of type PIXI.Point.', pos);
            return false;
        }
        else {
            return true;
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

        if (!this.checkload(id, jsonPath, pos)) { return; }

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
            scale = data.scale,
            hideAlpha = data.hideAlpha,
            anchorX = data.anchorX,
            anchorY = data.anchorY,
            hide = data.hide;

        if (!this.checkload(id, imgPath, pos)) { return; }

        let container = this.getElemByID(containerID);
        if (container == null || !(container instanceof PIXI.Container)) {
            container = this.app.stage;
        }
        let loader = new PIXI.loaders.Loader();
        loader.add(id, imgPath);
        var that = this;
        loader.load((loader, res) => {
            that.elems[id] = new PIXI.Sprite(res[id].texture);
            that.elems[id].anchor.x = isNaN(anchorX) ? 0.5 : anchorX;
            that.elems[id].anchor.y = isNaN(anchorY) ? 1.0 : anchorY;
            that.elems[id].position.x = pos.x * window.outerWidth;
            that.elems[id].position.y = pos.y * window.outerHeight;

            if (hideAlpha) { that.elems[id].alpha = 0; }
            if (hide) { that.elems[id].hide = true; }

            if (scale !== undefined && typeof scale === 'number' && scale > 0) {
                that.elems[id].scale.x = scale;
                that.elems[id].scale.y = scale;
            }
            if (typeof onload === 'function') {
                onload(id, that.elems[id]);
            }
        });
    }

    addTexture(data, onload) {
        if (data === undefined || typeof data !== 'object') {
            log('Renderer', 'Could not add because data (= %O) is invalid.', data);
            return;
        }
        let id = data.name,
            imgPath = data.path,
            pos = data.pt,
            containerID = data.containerID;
        
        if (!this.checkload(id, imgPath, pos)) { return; }

        let loader = new PIXI.loaders.Loader();
        loader.add(id, imgPath);
        var that = this;
        loader.load((loader, res) => {
            that.textures[id] = res[id].texture;
            that.elems[id] = new PIXI.extras.TilingSprite(
                res[id].texture,
                data.width || that.app.renderer.width,
                data.height || that.app.renderer.height);

            that.elems[id].position.y = pos.y * window.outerHeight

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
        
        if (!this.checkload(id, imgPath, pos)) { return; }

        let loader = new PIXI.loaders.Loader();
        loader.add(id, imgPath);
        var that = this;
        loader.load((loader, res) => {
            that.elems[id] = new PIXI.extras.TilingSprite(
                res[id].texture,
                data.width || that.app.renderer.width,
                data.height || that.app.renderer.height
            );

            that.elems[id].position.y = pos.y * window.outerHeight
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
            hide = data.hide,
            hideAlpha = data.hideAlpha,
            loop = data.loop,
            speed = data.speed || .2,
            scale = data.scale || 1,
            start = data.start || true;

        if (!this.checkload(id, jsonPath, pos)) { return; }

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
            that.elems[id].loop = loop;
            that.elems[id].animationSpeed = speed;

            if (data.start) {                
                that.elems[id].gotoAndPlay(0);
            } else {
                that.elems[id].gotoAndStop(0);
            }

            if (hide) { that.elems[id].hideOnLoad = true; }
            else if (hideAlpha) { that.elems[id].alpha = 0; }

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
        if (layer.hideOnLoad) {
            return;
        }
        else if (layer == null) {
            log('Renderer', 'Could not display layer because  ID (= %O) was not found.', id);
            return;
        }
        this.app.stage.addChild(layer);
    }
}