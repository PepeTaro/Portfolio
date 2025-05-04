#!/bin/bash

echo "[*] コンパイル中..."
javac -d ../bin ../src/*.java ./Demo.java
if [ $? -eq 1 ]; then
    echo "[!] コンパイルエラー"
    exit 1
fi

echo "[*] デモ実行"
cd ../bin/
java Demo
