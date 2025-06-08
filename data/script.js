// Fetch and display pin numbers
fetch('/pins')
    .then(r => r.json())
    .then(pins => {
        document.getElementById('ledPin').textContent = pins.ledPin;
        document.getElementById('servoPin').textContent = pins.servoPin;
    });

// Get element references
const onS = document.getElementById('onSlider'),
    offS = document.getElementById('offSlider'),
    onV = document.getElementById('onVal'),
    offV = document.getElementById('offVal'),
    tE = document.getElementById('timerEnable'),
    tI = document.getElementById('timerInterval'),
    onBtn = document.getElementById('onBtn'),
    offBtn = document.getElementById('offBtn');
updateServoPosBtn = document.getElementById('updateServoPosBtn'),
    updateTimerBtn = document.getElementById('updateTimerBtn');

// Fetch and set current values on page load
fetch('/status')
    .then(r => r.json())
    .then(status => {
        onS.value = status.on;
        onV.textContent = status.on;
        offS.value = status.off;
        offV.textContent = status.off;
        tE.checked = !!status.timerEnabled;
        tI.value = status.timerInterval;
    });

onS.oninput = () => {
    onV.textContent = onS.value;
    sendServoPos(false);
};
offS.oninput = () => {
    offV.textContent = offS.value;
    sendServoPos(false);
};

onBtn.onclick = () => {
    fetch('/on')
        .then(r => r.text())
        .then(txt => console.log('ON:', txt))
        .catch(e => alert('Failed to turn ON'));
};
offBtn.onclick = () => {
    fetch('/off')
        .then(r => r.text())
        .then(txt => console.log('OFF:', txt))
        .catch(e => alert('Failed to turn OFF'));
};

updateServoPosBtn.onclick = () => {
    sendServoPos(true);
}
updateTimerBtn.onclick = () => {
    sendTimer(true);
}

function sendServoPos(showAlert) {
    const q = new URLSearchParams({ on: onS.value, off: offS.value });
    fetch('/set?' + q)
        .then(r => r.text())
        .then(a => { if (showAlert) alert(a); });
}
function sendTimer(showAlert) {
    const q = new URLSearchParams({ enable: tE.checked ? 1 : 0, interval: tI.value });
    fetch('/timer?' + q)
        .then(r => r.text())
        .then(a => { if (showAlert) alert(a); });
}