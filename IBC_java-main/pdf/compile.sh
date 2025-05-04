#!/bin/bash

echo "[*] DVIファイル作成中..."
platex theory.tex
if [ $? -eq 1 ]; then
    echo "[!] DVIファイル作成に失敗"
    exit 1
fi

echo "[*] PDFファイル作成中..."
dvipdfmx -o theory.pdf theory.dvi
if [ $? -eq 1 ]; then
    echo "[!] PDFファイル作成に失敗"
    exit 1
fi
