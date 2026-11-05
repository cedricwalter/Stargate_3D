// webpage.h
// webpage.h
const char HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stargate DHD Mainframe</title>
    <style>
        body { font-family: 'Courier New', monospace; background-color: #06070a; color: #66fcf1; text-align: center; margin: 0; padding: 20px; }
        h1 { margin-bottom: 5px; letter-spacing: 2px; text-shadow: 0 0 10px #45f3ff; }

        .container { max-width: 400px; margin: 0 auto; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); }
        .button { background-color: #007bff; border: none; color: white; padding: 14px 28px; 
                  text-align: center; font-size: 16px; margin: 10px; cursor: pointer; 
                  border-radius: 6px; width: 80%; transition: background 0.2s; font-weight: bold; }
        .button:hover { background-color: #0056b3; }
        .button-off { background-color: #dc3545; }
        .button-off:hover { background-color: #bd2130; }
        #status-msg { margin-top: 20px; font-weight: bold; color: #28a745; min-height: 20px; }
        
        /* Display Panel for Selected Address */
        .address-display { width: 360px; max-width: 90%; background: #000; border: 2px solid #66fcf1; padding: 10px; margin: 15px auto; border-radius: 8px; display: flex; justify-content: center; gap: 6px; min-height: 45px; align-items: center; box-shadow: inset 0 0 10px rgba(102, 252, 241, 0.3); }
        .chevron-slot { width: 45px; height: 40px; background: #111; border: 1px solid #45f3ff; border-radius: 4px; display: flex; flex-direction: column; align-items: center; justify-content: center; font-weight: bold; font-size: 9px; color: #aaa; text-transform: uppercase; overflow: hidden; white-space: nowrap; }
        .chevron-slot.active { background: #ff7b00; border-color: #ffae00; box-shadow: 0 0 8px #ff7b00; color: #000; }
        
        /* Concentric Circular DHD Pedestal Layout */
        .dhd-pedestal { position: relative; width: 560px; height: 560px; margin: 30px auto; background: radial-gradient(circle, #2b3542 30%, #1f2833 70%, #12171f 100%); border-radius: 50%; box-shadow: 0 0 35px rgba(69, 243, 255, 0.4); border: 8px solid #bda37e; display: flex; justify-content: center; align-items: center; }
        
        /* The Big Red Activation Hemisphere */
        .dial-btn { position: absolute; z-index: 10; background: radial-gradient(circle at 35% 35%, #ff5252, #b30000 70%, #660000 100%); border: 4px solid #bda37e; color: white; border-radius: 50%; width: 120px; height: 120px; font-weight: bold; font-size: 18px; cursor: pointer; box-shadow: 0 8px 20px rgba(0,0,0,0.6), inset -5px -5px 15px rgba(0,0,0,0.4); transition: transform 0.1s, box-shadow 0.3s; text-transform: uppercase; letter-spacing: 1px; font-family: 'Courier New', monospace; }
        .dial-btn:hover { box-shadow: 0 0 25px #ff4d4d; transform: scale(1.03); }
        .dial-btn:active { transform: scale(0.96); }
        
        /* Individual DHD Constellation Keys */
        .glyph-btn { position: absolute; width: 62px; height: 46px; background: #3c4d5c; border: 2px solid #bda37e; color: #fff; font-weight: bold; font-size: 8px; cursor: pointer; border-radius: 4px; display: flex; flex-direction: column; align-items: center; justify-content: center; transform-origin: center center; transition: all 0.2s; box-shadow: 0 3px 5px rgba(0,0,0,0.3); text-transform: uppercase; overflow: hidden; }
        .glyph-btn .id-lbl { font-size: 10px; color: #ffae00; margin-bottom: 2px; }
        .glyph-btn:hover:not(:disabled) { background: #66fcf1; color: #0b0c10; box-shadow: 0 0 12px #66fcf1; transform: scale(1.08) !important; z-index: 5; }
        .glyph-btn:hover:not(:disabled) .id-lbl { color: #0b0c10; }
        .glyph-btn:disabled { background: #1a2229; color: #556270; border-color: #4a535e; cursor: not-allowed; box-shadow: none; }
        .glyph-btn:disabled .id-lbl { color: #556270; }
        
        .controls { margin-top: 25px; }
        .clear-btn { background: #1f2833; border: 1px solid #66fcf1; color: #66fcf1; padding: 10px 20px; cursor: pointer; border-radius: 4px; font-weight: bold; letter-spacing: 1px; }
        .clear-btn:hover { background: #66fcf1; color: #0b0c10; }
        #status { margin-top: 20px; font-size: 16px; font-weight: bold; min-height: 24px; text-shadow: 0 0 5px rgba(102, 252, 241, 0.5); }
        
        /* Responsive scaling for mobile devices */
        @media (max-width: 600px) {
            .dhd-pedestal { transform: scale(0.65); margin: -80px auto; }
            .address-display { margin-top: 10px; }
        }
    </style>
</head>
<body>

    <h1>DIAL-HOME DEVICE</h1>
    <div>Milky Way Gate Array Subsystem</div>
    
    <div class="container">
        <!-- Buttons call JavaScript functions instead of normal links -->
        <button class="button" onclick="sendCommand('/handleDialing')">Random Dial</button>

        <button class="button" onclick="sendCommand('/handleEarth')">Call Earth</button>
        <button class="button" onclick="sendCommand('/handleAbydos')">Call Abydos</button>
        <button class="button" onclick="sendCommand('/handleChulak')">Call Chulak</button>

        <button class="button button-off" onclick="sendCommand('/led-off')">Turn Off LED</button>
        <button class="button button-off" onclick="sendCommand('/led-on')">Turn On LED</button>

        <div id="status-msg"></div>
    </div>

    <!-- Chevron Address Sequence Tracker -->
    <div class="address-display">
        <div class="chevron-slot" id="c1"><div>CHEV 1</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c2"><div>CHEV 2</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c3"><div>CHEV 3</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c4"><div>CHEV 4</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c5"><div>CHEV 5</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c6"><div>CHEV 6</div><b class="val">-</b></div>
        <div class="chevron-slot" id="c7"><div>ORIGIN</div><b class="val">-</b></div>
    </div>

    <!-- Circular Pedestal Setup -->
    <div class="dhd-pedestal" id="pedestal">
        <!-- Big Red Control Sphere -->
        <button class="dial-btn" onclick="engageWormhole()">Dial</button>
    </div>

    <div class="controls">
        <button class="clear-btn" onclick="clearAddress()">Reset Coordinates</button>
        <button class="disengage-btn" onclick="disengageDHD()">Disengage DHD</button>
    </div>
    
    <div id="status">Ready to input coordinates...</div>

    <script>
    // Exact canonical 39 Milky Way DHD sequence
    const constellations = [
        "Earth", "Crater", "Virgo", "Bootes", "Centaurus", 
        "Libra", "Serpens", "Norma", "Scorpius", "Cra", 
        "Scutum", "Sagittarius", "Aquila", "Micro", "Capricorn", 
        "Piscis A.", "Equuleus", "Aquarius", "Pegasus", "Sculptor", 
        "Pisces", "Andromeda", "Triangulum", "Aries", "Perseus", 
        "Cetus", "Taurus", "Auriga", "Eridanus", "Orion", 
        "Canis M.", "Monoceros", "Gemini", "Hydra", "Lynx", 
        "Cancer", "Sextans", "Leo Min.", "Leo"
    ];

    let selectedSequence = [];
    const maxSymbols = 7;
    const pedestal = document.getElementById('pedestal');

    // Generate Concentric Circular DHD Keyboards via Trigonometric offsets
    // Inner Ring: 19 keys, Outer Ring: 20 keys
    const innerRingCount = 19;
    const outerRingCount = 20;
    
    const innerRadius = 150; // Pixels from central button
    const outerRadius = 225; // Pixels from central button

    const totalGlyphs = constellations.length;

    for (let i = 0; i < totalGlyphs; i++) {
        let isOuter = i >= innerRingCount;
        let ringIndex = isOuter ? (i - innerRingCount) : i;
        let ringTotal = isOuter ? outerRingCount : innerRingCount;
        let radius = isOuter ? outerRadius : innerRadius;
        
        // Calculate coordinate positioning step-by-step
        let angle = (ringIndex / ringTotal) * 2 * Math.PI - (Math.PI / 2);
        let x = Math.round(radius * Math.cos(angle));
        let y = Math.round(radius * Math.sin(angle));
        
        let btn = document.createElement('button');
        btn.className = 'glyph-btn';
        // Display both absolute address index and short constellation name
        btn.innerHTML = `<span class="id-lbl">#${i + 1}</span><span>${constellations[i]}</span>`;
        btn.id = 'g-' + (i + 1);
        
        // Apply polar position coordinates directly
        btn.style.left = `calc(50% + ${x}px - 31px)`;
        btn.style.top = `calc(50% + ${y}px - 23px)`;
        
        // Rotate the physical buttons slightly to point outward radially
        let deg = (angle * 180 / Math.PI) + 90;
        btn.style.transform = `rotate(${deg}deg)`;
        
        btn.onclick = () => selectGlyph(i + 1);
        pedestal.appendChild(btn);
    }

    function selectGlyph(id) {
        if (selectedSequence.length < maxSymbols && !selectedSequence.includes(id)) {
            selectedSequence.push(id);
            document.getElementById('g-' + id).disabled = true;
            updateDisplay();
        }
    }

    function updateDisplay() {
        for (let i = 1; i <= maxSymbols; i++) {
            let slot = document.getElementById('c' + i);
            let valLabel = slot.querySelector('.val');
            let indexData = selectedSequence[i - 1];
            
            if (indexData !== undefined) {
                valLabel.innerText = constellations[indexData - 1];
                slot.classList.add('active');
            } else {
                valLabel.innerText = '-';
                slot.classList.remove('active');
            }
        }
    }

    function clearAddress() {
        selectedSequence.forEach(id => {
            document.getElementById('g-' + id).disabled = false;
        });
        selectedSequence = [];
        updateDisplay();
        document.getElementById('status').innerText = "Coordinates cleared.";
        document.getElementById('status').style.color = "#66fcf1";
    }

    function disengageDHD() {
        document.getElementById('status').innerText = "disengage DHD connection...";
        document.getElementById('status').style.color = "#ff7b00";

        let queryParams = "?sequence=" + selectedSequence.join(',');
        
        fetch('/disengageDHD' + queryParams)
            .then(response => response.text())
            .then(data => {
                document.getElementById('status').innerText = data;
                document.getElementById('status').style.color = "#28a745";
            })
            .catch(err => {
                document.getElementById('status').innerText = "disengage DHD failed. No response from DHD.";
                document.getElementById('status').style.color = "#ff4d4d";
            });
    }

     function engageWormhole() {
        if (selectedSequence.length < maxSymbols) {
            document.getElementById('status').innerText = "Error: 7 gate coordinates required!";
            document.getElementById('status').style.color = "#ff4d4d";
            return;
        }

        document.getElementById('status').innerText = "Establishing wormhole connection...";
        document.getElementById('status').style.color = "#ff7b00";

        let queryParams = "?sequence=" + selectedSequence.join(',');
        
        fetch('/dial' + queryParams)
            .then(response => response.text())
            .then(data => {
                document.getElementById('status').innerText = data;
                document.getElementById('status').style.color = "#28a745";
            })
            .catch(err => {
                document.getElementById('status').innerText = "Chevron lock failed. No response from DHD.";
                document.getElementById('status').style.color = "#ff4d4d";
            });
    }
    
    // This function sends a stealth request to the D1 Mini in the background
    function sendCommand(endpoint) {
        let msgDiv = document.getElementById("status-msg");
        
        fetch(endpoint)
            .then(response => {
                if(response.ok) {
                    msgDiv.innerText = "Command executed successfully!";
                    msgDiv.style.color = "#28a745";
                } else {
                    msgDiv.innerText = "Server error occurred.";
                    msgDiv.style.color = "#dc3545";
                }
                // Clear the status message after 2 seconds
                setTimeout(() => { msgDiv.innerText = ""; }, 2000);
            })
            .catch(err => {
                msgDiv.innerText = "Failed to connect to D1 Mini.";
                msgDiv.style.color = "#dc3545";
            });
    }

    </script>
</body>
</html>
)=====";
