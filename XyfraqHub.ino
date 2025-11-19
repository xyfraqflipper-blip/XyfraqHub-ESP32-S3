#include <WiFi.h>
#include <WebServer.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

// WiFi AP
const char* ssid = "XyfraqESP-v2.0.0";
const char* password = "XyfraqESP";

WebServer server(80);
USBHIDKeyboard Keyboard;

String serialBuffer = "";

// Dashboard HTML (modern dark mode ohne Emojis)
const char dashboardHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
:root {
  --primary: #7c3aed;
  --primary-hover: #6d28d9;
  --bg-dark: #0f0f0f;
  --bg-card: #1a1a1a;
  --bg-card-hover: #252525;
  --text-primary: #ffffff;
  --text-secondary: #a1a1aa;
  --border: #2d2d2d;
  --success: #10b981;
  --warning: #f59e0b;
  --danger: #ef4444;
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Segoe UI', system-ui, sans-serif;
  background: var(--bg-dark);
  color: var(--text-primary);
  line-height: 1.6;
  min-height: 100vh;
  background: linear-gradient(135deg, #0f0f0f 0%, #1a1a1a 100%);
}

.container {
  max-width: 800px;
  margin: 0 auto;
  padding: 20px;
}

.header {
  text-align: center;
  margin-bottom: 2rem;
  animation: fadeInDown 0.8s ease;
}

.header h1 {
  font-size: 2.5rem;
  font-weight: 700;
  background: linear-gradient(135deg, var(--primary), #c4b5fd);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  margin-bottom: 1rem;
}

.header p {
  color: var(--text-secondary);
  font-size: 1.1rem;
  margin-bottom: 1.5rem;
}

.nav-tabs {
  display: flex;
  justify-content: center;
  gap: 0.5rem;
  margin-bottom: 2rem;
  flex-wrap: wrap;
}

.nav-btn {
  background: var(--bg-card);
  border: 1px solid var(--border);
  color: var(--text-primary);
  padding: 0.6rem 1.2rem;
  border-radius: 8px;
  font-size: 0.85rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.3s ease;
  text-decoration: none;
}

.nav-btn:hover {
  background: var(--bg-card-hover);
  border-color: var(--primary);
  transform: translateY(-1px);
}

.nav-btn.active {
  background: var(--primary);
  border-color: var(--primary);
}

.page {
  display: none;
  animation: fadeIn 0.5s ease;
}

.page.active {
  display: block;
}

.section {
  background: var(--bg-card);
  border-radius: 16px;
  padding: 1.5rem;
  margin-bottom: 1.5rem;
  border: 1px solid var(--border);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  animation: slideUp 0.6s ease;
  transition: transform 0.3s ease, box-shadow 0.3s ease;
}

.section:hover {
  transform: translateY(-2px);
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
}

.section h2 {
  font-size: 1.3rem;
  margin-bottom: 1rem;
  color: var(--text-primary);
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.section h2::before {
  content: '';
  width: 4px;
  height: 20px;
  background: var(--primary);
  border-radius: 2px;
}

.button-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 0.75rem;
}

.btn {
  background: var(--bg-card);
  border: 1px solid var(--border);
  color: var(--text-primary);
  padding: 1rem;
  border-radius: 12px;
  font-size: 0.9rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
}

.btn::before {
  content: '';
  position: absolute;
  top: 0;
  left: -100%;
  width: 100%;
  height: 100%;
  background: linear-gradient(90deg, transparent, rgba(124, 58, 237, 0.1), transparent);
  transition: left 0.5s ease;
}

.btn:hover {
  background: var(--bg-card-hover);
  border-color: var(--primary);
  transform: translateY(-2px);
  box-shadow: 0 8px 25px rgba(124, 58, 237, 0.2);
}

.btn:hover::before {
  left: 100%;
}

.btn:active {
  transform: translateY(0);
}

.terminal {
  background: #000;
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 1rem;
  font-family: 'Courier New', monospace;
  font-size: 0.9rem;
  height: 400px;
  overflow-y: auto;
  color: #00ff00;
  margin-bottom: 1rem;
}

.terminal-line {
  margin-bottom: 0.25rem;
}

.credits-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 1rem;
}

.credit-card {
  background: var(--bg-card);
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 1.5rem;
  text-align: center;
  transition: all 0.3s ease;
}

.credit-card:hover {
  transform: translateY(-5px);
  border-color: var(--primary);
}

.credit-card h3 {
  color: var(--primary);
  margin-bottom: 0.5rem;
}

.credit-card p {
  color: var(--text-secondary);
  margin-bottom: 1rem;
}

.discord-btn {
  background: #5865f2;
  color: white;
  border: none;
  padding: 0.75rem 1.5rem;
  border-radius: 8px;
  cursor: pointer;
  font-weight: 500;
  transition: all 0.3s ease;
}

.discord-btn:hover {
  background: #4752c4;
  transform: translateY(-2px);
}

.status-bar {
  position: fixed;
  bottom: 0;
  left: 0;
  right: 0;
  background: var(--bg-card);
  border-top: 1px solid var(--border);
  padding: 0.5rem 1rem;
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 0.8rem;
}

.status-indicator {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  color: var(--text-secondary);
}

.indicator {
  width: 6px;
  height: 6px;
  border-radius: 50%;
  background: var(--success);
  animation: pulse 2s infinite;
}

@keyframes fadeInDown {
  from {
    opacity: 0;
    transform: translateY(-30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

@keyframes slideUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

@keyframes pulse {
  0%, 100% {
    opacity: 1;
    transform: scale(1);
  }
  50% {
    opacity: 0.7;
    transform: scale(1.1);
  }
}

@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

@media (max-width: 768px) {
  .container {
    padding: 1rem;
  }
  
  .header h1 {
    font-size: 2rem;
  }
  
  .button-grid {
    grid-template-columns: 1fr;
  }
  
  .btn {
    padding: 1.2rem;
    font-size: 1rem;
  }
  
  .credits-grid {
    grid-template-columns: 1fr;
  }
  
  .nav-tabs {
    flex-direction: column;
    align-items: center;
  }
  
  .status-bar {
    flex-direction: column;
    gap: 0.25rem;
    text-align: center;
  }
}
</style>
</head>
<body>
<div class="container">
  <div class="header">
    <h1>Xyfraq Hub</h1>
    <p>Remote control your Windows system</p>
    
    <div class="nav-tabs">
      <a href="#" class="nav-btn active" onclick="showPage('hub')">Hub</a>
      <a href="#" class="nav-btn" onclick="showPage('cmd')">Serial Monitor</a>
      <a href="#" class="nav-btn" onclick="showPage('credits')">Credits</a>
    </div>
  </div>

  <!-- Hub Page -->
  <div id="hub-page" class="page active">
    <div class="section" style="animation-delay: 0.1s">
      <h2>System Controls</h2>
      <div class="button-grid">
        <button class="btn" onclick="sendCmd('taskmgr')">Task Manager</button>
        <button class="btn" onclick="sendCmd('powershell')">PowerShell</button>
        <button class="btn" onclick="sendCmd('run')">Run Dialog</button>
        <button class="btn" onclick="sendCmd('logoff')">Log Off</button>
        <button class="btn" onclick="sendCmd('shutdown')">Shutdown</button>
        <button class="btn" onclick="sendCmd('restart')">Restart</button>
        <button class="btn" onclick="sendCmd('lock')">Lock Screen</button>
      </div>
    </div>

    <div class="section" style="animation-delay: 0.2s">
      <h2>Windows Utilities</h2>
      <div class="button-grid">
        <button class="btn" onclick="sendCmd('settings')">Open Settings</button>
        <button class="btn" onclick="sendCmd('taskview')">Task View</button>
        <button class="btn" onclick="sendCmd('explorer')">File Explorer</button>
        <button class="btn" onclick="sendCmd('clipboard')">Clipboard History</button>
        <button class="btn" onclick="sendCmd('snipping')">Snipping Tool</button>
        <button class="btn" onclick="sendCmd('cmd')">Open CMD</button>
      </div>
    </div>

    <div class="section" style="animation-delay: 0.3s">
      <h2>Window Management</h2>
      <div class="button-grid">
        <button class="btn" onclick="sendCmd('maximize')">Maximize Window</button>
        <button class="btn" onclick="sendCmd('minimize')">Minimize Window</button>
        <button class="btn" onclick="sendCmd('close')">Close Window</button>
        <button class="btn" onclick="sendCmd('showdesktop')">Show Desktop</button>
      </div>
    </div>

    <div class="section" style="animation-delay: 0.4s">
      <h2>Quick Actions</h2>
      <div class="button-grid">
        <button class="btn" onclick="sendCmd('mute')">Mute/Unmute</button>
        <button class="btn" onclick="sendCmd('volup')">Volume Up</button>
        <button class="btn" onclick="sendCmd('voldown')">Volume Down</button>
        <button class="btn" onclick="sendCmd('playpause')">Play/Pause</button>
        <button class="btn" onclick="sendCmd('screenshot')">Take Screenshot</button>
        <button class="btn" onclick="sendCmd('project')">Project Screen</button>
      </div>
    </div>
  </div>

  <!-- Serial Monitor Page -->
  <div id="cmd-page" class="page">
    <div class="section">
      <h2>Serial Monitor</h2>
      <div class="terminal" id="terminal">
        <div class="terminal-line">Xyfraq Hub Serial Monitor v2.0.0</div>
        <div class="terminal-line">Connected to ESP32...</div>
        <div class="terminal-line">Waiting for data...</div>
      </div>
      <div style="display: flex; gap: 0.5rem;">
        <button class="btn" onclick="clearTerminal()">Clear Terminal</button>
        <button class="btn" onclick="refreshSerial()">Refresh Data</button>
      </div>
    </div>
  </div>

  <!-- Credits Page -->
  <div id="credits-page" class="page">
    <div class="section">
      <h2>Project Information</h2>
      <div class="credits-grid">
        <div class="credit-card">
          <h3>Developer</h3>
          <p>Xyfraq</p>
        </div>
        <div class="credit-card">
          <h3>Version</h3>
          <p>v2.0.0</p>
        </div>
        <div class="credit-card">
          <h3>Purpose</h3>
          <p>Education & Research</p>
        </div>
        <div class="credit-card">
          <h3>Disclaimer</h3>
          <p>For educational use only</p>
        </div>
      </div>
    </div>

    <div class="section">
      <h2>Join Our Community</h2>
      <div style="text-align: center;">
        <p style="margin-bottom: 1rem; color: var(--text-secondary);">
          Join our Discord server for updates, support, and community discussions!
        </p>
        <button class="discord-btn" onclick="joinDiscord()">
          Join Discord (hVWamQC249)
        </button>
      </div>
    </div>

    <div class="section">
      <h2>Features</h2>
      <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 1rem;">
        <div>
          <h4 style="color: var(--primary); margin-bottom: 0.5rem;">System Control</h4>
          <ul style="color: var(--text-secondary); padding-left: 1.5rem;">
            <li>Remote Task Manager</li>
            <li>PowerShell Access</li>
            <li>System Shutdown/Restart</li>
            <li>Screen Lock</li>
          </ul>
        </div>
        <div>
          <h4 style="color: var(--primary); margin-bottom: 0.5rem;">Utilities</h4>
          <ul style="color: var(--text-secondary); padding-left: 1.5rem;">
            <li>File Explorer</li>
            <li>Settings Access</li>
            <li>Snipping Tool</li>
            <li>Clipboard History</li>
          </ul>
        </div>
      </div>
    </div>
  </div>
</div>

<div class="status-bar">
  <div class="status-indicator">
    <div class="indicator"></div>
    <span>Connected</span>
  </div>
  <div class="status-indicator">
    <span>Xyfraq Hub v2.0.0 | Educational Use</span>
  </div>
</div>

<script>
let currentPage = 'hub';

function showPage(page) {
  // Hide all pages
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(btn => btn.classList.remove('active'));
  
  // Show selected page
  document.getElementById(page + '-page').classList.add('active');
  event.target.classList.add('active');
  currentPage = page;
  
  // Refresh serial data if on CMD page
  if (page === 'cmd') {
    refreshSerial();
  }
}

function sendCmd(cmd) {
  const btn = event.target;
  const originalText = btn.textContent;
  
  // Add loading animation
  btn.innerHTML = 'Loading...';
  btn.style.opacity = '0.8';
  btn.style.cursor = 'wait';
  
  fetch('/cmd?do=' + cmd)
    .then(response => response.text())
    .then(data => {
      // Success animation
      btn.style.background = '#10b981';
      btn.style.borderColor = '#10b981';
      btn.innerHTML = 'Success';
      
      setTimeout(() => {
        btn.style.background = '';
        btn.style.borderColor = '';
        btn.style.opacity = '1';
        btn.style.cursor = 'pointer';
        btn.textContent = originalText;
      }, 1000);
    })
    .catch(error => {
      // Error animation
      btn.style.background = '#ef4444';
      btn.style.borderColor = '#ef4444';
      btn.innerHTML = 'Error';
      
      setTimeout(() => {
        btn.style.background = '';
        btn.style.borderColor = '';
        btn.style.opacity = '1';
        btn.style.cursor = 'pointer';
        btn.textContent = originalText;
      }, 1000);
    });
}

function refreshSerial() {
  fetch('/serial')
    .then(response => response.text())
    .then(data => {
      const terminal = document.getElementById('terminal');
      terminal.innerHTML = '<div class="terminal-line">' + data.replace(/\n/g, '</div><div class="terminal-line">') + '</div>';
      terminal.scrollTop = terminal.scrollHeight;
    })
    .catch(error => {
      console.error('Error fetching serial data:', error);
    });
}

function clearTerminal() {
  document.getElementById('terminal').innerHTML = '<div class="terminal-line">Terminal cleared</div>';
}

function joinDiscord() {
  alert('Discord Invite Code: hVWamQC249\n\nCopy this code and use it to join our Discord server!');
}

// Auto-refresh serial data every 2 seconds if on CMD page
setInterval(() => {
  if (currentPage === 'cmd') {
    refreshSerial();
  }
}, 2000);

// Add staggered animation delays to sections
document.addEventListener('DOMContentLoaded', function() {
  const sections = document.querySelectorAll('.section');
  sections.forEach((section, index) => {
    section.style.animationDelay = (index * 0.1) + 's';
  });
});
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", dashboardHTML);
}

void handleCmd() {
  if(!server.hasArg("do")){
    server.send(400, "text/plain", "Missing argument");
    return;
  }

  String cmd = server.arg("do");

  // Log command to serial
  Serial.println("Command received: " + cmd);
  serialBuffer += "> Command: " + cmd + "\n";

  // --- System ---
  if(cmd == "taskmgr"){
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_ESC);
    Keyboard.releaseAll();
    serialBuffer += "Opened Task Manager\n";
  } else if(cmd == "powershell"){
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('r'); Keyboard.releaseAll(); delay(300);
    Keyboard.print("powershell"); Keyboard.write('\n');
    serialBuffer += "Opened PowerShell\n";
  } else if(cmd == "run"){
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('r'); Keyboard.releaseAll();
    serialBuffer += "Opened Run Dialog\n";
  } else if(cmd == "logoff"){
    Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_F4); Keyboard.releaseAll(); delay(200); Keyboard.write('\n');
    serialBuffer += "Initiated Log Off\n";
  } else if(cmd == "shutdown"){
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('r'); Keyboard.releaseAll(); delay(300);
    Keyboard.print("shutdown /s /t 0"); Keyboard.write('\n');
    serialBuffer += "Initiated Shutdown\n";
  } else if(cmd == "restart"){
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('r'); Keyboard.releaseAll(); delay(300);
    Keyboard.print("shutdown /r /t 0"); Keyboard.write('\n');
    serialBuffer += "Initiated Restart\n";
  } else if(cmd == "lock"){
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('l'); Keyboard.releaseAll();
    serialBuffer += "Locked Screen\n";
  }

  // --- Windows Utilities ---
  else if(cmd == "settings"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('i'); Keyboard.releaseAll(); 
    serialBuffer += "Opened Settings\n";
  }
  else if(cmd == "taskview"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_TAB); Keyboard.releaseAll(); 
    serialBuffer += "Opened Task View\n";
  }
  else if(cmd == "explorer"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('e'); Keyboard.releaseAll(); 
    serialBuffer += "Opened File Explorer\n";
  }
  else if(cmd == "clipboard"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('v'); Keyboard.releaseAll(); 
    serialBuffer += "Opened Clipboard History\n";
  }
  else if(cmd == "snipping"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('s'); Keyboard.releaseAll(); 
    serialBuffer += "Opened Snipping Tool\n";
  }
  else if(cmd == "cmd"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('r'); Keyboard.releaseAll(); delay(300); 
    Keyboard.print("cmd"); Keyboard.write('\n');
    serialBuffer += "Opened CMD\n";
  }

  // --- Window Management ---
  else if(cmd == "maximize"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press(0xDA); Keyboard.releaseAll(); 
    serialBuffer += "Maximized Window\n";
  }
  else if(cmd == "minimize"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press(0xD9); Keyboard.releaseAll(); 
    serialBuffer += "Minimized Window\n";
  }
  else if(cmd == "close"){ 
    Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_F4); Keyboard.releaseAll(); 
    serialBuffer += "Closed Window\n";
  }
  else if(cmd == "showdesktop"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('d'); Keyboard.releaseAll(); 
    serialBuffer += "Showed Desktop\n";
  }

  // --- Quick Actions ---
  else if(cmd == "mute"){ 
    Keyboard.press(KEY_RIGHT_CTRL); Keyboard.press(KEY_F1); Keyboard.releaseAll();
    serialBuffer += "Toggled Mute\n";
  }
  else if(cmd == "volup"){ 
    Keyboard.press(KEY_RIGHT_CTRL); Keyboard.press(KEY_F2); Keyboard.releaseAll();
    serialBuffer += "Volume Up\n";
  }
  else if(cmd == "voldown"){ 
    Keyboard.press(KEY_RIGHT_CTRL); Keyboard.press(KEY_F3); Keyboard.releaseAll();
    serialBuffer += "Volume Down\n";
  }
  else if(cmd == "playpause"){ 
    Keyboard.press(KEY_RIGHT_CTRL); Keyboard.press(KEY_F4); Keyboard.releaseAll();
    serialBuffer += "Play/Pause Media\n";
  }
  else if(cmd == "screenshot"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_PRINT_SCREEN); Keyboard.releaseAll();
    serialBuffer += "Took Screenshot\n";
  }
  else if(cmd == "project"){ 
    Keyboard.press(KEY_LEFT_GUI); Keyboard.press('p'); Keyboard.releaseAll();
    serialBuffer += "Opened Project Menu\n";
  }

  server.send(200, "text/plain", "OK");
}

void handleSerial() {
  server.send(200, "text/plain", serialBuffer);
}

void setup() {
  Serial.begin(115200);
  USB.begin();
  Keyboard.begin();
  WiFi.softAP(ssid, password);
  
  Serial.println("\n=== Xyfraq Hub v2.0.0 ===");
  Serial.println("Educational Use Only");
  Serial.println("Created by Xyfraq");
  Serial.println("Discord: hVWamQC249");
  Serial.println("======================");
  Serial.print("AP IP: "); 
  Serial.println(WiFi.softAPIP());

  serialBuffer = "Xyfraq Hub Started\nAP IP: " + WiFi.softAPIP().toString() + "\nReady for commands...\n";

  server.on("/", handleRoot);
  server.on("/hub", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/serial", handleSerial);
  server.begin();
}

void loop() {
  server.handleClient();
  
  // Read from Serial and add to buffer
  if (Serial.available()) {
    String serialData = Serial.readString();
    serialBuffer += serialData;
    
    // Keep buffer from growing too large
    if (serialBuffer.length() > 2000) {
      serialBuffer = serialBuffer.substring(serialBuffer.length() - 1500);
    }
  }
}
