# MQTT IoT Sensor Control System - Complete Documentation

## Table of Contents
1. [Project Overview](#project-overview)
2. [System Architecture](#system-architecture)
3. [Project Structure](#project-structure)
4. [Prerequisites](#prerequisites)
5. [Installation Guide](#installation-guide)
6. [Configuration Details](#configuration-details)
7. [Usage Instructions](#usage-instructions)
8. [API Reference](#api-reference)
9. [Monitoring and Debugging](#monitoring-and-debugging)
10. [Troubleshooting](#troubleshooting)
11. [Security Considerations](#security-considerations)
12. [Performance Optimization](#performance-optimization)
13. [Deployment](#deployment)
14. [Contributing](#contributing)

## Project Overview

This project implements a complete IoT sensor monitoring and control system using MQTT protocol. The system simulates an ESP32 device that publishes sensor data (temperature, humidity, light intensity) and subscribes to control commands through a centralized MQTT broker.

### Key Features
- Dockerized MQTT broker (Eclipse Mosquitto)
- User authentication and access control
- Real-time sensor data simulation
- Web-based dashboard using Node-RED
- SSH access for container management
- Comprehensive logging and monitoring
- Production-ready configuration

### Technology Stack
- **MQTT Broker**: Eclipse Mosquitto 2.x
- **Container Platform**: Docker & Docker Compose
- **Dashboard**: Node-RED
- **Simulation**: Python 3.x with paho-mqtt
- **Database**: Mosquitto persistence
- **Web Interface**: Node-RED Dashboard

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     MQTT IoT System Architecture                │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────────────┐    ┌─────────────────────────────────┐
│      Node-RED Dashboard  │    │         MQTT Explorer           │
│  ├─ Web UI (Port 1880)   │    │  ├─ Topic Monitoring            │
│  ├─ Control Panel        │◄──►│  ├─ Message Inspector           │
│  ├─ Data Visualization   │    │  └─ Debug Tool                  │
│  └─ START/STOP Commands  │    └─────────────────────────────────┘
└─────────────┬────────────┘                       │
              │                                    │
              ▼                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                 Mosquitto MQTT Broker                          │
│  ├─ Authentication (username/password)                         │
│  ├─ Access Control Lists (ACL)                                 │
│  ├─ Topic: UAS25-IOT/33423315/SUHU                            │
│  ├─ Topic: UAS25-IOT/33423315/KELEMBAPAN                      │
│  ├─ Topic: UAS25-IOT/33423315/LUX                             │
│  ├─ Topic: UAS25-IOT/Status                                   │
│  ├─ Persistence Layer                                          │
│  └─ SSH Access (Port 2222)                                    │
└─────────────┬───────────────────────────────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────────────────────────────┐
│              Python ESP32 Simulator (main.py)                  │
│  ├─ Subscribe: UAS25-IOT/Status                               │
│  ├─ Publish: UAS25-IOT/33423315/SUHU (Temperature)           │
│  ├─ Publish: UAS25-IOT/33423315/KELEMBAPAN (Humidity)        │
│  ├─ Publish: UAS25-IOT/33423315/LUX (Light Intensity)        │
│  └─ Auto START/STOP based on status                           │
└─────────────────────────────────────────────────────────────────┘
```

## Project Structure

```
mqtt/
├── README.md                    # This documentation
├── docker-compose.yml           # Container orchestration
├── main.py                      # Python ESP32 simulator
└── mosquitto/                   # MQTT broker configuration
    ├── Dockerfile               # Custom Mosquitto image with SSH
    ├── config/                  # Broker configuration files
    │   ├── mosquitto.conf       # Main broker configuration
    │   ├── passwd               # User authentication database
    │   └── acl.conf            # Access Control List
    ├── data/                    # Persistence storage
    │   └── mosquitto.db         # MQTT broker database
    └── log/                     # Log files
        └── mosquitto.log        # Broker activity logs
```

## Prerequisites

### System Requirements
- Operating System: Windows 10/11, macOS, or Linux
- RAM: Minimum 2GB, Recommended 4GB
- Storage: 1GB free space
- Network: Internet connection for Docker images

### Software Dependencies
- **Docker**: Version 20.10 or higher
- **Docker Compose**: Version 2.0 or higher
- **Python**: Version 3.8 or higher (for simulator)
- **Git**: For version control
- **Text Editor**: VS Code, Sublime Text, or similar

### Python Dependencies
```bash
pip install paho-mqtt
```

## Installation Guide

### Step 1: Environment Setup

1. **Install Docker Desktop**
   ```bash
   # Windows: Download from docker.com
   # macOS: Download from docker.com
   # Linux (Ubuntu):
   sudo apt update
   sudo apt install docker.io docker-compose
   sudo systemctl enable docker
   sudo usermod -aG docker $USER
   ```

2. **Verify Installation**
   ```bash
   docker --version
   docker-compose --version
   ```

### Step 2: Project Setup

1. **Clone or Create Project Structure**
   ```bash
   mkdir mqtt-iot-system
   cd mqtt-iot-system
   ```

2. **Create Directory Structure**
   ```bash
   mkdir -p mosquitto/{config,data,log}
   touch docker-compose.yml main.py
   touch mosquitto/Dockerfile
   touch mosquitto/config/{mosquitto.conf,passwd,acl.conf}
   ```

### Step 3: Generate User Passwords

1. **Create First User (uas25_ridwan)**
   ```bash
   docker run -it --rm \
     -v $(pwd)/mosquitto/config:/mosquitto/config \
     eclipse-mosquitto \
     mosquitto_passwd -c /mosquitto/config/passwd uas25_ridwan
   ```

2. **Add Second User (uas24_iot)**
   ```bash
   docker run -it --rm \
     -v $(pwd)/mosquitto/config:/mosquitto/config \
     eclipse-mosquitto \
     mosquitto_passwd /mosquitto/config/passwd uas24_iot
   ```

### Step 4: Build and Deploy

1. **Start All Services**
   ```bash
   docker-compose up -d --build
   ```

2. **Verify Deployment**
   ```bash
   docker ps
   ```

Expected output:
```
CONTAINER ID   IMAGE                     STATUS    PORTS                                          NAMES
xxxxxxxxxxxxx  mqtt-mosquitto           Up        0.0.0.0:1883->1883/tcp, 0.0.0.0:2222->22/tcp  mosquitto
xxxxxxxxxxxxx  nodered/node-red:latest  Up        0.0.0.0:1880->1880/tcp                         nodered
```

## Configuration Details

### Mosquitto Broker Configuration

**File: mosquitto/config/mosquitto.conf**
```ini
# Persistence configuration
persistence true
persistence_location /mosquitto/data/

# Logging configuration
log_dest file /mosquitto/log/mosquitto.log

# Network configuration
listener 1883
allow_anonymous false

# Authentication configuration
password_file /mosquitto/config/passwd
acl_file /mosquitto/config/acl.conf
```

### Access Control List Configuration

**File: mosquitto/config/acl.conf**
```ini
# User: uas25_ridwan permissions
user uas25_ridwan
topic readwrite UAS25-IOT/33423315/#
topic readwrite UAS25-IOT/Status

# User: uas24_iot permissions
user uas24_iot
topic readwrite UAS25-IOT/33423315/#
topic readwrite UAS25-IOT/Status
```

### Docker Compose Configuration

**File: docker-compose.yml**
```yaml
version: "3.8"

services:
  mosquitto:
    build: ./mosquitto
    container_name: mosquitto
    restart: unless-stopped
    ports:
      - "1883:1883"    # MQTT port
      - "2222:22"      # SSH port
    volumes:
      - ./mosquitto/config:/mosquitto/config
      - ./mosquitto/data:/mosquitto/data
      - ./mosquitto/log:/mosquitto/log

  nodered:
    image: nodered/node-red:latest
    container_name: nodered
    restart: unless-stopped
    ports:
      - "1880:1880"    # Node-RED web interface
    depends_on:
      - mosquitto
    volumes:
      - nodered_data:/data

volumes:
  nodered_data:
```

### Dockerfile Configuration

**File: mosquitto/Dockerfile**
```dockerfile
FROM eclipse-mosquitto:2

# Install SSH and setup with proper configuration for root login
RUN apk add --no-cache openssh \
    && echo 'root:root123' | chpasswd \
    && mkdir -p /var/run/sshd \
    && ssh-keygen -A \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Create startup script
RUN echo '#!/bin/sh' > /start.sh \
    && echo 'set -e' >> /start.sh \
    && echo '' >> /start.sh \
    && echo '/usr/sbin/sshd' >> /start.sh \
    && echo '' >> /start.sh \
    && echo 'exec /usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf' >> /start.sh \
    && chmod +x /start.sh

EXPOSE 22 1883

CMD ["/start.sh"]
```

## Usage Instructions

### Starting the System

1. **Start All Services**
   ```bash
   docker-compose up -d
   ```

2. **Check Service Status**
   ```bash
   docker-compose ps
   ```

3. **View Service Logs**
   ```bash
   docker-compose logs -f mosquitto
   docker-compose logs -f nodered
   ```

### ESP32 Simulator Usage

1. **Run Python Simulator**
   ```bash
   python main.py
   ```

2. **Expected Output**
   ```
   Connecting to MQTT broker...
   Connected to MQTT broker
   Subscribed to topic: UAS25-IOT/Status
   Waiting for START command...
   ```

3. **Send START Command**
   ```bash
   docker exec mosquitto mosquitto_pub \
     -h localhost \
     -u uas25_ridwan \
     -P uas25_ridwan \
     -t "UAS25-IOT/Status" \
     -m "START"
   ```

4. **Send STOP Command**
   ```bash
   docker exec mosquitto mosquitto_pub \
     -h localhost \
     -u uas25_ridwan \
     -P uas25_ridwan \
     -t "UAS25-IOT/Status" \
     -m "STOP"
   ```

### Node-RED Dashboard Setup

1. **Access Node-RED Interface**
   ```
   URL: http://localhost:1880
   ```

2. **Install Dashboard Nodes**
   - Go to Menu > Manage palette
   - Install: node-red-dashboard

3. **Create Control Flow**
   - Add MQTT In node (subscribe to UAS25-IOT/Status)
   - Add MQTT Out node (publish to UAS25-IOT/Status)
   - Add Inject nodes for START/STOP commands
   - Add Dashboard nodes for visualization

4. **Deploy Configuration**
   - Click Deploy button
   - Access dashboard at: http://localhost:1880/ui

### Manual MQTT Commands

**Subscribe to All Sensor Data**
```bash
docker exec mosquitto mosquitto_sub \
  -h localhost \
  -u uas25_ridwan \
  -P uas25_ridwan \
  -t "UAS25-IOT/33423315/#"
```

**Subscribe to Status Topic**
```bash
docker exec mosquitto mosquitto_sub \
  -h localhost \
  -u uas25_ridwan \
  -P uas25_ridwan \
  -t "UAS25-IOT/Status"
```

**Publish Test Temperature**
```bash
docker exec mosquitto mosquitto_pub \
  -h localhost \
  -u uas25_ridwan \
  -P uas25_ridwan \
  -t "UAS25-IOT/33423315/SUHU" \
  -m "25.5"
```

## API Reference

### MQTT Topics

| Topic | Description | Data Type | Access |
|-------|-------------|-----------|---------|
| `UAS25-IOT/33423315/SUHU` | Temperature sensor data | Float (Celsius) | Read/Write |
| `UAS25-IOT/33423315/KELEMBAPAN` | Humidity sensor data | Float (Percentage) | Read/Write |
| `UAS25-IOT/33423315/LUX` | Light intensity data | Integer (Lumens) | Read/Write |
| `UAS25-IOT/Status` | System control commands | String (START/STOP) | Read/Write |

### Message Formats

**Temperature Message**
```json
{
  "topic": "UAS25-IOT/33423315/SUHU",
  "payload": "26.5",
  "timestamp": "2024-01-15T10:30:00Z"
}
```

**Status Message**
```json
{
  "topic": "UAS25-IOT/Status",
  "payload": "START",
  "timestamp": "2024-01-15T10:30:00Z"
}
```

### Authentication

**Users and Permissions**
- **Username**: uas25_ridwan
  - Password: [Generated during setup]
  - Permissions: Full access to UAS25-IOT topics
- **Username**: uas24_iot
  - Password: [Generated during setup]
  - Permissions: Full access to UAS25-IOT topics

## Monitoring and Debugging

### Container Health Checks

1. **Check Container Status**
   ```bash
   docker ps
   docker-compose ps
   ```

2. **View Container Logs**
   ```bash
   docker logs mosquitto
   docker logs nodered
   ```

3. **Execute Commands in Container**
   ```bash
   docker exec -it mosquitto /bin/sh
   docker exec -it nodered /bin/bash
   ```

### MQTT Broker Monitoring

1. **View Mosquitto Logs**
   ```bash
   docker exec mosquitto cat /mosquitto/log/mosquitto.log
   ```

2. **Monitor Real-time Logs**
   ```bash
   docker logs -f mosquitto
   ```

3. **Check Broker Status**
   ```bash
   docker exec mosquitto mosquitto_pub \
     -h localhost \
     -t '$SYS/broker/uptime' \
     -n
   ```

### Network Diagnostics

1. **Test MQTT Connectivity**
   ```bash
   # From host system
   telnet localhost 1883
   
   # From inside container
   docker exec mosquitto netstat -tlnp
   ```

2. **Check Port Accessibility**
   ```bash
   nmap -p 1883,1880,2222 localhost
   ```

### SSH Access for Debugging

1. **Connect to Mosquitto Container**
   ```bash
   ssh root@localhost -p 2222
   # Password: root123
   ```

2. **Common Debug Commands**
   ```bash
   # Inside container
   ps aux                    # Check running processes
   netstat -tlnp            # Check listening ports
   cat /etc/ssh/sshd_config # Check SSH configuration
   cat /mosquitto/config/mosquitto.conf # Check MQTT config
   ```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Container Won't Start**
```bash
# Check logs
docker-compose logs mosquitto

# Common causes:
# - Port already in use
# - Configuration file errors
# - Permission issues

# Solutions:
sudo lsof -i :1883          # Check port usage
docker-compose down         # Stop all services
docker-compose up --build  # Rebuild and restart
```

**Issue 2: MQTT Connection Refused**
```bash
# Check if mosquitto is running
docker ps | grep mosquitto

# Check if port is exposed
docker port mosquitto

# Test connectivity
docker exec mosquitto mosquitto_pub -h localhost -t test -m "hello"
```

**Issue 3: Authentication Failed**
```bash
# Verify user exists
docker exec mosquitto cat /mosquitto/config/passwd

# Regenerate password
docker run -it --rm \
  -v $(pwd)/mosquitto/config:/mosquitto/config \
  eclipse-mosquitto \
  mosquitto_passwd -c /mosquitto/config/passwd uas25_ridwan
```

**Issue 4: SSH Connection Failed**
```bash
# Check SSH service status
docker exec mosquitto ps aux | grep ssh

# Check SSH configuration
docker exec mosquitto cat /etc/ssh/sshd_config | grep -E "(PermitRootLogin|PasswordAuthentication)"

# Restart container if needed
docker-compose restart mosquitto
```

**Issue 5: Python Simulator Not Connecting**
```bash
# Check MQTT broker accessibility
docker exec mosquitto mosquitto_pub -h localhost -t test -m "test"

# Verify credentials in main.py
# Check topic permissions in acl.conf
```

### Log Analysis

**Mosquitto Log Levels**
- ERROR: Critical errors
- WARNING: Non-critical issues
- NOTICE: Important events
- INFORMATION: General information
- DEBUG: Detailed debugging information

**Common Log Patterns**
```bash
# Successful connection
grep "New connection" /mosquitto/log/mosquitto.log

# Authentication failures
grep "Bad username or password" /mosquitto/log/mosquitto.log

# Access control violations
grep "ACL" /mosquitto/log/mosquitto.log
```

## Security Considerations

### Authentication Security
- Use strong passwords for MQTT users
- Regularly rotate passwords
- Implement certificate-based authentication for production

### Network Security
- Use SSL/TLS for MQTT connections in production
- Implement firewall rules
- Use VPN for remote access

### Container Security
- Run containers with non-root users when possible
- Regularly update base images
- Use Docker secrets for sensitive data

### Access Control
- Implement least-privilege principle in ACL
- Regularly audit user permissions
- Monitor access logs

## Performance Optimization

### Container Optimization
```yaml
# Resource limits in docker-compose.yml
services:
  mosquitto:
    deploy:
      resources:
        limits:
          memory: 512M
          cpus: '0.5'
        reservations:
          memory: 256M
          cpus: '0.25'
```

### MQTT Broker Tuning
```ini
# High-performance mosquitto.conf settings
max_connections 1000
max_keepalive 65535
max_packet_size 1048576
memory_limit 134217728
```

### Monitoring Metrics
- Connection count
- Message throughput
- Memory usage
- CPU utilization
- Disk I/O

## Deployment

### Development Environment
```bash
# Quick start for development
docker-compose -f docker-compose.yml up -d
```

### Production Environment
```yaml
# docker-compose.prod.yml
version: "3.8"
services:
  mosquitto:
    build: ./mosquitto
    restart: always
    environment:
      - MOSQUITTO_LOG_LEVEL=warning
    volumes:
      - mosquitto_config:/mosquitto/config
      - mosquitto_data:/mosquitto/data
      - mosquitto_logs:/mosquitto/log

volumes:
  mosquitto_config:
  mosquitto_data:
  mosquitto_logs:
```

### Health Checks
```yaml
# Health check configuration
healthcheck:
  test: ["CMD", "mosquitto_pub", "-h", "localhost", "-t", "health", "-m", "check"]
  interval: 30s
  timeout: 10s
  retries: 3
  start_period: 10s
```

### Backup and Recovery
```bash
# Backup configuration and data
tar -czf mqtt-backup-$(date +%Y%m%d).tar.gz mosquitto/

# Restore from backup
tar -xzf mqtt-backup-20240115.tar.gz
```

## Contributing

### Development Guidelines
1. Follow semantic versioning
2. Write comprehensive tests
3. Update documentation
4. Use conventional commit messages

### Code Style
- Python: Follow PEP 8
- Shell scripts: Use shellcheck
- Docker: Follow best practices
- YAML: Use consistent indentation

### Testing
```bash
# Run tests
python -m pytest tests/
docker-compose -f docker-compose.test.yml up --abort-on-container-exit
```
