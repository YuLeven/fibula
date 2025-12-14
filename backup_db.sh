#!/bin/bash

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
BACKUP_FILE="fibula_backup_$TIMESTAMP.sql"
LOCAL_DIR="/home/backups/fibula"

# Create backup directory if it doesn't exist
mkdir -p $LOCAL_DIR

# Run with pseudo-terminal allocation (-t)
ssh -t -p 13000 ian@fibulapvp.com "bash /home/ian/backup_database.sh" > "$LOCAL_DIR/$BACKUP_FILE"

# Check if the file is empty
if [ ! -s "$LOCAL_DIR/$BACKUP_FILE" ]; then
    echo "Backup file is empty"
    rm -f "$LOCAL_DIR/$BACKUP_FILE"
    exit 1
fi

echo "Database backup completed: $LOCAL_DIR/$BACKUP_FILE" 