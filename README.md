# Stroke — Linux Keylogger

<p align="center">
  <img src="https://img.shields.io/badge/C-11_(C99+)-orange?logo=c&logoColor=white">
  <img src="https://img.shields.io/badge/License-Educational-green">
</p>

```
          ╔══════════════════════════════════════╗
          ║   ╻ ╻╻ ╻┏━╸┏━┓┏━╸┏━┓                 ║
          ║   ┃┃┃┃┏┛┣╸ ┣┳┛┣╸ ┗┓┓                 ║
          ║   ╹┗┛┗┛┗━╸╹┗╸┗━╸┗━┛                  ║
          ║      ┏━┓┏━┓╻ ╻┏━╸┏━┓╻  ╻             ║
          ║      ┣┓┛┃ ┃┃ ┃┣╸ ┣┳┛┃  ┃             ║
          ║      ┗┛┛┗━┛╹ ╹╹  ╹┗╸╹  ╹             ║
          ║                                      ║
          ║          _                           ║
          ║         | |                          ║
          ║      ___| |_ _ __ ___   ___ ___      ║
          ║     / __| __| '__/ _ \ / __/ __|     ║
          ║     \__ \ |_| | | (_) | (__\__ \     ║
          ║     |___/\__|_|  \___/ \___|___/     ║
          ║                                      ║
          ║     STROKE — Linux Keylogger         ║
          ╚══════════════════════════════════════╝
```

A C program that interfaces with the **Linux input subsystem** (`/dev/input/event*`) to capture and log keystrokes. For educational use only in controlled environments.

## Features

| Capability | Description |
|---|---|
| **Auto device detection** | Scans `/dev/input/` for keyboard devices |
| **Full key mapping** | Letters, numbers, symbols, special keys |
| **Shift tracking** | Left + Right shift state awareness |
| **Timestamped logs** | `[HH:MM:SS]` per keystroke |
| **Session tracking** | Start/end boundaries in log file |
| **Graceful exit** | Ctrl+C cleanly flushes and closes |
| **Portable C** | Compiles on any Linux with kernel headers |

## Build & Run

```bash
gcc -Wall -Wextra -o stroke stroke.c

# If user is in 'input' group → no root needed
./stroke

# Or specify device manually:
./stroke /dev/input/event3
```

## Sample Log

```
=== Session started: Thu May 28 20:58:19 2026
[20:58:19] h
[20:58:19] e
[20:58:19] l
[20:58:19] l
[20:58:19] o
[20:58:19]
[20:58:19] W
[20:58:19] o
[20:58:19] r
[20:58:19] l
[20:58:19] d
[20:58:21] [ENTER]
=== Session ended: Thu May 28 20:58:22 2026
```

## Testing

```bash
# Compile check
gcc -Wall -Wextra -o stroke stroke.c

# Run (add /dev/input/eventX if auto-detect fails)
./stroke

# Check log output
cat keystrokes.log
```

## License

**Educational purpose only.** Use responsibly and only on systems you own or have explicit permission to test.

---

<sub>Built with [opencode](https://opencode.ai)</sub>
