#!/bin/bash
# Testes automatizados para o comando 'ls'

SANDBOX="tests/_sandbox_ls"
BIN="./bin/ls"

echo "============================================="
echo "  TESTES: ls"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX/subdir"

# Ficheiros de tamanhos diferentes
echo "x" > "$SANDBOX/pequeno.txt"
seq 1 100 > "$SANDBOX/medio.txt"
seq 1 10000 > "$SANDBOX/grande.txt"
touch "$SANDBOX/vazio.txt"
touch -t 202403150900 "$SANDBOX/antigo.txt"
sleep 1
touch "$SANDBOX/recente.txt"

echo ""
echo "[1] Default - lista por nome"
$BIN "$SANDBOX"

echo ""
echo "[2] -l (listagem longa)"
$BIN -l "$SANDBOX"

echo ""
echo "[3] -os (por tamanho crescente)"
$BIN -os "$SANDBOX"

echo ""
echo "[4] -od (por data)"
$BIN -od "$SANDBOX"

echo ""
echo "[5] -l -os (longo + tamanho)"
$BIN -l -os "$SANDBOX"

echo ""
echo "[6] -c (colunas)"
# Criar mais ficheiros para o efeito ser visivel
for i in $(seq 1 20); do touch "$SANDBOX/f${i}.txt"; done
$BIN -c "$SANDBOX"

echo ""
echo "[7] Diretorio vazio"
mkdir -p "$SANDBOX/vazia"
$BIN "$SANDBOX/vazia"; echo "  exit code: $?"

echo ""
echo "[8] Caminho inexistente"
$BIN /naoexiste; echo "  exit code: $?"

echo ""
echo "[9] Caminho sem permissoes"
$BIN /root 2>&1 | head -1; echo "  exit code: $?"

echo ""
echo "[10] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'ls' concluidos."
echo "============================================="
