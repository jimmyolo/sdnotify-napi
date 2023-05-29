// READY=1
export function ready(): void

// STOPPING=1
export function stopping(pid: number): void

// STATUS={msg}\n
export function status(msg: string): void

// WATCHDOG=1
export function watchdog(): void

export function watchdogInterval(): number
export function startWatchdogMode(interval: number): void
export function stopWatchdogMode(): void
