#!/bin/bash
export LANG=it_IT
VERSION=$(git describe)
DATE=$(date +"%d %B %Y")

cat <<EOF
#define VERSIONSTRING "versione ${VERSION} del ${DATE}"
EOF

