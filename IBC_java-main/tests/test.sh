#!/bin/bash

run_test(){
    echo "[*] $1"
    java $1
    if [ $? -eq 1 ]; then
	echo "[!] テストエラー"
	exit 1
    fi
}

echo "[*] コンパイル中..."
javac -d ../bin ../src/*.java ./*.java
if [ $? -eq 1 ]; then
    echo "[!] コンパイルエラー"
    exit 1
fi

echo "[*] テストを実行中..."
cd ../bin/

run_test TestFq
run_test TestFq2
run_test TestEC
run_test TestPairing
run_test TestIBC

echo "[*] テスト終了"
