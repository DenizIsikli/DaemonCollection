# LocalDNSCacheDaemon Setup Guide
This guide will help you set up and run the `LocalDNSCacheDaemon` on your system.

## Prerequisites

Before starting, make sure the following are installed:

1. **g++** (for compiling the C++ code)
2. **systemd** (to manage the daemon as a service)

To install `g++` and `systemd`, run the following commands (for Debian/Ubuntu-based systems):

```bash
sudo apt update
sudo apt install g++ systemd
```

## Step 1: Clone the repository

```bash
git clone https://github.com/DenizIsikli/LocalDNSCacheDaemon.git -- HTTPS
git clone git@github.com:DenizIsikli/LocalDNSCacheDaemon.git -- SSH
cd LocalDNSCacheDaemon
```

## Step 2: Compile the Daemon

```bash
g++ -o LocalDNSCacheDaemon LocalDNSCacheDaemon.cpp
```

## Step 3: Create a Systemd Service

To run the daemon as a background service, create a sytemd service file.

1. Open a new service file using a text editor:

```bash
sudo nano /etc/systemd/system/LocalDNSCacheDaemon.service
```

2. Copy and paste the following content into the file:

```ini
[Unit]
Description=Local DNS Cache Daemon
After=network.target

[Service]
ExecStart=/path/to/LocalDNSCacheDaemon
Restart=always
User=root
Group=root

[Install]
WantedBy=multi-user.target
```

**Important:** Replace `/path/to/LocalDNSCacheDaemon` with the absolute path to the compiled daemon executable in your project folder.

3. Save and close the file.

## Step 4: Reload Systemd and Start the Service

Reload systemd to register the new service

```bash
sudo systemctl daemon-reload
```

Start the service

```bash
sudo systemctl start LocalDNSCacheDaemon
```

Enable the service to start on boot

```bash
sudo systemctl enable LocalDNSCacheDaemon
```

## Step 5: Stop or Restart the Daemon

To stop the daemon:

```bash
sudo systemctl stop LocalDNSCacheDaemon
```

To restart the daemon:

```bash
sudo systemctl restart LocalDNSCacheDaemon
```
