# Robot Sumo Bidireccional - ESP32-S3

**Stack Tecnológico:**
![C/C++](https://img.shields.io/badge/C%2FC%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![FreeRTOS](https://img.shields.io/badge/FreeRTOS-20232A?style=for-the-badge&logo=freertos&logoColor=29B6F6) ![ESP-IDF 6.2](https://img.shields.io/badge/ESP--IDF_6.2-E7352C?style=for-the-badge&logo=espressif&logoColor=white) ![STM32 Bare-Metal](https://img.shields.io/badge/STM32_Bare--Metal-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)

**Entorno de Desarrollo:**
![Arch Linux](https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=arch-linux&logoColor=white) ![Hyprland](https://img.shields.io/badge/Hyprland-00A489?style=for-the-badge&logo=hyprland&logoColor=white) ![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white) ![Ninja](https://img.shields.io/badge/Ninja-333333?style=for-the-badge&logo=ninja&logoColor=white) ![Neovim](https://img.shields.io/badge/Neovim-57A143?style=for-the-badge&logo=neovim&logoColor=white)

Este proyecto implementa el firmware completo y la integración de hardware de un robot de sumo competitivo de categoría Mini Sumo (10 cm x 10 cm, sin límite de altura). El diseño del robot es **bidireccional**, permitiéndole atacar y defenderse de manera eficaz sin necesidad de girar sobre su propio eje ante ataques por la retaguardia.

El sistema toma decisiones en tiempo real mediante una máquina de estados ejecutada sobre el RTOS FreeRTOS, procesando datos de múltiples sensores y controlando un sistema de tracción en las 4 ruedas. El código base ha sido migrado a **ESP-IDF v6**, utilizando una arquitectura modular basada en componentes y el sistema de construcción nativo `idf.py`.

### Características principales del Software

| Característica | Detalle |
|---|---|
| Procesamiento Dual | ESP32-S3 (Cerebro Táctico) + STM32G474RET6 (Músculo/Bare-Metal) |
| Lenguajes & Frameworks | C/C++ nativo sobre ESP-IDF 6.x (FreeRTOS) + STM32 LL / HAL |
| Herramienta de build | idf.py / CMake / STM32CubeMX |
| Comunicación I2C | Lectura de 6 sensores ToF por I2C a 400 kHz en una tarea dedicada, sincronizada por las interrupciones GPIO de cada sensor. |
| Comunicación SPI | "Bus de la Verdad" a 10 MHz vía DMA, con paquetes de 20 bytes y CRC8 para comandos inter-MCU. |
| Control de motores | Delegado al STM32. TIM1/TIM6 gestionan 4 drivers DRV8874 en 2 canales de tracción (IN compartidas por lado) y ADC IPROPI independiente. |
| Conectividad (ESP32) | WiFi + MQTT + mDNS + SmartConfig + OTA (Desactivados en combate) |
| Robustez | Watchdog (WDT), timeout I2C de 5 ms con recuperación del bus y Kill-Switch analógico (22ns). |

---

## Resultados del Prototipo V1

<img src="prototipoV1.jpeg" alt="Prototipo V1" width="600" />

El ensamblaje inicial V1 con tracción a 100 RPM sirvió como validación mecánica y de distribución de componentes. Sin embargo, al iniciar la fase de energización, el sistema presentó fallos de hardware críticos inmediatos:

- **Diagnóstico:** El uso de cableado no dedicado impidió un aislamiento adecuado entre las etapas de potencia de la batería y la lógica de control. Se detectaron riesgos inminentes de cortocircuito general antes de poder validar el firmware en movimiento.
- **Decisión Técnica:** En lugar de forzar parches sobre un hardware frágil, se abortó la etapa de pruebas dinámicas del primer prototipo por seguridad y fiabilidad.
- **Lección Aprendida:** Comprendí que el software más optimizado es inútil sobre un hardware inestable. Para competir de forma segura, es mandatorio diseñar circuitos impresos (PCBs) que aíslen el ruido de los motores, manejen corrientes altas sin riesgo térmico y aseguren la integridad de las señales.

---

## 🚀 Evolución a la Versión V2 (Arquitectura Híbrida en PCB)

A raíz de los hallazgos del primer prototipo, el proyecto pivotó hacia un diseño profesional en PCB y una arquitectura de hardware distribuida. Esta separación previene que la etapa de tracción comprometa al procesador principal:

- **Arquitectura Dual Inter-MCU:** Implementación de bus SPI de alta velocidad entre el ESP32-S3 y el STM32G474RET6, operando estrictamente por DMA con empaquetado de structs en C/C++.
- **Actualización de etapa de potencia:** Uso de drivers DRV8874, aprovechando el pin IPROPI para lectura analógica de corriente (muestreo sincronizado por TIM1 vía ADC con DMA en el STM32) para detección de stall por AWD2 y maniobra de retroceso.
- **Migración del regulador lógico:** A AP63203WU-7.
- **Corrección de interrupción:** Ruteo de `INT_STM` del STM32 al GPIO14 del ESP32 y de las interrupciones individuales de los VL53L1X a GPIO38, GPIO39, GPIO40, GPIO8, GPIO18 y GPIO17.
- **Delegación total del control de tracción:** Coprocesador STM32G474RET6, operando estrictamente en Bare-Metal (Low Layer). Uso de TIM1 para PWM y TIM6 con Lookup Tables para rampas de aceleración asíncronas.
- **Frenado activo de latencia cero (nanosegundos):** Ruteando TCRT1/2/4 a los comparadores analógicos internos del STM32 vinculados al TIM1_BRK; TCRT3 usa ADC1 AWD1 y break por software.
- **Telemetría en modo prueba:** Envío periódico por MQTT (100 ms) de estado, motores, ToF y evasión; el modo combate no inicia WiFi/MQTT. El volcado diferido a PSRAM/LittleFS no está implementado.
- **Inyección de configuración en caliente:** Conexión WiFi en modo STA con SmartConfig/mDNS para publicar telemetría y ajustar el NVS por MQTT al entrar en modo de prueba.

---

## Especificaciones de Hardware (Actual)

* **Arquitectura:** Híbrida Asimétrica.
* **Cerebro Táctico:** ESP32-S3 (ESP-IDF, FreeRTOS, SMP). Gestiona estrategia y orquestación.
* **Músculo y Reflejos:** STM32G474RET6 operando en Bare-Metal/HAL.
* **Tracción:** 4 Motores Pololu N20 de **1000 RPM**.
* **Control de Potencia:** 4 Drivers DRV8874 en 2 canales de tracción; IN compartidas por lado e IPROPI independiente.
* **Detección de Rival (ToF):** 6 Sensores de Tiempo de Vuelo 3 frontales, 3 traseros. Transmisión I2C a 400 kHz sincronizada por interrupción.
* **Reacción de Borde:** TCRT1/2/4 por comparadores analógicos al TIM1_BRK; TCRT3 por ADC1 AWD1 con break por software.
* **Comunicación Inter-MCU:** (SPI Esclavo + DMA). Structs empaquetados a 1 byte, tramas de 20 bytes, CRC8 y transferencias sin bloqueo.
* **Alimentación:** Batería LiPo 2s 2200mAh 50C; riel de motores con fusible de 10 A y PTC de 1.5 A para la lógica.
* **Regulación Lógica:** AP63203WU-7.

---

## Optimización Real-Time

El sistema integra optimizaciones de bajo nivel para garantizar un rendimiento Hard Real-Time en combate:

1. **Reacción de Baja Latencia (Bypass de Software):** La detección de los bordes del tatami se resuelve en el STM32: TCRT1/2/4 entran a comparadores internos conectados a `TIM1_BRK` y TCRT3 usa ADC1 AWD1 con break por software. El STM32 frena en hardware y avisa al ESP32 por `INT_STM` (GPIO14); la Máquina de Estados consume esa bandera en su ciclo de 1 ms.
2. **Concurrencia sin bloqueos (Control de Motores):** Los setpoints viajan como dos `int16_t` dentro de `Esp_t` por SPI DMA; el STM32 aplica la rampa con TIM6 y no hay mutex en el camino de control.
3. **Empaquetado Estricto de Memoria:** El sistema de comunicación y telemetría utiliza estructuras con empaquetado forzado a 1 byte (`#pragma pack(push, 1)`) para prevenir corrupción de memoria por desalineación introducida por el compilador.

---

## Arquitectura del Software

El firmware utiliza al máximo las capacidades del ESP32-S3 mediante múltiples tareas FreeRTOS supervisadas por un **Task Watchdog (WDT)**. El sistema implementa una lógica de reinicio rápido en caso de fallo, omitiendo esperas innecesarias para reincorporarse al combate al instante.

```text
ESP32-S3 (Core 0)              ESP32-S3 (Core 1)              STM32G474 (Bare-Metal)
──────────────────────         ──────────────────────         ──────────────────────
Matriz ToF I2C (Prio: 10)      Máquina de Estados (Prio: 2)   Control Motores (TIM1/TIM6)
Telemetría/Logs (Prio: 1)      SPI DMA (FSM)                  ADC IPROPI (Corriente)
Audio/Musica (Prio: 1)                                        Comparadores/AWD1 (Borde)
ISR INT_STM (GPIO14)
```

### Módulos por Componentes (ESP32-S3)

- **`core`** — El corazón táctico.
    - **`MaquinaEstados`**: Lógica central de toma de decisiones.
    - **Estrategias:** Implementa un **Patrón Strategy** polimórfico (`EstrategiaBase`, `EstrategiaEstandar`, `Estrategia1`, `Estrategia2`).
    - **`GestorI2C`**: Arquitectura de bus I2C con autorrecuperación en caso de fallos.
    - **`GestorBorde`**: Procesamiento de interrupciones asíncronas para reacciones de supervivencia.
    - **`Nvs`**: Abstracción persistente para configuración táctica.
- **`actuadores`** — Delegación de potencia.
    - **`Velocidades`**: Estructuras y empaquetado de setpoints cinéticos transmitidos vía SPI DMA hacia el STM32.
- **`sensores`** — Drivers de percepción.
    - **`SensorTof`**: Driver de la matriz ToF (I2C 400 kHz + interrupciones GPIO) para localización de alta velocidad del oponente.
- **`comunicaciones`** — Gestión del SPI DMA ("Bus de la Verdad") activo en combate. Stack inalámbrico (WiFi, MQTT, OTA, mDNS) desactivado durante asaltos.
- **`ui` / `configuracion` / `Musica`** — Gestión de LED RGB, pines/eventos y tonos.

### Coprocesador STM32G474 (Bare-Metal)
- **`TIM1` / `TIM6`**: PWM de tracción y Lookup Tables para curvas de aceleración asíncronas.
- **`ADC (IPROPI)`**: Sensado de corriente sincronizado por TIM1 para detección de stall (atasco) por AWD2 y retroceso.
- **`Comparadores`**: "Kill-Switch" analógico para TCRT1/2/4 ruteado a `TIM1_BRK` (~22ns); TCRT3 usa ADC1 AWD1 y break por software.

---

## Modos de Operación

| Modo | Descripción | Activación |
|---|---|---|
| `0` | **Prueba / Telemetría** — Activa WiFi y MQTT. Permite monitoreo y ajustes en tiempo real. | Botón presionado al encender; el modo 0 queda guardado en NVS. |
| `1` | **Combate Autónomo** — Modo competitivo estricto. Transmisión inalámbrica desactivada para máximo rendimiento. | Encendido normal (Por defecto). |

---

## Parámetros Ajustables (NVS)

Las variables tácticas críticas se pueden ajustar remotamente:

| Clave NVS | Descripción |
|---|---|
| **Tiempos y Estrategia (NVS: tiempos)** | |
| `estrategia` | Algoritmo inicial leído de `tiempos` al arrancar; MQTT lo guarda en `sistema` y aún no se aplica. |
| `tof_corto_plazo` / `tof_largo_plazo` | Ventanas de filtrado anti-jitter para ToFs. |
| `recta_star` / `giro_star` | Tiempos de maniobra inicial (Estrategia Star). |
| `retroceso` | Tiempo de retroceso de la evasión (100-1500 ms). |
| `t_stall` | Reservado; el STM32 usa `TStall = 250 ms` fijo. |
| **Velocidades (NVS: motores)** | |
| `tiempo_rampa` | Reservado; la rampa se aplica con una LUT fija de 50 pasos en el STM32. |
| `u_stall` | Reservado; el STM32 no lo aplica. |
| `velocidad_nI` / `nD` | Velocidad Normal (Búsqueda). |
| `velocidad_aI` / `aD` | Velocidad de Ataque. |
| `velocidad_mI` / `mD` | Velocidad Maxima. |
| `velocidad_pI` / `pD` | Velocidad de giro pronunciado. |
| `velocidad_gI` / `gD` | Velocidad de Giro. |
| `velocidad_hI` / `hD` | Reservada; no implementada en el firmware actual. |
| **Sensores (NVS: sensores)** | |
| `dist_max` | Límite de distancia de los ToF (mm). |
| `umbral_color` | Umbral de los TCRT (`u_limite`) enviado al STM32. |
| **Sistema (NVS: sistema)** | |
| `modo` | 0 = prueba/telemetría; 1 = combate. |
| `monitor` | Nivel de logs por consola (0-3). |

---

## Ecosistema de Herramientas

- 🐍 **`telemetria.py`**: Middleware en Python que escucha `robot/telemetria` por MQTT, encola los JSON en SQLite y los reenvía por HTTP por lotes.
- 📜 **`commit.py`**: Script de automatización que inyecta el hash del commit para trazabilidad del firmware.
- 📦 **`full_codebase.sh`**: Utilidad para el mantenimiento y empaquetado del repositorio.

---

## Estructura del Proyecto

```text
├── esp32-s3/                   # Cerebro Táctico (ESP-IDF v6)
│   ├── components/
│   │   ├── actuadores/         # Estructuras de payload cinético (Velocidades)
│   │   ├── comunicaciones/     # Driver SPI (Bus de la Verdad), WiFi, MQTT, OTA
│   │   ├── configuracion/      # Pines y Eventos
│   │   ├── core/               # Máquina de estados, Estrategias y NVS
│   │   ├── sensores/           # Drivers matriz ToF (I2C + INT)
│   │   ├── ui/                 # LED RGB (Interfaz centralizada)
│   │   └── Musica/             # Buzzer
│   ├── main/main.cpp           # Punto de entrada e inicialización
│   ├── partitions_s3.csv       # Tabla de particiones NVS
│   └── sdkconfig               # Configuración FreeRTOS/ESP32
├── stm32g4/                    # Músculo y Reflejos (Bare-Metal)
│   ├── Core/
│   │   ├── Inc/                # Cabeceras (LL predominante, HAL, main.h)
│   │   └── Src/                # Controladores TIM1/TIM6, ADC IPROPI y main.c
│   ├── Drivers/                # CMSIS y STM32G4xx_HAL_Driver
│   └── stm32g4.ioc             # Configuración de STM32CubeMX
```

---

## Compilación y Flasheo

### Cerebro Táctico (ESP32-S3)
```bash
cd esp32-s3/
# Limpiar y compilar
idf.py fullclean build
# Flashear y monitorear
idf.py -p [PUERTO] flash monitor
```

### Coprocesador Muscular (STM32G474)
```bash
cd stm32g4/
# Compilar proyecto vía CMake
cmake --build build/Debug

# Generar binario (.bin) a partir del ELF
arm-none-eabi-objcopy -O binary build/Debug/*.elf build/Debug/firmware.bin

# Flashear vía STM32CubeProgrammer (Protocolo Oficial SWD)
STM32_Programmer_CLI -c port=SWD mode=UR -w build/Debug/*.elf -v -rst

# Flashear vía st-link (Alternativa Open Source)
st-flash --reset write build/Debug/*.bin 0x8000000
```
