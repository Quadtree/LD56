console.log(`main.js`)

function resized() {
    const canvasElement = document.getElementById("canvas");

    if (!canvasElement || !Module.InjectPageSize) {
        setTimeout(resized, 10);
        return;
    }

    if (window.devicePixelRatio != 1) {
        canvasElement.width = Math.round(document.documentElement.clientWidth * window.devicePixelRatio);
        canvasElement.height = Math.round(document.documentElement.clientHeight * window.devicePixelRatio);
        canvasElement.style.width = `${canvasElement.width / window.devicePixelRatio}px`;
        canvasElement.style.height = `${canvasElement.height / window.devicePixelRatio}px`;
    }

    Module.InjectPageSize(
        document.documentElement.clientWidth * window.devicePixelRatio,
        document.documentElement.clientHeight * window.devicePixelRatio
    );

    console.log(`canvasElement size set to ${canvasElement.width},${canvasElement.height} ${canvasElement.width / window.devicePixelRatio}px ${canvasElement.height / window.devicePixelRatio}px`)
}

window.addEventListener("resize", () => setTimeout(resized, 0));
setTimeout(resized, 0);

const soundFiles = {};

function playSound(filename, volume) {
    if (typeof soundFiles[filename] === "undefined") {
        soundFiles[filename] = [];
    }

    let audioElement = null;
    for (const it of soundFiles[filename]) {
        if (it.ended) {
            audioElement = it;
            break;
        }
    }

    if (!audioElement) {
        console.log(`Creating new audio element for ${filename}`);
        audioElement = new Audio(filename);
        soundFiles[filename].push(audioElement);
    }

    audioElement.currentTime = 0;
    audioElement.volume = volume;
    audioElement.play();
}