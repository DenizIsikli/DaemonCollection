# Daemon of Your Choice Setup Guide
This guide will help you set up and run the daemon of your choice on your system.

## Prerequisites

Before starting, make sure the following are installed:

1. **g++** (for compiling the C++ code)
2. **systemd** (to manage the daemon as a service)

To install `g++` and `systemd`, run the following commands (for Debian/Ubuntu-based systems):

```bash
sudo apt update
sudo apt install g++ systemd
```

## Step 1: Clone the Repository

1. Clone the repository using HTTPS or SSH

```bash
git clone https://github.com/YourUsername/daemoncollection.git -- HTTPS
git clone git@github.com:YourUsername/daemoncollection.git -- SSH
```
2. Navigate to the repository

```bash
cd daemoncollection/ProjectFolder
```

Replace `ProjectFolder` with the folder of the daemon you want to set up

## Step 2: Compile the Daemon

1. Compile the daemon using g++

```bash
g++ -o ProjectFolder ProjectFolder.cpp
```

Replace `ProjectFolder` with the name of your chosen project

## Step 3: Create a Systemd Service

To run the daemon as a background service, you need to create a systemd service file

1. Open a new service file using a text editor

```bash
sudo nano /etc/systemd/system/ProjectFolder.service
```

Replace `ProjectFolder` with the name of your chosen project

2. Copy and paste the following content into the file

```bash
[Unit]
Description=ProjectFolder Daemon
After=network.target

[Service]
ExecStart=/path/to/daemoncollection/ProjectFolder/ProjectFolder
Restart=always
User=root
Group=root

[Install]
WantedBy=multi-user.target
```

**Important:** Replace `/path/to/daemoncollection/ProjectFolder/ProjectFolder` with the absolute path to the compiled daemon executable

3. Save and exit the text editor

## Step 4: Reload Systemd and Start the Daemon

1. Reload systemd to register the new service

```bash
sudo systemctl daemon-reload
```

2. Start the daemon service

```bash
sudo systemctl start ProjectFolder
```

3. Enable the daemon to start on boot

```bash
sudo systemctl enable ProjectFolder
```

Replace `ProjectFolder` with the name of your chosen project

## Step 5: Stop or Restart the Daemon

1. To stop the daemon

```bash
sudo systemctl stop ProjectFolder
```

2. To restart the daemon

```bash
sudo systemctl restart ProjectFolder
```

## Notes
- `ProjectFolder` refers to the folder name of the daemon you want to set up
- Customize the paths and project-specific details as needed for each daemon
