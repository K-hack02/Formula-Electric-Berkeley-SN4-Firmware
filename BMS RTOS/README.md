# Formula Electric BMS — RTOS Architecture

## Task Overview (rate & priority)
- **ADBMS** — **20 ms**; balance tick **2000 ms**; **priority: Realtime**  
  Samples cell voltages/temps, validates, and publishes results via a double buffer.

- **State Machine** — **10 ms**; **priority: Realtime**  
  Manages BMS states, transition logic, and HV hardware sequencing.

- **Charging** — **50 ms**; **priority: High**  
  Chooses charging mode (normal/trickle), runs charging calculations, enforces limits.

- **CAN** — **10 ms**; **priority: AboveNormal**  
  Owns all CAN transmission; other tasks request sends via notifications/queues.

- **UART** — **1000 ms**; **priority: BelowNormal**  
  Periodically dumps system status for monitoring.

- **defaultTask** — background/idle work; **priority: Low**.

---

## Race-Condition Handling
- **Single variable updates** → protected with critical sections (taskENTER_CRITICAL / taskEXIT_CRITICAL).
- **Struct updates** → **sequence counter** pattern:  
  - Writer increments the counter before & after the write.  
  - Reader retries if the counter is odd or changed during the read.  
- **ADBMS accumulator (large struct)** → **double-buffered** snapshot:  
  - Writer fills a “back” buffer under mutex.  
  - Pointer swap publishes it as the current read-only buffer.
  - Readers always see a consistent snapshot.

---

## Inter-Task Communication
- **ADBMS → State Machine (fault trigger):** **Direct Task Notification** (fast one-bit signal).  
- **Charging → CAN (send request):** **Direct Task Notification** to wake CAN task for an immediate send.  
- **State Machine → ADBMS (balance control):** **Queue** carrying `START`/`STOP` commands.  
- **State Machine → Charging (charge control):** **Queue** carrying `START`/`STOP` commands.

---

## Why This Works
- **Clear ownership:** each task controls its domain; no shared side-effects.  
- **Safe reads/writes:** critical sections for scalars, counter+retry for mid-sized structs, double-buffer for large data.
- **Efficient signaling:** notifications for one-bit events; queues for multi-state commands.  
- **Deterministic timing:** periods match subsystem needs; priorities ensure sampling and state logic are serviced on time.

---

## Notes & Tunables
- If SM work grows, consider lowering SM to **High** (keeping ADBMS at **Realtime**) to reduce priority inversion risk.  
- For CAN/UART throughput bursts, prefer DMA or ring/stream buffers and avoid busy-waits.  
- Monitor `uxTaskGetStackHighWaterMark()` per task and right-size stacks with ~20% headroom.

