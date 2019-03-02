// current pixel density
let density;
// gui buttons
let execute;
let reset;
// execution parameters
let start = false;
let speed;
// data RAM
const memorySize = 24577;
let memoryBegin, memoryEnd;
// logs div
let consoleLogText;

// Emscripten global variable
/* globals Module */
// p5 canvas related functions
/* globals createCanvas background */
// p5 pixel related variable
/* globals pixelDensity */
// p5 dom related function
/* globals select */
/* exported setup */
function setup() {
  window.addEventListener('keydown', (e) => {
    // space and arrow keys
    if ([32, 37, 38, 39, 40].indexOf(e.keyCode) > -1) {
      e.preventDefault();
    }
  }, false);

  const fileReader = new FileReader();
  fileReader.onload = (event) => {
    if (event.target.readyState !== 2) return;
    if (event.target.error) {
      alert('Error while reading file! Please load again...');
      return;
    }

    Module.ccall('ReadProgram', null, ['string'], [event.target.result]);

    execute.removeAttribute('disabled');
  };

  setTimeout(() => {
    const memory = Module._malloc(memorySize * 2); // int16 - two bytes long
    memoryBegin = memory / 2;
    memoryEnd = memoryBegin + memorySize;
    Module.ccall('SetMemoryPtr', null, ['number'], [memory]);

    const fileInput = document.getElementById('fileInput');
    fileInput.onclick = () => {
      fileInput.value = '';
      Module.ccall('Clear');
      start = false;
      background(230);
      execute.attribute('disabled', '');
      consoleLogText.innerHTML = '';
    };
    fileInput.oninput = (event) => {
      fileReader.readAsText(event.target.files[0]);
    };

    execute = select('#execute');
    reset = select('#reset');

    execute.mousePressed(() => {
      execute.attribute('disabled', '');
      Module.ccall('InitializeExecution');
      console.log('Starting execution ...');
      start = true;
    });

    reset.mousePressed(() => {
      Module.ccall('Reset');
      background(230);
      execute.removeAttribute('disabled');
      consoleLogText.innerHTML = '';
      start = false;
    });
  }, 1000);

  createCanvas(512, 256).parent('canvas');
  background(230);
  density = pixelDensity();

  const speedSlider = select('#speed');
  speed = speedSlider.value();
  speedSlider.changed(() => {
    speed = speedSlider.value();
  });

  consoleLogText = document.getElementById('console-log-text');
}

/* exported draw */
function draw() {
  if (start) nextFrame();
}

// p5 pixel related functions and variables
/* globals loadPixels updatePixels pixels */
// p5 canvas width variable
/* globals width */
// p5 keyboard related variables
/* globals keyIsPressed keyCode */
// p5 key constants
/* globals ENTER BACKSPACE LEFT_ARROW RIGHT_ARROW UP_ARROW DOWN_ARROW DELETE ESCAPE */
function nextFrame() {
  Module.ccall('Execute', 'number', ['number'], [speed]);

  const memoryArr = Module.HEAP16.subarray(memoryBegin, memoryEnd);
  loadPixels();
  let x = 0, y = 0;
  for (let i = 16384; i < 24576; ++i) {
    for (let j = 0; j < 16; ++j) {
      let c = ((memoryArr[i] >> j) & 1) ? 0 : 230;
      for (let k = 0; k < density; ++k) {
        for (let l = 0; l < density; ++l) {
          const idx = ((y * density + l) * width * density + (x * density + k)) * 4;
          pixels[idx] = pixels[idx + 1] = pixels[idx + 2] = c;
          pixels[idx + 3] = 255;
        }
      }
      if (++x == 512) {
        x = 0;
        ++y;
      }
    }
  }
  updatePixels();

  let pressedKey = 0;
  if (keyIsPressed) {
    pressedKey = keyCode;
    switch (keyCode) {
    case ENTER:
      pressedKey = 128;
      break;
    case BACKSPACE:
      pressedKey = 129;
      break;
    case LEFT_ARROW:
      pressedKey = 130;
      break;
    case UP_ARROW:
      pressedKey = 131;
      break;
    case RIGHT_ARROW:
      pressedKey = 132;
      break;
    case DOWN_ARROW:
      pressedKey = 133;
      break;
    case DELETE:
      pressedKey = 139;
      break;
    case ESCAPE:
      pressedKey = 140;
      break;
    }
    if (pressedKey >= 97 && pressedKey <= 122)
      pressedKey -= 32;
  }
  Module.ccall('SetKey', null, ['number'], [pressedKey]);
}

