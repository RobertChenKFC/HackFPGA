let fileReader;
let files;
let fileIdx;
let start = false;
let bufBegin;
let bufEnd;
let speed;
let execute;
let reset;
let density;
let consoleLogText;

// DEBUG
var debugMessage;

function readFile() {
  if (fileIdx == -1) {
    return;
  } else if (fileIdx < files.length) {
    fileReader.readAsText(files[fileIdx]);
  } else if (fileIdx == files.length) {
    if (Module.ccall('InitializeExecution')) {
      execute.removeAttribute('disabled');
      reset.removeAttribute('disabled');
    }
  }
}

function setup() {
  window.addEventListener("keydown", function(e) {
    // space and arrow keys
    if([32, 37, 38, 39, 40].indexOf(e.keyCode) > -1) {
      e.preventDefault();
    }
  }, false);

  fileReader = new FileReader();
  fileReader.onload = (event) => {
    if (event.target.readyState != 2) return;
    if (event.target.error) {
      alert('Error while reading file! Please load again...');
      return;
    }

    if (!Module.ccall('CompileFile', null, ['string', 'string'], [files[fileIdx].name, event.target.result]))
      fileIdx = -1;
    else
      ++fileIdx;

    readFile();
  };

  setTimeout(() => {
    const memory = new Int32Array(24577); // Change to 32-bit
    const buf = Module._malloc(memory.length * memory.BYTES_PER_ELEMENT);
    bufBegin = buf / memory.BYTES_PER_ELEMENT;
    bufEnd = bufBegin + memory.length;
    Module.HEAP32.set(memory, bufBegin); // Change to 32-bit
    Module.ccall('SetMemoryPtr', null, ['number'], [buf]);

    const fileInput = document.getElementById('fileInput');
    fileInput.onclick = () => {
      fileInput.value = '';
      Module.ccall('Clear');
      start = false;
      background(230);
      execute.attribute('disabled', '');
      reset.attribute('disabled', '');
      consoleLogText.innerHTML = '';
    };
    fileInput.oninput = (event) => {
      files = event.target.files;
      fileIdx = 0;
      readFile();
    };

    execute = select('#execute');
    reset = select('#reset');

    execute.mousePressed(() => {
      execute.attribute('disabled', '');
      console.log('Starting execution...');
      start = true;
    });

    reset.mousePressed(() => {
      Module.ccall('Reset');
      background(230);
      start = false;
      execute.removeAttribute('disabled');
      consoleLogText.innerHTML = '';
    });

    const speedSlider = select('#speed');
    speed = speedSlider.value();
    speedSlider.changed(() => {
      speed = speedSlider.value();
    });
  }, 1000);

  createCanvas(512, 256).parent('canvas');
  background(230);
  density = pixelDensity();

  consoleLogText = document.getElementById('console-log-text');
}

function draw() {
  if (start) nextFrame();
}

function nextFrame() {
  if (Module.ccall('Execute', 'number', ['number'], [speed])) {
    console.log('Program halted.');
    execute.attribute('disabled', '');
    start = false;
  }

  const memory = Module.HEAP32.subarray(bufBegin, bufEnd);
  loadPixels();
  let x = 0, y = 0;
  for (let i = 16384; i < 24576; ++i) {
    for (let j = 0; j < 16; ++j) {
      let c = ((memory[i] >> j) & 1) ? 0 : 230;
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

