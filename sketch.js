let fileReader;
let files;
let fileIdx;
let start = false;
let bufBegin;
let bufEnd;
let speed;
let execute;
let reset;
let getVmCode;
let getAssembly;
let getMachineCode;
let density;
let consoleLogText;
let vmCodeStrings = {};
let assemblyString;
let machineCode = [];
let progName;
const RAM_SIZE = 24577;
const ROM_SIZE = 512 * 1024;
const WORD_SIZE = 4;
const OS_FILES = [
  "/os/Array.jack",
  "/os/Keyboard.jack",
  "/os/Math.jack",
  "/os/Memory.jack",
  "/os/Output.jack",
  "/os/Screen.jack",
  "/os/String.jack",
  "/os/Sys.jack"
];

function saveFile(contents, filename, type = "text/plain") {
  const link = document.createElement("a");
  const file = new Blob([contents], { type: type });
  link.href = URL.createObjectURL(file);
  link.download = filename;
  link.click();
  URL.revokeObjectURL(link.href);
}

function saveBinaryFile(contents, filename) {
  saveFile(contents, filename, "octet/stream");
}

class SharedString {
  constructor(input) {
    const uint8Arr = new Uint8Array(input);
    this.numBytes = uint8Arr.length * uint8Arr.BYTES_PER_ELEMENT;
    this.dataPtr = Module._malloc(this.numBytes);
    this.dataOnHeap = new Uint8Array(
        Module.HEAPU8.buffer, this.dataPtr, this.numBytes);
    this.dataOnHeap.set(uint8Arr);
  }

  getPtr() {
    return this.dataOnHeap.byteOffset;
  }

  getLength() {
    return this.numBytes;
  }

  getString() {
    return new TextDecoder().decode(this.dataOnHeap);
  }

  getArrayBuffer() {
    const dest = new ArrayBuffer(this.getLength());
    new Uint8Array(dest).set(this.dataOnHeap);
    return dest;
  }

  free() {
    Module._free(this.dataPtr);
  }
}

function compileOsFiles(callback, osFileIdx = 0) {
  if (osFileIdx < OS_FILES.length) {
    const filePath = OS_FILES[osFileIdx];
    fetch(filePath)
      .then(r => r.arrayBuffer())
      .then(text => {
        const filename = filePath.split("/").pop();
        const fileContent = new SharedString(text);
        Module.ccall(
            'CompileFile', "number", ['string', 'number', 'number'],
            [filename, fileContent.getPtr(), fileContent.getLength()]);
        fileContent.free();
        Module.ccall("TranslateFile", null, ["string"], [filename]);
        compileOsFiles(callback, osFileIdx + 1);
      });
  } else {
    callback();
  }
}

function readFile() {
  if (fileIdx == -1) {
    return;
  } else if (fileIdx < files.length) {
    fileReader.readAsArrayBuffer(files[fileIdx]);
  } else if (fileIdx == files.length) {
    compileOsFiles(() => {
      const assemblyLength = Module.ccall(
          "GetAssemblyLength", "number", [], []);
      const assembly = new SharedString(assemblyLength);
      Module.ccall("GetAssembly", null, ["number"], [assembly.getPtr()]);
      assemblyString = assembly.getString();
      assembly.free();

      Module.ccall("Assemble", null, [], []);

      const curMachineCode = new SharedString(ROM_SIZE);
      for (let i = 0; i < WORD_SIZE; ++i) {
        Module.ccall(
            "GetMachineCode", null, ["number", "number"],
            [curMachineCode.getPtr(), i]);
        machineCode.push(curMachineCode.getArrayBuffer());
      }
      curMachineCode.free();

      if (Module.ccall('InitializeExecution')) {
        execute.removeAttribute('disabled');
        reset.removeAttribute('disabled');
        getVmCode.removeAttribute("disabled");
        getAssembly.removeAttribute("disabled");
        getMachineCode.removeAttribute("disabled");
      }
    });
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

    const filename = files[fileIdx].name
    const fileContent = new SharedString(event.target.result);
    const vmCodeLength = Module.ccall(
        'CompileFile', "number", ['string', 'number', 'number'],
        [filename, fileContent.getPtr(), fileContent.getLength()]);
    if (vmCodeLength == 0) {
      fileIdx = -1;
    } else {
      const vmCodeString = new SharedString(vmCodeLength);
      Module.ccall("GetVmCodeString", null, ["string", "number"],
                   [filename, vmCodeString.getPtr()]);
      vmCodeStrings[filename] = vmCodeString.getString();
      vmCodeString.free();

      Module.ccall("TranslateFile", null, ["string"],
                   [filename]);

      ++fileIdx;
    }
    fileContent.free();

    readFile();
  };

  setTimeout(() => {
    Module.ccall("Init", null, [], []);

    const memory = new Int32Array(RAM_SIZE); // Change to 32-bit
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
      getVmCode.attribute("disabled", "");
      getAssembly.attribute("disabled", "");
      getMachineCode.attribute("disabled", "");
      consoleLogText.innerHTML = '';
      vmCodeStrings = {};
      machineCode = [];
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

    progName = select("#progName");
    getVmCode = select("#vmCode");
    getAssembly = select("#assembly");
    getMachineCode = select("#machineCode");
    getVmCode.mousePressed(() => {
      const zip = JSZip();
      for (let filename in vmCodeStrings) {
        zip.file(
            filename.replace(".jack", ".vm"), vmCodeStrings[filename]);
      }
      zip.generateAsync({type: "blob"}).then(zipFile => {
        saveBinaryFile(zipFile, progName.value() + ".vm.zip");
      });
    });
    getAssembly.mousePressed(() => {
      saveFile(assemblyString, progName.value() + ".asm");
    });
    getMachineCode.mousePressed(() => {
      const zip = JSZip();
      for (let i = 0; i < machineCode.length; ++i)
        zip.file(`${i}.hack`, machineCode[i]);
      zip.generateAsync({type: "blob"}).then(zipFile => {
        saveBinaryFile(zipFile, progName.value() + ".hack.zip");
      });
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

