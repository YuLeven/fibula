#!/bin/bash

# Configuration variables
REMOTE_USER="fibula"
PROJECT_PATH="/home/${REMOTE_USER}/fibula/Fibula.uproject"
OUTPUT_DIR="/home/${REMOTE_USER}/release"
STAGING_DIR="/home/${REMOTE_USER}/release"
UE_PATH="../UnrealEngine-5.5.0-release"
BUILD_CONFIG="${BUILD_CONFIG:-Development}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if project file exists
if [ ! -f "$PROJECT_PATH" ]; then
    echo -e "${RED}Error: Project file not found at $PROJECT_PATH${NC}"
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"
mkdir -p "$STAGING_DIR"

# Clear the output directory
echo -e "${YELLOW}Clearing output directory...${NC}"
rm -rf "$OUTPUT_DIR"/*

echo -e "${YELLOW}Starting Dedicated Server Build Process...${NC}"

# Execute the build command
"$UE_PATH/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun \
    -project="$PROJECT_PATH" \
    -noP4 \
    -clientconfig="$BUILD_CONFIG" \
    -serverconfig="$BUILD_CONFIG" \
    -server \
    -cook \
    -allmaps \
    -build \
    -stage \
    -pak \
    -package \
    -PackageOutputDir="$OUTPUT_DIR" \
    -stagingdirectory="$STAGING_DIR" \
    -target="FibulaServer"

BUILD_RESULT=$?

if [ $BUILD_RESULT -eq 0 ]; then
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "Output location: ${YELLOW}$OUTPUT_DIR${NC}"

    # Create timestamp for unique naming
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    SERVER_PACKAGE_NAME="fibula_server_${TIMESTAMP}.tar.gz"

    echo -e "${YELLOW}Creating tarball...${NC}"
    cd "$OUTPUT_DIR"
    tar -czf "$SERVER_PACKAGE_NAME" *

    # Generate SHA256 checksum
    echo -e "${YELLOW}Generating checksum...${NC}"
    SHA256SUM=$(sha256sum "$SERVER_PACKAGE_NAME" | cut -d' ' -f1)
    FINAL_PACKAGE_NAME="fibula_server_${SHA256SUM:0:8}.tar.gz"

    # Rename the file with checksum
    mv "$SERVER_PACKAGE_NAME" "$FINAL_PACKAGE_NAME"

    # Upload to server using SFTP
    echo -e "${YELLOW}Uploading to server...${NC}"
    sftp -P 13000 ian@fibulapvp.com <<EOF
put "$FINAL_PACKAGE_NAME"
bye
EOF

    UPLOAD_RESULT=$?

    if [ $UPLOAD_RESULT -eq 0 ]; then
        echo -e "${GREEN}Upload completed successfully!${NC}"
        echo -e "Package: ${YELLOW}$FINAL_PACKAGE_NAME${NC}"
        echo -e "Checksum: ${YELLOW}${SHA256SUM}${NC}"
    else
        echo -e "${RED}Upload failed with error code: $UPLOAD_RESULT${NC}"
        exit $UPLOAD_RESULT
    fi

    # Cleanup
    rm "$FINAL_PACKAGE_NAME"
else
    echo -e "${RED}Build failed with error code: $BUILD_RESULT${NC}"
    exit $BUILD_RESULT
fi 