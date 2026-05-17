#!/bin/bash
# Testes automatizados para o comando 'rm' (sem interactivos)

SANDBOX="tests/_sandbox_rm"
BIN="./bin/rm"

echo "============================================="
echo "  TESTES: rm"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX"

echo ""
echo "[1] Apagar 1 ficheiro"
echo "x" > "$SANDBOX/a.txt"
$BIN "$SANDBOX/a.txt"
[ ! -f "$SANDBOX/a.txt" ] && echo "  OK - a.txt apagado" || echo "  FALHA"

echo ""
echo "[2] Apagar varios"
echo "x" > "$SANDBOX/b.txt"; echo "x" > "$SANDBOX/c.txt"; echo "x" > "$SANDBOX/d.txt"
$BIN "$SANDBOX/b.txt" "$SANDBOX/c.txt" "$SANDBOX/d.txt"
ls "$SANDBOX" 2>/dev/null

echo ""
echo "[3] Ficheiro inexistente (exit != 0)"
$BIN "$SANDBOX/naoexiste.txt"; echo "  exit code: $?"

echo ""
echo "[4] Mistura: existente + inexistente (exit 1 mas apaga o valido)"
echo "x" > "$SANDBOX/e.txt"
$BIN "$SANDBOX/e.txt" "$SANDBOX/naoexiste.txt"; echo "  exit code: $?"
[ ! -f "$SANDBOX/e.txt" ] && echo "  OK - e.txt foi apagado mesmo com erro"

echo ""
echo "[5] Sem argumentos"
$BIN; echo "  exit code: $?"

echo ""
echo "[6] Flag invalida"
$BIN -xyz; echo "  exit code: $?"

echo ""
echo "[7] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'rm' concluidos."
echo "  Nota: testes interactivos (-y) devem ser feitos manualmente."
echo "============================================="
