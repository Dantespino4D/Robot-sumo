const telemetryState = {
  tof: { ai: 800, ac: 800, ad: 800, bi: 800, bc: 800, bd: 800 },
  tcrt: { fl: false, fr: false, bl: false, br: false },
  motors: { left: 0, right: 0, stallL: false, stallR: false },
  fsm: { state: "IDLE", strategy: "Estrategia prototipo", cycleTime: 0 },
  storage: { used: 153600, total: 1048576 }, // Mock 1MB partition
  mqttConnected: false, // Estado de conexion a TIG (Orange Pi)
  score: { wins: 0, losses: 0 }, // Marcador del torneo (Mejor de 3)
  battery: 2800, // RAW ADC (e.g. 2800 = ~8.4V)
  
  // Diagnóstico Avanzado
  evasion: 0,
  inicio: 0,
  wifi: -60,
  heap: 150000,
  tiempo: 0,
  drv: { d1: 0, d2: 0, d3: 0, d4: 0 },
  tofDiag: {
    ai: { est: 0, sig: 50, amb: 10 },
    ac: { est: 0, sig: 50, amb: 10 },
    ad: { est: 0, sig: 50, amb: 10 },
    bi: { est: 0, sig: 50, amb: 10 },
    bc: { est: 0, sig: 50, amb: 10 },
    bd: { est: 0, sig: 50, amb: 10 }
  }
};

const mockNvsProfiles = {}; // Simulador de la memoria NVS del ESP32


// ==========================================
// ACTUALIZAR INTERFAZ
// ==========================================
function updateTelemetryUI() {
  // 1. Sensores ToF
  ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'].forEach(id => {
    const dist = telemetryState.tof[id];
    document.getElementById(`dist-${id}`).textContent = dist;
    
    const bar = document.getElementById(`bar-${id}`);
    const maxToF = 1200; // asumiendo 1200mm de rango máximo visual
    let percent = (dist / maxToF) * 100;
    if (percent > 100) percent = 100;
    bar.style.height = `${percent}%`;

    // Cambiar color a rojo si está muy cerca (<200mm)
    if (dist < 200) {
      bar.style.backgroundColor = 'var(--danger)';
    } else {
      bar.style.backgroundColor = 'var(--accent)';
    }
  });

  // 2. Sensores TCRT (Piso)
  ['fl', 'fr', 'bl', 'br'].forEach(id => {
    const el = document.getElementById(`tcrt-${id}`);
    if (telemetryState.tcrt[id]) {
      el.classList.add('active'); // detecta blanco
    } else {
      el.classList.remove('active');
    }
  });

  // 3. Motores PWM y Stall
  const updateMotor = (side, pwm, stall) => {
    const posBar = document.getElementById(`pwm-${side}-pos`);
    const negBar = document.getElementById(`pwm-${side}-neg`);
    const valText = document.getElementById(`val-pwm-${side}`);
    const stallAlert = document.getElementById(`stall-${side}`);

    valText.textContent = pwm;

    let percent = (Math.abs(pwm) / 1023) * 50; // Max 50% of total bar height
    if (percent > 50) percent = 50;

    if (pwm >= 0) {
      posBar.style.height = `${percent}%`;
      negBar.style.height = '0%';
    } else {
      posBar.style.height = '0%';
      negBar.style.height = `${percent}%`;
    }

    if (stall) {
      stallAlert.classList.add('active');
      posBar.style.backgroundColor = 'var(--danger)';
      negBar.style.backgroundColor = 'var(--danger)';
    } else {
      stallAlert.classList.remove('active');
      posBar.style.backgroundColor = 'var(--accent)';
      negBar.style.backgroundColor = 'var(--accent)';
    }
  };

  updateMotor('l', telemetryState.motors.left, telemetryState.motors.stallL);
  updateMotor('r', telemetryState.motors.right, telemetryState.motors.stallR);

  // 4. Máquina de Estados
  const stateEl = document.getElementById('fsm-state');
  if (stateEl) {
    stateEl.textContent = telemetryState.fsm.state;
    stateEl.title = telemetryState.fsm.state; // Permite ver el texto completo al poner el ratón encima
  }
  
  const strategyEl = document.getElementById('fsm-strategy');
  if (strategyEl) {
    strategyEl.textContent = telemetryState.fsm.strategy;
    strategyEl.title = telemetryState.fsm.strategy;
  }

  const cycleEl = document.getElementById('fsm-cycletime');
  if (cycleEl) {
    cycleEl.textContent = telemetryState.fsm.cycleTime + ' ms';
  }

  // 5. Almacenamiento LittleFS
  const storageText = document.getElementById('storage-text');
  const storageFill = document.getElementById('storage-fill');
  if (storageText && storageFill) {
    const usedKb = (telemetryState.storage.used / 1024).toFixed(1);
    const totalKb = (telemetryState.storage.total / 1024).toFixed(1);
    storageText.textContent = `${usedKb} KB / ${totalKb} KB`;

    let percent = (telemetryState.storage.used / telemetryState.storage.total) * 100;
    if (percent > 100) percent = 100;
    
    storageFill.style.width = `${percent}%`;
    storageFill.className = 'storage-fill'; // reset classes
    if (percent > 90) {
      storageFill.classList.add('critical');
    } else if (percent > 75) {
      storageFill.classList.add('warning');
    }
  }

  // 6. Conexión MQTT a TIG
  const mqttLed = document.getElementById('mqtt-led');
  const btnTig = document.getElementById('btn-sync-tig');
  if (mqttLed) {
    if (telemetryState.mqttConnected) {
      mqttLed.classList.add('connected');
      mqttLed.title = "MQTT Conectado a Orange Pi";
      if (btnTig && !btnTig.classList.contains('loading')) {
        btnTig.disabled = false;
        btnTig.textContent = "Sincronizar y Vaciar LittleFS";
      }
    } else {
      mqttLed.classList.remove('connected');
      mqttLed.title = "MQTT Desconectado";
      if (btnTig && !btnTig.classList.contains('loading')) {
        btnTig.disabled = true;
        btnTig.textContent = "Sin conexión al servidor TIG";
      }
    }
  }

  // 7. Marcador del Combate
  const scoreWins = document.getElementById('score-wins');
  const scoreLosses = document.getElementById('score-losses');
  const matchStatus = document.getElementById('match-status');
  
  if (scoreWins && scoreLosses && matchStatus) {
    scoreWins.textContent = telemetryState.score.wins;
    scoreLosses.textContent = telemetryState.score.losses;

    if (telemetryState.score.wins >= 2) {
      matchStatus.textContent = "¡VICTORIA DEL MATCH! 🏆";
      matchStatus.className = "match-status victory";
    } else if (telemetryState.score.losses >= 2) {
      matchStatus.textContent = "DERROTA DEL MATCH 💀";
      matchStatus.className = "match-status defeat";
    } else {
      matchStatus.textContent = "Combate en curso (Mejor de 3)";
      matchStatus.className = "match-status";
    }
  }

  // 8. Batería (Conversión ADC a Voltaje en JS)
  const batEl = document.getElementById('battery-monitor');
  if (batEl) {
    // Asumiendo ADC de 12 bits (0-4095) y un divisor de tensión donde 4095 = 12.6V max (por ejemplo)
    // Ajusta la constante '333.3' según tu divisor resistivo real
    const volt = (telemetryState.battery / 333.3).toFixed(2);
    batEl.textContent = `🔋 ${volt} V`;
    if (volt <= 7.2) {
      batEl.classList.add('critical');
    } else {
      batEl.classList.remove('critical');
    }
  }

  // 9. Panel de Diagnóstico
  const setEl = (id, val, cls) => {
    const el = document.getElementById(id);
    if (el) {
      el.textContent = val;
      if (cls !== undefined) el.className = `val ${cls}`;
    }
  };

  setEl('diag-evasion', telemetryState.evasion ? "ACTIVO" : "INACTIVO", telemetryState.evasion ? "danger" : "safe");
  setEl('diag-inicio', telemetryState.inicio ? "GO" : "ESPERA", telemetryState.inicio ? "safe" : "warning");
  setEl('diag-wifi', `${telemetryState.wifi} dBm`);
  setEl('diag-heap', `${(telemetryState.heap / 1024).toFixed(1)} KB`, telemetryState.heap < 20000 ? "danger" : "safe");
  
  // Convert uptime ms to HH:MM:SS
  const totalSec = Math.floor(telemetryState.tiempo / 1000);
  const h = String(Math.floor(totalSec / 3600)).padStart(2, '0');
  const m = String(Math.floor((totalSec % 3600) / 60)).padStart(2, '0');
  const s = String(totalSec % 60).padStart(2, '0');
  setEl('diag-tiempo', `${h}:${m}:${s}`);

  // DRV (IPROPI)
  setEl('diag-drv1', telemetryState.drv.d1);
  setEl('diag-drv2', telemetryState.drv.d2);
  setEl('diag-drv3', telemetryState.drv.d3);
  setEl('diag-drv4', telemetryState.drv.d4);

  // ToFs (Est, Sig, Amb)
  ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'].forEach(id => {
    const t = telemetryState.tofDiag[id];
    setEl(`tof-est-${id}`, t.est, t.est !== 0 ? "danger" : "safe");
    setEl(`tof-sig-${id}`, t.sig);
    setEl(`tof-amb-${id}`, t.amb);
  });
}

// ==========================================
// MOCK SIMULATOR
// ==========================================
let simInterval;
function startMockSimulator() {
  const maxToF = 1200;
  
  simInterval = setInterval(() => {
    telemetryState.motors.left = Math.floor(Math.random() * 2047) - 1023;
    telemetryState.motors.right = Math.floor(Math.random() * 2047) - 1023;
    telemetryState.motors.stallL = Math.random() > 0.95;
    telemetryState.motors.stallR = Math.random() > 0.95;
    
    ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'].forEach(id => {
      telemetryState.tof[id] = Math.floor(Math.random() * maxToF);
    });
    
    ['fl', 'fr', 'bl', 'br'].forEach(id => {
      telemetryState.tcrt[id] = Math.random() > 0.9;
    });

    const states = ["BUSQUEDA_ESTRELLA", "ATAQUE_PRONUNCIADO", "EVASION", "IDLE"];
    telemetryState.fsm.state = states[Math.floor(Math.random() * states.length)];
    telemetryState.fsm.cycleTime = Math.floor(Math.random() * 10) + 5; 

    const strategySelect = document.getElementById('estrategia');
    if (strategySelect) {
      telemetryState.fsm.strategy = strategySelect.options[strategySelect.selectedIndex].text;
    }

    telemetryState.storage.used += 1024;
    if (telemetryState.storage.used > telemetryState.storage.total) {
      telemetryState.storage.used = telemetryState.storage.total; 
    }

    if (Math.random() > 0.98) {
      telemetryState.mqttConnected = !telemetryState.mqttConnected;
    }

    if (!telemetryState.matchTimer) telemetryState.matchTimer = 0;
    telemetryState.matchTimer++;
    
    if (telemetryState.matchTimer > 50) { 
      telemetryState.matchTimer = 0;
      if (telemetryState.score.wins < 2 && telemetryState.score.losses < 2) {
        if (Math.random() > 0.5) telemetryState.score.wins++;
        else telemetryState.score.losses++;
      } else {
        telemetryState.score.wins = 0;
        telemetryState.score.losses = 0;
      }
    }

    // Drenaje de batería simulado (ADC)
    telemetryState.battery -= 1;
    if (telemetryState.battery < 2300) telemetryState.battery = 2800; // ~6.9V a 8.4V

    // Mock Diagnósticos
    telemetryState.evasion = Math.random() > 0.95 ? 1 : 0;
    telemetryState.inicio = Math.random() > 0.5 ? 1 : 0;
    telemetryState.wifi = -50 - Math.floor(Math.random() * 30);
    telemetryState.heap = 150000 + Math.floor(Math.random() * 5000) - 2500;
    telemetryState.tiempo += 100;
    telemetryState.drv.d1 = Math.floor(Math.random() * 500);
    telemetryState.drv.d2 = Math.floor(Math.random() * 500);
    telemetryState.drv.d3 = Math.floor(Math.random() * 500);
    telemetryState.drv.d4 = Math.floor(Math.random() * 500);
    
    ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'].forEach(id => {
      telemetryState.tofDiag[id].est = Math.random() > 0.95 ? 4 : 0; // 4 = error común
      telemetryState.tofDiag[id].sig = 50 + Math.floor(Math.random() * 200);
      telemetryState.tofDiag[id].amb = Math.floor(Math.random() * 50);
    });

    updateTelemetryUI();
  }, 100); 
}

// ==========================================
// WEBSOCKET (PREPARADO PARA FUTURO ESP32)
// ==========================================
let ws = null;
function initWebSocket() {
  const gateway = `ws://${window.location.hostname}/ws`;
  ws = new WebSocket(gateway);
  
  ws.onopen = () => {
    console.log('WebSocket conectado');
    if (simInterval) {
      clearInterval(simInterval);
      simInterval = null;
    }
  };
  
  ws.onclose = () => {
    setTimeout(initWebSocket, 2000);
  };
  
  ws.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);
      updateTelemetryUI();
    } catch (e) {
      console.error('Error parseando JSON:', e);
    }
  };
}

// ==========================================
// UX DEL FORMULARIO Y AJAX/WEBSOCKET
// ==========================================
function setupFormUX() {
  const form = document.getElementById('configuracion');
  const toast = document.getElementById('toast');

  // Sliders
  document.querySelectorAll('.slider-group').forEach(group => {
    const range = group.querySelector('input[type="range"]');
    const num = group.querySelector('input[type="number"]');
    if (range && num) {
      range.addEventListener('input', () => { num.value = range.value; });
      num.addEventListener('input', () => { range.value = num.value; });
    }
  });

  const selectModo = document.getElementById('modo');
  if (selectModo) {
    selectModo.value = "1";
  }

  if (form) {
    form.addEventListener('submit', (e) => {
      e.preventDefault(); 
      const formData = new FormData(form);
      const configObj = Object.fromEntries(formData.entries());
      configObj.reboot = true;

      const esCombate = configObj.modo === "1";
      const modoTexto = esCombate ? "Combate" : "Prueba";
      
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ type: "SET_CONFIG", payload: configObj }));
      }
      
      toast.textContent = esCombate ? `⚙️ Modo ${modoTexto} Activado. Reiniciando Robot...` : `🛠️ Modo ${modoTexto} Activado. Reiniciando Robot...`;
      toast.style.backgroundColor = esCombate ? "var(--warning)" : "var(--safe)";
      toast.classList.remove('hidden');
      setTimeout(() => { toast.classList.add('hidden'); }, 4000);
    });
  }

  // TIG Sync
  const btnTig = document.getElementById('btn-sync-tig');
  if (btnTig) {
    btnTig.addEventListener('click', () => {
      btnTig.disabled = true;
      btnTig.classList.add('loading');
      btnTig.textContent = "⏳ Sincronizando MQTT...";
      setTimeout(() => {
        btnTig.classList.remove('loading');
        btnTig.textContent = "✅ Volcado Exitoso";
        btnTig.style.backgroundColor = "var(--safe)";
        btnTig.style.color = "#000";
        telemetryState.storage.used = 0;
        setTimeout(() => {
          btnTig.disabled = false;
          btnTig.textContent = "Sincronizar y Vaciar LittleFS";
          btnTig.style.backgroundColor = "transparent";
          btnTig.style.color = "var(--accent)";
        }, 3000);
      }, 2500);
    });
  }

  // E-STOP
  const btnEstop = document.getElementById('btn-estop');
  if (btnEstop) {
    btnEstop.addEventListener('click', () => {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ type: "ESTOP" }));
      }
      toast.textContent = "🛑 EMERGENCIA ACTIVADA: MOTORES APAGADOS";
      toast.style.backgroundColor = "var(--danger)";
      toast.style.color = "#fff";
      toast.classList.remove('hidden');
      setTimeout(() => { toast.classList.add('hidden'); }, 5000);
    });
  }

  // EXPORTAR PERFIL
  const btnExport = document.getElementById('btn-export');
  if (btnExport) {
    btnExport.addEventListener('click', () => {
      const formData = new FormData(form);
      const configObj = Object.fromEntries(formData.entries());
      const dataStr = "data:text/json;charset=utf-8," + encodeURIComponent(JSON.stringify(configObj, null, 2));
      const downloadAnchorNode = document.createElement('a');
      downloadAnchorNode.setAttribute("href", dataStr);
      downloadAnchorNode.setAttribute("download", "perfil_sumo.json");
      document.body.appendChild(downloadAnchorNode);
      downloadAnchorNode.click();
      downloadAnchorNode.remove();
      toast.textContent = "💾 Perfil Exportado Exitosamente";
      toast.style.backgroundColor = "var(--safe)";
      toast.classList.remove('hidden');
      setTimeout(() => { toast.classList.add('hidden'); }, 3000);
    });
  }

  // IMPORTAR PERFIL A NVS (SILENCIOSO)
  const fileImport = document.getElementById('file-import');
  const nvsSelect = document.getElementById('nvs-select');
  
  if (fileImport) {
    fileImport.addEventListener('change', (event) => {
      const file = event.target.files[0];
      if (!file) return;
      
      const reader = new FileReader();
      reader.onload = (e) => {
        try {
          const configObj = JSON.parse(e.target.result);
          
          // En lugar de aplicarlo, pedimos un nombre para guardarlo en la NVS
          const profileName = prompt("Ingresa un nombre para guardar este perfil en la memoria del robot (ej. AntiCuna):", file.name.replace('.json', ''));
          
          if (profileName) {
            // Guardar en simulador NVS
            mockNvsProfiles[profileName] = configObj;
            
            // Simular orden al ESP32
            if (ws && ws.readyState === WebSocket.OPEN) {
              ws.send(JSON.stringify({ type: "SAVE_PROFILE_NVS", name: profileName, payload: configObj }));
            }
            
            // Actualizar la lista desplegable
            if (nvsSelect) {
              // Si estaba vacío quitamos la opción por defecto
              if (nvsSelect.options[0].value === "") {
                nvsSelect.innerHTML = "";
              }
              const opt = document.createElement('option');
              opt.value = profileName;
              opt.textContent = profileName;
              nvsSelect.appendChild(opt);
              nvsSelect.value = profileName;
            }
            
            toast.textContent = `💾 Perfil '${profileName}' guardado en la memoria NVS del robot.`;
            toast.style.backgroundColor = "var(--safe)";
            toast.classList.remove('hidden');
            setTimeout(() => { toast.classList.add('hidden'); }, 4000);
          }
        } catch (err) {
          console.error("Error parseando JSON", err);
          alert("Archivo JSON inválido");
        }
      };
      reader.readAsText(file);
      fileImport.value = "";
    });
  }

  // CARGAR PERFIL NVS AL FORMULARIO
  const btnLoadNvs = document.getElementById('btn-load-nvs');
  if (btnLoadNvs && nvsSelect) {
    btnLoadNvs.addEventListener('click', () => {
      const profileName = nvsSelect.value;
      if (!profileName || !mockNvsProfiles[profileName]) {
        alert("Selecciona un perfil válido de la lista.");
        return;
      }
      
      const configObj = mockNvsProfiles[profileName];
      for (const key in configObj) {
        const el = form.elements[key];
        if (el) {
          el.value = configObj[key];
          // Sincronizar sliders visuales
          if (el.type === 'number') {
            const rangeEl = document.getElementById('range_' + key);
            if (rangeEl) rangeEl.value = configObj[key];
          }
        }
      }
      
      toast.textContent = `📂 Perfil '${profileName}' cargado al panel. ¡Revisa y aplica!`;
      toast.style.backgroundColor = "var(--accent)";
      toast.style.color = "#fff";
      toast.classList.remove('hidden');
      setTimeout(() => { toast.classList.add('hidden'); }, 4000);
    });
  }
}

document.addEventListener('DOMContentLoaded', () => {
  startMockSimulator();
  setupFormUX();
});
