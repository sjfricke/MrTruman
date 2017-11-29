const defaultVertexShader = `
attribute vec2 aVertexPosition;
attribute vec2 aTextureCoord;
uniform mat3 projectionMatrix;
varying vec2 vTextureCoord;
void main(void)
{
    gl_Position = vec4((projectionMatrix * vec3(aVertexPosition, 1.0)).xy, 0.0, 1.0);
    vTextureCoord = aTextureCoord;
}
`;

const lightSwitchFS = `
precision mediump float;
varying vec2 vTextureCoord;
uniform sampler2D uSampler;
uniform float mode;
vec3 black = vec3(0.0,0.0,0.0);
void main(void)
{
    float x = vTextureCoord.x;
    float y = vTextureCoord.y;
    vec4 pixel = texture2D(uSampler, vTextureCoord.xy);
    if (mode == 1.0) {
        gl_FragColor = pixel;
    }
    else {
        // 0 is full black, 1.0 is full on
        vec3 color = mix(black, pixel.xyz, 0.7);
        gl_FragColor = vec4(color, pixel.a);
    }
}
`;

const asciiFS = `
varying vec2 vTextureCoord;
uniform vec4 filterArea;
uniform float pixelSize;
uniform sampler2D uSampler;
vec2 mapCoord( vec2 coord ){
    coord *= filterArea.xy;
    coord += filterArea.zw;
    return coord;
}
vec2 unmapCoord( vec2 coord ){
    coord -= filterArea.zw;
    coord /= filterArea.xy;
    return coord;
}
vec2 pixelate(vec2 coord, vec2 size){
    return floor( coord / size ) * size;
}
vec2 getMod(vec2 coord, vec2 size){
    return mod( coord , size) / size;
}
float character(float n, vec2 p){
    p = floor(p*vec2(4.0, -4.0) + 2.5);
    if (clamp(p.x, 0.0, 4.0) == p.x && clamp(p.y, 0.0, 4.0) == p.y){
        if (int(mod(n/exp2(p.x + 5.0*p.y), 2.0)) == 1) return 1.0;
    }
    return 0.0;
}
void main()
{
    vec2 coord = mapCoord(vTextureCoord);
    vec2 pixCoord = pixelate(coord, vec2(pixelSize));
    pixCoord = unmapCoord(pixCoord);
    vec4 color = texture2D(uSampler, pixCoord);
    float gray = (color.r + color.g + color.b) / 3.0;
    float n =  65536.0;             // .
    if (gray > 0.2) n = 65600.0;    // :
    if (gray > 0.3) n = 332772.0;   // *
    if (gray > 0.4) n = 15255086.0; // o
    if (gray > 0.5) n = 23385164.0; // &
    if (gray > 0.6) n = 15252014.0; // 8
    if (gray > 0.7) n = 13199452.0; // @
    if (gray > 0.8) n = 11512810.0; // #
    vec2 modd = getMod(coord, vec2(pixelSize));
    gl_FragColor = color * character( n, vec2(-1.0) + modd * 2.0);
}
`;

const flashFS = `
varying vec2 vTextureCoord;
varying vec4 vColor;
uniform sampler2D uSampler;
uniform float distance;
uniform float outerStrength;
uniform float innerStrength;
uniform vec4 glowColor;
uniform vec4 filterArea;
uniform vec4 filterClamp;
vec2 px = vec2(1.0 / filterArea.x, 1.0 / filterArea.y);
void main(void) {
    const float PI = 3.14159265358979323846264;
    vec4 ownColor = texture2D(uSampler, vTextureCoord);
    vec4 curColor;
    float totalAlpha = 0.0;
    float maxTotalAlpha = 0.0;
    float cosAngle;
    float sinAngle;
    vec2 displaced;
    for (float angle = 0.0; angle <= PI * 2.0; angle += %QUALITY_DIST%) {
       cosAngle = cos(angle);
       sinAngle = sin(angle);
       for (float curDistance = 1.0; curDistance <= %DIST%; curDistance++) {
           displaced.x = vTextureCoord.x + cosAngle * curDistance * px.x;
           displaced.y = vTextureCoord.y + sinAngle * curDistance * px.y;
           curColor = texture2D(uSampler, clamp(displaced, filterClamp.xy, filterClamp.zw));
           totalAlpha += (distance - curDistance) * curColor.a;
           maxTotalAlpha += (distance - curDistance);
       }
    }
    maxTotalAlpha = max(maxTotalAlpha, 0.0001);
    ownColor.a = max(ownColor.a, 0.0001);
    ownColor.rgb = ownColor.rgb / ownColor.a;
    float outerGlowAlpha = (totalAlpha / maxTotalAlpha)  * outerStrength * (1. - ownColor.a);
    float innerGlowAlpha = ((maxTotalAlpha - totalAlpha) / maxTotalAlpha) * innerStrength * ownColor.a;
    float resultAlpha = (ownColor.a + outerGlowAlpha);
    gl_FragColor = vec4(mix(mix(ownColor.rgb, glowColor.rgb, innerGlowAlpha / ownColor.a), glowColor.rgb, outerGlowAlpha / resultAlpha) * resultAlpha, resultAlpha);
}
`;

const embossFS = `
precision mediump float;
varying vec2 vTextureCoord;
uniform sampler2D uSampler;
uniform float strength;
uniform vec4 filterArea;
void main(void){
	vec2 onePixel = vec2(1.0 / filterArea);
	vec4 color;
	color.rgb = vec3(0.5);
	color -= texture2D(uSampler, vTextureCoord - onePixel) * strength;
	color += texture2D(uSampler, vTextureCoord + onePixel) * strength;
	color.rgb = vec3((color.r + color.g + color.b) / 3.0);
	float alpha = texture2D(uSampler, vTextureCoord).a;
	gl_FragColor = vec4(color.rgb * alpha, alpha);
}
`;

class LightSwitchFilter extends PIXI.Filter {
    constructor(mode = 0.0) {
        super(defaultVertexShader, lightSwitchFS);
        this.mode = mode;
    }
    get mode() {return this.uniforms.mode;}
    set mode(value) {this.uniforms.mode = value;}
}

class AsciiFilter extends PIXI.Filter {
    constructor(size = 7) {
        super(defaultVertexShader, asciiFS);
        this.size = size;
    }
    get size() {return this.uniforms.pixelSize;}
    set size(value) {this.uniforms.pixelSize = value;}
}

class EmbossFilter extends PIXI.Filter {
    constructor(strength = 2){
        super(defaultVertexShader, embossFS);
        this.strength = strength;
    }
    get strength() {return this.uniforms.strength;}
    set strength(value) {this.uniforms.strength = value;}
}


class GlowFilter extends PIXI.Filter {
    constructor(distance = 10, outerStrength = 4, innerStrength = 0, color = 0xffffff, quality = 0.2) {
        super(defaultVertexShader, flashFS
            .replace(/%QUALITY_DIST%/gi, '' + (1 / quality / distance).toFixed(7))
            .replace(/%DIST%/gi, '' + distance.toFixed(7)));
        this.uniforms.glowColor = new Float32Array([0, 0, 0, 1]);
        this.distance = distance;
        this.color = color;
        this.outerStrength = outerStrength;
        this.innerStrength = innerStrength;
    }
    get color() {return PIXI.utils.rgb2hex(this.uniforms.glowColor);}
    set color(value) {PIXI.utils.hex2rgb(value, this.uniforms.glowColor);}
    get distance() {return this.uniforms.distance; }
    set distance(value) {this.uniforms.distance = value;}
    get outerStrength() {return this.uniforms.outerStrength;}
    set outerStrength(value) {this.uniforms.outerStrength = value;}
    get innerStrength() {return this.uniforms.innerStrength;}
    set innerStrength(value) {this.uniforms.innerStrength = value;}
}

const lightSwitchFilter = new LightSwitchFilter();
const asciiFilter = new AsciiFilter();
const embossFilter = new EmbossFilter();
const glowFilter = new GlowFilter();