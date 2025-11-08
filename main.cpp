// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>      // for sleep()
#include <dirent.h>      // for process directory listing
#include <iomanip>       // for formatting
#include <sstream>
#include <vector>
#include <algorithm>     // for all_of

using namespace std;

// ANSI color codes for terminal
#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"

// Function to calculate CPU usage (reads /proc/stat)
float getCPUUsage() {
    static unsigned long long lastUser = 0, lastNice = 0, lastSystem = 0, lastIdle = 0, lastIowait = 0, lastIrq = 0, lastSoftIrq = 0, lastSteal = 0;
    ifstream file("/proc/stat");
    if (!file.is_open()) return 0.0f;

    string cpu;
    unsigned long long user=0, nice=0, system=0, idle=0, iowait=0, irq=0, softirq=0, steal=0;
    // read the first line starting with "cpu"
    file >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    unsigned long long lastTotal = lastUser + lastNice + lastSystem + lastIdle + lastIowait + lastIrq + lastSoftIrq + lastSteal;
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;

    unsigned long long totalDiff = total - lastTotal;
    unsigned long long idleDiff = (idle + iowait) - (lastIdle + lastIowait);

    float usage = 0.0f;
    if (totalDiff != 0) {
        usage = (float)(totalDiff - idleDiff) / (float)totalDiff * 100.0f;
    }

    lastUser = user; lastNice = nice; lastSystem = system; lastIdle = idle;
    lastIowait = iowait; lastIrq = irq; lastSoftIrq = softirq; lastSteal = steal;

    return usage;
}

// Function to calculate memory usage (reads /proc/meminfo)
float getMemoryUsage() {
    ifstream file("/proc/meminfo");
    if (!file.is_open()) return 0.0f;

    string label;
    unsigned long long totalMem = 0, freeMem = 0, bufferMem = 0, cacheMem = 0;
    while (file >> label) {
        if (label == "MemTotal:") { file >> totalMem; }
        else if (label == "MemFree:") { file >> freeMem; }
        else if (label == "Buffers:") { file >> bufferMem; }
        else if (label == "Cached:") { file >> cacheMem; }
        // skip rest of the line if any
        if (file.peek() == '\n') file.ignore();
    }
    file.close();

    if (totalMem == 0) return 0.0f;
    unsigned long long usedMem = totalMem - (freeMem + bufferMem + cacheMem);
    float usage = (float)usedMem / (float)totalMem * 100.0f;
    return usage;
}

// Function to count total running processes by counting numeric entries in /proc
int getProcessCount() {
    int count = 0;
    DIR* dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            string name = entry->d_name;
            if (all_of(name.begin(), name.end(), ::isdigit)) {
                count++;
            }
        }
    }
    closedir(dir);
    return count;
}

// Helper to choose color based on value
const char* colorForValue(float val) {
    if (val < 50.0f) return GREEN;
    else if (val < 80.0f) return YELLOW;
    else return RED;
}

// Function to read uptime (seconds)
unsigned long long getUptimeSeconds() {
    ifstream up("/proc/uptime");
    if (!up.is_open()) return 0;
    double uptime = 0.0;
    up >> uptime;
    up.close();
    return (unsigned long long)uptime;
}

void printHeader() {
    cout << CYAN << "========================================\n";
    cout << "        SYSTEM MONITOR TOOL (C++)       \n";
    cout << "========================================" << RESET << "\n\n";
}

// Function to display system statistics (color-coded)
void displaySystemStats() {
    // compute values
    float cpu = getCPUUsage();
    float mem = getMemoryUsage();
    int procCount = getProcessCount();
    unsigned long long uptimeSec = getUptimeSeconds();

    // format uptime
    unsigned int hours = uptimeSec / 3600;
    unsigned int minutes = (uptimeSec % 3600) / 60;

    system("clear");  // clear for live update

    printHeader();

    cout << fixed << setprecision(2);
    cout << "CPU Usage:      " << colorForValue(cpu) << setw(6) << cpu << " %" << RESET << "\n";
    cout << "Memory Usage:   " << colorForValue(mem) << setw(6) << mem << " %" << RESET << "\n";

    // processes (white)
    cout << WHITE << "Processes:      " << RESET << procCount << "\n";

    // uptime
    cout << WHITE << "Uptime:         " << RESET << hours << "h " << minutes << "m\n";

    cout << "\n----------------------------------------\n";
    cout << "Refreshing every 2 seconds. Press Ctrl+C to exit.\n";
}

int main(int argc, char** argv) {
    // If user passes an interval arg, use it (in seconds)
    int interval = 2;
    if (argc >= 2) {
        try {
            int v = stoi(argv[1]);
            if (v > 0 && v <= 10) interval = v;
        } catch (...) { /* ignore */ }
    }

    // initial sleep to allow CPU baseline to get previous values on first call
    // call getCPUUsage() once to prime internal static values
    getCPUUsage();

    while (true) {
        displaySystemStats();
        sleep(interval);
    }
    return 0;
}
