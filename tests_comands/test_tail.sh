#!/bin/bash
# Testes automatizados para o comando 'tail'

SANDBOX="tests/_sandbox_tail"
BIN="./bin/tail"

echo "============================================="
echo "  TESTES: tail"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX"
seq 1 20 > "$SANDBOX/exemplo.txt"
seq 1 3 > "$SANDBOX/poucas.txt"
seq 100 110 > "$SANDBOX/outro.txt"
touch "$SANDBOX/vazio.txt"

echo ""
echo "[1] Default (10 ultimas linhas)"
$BIN "$SANDBOX/exemplo.txt"

echo ""
echo "[2] -5 (ultimas 5 linhas)"
$BIN -5 "$SANDBOX/exemplo.txt"

echo ""
echo "[3] -5 -n (CRITICO: numeros = posicao real 16-20)"
$BIN -5 -n "$SANDBOX/exemplo.txt"

echo ""
echo "[4] -3 -E"
$BIN -3 -E "$SANDBOX/exemplo.txt"

echo ""
echo "[5] -3 -n -E (combinacao)"
$BIN -3 -n -E "$SANDBOX/exemplo.txt"

echo ""
echo "[6] Ficheiro com 3 linhas, pedido -10 (mostra todas)"
$BIN -10 "$SANDBOX/poucas.txt"

echo ""
echo "[7] Ficheiro vazio"
$BIN "$SANDBOX/vazio.txt"; echo "  exit code: $?"

echo ""
echo "[8] Multiplos ficheiros"
$BIN -3 "$SANDBOX/exemplo.txt" "$SANDBOX/outro.txt"

echo ""
echo "[9] Ajuda (-h)"
$BIN -h

echo ""
echo "[10] Ficheiro inexistente"
$BIN naoexiste.txt; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'tail' concluidos."
echo "============================================="
