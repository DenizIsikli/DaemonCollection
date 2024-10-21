#include <cstdlib>
#include <cstring>
#include <ctime>
#include <csignal>

#include <iostream>
#include <fstream>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void logMessage(const string &message) {
    ofstream logFile("./dns_cache_daemon.log", ios_base::app);
    if (logFile.is_open()) {
        logFile << time(NULL) << ": " << message << endl;
    }
}

void signalHandler(int signal) {
    if (signal == SIGTERM) {
        logMessage("Received SIGTERM, shutting down gracefully...");
    } else if (signal == SIGINT) {
        logMessage("Received SIGINT, shutting down gracefully...");
    }

    exit(0);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) close(fd);

    int devNull = open("/dev/null", O_RDWR);
    dup2(devNull, STDIN_FILENO);
    dup2(devNull, STDOUT_FILENO);
    dup2(devNull, STDERR_FILENO);
}

void sysTemp() {
    vector<pair<string, string>> sensorPaths = {
        {"CPU", "/sys/class/thermal/thermal_zone0/temp"},
        {"RAM", "/sys/class/hwmon/hwmon2/temp"}
    };

    for (const auto &[sensorName, sensorPath] : sensorPaths) {
        if (sensorPath.find("thermal") != string::npos) {
            ifstream tempFile(sensorPath);
            if (tempFile.is_open()) {
                int tempMillidegrees;
                tempFile >> tempMillidegrees;
                double temperature = tempMillidegrees / 1000.0;

                logMessage(sensorName + " temperature: " + to_string(temperature) + " °C");
            } else {
                logMessage("Failed to read temperature for " + sensorName);
            }
        } else {
            int idx = 1;
            while (true) {
                string labelPath = sensorPath + to_string(idx) + "_label";
                string inputPath = sensorPath + to_string(idx) + "_input";
                ifstream tempFileLabel(labelPath);
                ifstream tempFile(inputPath);

                if (tempFileLabel.is_open() && tempFile.is_open()) {
                    string label;
                    int tempMillidegrees;

                    getline(tempFileLabel, label);
                    tempFile >> tempMillidegrees;
                    double temperature = tempMillidegrees / 1000.0;

                    logMessage(sensorName + " (" + label + ") temperature: " + to_string(temperature) + " °C");
                } else {
                    if (!tempFileLabel.is_open() && !tempFile.is_open()) {
                        break;
                    } else {
                        logMessage("Failed to read temperature for " + sensorName + " at index " + to_string(idx));
                    }
                }
                idx++;
            }
        }
    }
}

int main() {
    daemonize();

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    while (true) {
        sysTemp();
        sleep(5);
    }

    return 0;
}
