#include <cstdlib>
#include <cstring>
#include <ctime>
#include <csignal>

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>
#include <fcntl.h>

using namespace std;

struct CacheEntry {
    string ipAddr;
    time_t ttlExpiry;
};

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
    umask(0);

    char projectDir[PATH_MAX];
    if (getcwd(projectDir, sizeof(projectDir)) != nullptr) {
        if (chdir(projectDir) < 0) {
            exit(EXIT_FAILURE);
        }
    } else {
        exit(EXIT_FAILURE);
    }

    int devNull = open("/dev/null", O_RDWR);
    dup2(devNull, STDIN_FILENO);
    dup2(devNull, STDOUT_FILENO);
    dup2(devNull, STDERR_FILENO);

    close(devNull);
}

unordered_map<string, CacheEntry> dnsCache;

string resolveDNS(const string &domain) {
    time_t currentTime = time(NULL);

    if (dnsCache.find(domain) != dnsCache.end()) {
        CacheEntry entry = dnsCache[domain];
        if (entry.ttlExpiry > currentTime) {
            logMessage("Cache hit for domain: " + domain);
            return entry.ipAddr;
        } else {
            logMessage("Cache expired for domain: " + domain);
            dnsCache.erase(domain);
        }
    }

    logMessage("Cache miss for domain: " + domain);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    int result = getaddrinfo(domain.c_str(), NULL, &hints, &res);
    if (result != 0) {
        logMessage("Failed to resolve domain: " + domain + " (" + gai_strerror(result) + ")");
        return "";
    }

    char ipAddr[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in*) res->ai_addr;
    inet_ntop(AF_INET, &addr->sin_addr, ipAddr, INET_ADDRSTRLEN);

    freeaddrinfo(res);

    CacheEntry newEntry { string(ipAddr), currentTime + 60 };
    dnsCache[domain] = newEntry;

    logMessage("Resolved IP address for domain: " + domain + " -> " + ipAddr);

    return string(ipAddr);
}

int main() {
    daemonize();
    
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    vector<string> domains = {"github.com", "codeforces.com", "leetcode.com"};
    while (true) {
        for (const string &domain : domains) {
            string ipAddr = resolveDNS(domain);
            if (!ipAddr.empty()) {
                logMessage("Resolved IP address for domain: " + domain + " -> " + ipAddr);
            } else {
                logMessage("Failed to resolve domain: " + domain);
            }
        }

        sleep(60);
    }

    return 0;
}
