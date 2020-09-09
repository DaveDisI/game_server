const frameTime = 1000.0 / 60.0;

var canvas;
var gl;

var websock;
var serverInterval;
var updateInterval;

var position = new Vector2(0, 0);
var speed = new Vector2(Math.random() * 5, Math.random() * 5);
var posArr = new Int32Array(2);
var quadPositions;

var totalQuads = 0;

window.onload = function(){
    window.addEventListener('resize', windowResize);
    canvas = document.getElementById("canvasID");
    gl = canvas.getContext('webgl2');
    
    canvas.style.position = 'absolute';
    canvas.width = window.innerWidth * 0.95;
    canvas.height = window.innerHeight * 0.95;
    canvas.style.border = "solid";
    windowResize()
   
    gl.clearColor(1, 0, 0.25, 1);

    quadPositions = new Int32Array(256);
    initCanvasRenderer(canvas.width, canvas.height);


    gl.clear(gl.COLOR_BUFFER_BIT);

    websock = new WebSocket("ws://localhost:8080");
    websock.binaryType = 'arraybuffer';
    websock.onopen = function(evt) { 
        console.log("Connection Open");
        serverInterval = setInterval(contactServer, 1000 / 30);
        updateInterval = setInterval(updateGame, frameTime);
    };
    websock.onclose = function(evt) { 
        console.log("Connection Closed");
        clearInterval(serverInterval);
        clearInterval(updateInterval);
    };
    websock.onmessage = function(evt) { 
        var dat = new Int32Array(evt.data);
        console.log(dat);
        totalQuads = dat[0];
        for(let i = 1; i < dat.length; i++){
            quadPositions[i - 1] = dat[i];
        }
    };
    websock.onerror = function(evt) { 
        console.log(evt.data);
        clearInterval(serverInterval);
        clearInterval(updateInterval);
    };
}

function contactServer(){
    posArr[0] = position.x;
    posArr[1] = position.y;
    websock.send(posArr);
}

function updateGame(){
    position.x += speed.x;
    position.y += speed.y;
    if(position.x > canvas.width || position.x < 0){
        speed.x = -speed.x;
    }
    if(position.y > canvas.height || position.y < 0){
        speed.y = -speed.y;
    }
    
    gl.clear(gl.COLOR_BUFFER_BIT);
    let p = new Vector2();
    for(let i = 0; i < totalQuads; i++){
        p.x = quadPositions[i * 2];
        p.y = quadPositions[i * 2 + 1];
        renderQuad(p, new Vector2(100, 100), new Vector4(0, 0, 1, 1));
    }
}

function windowResize(){
    canvas.width = window.innerWidth * 0.95;
    canvas.height = window.innerHeight * 0.95;
    gl.viewport(0, 0, canvas.width, canvas.height);
}