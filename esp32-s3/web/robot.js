const maxToF = 1200; // Máximo rango confiable VL53L1X

const telemetryState = {
  tof: { ai: 800, ac: 800, ad: 800, bi: 800, bc: 800, bd: 800 },
  tcrt: { fl: false, fr: false, bl: false, br: false },
  motors: { left: 0, right: 0, stallL: false, stallR: false },
  fsm: { state: "IDLE", strategy: "Estrategia prototipo", cycleTime: 0 }
};

// ==========================================
// RENDER UI
// ==========================================
function updateTelemetryUI() {
  const triggerDist = parseInt(document.getElementById('distancia_maxima').value) || 400;

  // 1. ToFs
  const tofs = ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'];
  tofs.forEach(id => {
    const dist = telemetryState.tof[id];
    const bar = document.getElementById(`bar-${id}`);
    const distText = document.getElementById(`dist-${id}`);
    
    distText.textContent = dist + ' mm';
    
    // Calcular altura (inverso: menor distancia = barra más llena)
    let percent = 0;
    if (dist <= maxToF) {
      percent = 100 - (dist / maxToF) * 100;
    }
    bar.style.height = `${percent}%`;

    // Conmutación de detección
    if (dist <= triggerDist) {
      bar.classList.add('detected');
    } else {
      bar.classList.remove('detected');
    }
  });

  // 2. TCRT
  const tcrts = ['fl', 'fr', 'bl', 'br'];
  tcrts.forEach(id => {
    const el = document.getElementById(`tcrt-${id}`);
    if (telemetryState.tcrt[id]) {
      el.classList.add('line-detected');
    } else {
      el.classList.remove('line-detected');
    }
  });

  // 3. Motores
  const updateMotor = (side, pwm, stall) => {
    const posBar = document.getElementById(`pwm-${side}-pos`);
    const negBar = document.getElementById(`pwm-${side}-neg`);
    const valText = document.getElementById(`val-pwm-${side}`);
    const stallAlert = document.getElementById(`stall-${side}`);

    valText.textContent = pwm;

    let percent = (Math.abs(pwm) / 1023) * 100;
    if (percent > 100) percent = 100;

    if (pwm >= 0) {
      posBar.style.height = `${percent}%`;
      negBar.style.height = '0%';
    } else {
      posBar.style.height = '0%';
      negBar.style.height = `${percent}%`;
    }

    if (stall) {
      stallAlert.classList.add('active');
    } else {
      stallAlert.classList.remove('active');
    }
  };

  updateMotor('l', telemetryState.motors.left, telemetryState.motors.stallL);
  updateMotor('r', telemetryState.motors.right, telemetryState.motors.stallR);

  // 4. Máquina de Estados
  document.getElementById('fsm-state').textContent = telemetryState.fsm.state;
  document.getElementById('fsm-strategy').textContent = telemetryState.fsm.strategy;
  document.getElementById('fsm-cycletime').textContent = telemetryState.fsm.cycleTime + ' ms';
}


// ==========================================
// SIMULADOR (MOCK DATA)
// ==========================================
let simInterval = null;
function startMockSimulator() {
  if (simInterval) clearInterval(simInterval);
  
  simInterval = setInterval(() => {
    // Generar valores aleatorios coherentes
    
    // PWM: -1023 a 1023
    telemetryState.motors.left = Math.floor(Math.random() * 2047) - 1023; 
    telemetryState.motors.right = Math.floor(Math.random() * 2047) - 1023;
    telemetryState.motors.stallL = Math.random() > 0.95;
    telemetryState.motors.stallR = Math.random() > 0.95;
    
    // ToFs
    ['ai', 'ac', 'ad', 'bi', 'bc', 'bd'].forEach(id => {
      telemetryState.tof[id] = Math.floor(Math.random() * maxToF);
    });
    
    // TCRT
    ['fl', 'fr', 'bl', 'br'].forEach(id => {
      telemetryState.tcrt[id] = Math.random() > 0.9;
    });

    // FSM
    const states = ["BUSQUEDA_ESTRELLA", "ATAQUE_PRONUNCIADO", "EVASION", "IDLE"];
    telemetryState.fsm.state = states[Math.floor(Math.random() * states.length)];
    telemetryState.fsm.cycleTime = Math.floor(Math.random() * 10) + 5; // 5-15ms

    // Sincronizar Estrategia con el select actual
    const strategySelect = document.getElementById('estrategia');
    telemetryState.fsm.strategy = strategySelect.options[strategySelect.selectedIndex].text;

    updateTelemetryUI();
  }, 100); // 10Hz refresco visual
}


// ==========================================
// WEBSOCKET (PREPARADO PARA FUTURO ESP32)
// ==========================================
let ws = null;
function initWebSocket() {
  const gateway = `ws://${window.location.hostname}/ws`;
  console.log('Intentando conectar a:', gateway);
  
  ws = new WebSocket(gateway);
  
  ws.onopen = () => {
    console.log('WebSocket conectado');
    // Si se conecta el WS real, detenemos el simulador para usar datos reales
    if (simInterval) {
      clearInterval(simInterval);
      simInterval = null;
    }
  };
  
  ws.onclose = () => {
    console.log('WebSocket desconectado, reintentando...');
    setTimeout(initWebSocket, 2000);
  };
  
  ws.onmessage = (event) => {
    try {
      const data = JSON.parse(event.data);
      // data debe estructurarse para actualizar telemetryState
      // Ejemplo: si el backend envía algo como { "tof_ai": 300 }
      // Aquí mapearíamos eso al estado.
      
      // ... mapeo de datos reales al telemetryState ...

      updateTelemetryUI();
    } catch (e) {
      console.error('Error parseando JSON del WS:', e);
    }
  };
}

// ==========================================
// INICIO
// ==========================================
document.addEventListener('DOMContentLoaded', () => {
  // Arrancamos el simulador por defecto para poder ver la interfaz funcionando en local
  startMockSimulator();

  // Descomentar para habilitar conexión real vía WebSocket:
  // initWebSocket();
});

// ==========================================
// UX DEL FORMULARIO Y AJAX/WEBSOCKET
// ==========================================
function setupFormUX() {
  const form = document.getElementById('configuracion');
  const toast = document.getElementById('toast');

  // Sincronizar Sliders con Inputs numéricos
  document.querySelectorAll('.slider-group').forEach(group => {
    const range = group.querySelector('input[type="range"]');
    const num = group.querySelector('input[type="number"]');
    
    if (range && num) {
      range.addEventListener('input', () => {
        num.value = range.value;
      });
      num.addEventListener('input', () => {
        range.value = num.value;
      });
    }
  });

  // Interceptar Submit para enviar por AJAX/WebSocket sin recargar
  form.addEventListener('submit', (e) => {
    e.preventDefault(); // Evita recarga de página

    const formData = new FormData(form);
    const configObj = Object.fromEntries(formData.entries());

    console.log("Datos listos para enviar al ESP32:", configObj);

    // FUTURO: Enviar por WebSocket si está conectado
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ type: "SET_CONFIG", payload: configObj }));
    } else {
      console.warn("WebSocket no conectado, simulando envío AJAX...");
      // Aquí podrías usar fetch() si tu ESP32 usa endpoints REST en lugar de WS
    }

    // Mostrar Notificación Toast
    toast.classList.remove('hidden');
    setTimeout(() => {
      toast.classList.add('hidden');
    }, 3000);
  });
}

// Agregar al inicio del DOMContentLoaded existente
const oldContentLoaded = window.onload;
document.addEventListener('DOMContentLoaded', () => {
  setupFormUX();
});
