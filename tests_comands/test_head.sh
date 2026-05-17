#!/bin/bash
# Testes automatizados para o comando 'head'
# Corre a partir da raiz do projeto: ./tests/test_head.sh

SANDBOX="tests/_sandbox_head"
BIN="./bin/head"

echo "============================================="
echo "  TESTES: head"
echo "============================================="

# Setup
rm -rf "$SANDBOX"
mkdir -p "$SANDBOX"
seq 1 20 > "$SANDBOX/exemplo.txt"
seq 100 105 > "$SANDBOX/outro.txt"

echo ""
echo "[1] Default (10 primeiras linhas)"
$BIN "$SANDBOX/exemplo.txt"

echo ""
echo "[2] -5 (primeiras 5 linhas)"
$BIN -5 "$SANDBOX/exemplo.txt"

echo ""
echo "[3] -n (numeradas)"
$BIN -n "$SANDBOX/exemplo.txt"

echo ""
echo "[4] -E (marca fim de linha)"
$BIN -E "$SANDBOX/exemplo.txt"

echo ""
echo "[5] -3 -n -E (combinacao)"
$BIN -3 -n -E "$SANDBOX/exemplo.txt"

echo ""
echo "[6] Multiplos ficheiros"
$BIN -3 "$SANDBOX/exemplo.txt" "$SANDBOX/outro.txt"

echo ""
echo "[7] Ajuda (-h)"
$BIN -h

echo ""
echo "[8] Ficheiro inexistente (esperado: erro + exit 1)"
$BIN "$SANDBOX/naoexiste.txt"; echo "  exit code: $?"

echo ""
echo "[9] Sem argumentos (esperado: erro + exit 1)"
$BIN; echo "  exit code: $?"

# Cleanup
rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'head' concluidos."
echo "============================================="
