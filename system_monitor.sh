#!/bin/bash

# Configuration & File Definitions
LOG_FILE="./system_monitor.log"
CONFIG_FILE="./monitor_config.cfg"
MONITOR_INTERVAL=60 
export PS4='+($LINENO): ' 

# Initialization and Setup
load_config() {
    if [[ ! -f "$CONFIG_FILE" ]]; then
        echo "Creating default configuration file: $CONFIG_FILE"
        cat > "$CONFIG_FILE" <<- EOF
CPU_THRESHOLD=80
MEM_THRESHOLD=90
DISK_THRESHOLD=95
EOF
    fi
    source "$CONFIG_FILE"
}

# Function to log all activity with a timestamp
log_activity() {
    local message="$1"
    echo "$(date +"%Y-%m-%d %H:%M:%S") | $message" | tee -a "$LOG_FILE"
}

# Function to extract system metrics and store them globally
get_system_metrics() {
    CPU_IDLE=$(top -bn1 2>/dev/null | grep "Cpu(s)" | sed "s/.*, *\([0-9.]*\)%* id.*/\1/")
    CPU_USAGE=$(awk "BEGIN {print int(100 - $CPU_IDLE)}")

    # Memory Usage (%) - Use 'free' to get used/total in MB
    MEM_USED_MB=$(free -m 2>/dev/null | awk 'NR==2{print $3}')
    MEM_TOTAL_MB=$(free -m 2>/dev/null | awk 'NR==2{print $2}')
    if [[ -z "$MEM_TOTAL_MB" || "$MEM_TOTAL_MB" -eq 0 ]]; then
        MEM_USAGE=0
    else
        MEM_USAGE=$(awk "BEGIN {print int(($MEM_USED_MB/$MEM_TOTAL_MB) * 100)}")
    fi

    DISK_USAGE=$(df -h / 2>/dev/null | awk 'NR==2{print $5}' | sed 's/%//')
    
    # Running Processes Count
    PROC_COUNT=$(ps -e 2>/dev/null | wc -l)

    # Error Handling
    if [[ -z "$CPU_USAGE" || -z "$DISK_USAGE" ]]; then
        log_activity "ERROR: Failed to retrieve all system metrics. Missing commands or permissions."
    fi
}

# Function to check thresholds and issue alerts
check_thresholds() {
    local alert_issued=false

    if (( CPU_USAGE > CPU_THRESHOLD )); then
        log_activity "ALERT! High CPU Usage: ${CPU_USAGE}% (Threshold: ${CPU_THRESHOLD}%)"
        alert_issued=true
    fi

    if (( MEM_USAGE > MEM_THRESHOLD )); then
        log_activity "ALERT! High Memory Usage: ${MEM_USAGE}% (Threshold: ${MEM_THRESHOLD}%)"
        alert_issued=true
    fi

    if (( DISK_USAGE > DISK_THRESHOLD )); then
        log_activity "ALERT! High Disk Usage: ${DISK_USAGE}% (Threshold: ${DISK_THRESHOLD}%)"
        alert_issued=true
    fi

    if ! $alert_issued; then
        log_activity "Status OK: CPU=${CPU_USAGE}%, Mem=${MEM_USAGE}%, Disk=${DISK_USAGE}%"
    fi
}

# Function to run the monitoring loop in the background
start_monitoring() {
    log_activity "Starting automated monitoring (Interval: ${MONITOR_INTERVAL}s)..."
    ( while true; do
        load_config # Reload config in case thresholds changed
        get_system_metrics
        check_thresholds
        sleep $MONITOR_INTERVAL
    done ) & # Run in background
    MONITOR_PID=$!
    echo "Monitoring process started with PID: $MONITOR_PID"
}

# Function to stop the monitoring loop
stop_monitoring() {
    local pid_to_kill=$(ps aux | grep "start_monitoring" | grep -v grep | awk '{print $2}')
    if [[ ! -z "$pid_to_kill" ]]; then
        kill "$pid_to_kill" 2>/dev/null
        log_activity "Monitoring process (PID $pid_to_kill) stopped."
    else
        echo "No active monitoring process found."
    fi
}

# Function to allow the user to change alert thresholds
set_thresholds() {
    echo "-- Set Alert Thresholds --"
    read -p "New CPU Threshold (current: $CPU_THRESHOLD%): " new_cpu
    read -p "New Memory Threshold (current: $MEM_THRESHOLD%): " new_mem
    read -p "New Disk Threshold (current: $DISK_THRESHOLD%): " new_disk

    # Simple validation
    if [[ "$new_cpu" =~ ^[0-9]+$ && "$new_cpu" -le 100 ]]; then
        sed -i "s/CPU_THRESHOLD=.*/CPU_THRESHOLD=$new_cpu/" "$CONFIG_FILE"
    else
        log_activity "Invalid CPU value. Keeping $CPU_THRESHOLD."
    fi

    if [[ "$new_mem" =~ ^[0-9]+$ && "$new_mem" -le 100 ]]; then
        sed -i "s/MEM_THRESHOLD=.*/MEM_THRESHOLD=$new_mem/" "$CONFIG_FILE"
    else
        log_activity "Invalid Memory value. Keeping $MEM_THRESHOLD."
    fi

    if [[ "$new_disk" =~ ^[0-9]+$ && "$new_disk" -le 100 ]]; then
        sed -i "s/DISK_THRESHOLD=.*/DISK_THRESHOLD=$new_disk/" "$CONFIG_FILE"
    else
        log_activity "Invalid Disk value. Keeping $DISK_THRESHOLD."
    fi
    load_config
    echo "Thresholds updated. The monitoring loop will detect changes on the next cycle."
}

# Menu Functions

view_status() {
    get_system_metrics
    echo "-- Current System Status --"
    echo "Time: $(date +"%H:%M:%S")"
    echo "CPU Usage:        ${CPU_USAGE}% (Threshold: ${CPU_THRESHOLD}%)"
    echo "Memory Usage:     ${MEM_USAGE}% (Threshold: ${MEM_THRESHOLD}%)"
    echo "Disk Usage (/):   ${DISK_USAGE}% (Threshold: ${DISK_THRESHOLD}%)"
    echo "Running Processes: $PROC_COUNT"
}

view_logs() {
    echo "-- System Logs ($LOG_FILE) --"
    if [[ -f "$LOG_FILE" ]]; then
        tail -n 20 "$LOG_FILE"
    else
        echo "Log file not found."
    fi
}

clear_logs() {
    if [[ -f "$LOG_FILE" ]]; then
        read -r -p "Are you sure you want to clear the logs? (y/N): " response
        if [[ "$response" =~ ^([yY])$ ]]; then
            > "$LOG_FILE" # Clears the file content
            log_activity "Log file successfully cleared by user."
            echo "Logs cleared."
        fi
    else
        echo "Log file does not exist."
    fi
}

display_menu() {
    echo -e "\n--- Automated System Monitor Menu ---"
    echo "1. View current system status"
    echo "2. Set alert thresholds"
    echo "3. View logs (last 20 entries)"
    echo "4. Clear logs"
    echo "5. Start automated monitoring (Background)"
    echo "6. Stop automated monitoring"
    echo "0. Exit"
    echo "-------------------------------------"
}

# Main Program Logic

load_config
trap 'stop_monitoring; log_activity "Monitor script terminated."; exit' INT TERM EXIT

while true; do
    display_menu
    read -r -p "Enter your choice (0-6): " choice

    case "$choice" in
        1) view_status ;;
        2) set_thresholds ;;
        3) view_logs ;;
        4) clear_logs ;;
        5) start_monitoring ;;
        6) stop_monitoring ;;
        0) log_activity "Exiting System Monitor Script."; exit 0 ;;
        *) log_activity "Invalid input: $choice. Please enter a number from 0 to 6." ;;
    esac
done
