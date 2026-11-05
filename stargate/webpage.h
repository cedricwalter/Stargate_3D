// webpage.h
const char HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stargate</title>
    <style>
        body { font-family: 'Segoe UI', Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f0f2f5; color: #333; }
        .container { max-width: 400px; margin: 0 auto; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.1); }
        .button { background-color: #007bff; border: none; color: white; padding: 14px 28px; 
                  text-align: center; font-size: 16px; margin: 10px; cursor: pointer; 
                  border-radius: 6px; width: 80%; transition: background 0.2s; font-weight: bold; }
        .button:hover { background-color: #0056b3; }
        .button-off { background-color: #dc3545; }
        .button-off:hover { background-color: #bd2130; }
        #status-msg { margin-top: 20px; font-weight: bold; color: #28a745; min-height: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Control your stargate</h2>
        <p>Control your device seamlessly.</p>
        
        <!-- Buttons call JavaScript functions instead of normal links -->
        <button class="button" onclick="sendCommand('/handleDialing')">Dial</button>
        <button class="button" onclick="sendCommand('/handleDemo')">Demo</button>
        <button class="button" onclick="sendCommand('/handlePlayTheme')">Play theme</button>
        <button class="button" onclick="sendCommand('/handleChevronDemo')">Run chevron demo</button>
        <button class="button" onclick="sendCommand('/handleStopAll')">Stop all</button>

        <button class="button button-off" onclick="sendCommand('/led-off')">Turn Off LED</button>
        <button class="button button-off" onclick="sendCommand('/led-on')">Turn On LED</button>

        <div id="status-msg"></div>
    </div>

    <script>
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