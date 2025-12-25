/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.web_content.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.web_content.h"

String getHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Plant Monitoring System v3.0</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 50%, #f093fb 100%);
      min-height: 100vh;
      color: #fff;
      overflow-x: hidden;
    }
    .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
    .page-section { display: block; animation: fadeIn 0.4s ease-in-out; }
    .hidden { display: none !important; }
    @keyframes fadeIn { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }
    .header { text-align: center; margin-bottom: 30px; }
    h1 { font-size: 2em; font-weight: 700; text-shadow: 2px 2px 8px rgba(0,0,0,0.3); margin-bottom: 10px; display: flex; align-items: center; justify-content: center; gap: 15px; flex-wrap: wrap; }
    .version-badge { font-size: 0.4em; background: rgba(255,255,255,0.2); padding: 4px 12px; border-radius: 20px; font-weight: 500; }
    .status-badge { display: inline-block; padding: 8px 20px; border-radius: 25px; font-size: 0.8em; font-weight: 600; text-transform: uppercase; letter-spacing: 1px; background: rgba(0,0,0,0.3); backdrop-filter: blur(10px); }
    .status-badge.connected { background: linear-gradient(135deg, #4CAF50, #8BC34A); box-shadow: 0 4px 15px rgba(76, 175, 80, 0.4); }
    .status-badge.updating { background: linear-gradient(135deg, #FF9800, #FF5722); animation: pulse 1.5s infinite; }
    @keyframes pulse { 0%, 100% { opacity: 1; transform: scale(1); } 50% { opacity: 0.8; transform: scale(1.05); } }
    .nav-tabs { display: flex; gap: 10px; margin-bottom: 25px; overflow-x: auto; padding-bottom: 10px; }
    .nav-tab { flex: 1; min-width: 120px; padding: 12px 20px; background: rgba(255,255,255,0.1); border: 2px solid rgba(255,255,255,0.2); border-radius: 12px; cursor: pointer; text-align: center; font-weight: 600; transition: all 0.3s; backdrop-filter: blur(10px); }
    .nav-tab:hover { background: rgba(255,255,255,0.2); transform: translateY(-2px); }
    .nav-tab.active { background: rgba(255,255,255,0.25); border-color: rgba(255,255,255,0.5); box-shadow: 0 4px 15px rgba(0,0,0,0.2); }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr)); gap: 15px; margin-bottom: 25px; }
    .card, .panel { background: rgba(255,255,255,0.15); backdrop-filter: blur(10px); border-radius: 20px; padding: 20px; border: 1px solid rgba(255,255,255,0.2); box-shadow: 0 8px 32px rgba(0,0,0,0.1); transition: all 0.3s; }
    .card:hover { transform: translateY(-5px); box-shadow: 0 12px 40px rgba(0,0,0,0.2); }
    .card-icon { font-size: 2em; margin-bottom: 10px; opacity: 0.9; }
    .card-title { font-size: 0.75em; opacity: 0.9; margin-bottom: 8px; text-transform: uppercase; letter-spacing: 1px; }
    .card-value { font-size: 2.5em; font-weight: 700; line-height: 1; margin-bottom: 5px; }
    .card-unit { font-size: 1em; opacity: 0.8; font-weight: 500; }
    .panel h2, .panel h3 { margin-bottom: 20px; font-size: 1.3em; font-weight: 600; }
    .progress-bar { width: 100%; height: 14px; background: rgba(0,0,0,0.2); border-radius: 10px; overflow: hidden; margin-top: 10px; position: relative; }
    .progress-fill { height: 100%; background: linear-gradient(90deg, #4CAF50, #8BC34A, #CDDC39); transition: width 0.5s ease; border-radius: 10px; position: relative; overflow: hidden; }
    .progress-fill::after { content: ''; position: absolute; top: 0; left: 0; right: 0; bottom: 0; background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent); animation: shimmer 2s infinite; }
    @keyframes shimmer { 0% { transform: translateX(-100%); } 100% { transform: translateX(100%); } }
    .chart-container { position: relative; height: 250px; background: rgba(0,0,0,0.2); border-radius: 15px; padding: 15px; margin-bottom: 20px; }
    #dataChart { width: 100% !important; height: 100% !important; }
    .control-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; padding: 15px; background: rgba(255,255,255,0.1); border-radius: 12px; transition: all 0.3s; }
    .control-row:hover { background: rgba(255,255,255,0.15); }
    .control-label { display: flex; flex-direction: column; gap: 5px; }
    .control-label .subtitle { font-size: 0.8em; opacity: 0.7; }
    .switch { position: relative; display: inline-block; width: 56px; height: 30px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: rgba(0,0,0,0.3); transition: .4s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 22px; width: 22px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; box-shadow: 0 2px 5px rgba(0,0,0,0.2); }
    input:checked + .slider { background: linear-gradient(135deg, #4CAF50, #8BC34A); }
    input:checked + .slider:before { transform: translateX(26px); }
    .btn { width: 100%; padding: 15px; border: none; border-radius: 12px; font-size: 1em; font-weight: 600; cursor: pointer; margin-top: 15px; transition: all 0.3s; color: white; text-transform: uppercase; letter-spacing: 1px; }
    .btn-primary { background: linear-gradient(135deg, #667eea, #764ba2); box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4); }
    .btn-primary:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(102, 126, 234, 0.6); }
    .btn-secondary { background: rgba(255,255,255,0.2); border: 2px solid rgba(255,255,255,0.3); }
    .btn-secondary:hover { background: rgba(255,255,255,0.3); }
    .btn-danger { background: linear-gradient(135deg, #f093fb, #f5576c); box-shadow: 0 4px 15px rgba(245, 87, 108, 0.4); }
    .btn:active { transform: scale(0.98); }
    input[type="text"], input[type="password"], input[type="number"], select { width: 100%; padding: 15px; margin: 10px 0; border-radius: 12px; border: 2px solid rgba(255,255,255,0.3); background: rgba(255,255,255,0.1); color: white; font-size: 1em; transition: all 0.3s; }
    input:focus, select:focus { outline: none; border-color: rgba(255,255,255,0.6); background: rgba(255,255,255,0.15); }
    ::placeholder { color: rgba(255,255,255,0.6); }
    select option { background: #764ba2; color: white; }
    .preset-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-top: 15px; }
    .preset-card { background: rgba(255,255,255,0.1); padding: 15px; border-radius: 12px; border: 2px solid rgba(255,255,255,0.2); cursor: pointer; transition: all 0.3s; }
    .preset-card:hover { background: rgba(255,255,255,0.2); border-color: rgba(255,255,255,0.4); transform: translateY(-3px); }
    .preset-card.active { border-color: #4CAF50; background: rgba(76, 175, 80, 0.2); }
    .preset-name { font-weight: 600; font-size: 1.1em; margin-bottom: 5px; }
    .preset-desc { font-size: 0.85em; opacity: 0.8; margin-bottom: 8px; }
    .preset-values { font-size: 0.8em; opacity: 0.7; }
    .info-box { background: rgba(33, 150, 243, 0.2); border-left: 4px solid #2196F3; padding: 15px; border-radius: 8px; margin: 15px 0; font-size: 0.9em; }
    .warning-box { background: rgba(255, 152, 0, 0.2); border-left: 4px solid #FF9800; padding: 15px; border-radius: 8px; margin: 15px 0; font-size: 0.9em; }
    .wifi-info { background: rgba(0,0,0,0.2); padding: 15px; border-radius: 12px; margin-bottom: 20px; }
    .wifi-info-row { display: flex; justify-content: space-between; padding: 8px 0; border-bottom: 1px solid rgba(255,255,255,0.1); }
    .wifi-info-row:last-child { border-bottom: none; }
    .wifi-label { opacity: 0.8; }
    .wifi-value { font-weight: 600; }
    @media (max-width: 768px) {
      .container { padding: 15px; }
      h1 { font-size: 1.5em; }
      .grid { grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); }
      .card-value { font-size: 2em; }
      .nav-tabs { gap: 5px; }
      .nav-tab { min-width: 100px; padding: 10px 15px; font-size: 0.9em; }
    }
  </style>
</head>
<body>

  <div id="page-dashboard" class="page-section">
    <div class="container">
      <div class="header">
        <h1>Smart Plant Monitoring System <span class="version-badge">v3.0</span></h1>
        <span id="connection-status" class="status-badge updating">Menghubungkan...</span>
      </div>

      <div class="nav-tabs">
        <div class="nav-tab active" onclick="switchTab('overview')">Overview</div>
        <div class="nav-tab" onclick="switchTab('chart')">Grafik</div>
        <div class="nav-tab" onclick="switchTab('settings')">Pengaturan</div>
      </div>

      <div id="tab-overview" class="tab-content">
        <div class="grid">
          <div class="card">
            <div class="card-icon">🌡️</div>
            <div class="card-title">Suhu Udara</div>
            <span class="card-value" id="airTemp">--</span> <span class="card-unit">°C</span>
          </div>
          <div class="card">
            <div class="card-icon">💧</div>
            <div class="card-title">Kelembaban Udara</div>
            <span class="card-value" id="airHumidity">--</span> <span class="card-unit">%</span>
          </div>
          <div class="card">
            <div class="card-icon">🌡️</div>
            <div class="card-title">Suhu Tanah</div>
            <span class="card-value" id="soilTemp">--</span> <span class="card-unit">°C</span>
          </div>
          <div class="card">
            <div class="card-icon">🌿</div>
            <div class="card-title">Kelembaban Tanah</div>
            <span class="card-value" id="soilMoisture">--</span> <span class="card-unit">%</span>
            <div class="progress-bar"><div class="progress-fill" id="moistureBar" style="width:0%"></div></div>
          </div>
        </div>
         
        <div class="panel">
          <h3>Analisis AI Machine Learning</h3>
          <div style="text-align:center; margin:20px 0;">
            <div style="font-size:3em; font-weight:bold;" id="mlScore">NULL</div>
            <div style="font-size:0.9em; opacity:0.8;">Skor Keputusan (0 = Tidak Perlu | 1 = Perlu Air)</div>
          </div>
          <div class="progress-bar"><div class="progress-fill" id="mlBar" style="width:0%"></div></div>
          <div class="info-box" style="margin-top:15px;">
            <strong>💡 Info:</strong> Sistem Machine Learning akan menghitung kebutuhan air berdasarkan 4 faktor yakni:
            kelembaban tanah (50%), suhu tanah (20%), kelembaban udara (20%), dan suhu udara (10%).
          </div>
        </div>
      </div>

      <div id="tab-chart" class="tab-content hidden">
        <div class="panel">
          <h3>Grafik Data Historis (24 Jam)</h3>
          <div class="chart-container"><canvas id="dataChart"></canvas></div>
          <div class="info-box">
            <strong>💡 Info:</strong> Data akan diperbarui setiap 20 menit. Grafik akan menampilkan 72 titik data yang di dapatkan dalam 24 jam terakhir.
          </div>
        </div>
      </div>

      <div id="tab-settings" class="tab-content hidden">
        <div class="panel">
          <h3>📡 Konfigurasi WiFi</h3>
          <div class="wifi-info">
            <div class="wifi-info-row"><span class="wifi-label">Status:</span><span class="wifi-value" id="wifi-status">NaN</span></div>
            <div class="wifi-info-row"><span class="wifi-label">SSID:</span><span class="wifi-value" id="wifi-ssid">NaN</span></div>
            <div class="wifi-info-row"><span class="wifi-label">IP Address:</span><span class="wifi-value" id="wifi-ip">NaN</span></div>
            <div class="wifi-info-row"><span class="wifi-label">Signal (RSSI):</span><span class="wifi-value" id="wifi-rssi">NaN</span></div>
          </div>
          <input type="text" id="wifiSSID" placeholder="Nama WiFi (SSID)">
          <input type="password" id="wifiPassword" placeholder="Password WiFi">
          <button class="btn btn-primary" onclick="saveWiFi()">Simpan & Hubungkan</button>
          <div id="wifiMsg" style="text-align:center; margin-top:10px; font-weight:bold; white-space: pre-wrap;"></div>
        </div>

        <div class="panel">
          <h3>Konfigurasi IP Address</h3>
          <div class="control-row">
            <span>Gunakan IP Statis</span>
            <label class="switch"><input type="checkbox" id="useStaticIP" onchange="toggleStaticIPFields()"><span class="slider"></span></label>
          </div>
          <div id="staticIPFields" style="display:none;">
            <input type="text" id="staticIP" placeholder="IP Address (Contoh, 192.168.1.100)">
            <input type="text" id="gateway" placeholder="Gateway (Contoh, 192.168.1.1)">
            <input type="text" id="subnet" placeholder="Subnet Mask (Contoh, 255.255.255.0)">
            <input type="text" id="dns" placeholder="DNS Server (opsional)">
            <div class="info-box">
              <strong>💡 Tips:</strong> Pastikan IP yang dipilih tidak digunakan perangkat lain. Restart diperlukan setelah mengubah konfigurasi IP.
            </div>
          </div>
          <button class="btn btn-primary" onclick="saveStaticIP()">Simpan Konfigurasi IP</button>
        </div>

        <div class="panel">
          <h3>Kontrol Perangkat</h3>
          <div class="control-row">
            <div class="control-label"><span>Valve/Pompa Air</span><span class="subtitle">Kontrol manual valve air</span></div>
            <label class="switch"><input type="checkbox" id="relaySwitch" onchange="toggleRelay()"><span class="slider"></span></label>
          </div>
          <div class="control-row">
            <div class="control-label"><span>Mode Otomatis (AI)</span><span class="subtitle">Sistem AI menentukan kapan menyiram</span></div>
            <label class="switch"><input type="checkbox" id="autoSwitch" onchange="toggleAuto()" checked><span class="slider"></span></label>
          </div>
          <div class="warning-box">
            <strong>⚠️ Perhatian:</strong> Saat mode otomatis aktif, kontrol manual valve akan dinonaktifkan.
          </div>
        </div>

        <div class="panel">
          <h3>Konfigurasi Machine Learning</h3>
          <label style="display:block; margin-top:15px; opacity:0.9;">Threshold AI (0.0 - 1.0):</label>
          <input type="number" id="mlThreshold" min="0" max="1" step="0.01" value="0.45">
          <div style="font-size:0.85em; opacity:0.8; margin-top:5px;">Nilai saat ini: <strong id="currentThreshold">0.45</strong></div>
          <label style="display:block; margin-top:20px; opacity:0.9;">Target Kelembaban Optimal (%):</label>
          <input type="number" id="optimalMoisture" min="0" max="100" step="5" value="60">
          <button class="btn btn-primary" onclick="saveMLConfig()">Simpan Konfigurasi ML</button>
          <div class="info-box" style="margin-top:15px;">
            <strong>Penjelasan:</strong><br>
            • Threshold rendah (0.3-0.4) = Lebih sering menyiram<br>
            • Threshold sedang (0.4-0.5) = Penyiraman seimbang<br>
            • Threshold tinggi (0.5-0.6) = Lebih jarang menyiram
          </div>
        </div>

        <div class="panel">
          <h3>Preset Jenis Tanaman</h3>
          <p style="opacity:0.8; margin-bottom:15px;">Pilih jenis tanaman untuk konfigurasi otomatis:</p>
          <div class="preset-grid" id="presetGrid"></div>
        </div>

        <div class="panel">
          <h3>Tindakan Sistem</h3>
          <button class="btn btn-secondary" onclick="if(confirm('Restart ESP32?')) restartSystem()">Restart Sistem</button>
          <button class="btn btn-danger" onclick="if(confirm('Reset WiFi ke mode AP?')) clearWiFi()">Reset WiFi</button>
        </div>
      </div>
    </div>
  </div>
  
  <footer style="text-align: center; margin-top: 40px; padding: 20px 0; font-size: 0.85em; opacity: 0.7; border-top: 1px solid rgba(255,255,255,0.1);">
          <p>© 2026 <strong>ApexCoders X CodecZtron</strong></p>
          <p style="font-size: 0.8em; margin-top: 5px;">Smart Plant Monitoring System • All Rights Reserved</p>
      </footer>
    </div> </div> <script src="https://cdn.jsdelivr.net/npm/chart.js@3.9.1/dist/chart.min.js"></script>
    
  <script src="https://cdn.jsdelivr.net/npm/chart.js@3.9.1/dist/chart.min.js"></script>
  <script>
    let eventSource;
    let myChart;
    let currentPresets = [];
    const statusEl = document.getElementById('connection-status');

    function switchTab(tabName) {
      document.querySelectorAll('.tab-content').forEach(el => el.classList.add('hidden'));
      document.querySelectorAll('.nav-tab').forEach(el => el.classList.remove('active'));
      document.getElementById('tab-' + tabName).classList.remove('hidden');
      event.target.classList.add('active');
      if(tabName === 'chart') loadChartData();
    }

    function initSSE() {
      if (!!window.EventSource) {
        eventSource = new EventSource('/events');
        eventSource.addEventListener('sensor-data', function(e) {
          const data = JSON.parse(e.data);
          updateUI(data);
        });
        eventSource.onopen = function() {
          statusEl.textContent = "TERHUBUNG";
          statusEl.classList.remove('updating');
          statusEl.classList.add('connected');
        };
        eventSource.onerror = function() {
          statusEl.textContent = "MENGHUBUNGKAN ULANG...";
          statusEl.classList.remove('connected');
          statusEl.classList.add('updating');
        };
      }
    }

    function updateUI(data) {
      document.getElementById('airTemp').textContent = data.airTemp.toFixed(1);
      document.getElementById('airHumidity').textContent = data.airHumidity.toFixed(0);
      document.getElementById('soilTemp').textContent = data.soilTemp.toFixed(1);
      const moisture = parseInt(data.soilMoisture);
      document.getElementById('soilMoisture').textContent = moisture;
      document.getElementById('moistureBar').style.width = moisture + '%';
      document.getElementById('mlScore').textContent = data.mlScore.toFixed(2);
      document.getElementById('mlBar').style.width = (data.mlScore * 100) + '%';
    }

    async function fetchData() {
      try {
        const res = await fetch('/api/data');
        const data = await res.json();
        updateUI(data);
        document.getElementById('relaySwitch').checked = data.relayState;
        document.getElementById('autoSwitch').checked = data.autoMode;
        document.getElementById('wifi-status').textContent = data.isAPMode ? 'Mode AP' : 'Terhubung';
        document.getElementById('wifi-ssid').textContent = data.isAPMode ? 'SPMSv3-Config' : data.ssid;
        document.getElementById('wifi-ip').textContent = data.ipAddress;
        document.getElementById('wifi-rssi').textContent = data.isAPMode ? 'N/A' : data.rssi + ' dBm';
        document.getElementById('mlThreshold').value = data.mlThreshold;
        document.getElementById('currentThreshold').textContent = data.mlThreshold;
      } catch(e) { console.log("Fetch error"); }
    }

    async function loadChartData() {
      try {
        const res = await fetch('/api/chart');
        const data = await res.json();
        if(myChart) myChart.destroy();
        const ctx = document.getElementById('dataChart').getContext('2d');
        myChart = new Chart(ctx, {
          type: 'line',
          data: {
            labels: data.timestamps.map((t, i) => i % 6 === 0 ? new Date(t*1000).toLocaleTimeString('id-ID', {hour:'2-digit', minute:'2-digit'}) : ''),
            datasets: [{
              label: 'Suhu Udara (°C)',
              data: data.temps,
              borderColor: 'rgba(255, 99, 132, 1)',
              backgroundColor: 'rgba(255, 99, 132, 0.2)',
              tension: 0.3
            },{
              label: 'Kelembaban Udara (%)',
              data: data.humidity,
              borderColor: 'rgba(54, 162, 235, 1)',
              backgroundColor: 'rgba(54, 162, 235, 0.2)',
              tension: 0.3
            },{
              label: 'Kelembaban Tanah (%)',
              data: data.soilMoisture,
              borderColor: 'rgba(75, 192, 192, 1)',
              backgroundColor: 'rgba(75, 192, 192, 0.2)',
              tension: 0.3
            }]
          },
          options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: { legend: { labels: { color: 'white' }}},
            scales: {
              x: { ticks: { color: 'white' }, grid: { color: 'rgba(255,255,255,0.1)' }},
              y: { ticks: { color: 'white' }, grid: { color: 'rgba(255,255,255,0.1)' }}
            }
          }
        });
      } catch(e) { console.log("Chart error"); }
    }

    async function loadPresets() {
      try {
        const res = await fetch('/api/presets');
        currentPresets = await res.json();
        const grid = document.getElementById('presetGrid');
        grid.innerHTML = currentPresets.map((p, i) => `
          <div class="preset-card" onclick="applyPreset(${i})">
            <div class="preset-name">${p.name}</div>
            <div class="preset-desc">${p.description}</div>
            <div class="preset-values">Kelembaban: ${p.moistureTarget}% | Threshold: ${p.threshold}</div>
          </div>
        `).join('');
      } catch(e) { console.log("Presets error"); }
    }

    async function applyPreset(idx) {
      try {
        await fetch('/api/applypreset', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'index='+idx});
        alert('Preset "' + currentPresets[idx].name + '" diterapkan!');
        fetchData();
      } catch(e) { alert('Gagal menerapkan preset'); }
    }

    async function toggleRelay() {
      const state = document.getElementById('relaySwitch').checked;
      await fetch('/api/relay', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'state='+state});
    }

    async function toggleAuto() {
      const state = document.getElementById('autoSwitch').checked;
      await fetch('/api/automode', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'state='+state});
    }

    async function saveWiFi() {
      const ssid = document.getElementById('wifiSSID').value;
      const pass = document.getElementById('wifiPassword').value;
      const msg = document.getElementById('wifiMsg');
      
      if(!ssid) { msg.textContent = "SSID wajib diisi!"; return; }
      
      msg.textContent = "Sedang menghubungi WiFi... (Mohon tunggu ±15 detik)";
      msg.style.color = "white";

      try {
        const response = await fetch('/api/wifi', {
          method:'POST', 
          headers:{'Content-Type':'application/x-www-form-urlencoded'}, 
          body:'ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(pass)
        });
        
        const text = await response.text(); 

        if (response.ok) {
          // Sukses (200)
          msg.textContent = "SUKSES: " + text;
          msg.style.color = "#4CAF50"; // Green
        } else {
          // Gagal (400) - Password Salah / Timeout
          msg.textContent = "GAGAL: " + text;
          msg.style.color = "#ff5252"; // Red
        }
      } catch(e) { 
        msg.textContent = "Error: Terputus dari perangkat (Timeout)."; 
        msg.style.color = "#ff5252"; 
      }
    }

    function toggleStaticIPFields() {
      const checked = document.getElementById('useStaticIP').checked;
      document.getElementById('staticIPFields').style.display = checked ? 'block' : 'none';
    }

    async function saveStaticIP() {
      const useStatic = document.getElementById('useStaticIP').checked;
      let body = 'useStatic=' + useStatic;
      if(useStatic) {
        const ip = document.getElementById('staticIP').value;
        const gw = document.getElementById('gateway').value;
        const sn = document.getElementById('subnet').value;
        const dns = document.getElementById('dns').value || gw;
        if(!ip || !gw || !sn) { alert('IP, Gateway, dan Subnet wajib diisi!'); return; }
        body += '&ip='+encodeURIComponent(ip)+'&gateway='+encodeURIComponent(gw)+'&subnet='+encodeURIComponent(sn)+'&dns='+encodeURIComponent(dns);
      }
      try {
        await fetch('/api/staticip', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:body});
        alert('Konfigurasi IP tersimpan! Restart untuk menerapkan.');
      } catch(e) { alert('Gagal menyimpan'); }
    }

    async function saveMLConfig() {
      const threshold = document.getElementById('mlThreshold').value;
      const optimal = document.getElementById('optimalMoisture').value;
      try {
        await fetch('/api/mlconfig', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'threshold='+threshold+'&optimalMoisture='+optimal});
        alert('Konfigurasi ML tersimpan!');
        fetchData();
      } catch(e) { alert('Gagal menyimpan'); }
    }
    
    async function restartSystem() {
      await fetch('/api/reset', {method:'POST'});
      alert('Sistem restart...');
    }

    async function clearWiFi() {
      await fetch('/api/clearwifi', {method:'POST'});
      alert('WiFi direset! ESP32 akan restart ke mode AP.');
    }

    window.onload = function() {
      initSSE();
      fetchData();
      loadPresets();
      setInterval(fetchData, 10000);
    };
  </script>
</body>
</html>
)rawliteral";
}