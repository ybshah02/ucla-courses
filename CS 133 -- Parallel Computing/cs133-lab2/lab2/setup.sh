#!/bin/bash
if [ "$(uname -s)" == 'Darwin' ]; then
  xcode-select --install
  which brew || /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  brew install mpich llvm boost zip
else
  sudo apt update
  sudo env DEBIAN_FRONTEND=noninteractive apt install build-essential mpich zip -y
fi
