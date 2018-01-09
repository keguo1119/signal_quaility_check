#!/bin/sh

rm -rf scanner/
git clone https://github.com/keguo1119/scanner.git

cd scanner
chmod 0777 scanner_2p
mkdir file
./scanner_2p
