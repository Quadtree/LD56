console.log(`main.js`)

export function resized() {
    const canvasElement = document.getElementById("canvas");

    if (window.devicePixelRatio != 1) {
        canvasElement.width = Math.round(document.documentElement.clientWidth * window.devicePixelRatio);
        canvasElement.height = Math.round(document.documentElement.clientHeight * window.devicePixelRatio);
        canvasElement.style.width = `${canvasElement.width / window.devicePixelRatio}px`;
        canvasElement.style.height = `${canvasElement.height / window.devicePixelRatio}px`;
    }

    if (typeof Module.InjectPageSize === "function") {
        Module.InjectPageSize(
            document.documentElement.clientWidth * window.devicePixelRatio,
            document.documentElement.clientHeight * window.devicePixelRatio
        );
    }

    console.log(`canvasElement size set to ${canvasElement.width},${canvasElement.height} ${canvasElement.width / window.devicePixelRatio}px ${canvasElement.height / window.devicePixelRatio}px`)
}

window.addEventListener("resize", () => setTimeout(resized, 0));
setTimeout(resized, 0);
