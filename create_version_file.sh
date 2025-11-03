#!/bin/bash
export LC_ALL=it_IT
VERSION=$(git describe --tags)
DATE=$(date +"%d %B %Y")

cat <<EOF
#define VERSIONSTRING "versione ${VERSION} del ${DATE}"
EOF

