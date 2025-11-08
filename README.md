#  System Monitor Tool (C++ in Docker)

##  Project Overview
The **System Monitor Tool** is a console-based application developed in **C++** that displays real-time system information — including CPU usage, memory usage, process count, and system uptime.  
It replicates basic functionality of the Linux `top` command.

This project runs inside a **Docker container** using an Ubuntu environment, so it works even on Windows systems without requiring a native Linux OS.

---

##  Features
- Displays **CPU usage** in real-time (from `/proc/stat`)
- Shows **Memory usage** using data from `/proc/meminfo`
- Counts the number of **active processes**
- Displays **system uptime** in hours and minutes
- Refreshes automatically every 2 seconds
- **Color-coded output**:
  - 🟢 Green → Normal (<50%)
  - 🟡 Yellow → Moderate (50–80%)
  - 🔴 Red → High (>80%)

---

##  Tech Stack
- **Language:** C++
- **Environment:** Linux (Ubuntu via Docker)
- **Tools:** Docker, g++

---

##  Project Setup

###  Clone or Create Project Folder
```bash
mkdir system-monitor
cd system-monitor
