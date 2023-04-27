#!/bin/bash

ROOT=$(pwd)
ASSETS=$ROOT/assets
ASSETS_URL=https://github.com/SchwarzStein/arm-cca-attestation-syscall-artefacts/releases/download/init/assets.tar.gz


if [ -d "$ASSETS" ]; then
  # Take action if $DIR exists. #
  echo "Assets directory ${DIR} exists, skipping"
  echo "Delete the directory and re-run this script if necessary"
  exit
fi

wget $ASSETS_URL
tar -xzvf assets.tar.gz

