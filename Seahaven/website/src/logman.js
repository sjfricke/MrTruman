const myconsole = {
    log: console.log,
    warn: console.warn,
    error: console.error
}
console.log = function () { };
console.warn = function () { };
console.error = function () { };

function log() {
    logType.apply(this, ['log'].concat(
        Array.prototype.slice.call(arguments)));
}

function warn() {
    logType.apply(this, ['warn'].concat(
        Array.prototype.slice.call(arguments)));
}

function err() {
    logType.apply(this, ['error'].concat(
        Array.prototype.slice.call(arguments)));
}

function logType(type, name, msg) {
    if (type !== 'log'
        && type !== 'warn'
        && type !== 'error') return;
    if (myconsole[type]
        && typeof name === 'string') {
        if (typeof msg !== 'string') {
            msg = msg + '';
        }
        if (typeof msg !== 'string') return;
        let slicedArgs = Array.prototype.slice.call(arguments, 3);
        (myconsole[type]).apply(this,
            [('%cClass ' + name + ':%c ' + msg), 'color: #a6cd94', 'color: grey'].concat(
            Array.prototype.slice.call(slicedArgs))
            );
    }
}
