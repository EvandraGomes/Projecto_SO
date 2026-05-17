#!/bin/bash
# Testes automatizados para o comando 'grep'

SANDBOX="tests/_sandbox_grep"
BIN="./bin/grep"

echo "============================================="
echo "  TESTES: grep"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX"

cat > "$SANDBOX/texto.txt" << 'INNER_EOF'
Hello World
this is a test line
Another line with TEST
hello world
HELLO WORLD
The last line
INNER_EOF

cat > "$SANDBOX/outro.txt" << 'INNER_EOF'
Just some content
Hello again
no match here
INNER_EOF

echo ""
echo "[1] Procura case-sensitive 'Hello'"
$BIN "Hello" "$SANDBOX/texto.txt"

echo ""
echo "[2] Case-insensitive (-i)"
$BIN -i "hello" "$SANDBOX/texto.txt"

echo ""
echo "[3] Numeracao (-n)"
$BIN -n "test" "$SANDBOX/texto.txt"

echo ""
echo "[4] -n -i combinados"
$BIN -n -i "test" "$SANDBOX/texto.txt"

echo ""
echo "[5] Contagem (-c)"
$BIN -c "Hello" "$SANDBOX/texto.txt"

echo ""
echo "[6] -c -i (contagem case-insensitive)"
$BIN -c -i "hello" "$SANDBOX/texto.txt"

echo ""
echo "[7] Inverter (-v)"
$BIN -v "Hello" "$SANDBOX/texto.txt"

echo ""
echo "[8] -v -i (nao contem hello em qualquer caixa)"
$BIN -v -i "hello" "$SANDBOX/texto.txt"

echo ""
echo "[9] -c -v -i (stress test - contagem das que NAO contem)"
$BIN -c -v -i "hello" "$SANDBOX/texto.txt"

echo ""
echo "[10] Multiplos ficheiros (prefixo ficheiro:)"
$BIN -i "hello" "$SANDBOX/texto.txt" "$SANDBOX/outro.txt"

echo ""
echo "[11] -c em multiplos ficheiros"
$BIN -c -i "hello" "$SANDBOX/texto.txt" "$SANDBOX/outro.txt"

echo ""
echo "[12] Sem matches"
$BIN "zzz" "$SANDBOX/texto.txt"; echo "  exit code: $?"

echo ""
echo "[13] Ficheiro inexistente"
$BIN "test" naoexiste.txt; echo "  exit code: $?"

echo ""
echo "[14] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'grep' concluidos."
echo "============================================="
