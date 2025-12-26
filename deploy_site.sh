#!/bin/bash

# Configuration variables
REMOTE_USER="fibula"
PROJECT_PATH="/home/${REMOTE_USER}/fibula/fibula_site"
OUTPUT_DIR="/home/${REMOTE_USER}/release"
REMOTE_HOST="fibulapvp.com"
REMOTE_PORT="13000"
REMOTE_DEPLOY_PATH="/home/${REMOTE_USER}/fibula_site"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if project directory exists
if [ ! -d "$PROJECT_PATH" ]; then
    echo -e "${RED}Error: Project directory not found at $PROJECT_PATH${NC}"
    exit 1
fi

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

echo -e "${YELLOW}Starting Phoenix Release Build Process...${NC}"

# Navigate to project directory
cd "$PROJECT_PATH"

# Build release
echo -e "${YELLOW}Building release...${NC}"
MIX_ENV=prod mix do deps.get --only prod, compile, assets.deploy, release

BUILD_RESULT=$?

if [ $BUILD_RESULT -eq 0 ]; then
    echo -e "${GREEN}Build completed successfully!${NC}"
    
    # Create timestamp for unique naming
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    RELEASE_NAME="fibula_site_${TIMESTAMP}.tar.gz"
    
    echo -e "${YELLOW}Creating tarball...${NC}"
    cd "_build/prod/rel/"
    tar -czf "${OUTPUT_DIR}/${RELEASE_NAME}" fibula_site
    
    # Generate SHA256 checksum
    echo -e "${YELLOW}Generating checksum...${NC}"
    cd "${OUTPUT_DIR}"
    SHA256SUM=$(sha256sum "$RELEASE_NAME" | cut -d' ' -f1)
    FINAL_RELEASE_NAME="fibula_site_${SHA256SUM:0:8}.tar.gz"
    
    # Rename the file with checksum
    mv "$RELEASE_NAME" "$FINAL_RELEASE_NAME"
    
    # Upload to server using SFTP
    echo -e "${YELLOW}Uploading to server...${NC}"
    sftp -P $REMOTE_PORT ${REMOTE_USER}@${REMOTE_HOST} <<EOF
put "$FINAL_RELEASE_NAME"
bye
EOF
    
    UPLOAD_RESULT=$?
    
    if [ $UPLOAD_RESULT -eq 0 ]; then
        echo -e "${GREEN}Upload completed successfully!${NC}"
        echo -e "Package: ${YELLOW}$FINAL_RELEASE_NAME${NC}"
        echo -e "Checksum: ${YELLOW}${SHA256SUM}${NC}"

        # Deploy on remote server
        echo -e "${YELLOW}Deploying on remote server...${NC}"
        ssh -p $REMOTE_PORT ${REMOTE_USER}@${REMOTE_HOST} << EOF
            # Create deploy directory if it doesn't exist
            mkdir -p ${REMOTE_DEPLOY_PATH}
            
            # Clean up old deployment
            rm -rf ${REMOTE_DEPLOY_PATH}/*
            
            # Unpack new release
            tar -xzf /home/${REMOTE_USER}/$FINAL_RELEASE_NAME
            
            # Clean up tarball
            rm $FINAL_RELEASE_NAME
EOF

        DEPLOY_RESULT=$?
        
        if [ $DEPLOY_RESULT -eq 0 ]; then
            echo -e "${GREEN}Remote deployment completed successfully!${NC}"
        else
            echo -e "${RED}Remote deployment failed with error code: $DEPLOY_RESULT${NC}"
            exit $DEPLOY_RESULT
        fi
    else
        echo -e "${RED}Upload failed with error code: $UPLOAD_RESULT${NC}"
        exit $UPLOAD_RESULT
    fi
    
    rm "$FINAL_RELEASE_NAME"
else
    echo -e "${RED}Build failed with error code: $BUILD_RESULT${NC}"
    exit $BUILD_RESULT
fi 