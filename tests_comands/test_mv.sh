#!/bin/bash
# Testes automatizados para o comando 'mv' (sem interactivos)

SANDBOX="tests/_sandbox_mv"
BIN="./bin/mv"

echo "============================================="
echo "  TESTES: mv"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX/subdir"

echo ""
echo "[1] Renomear no mesmo dir"
echo "ola" > "$SANDBOX/a.txt"
$BIN "$SANDBOX/a.txt" "$SANDBOX/b.txt"
[ -f "$SANDBOX/b.txt" ] && [ ! -f "$SANDBOX/a.txt" ] && echo "  OK"
echo "  conteudo: $(cat $SANDBOX/b.txt)"

echo ""
echo "[2] Mover para subdiretorio"
$BIN "$SANDBOX/b.txt" "$SANDBOX/subdir/c.txt"
[ -f "$SANDBOX/subdir/c.txt" ] && echo "  OK - movido para subdir"

echo ""
echo "[3] Sobrescrever (sem -i, silencioso)"
echo "novo" > "$SANDBOX/x.txt"
echo "antigo" > "$SANDBOX/y.txt"
$BIN "$SANDBOX/x.txt" "$SANDBOX/y.txt"
echo "  y.txt conteudo: $(cat $SANDBOX/y.txt)  (deveria ser 'novo')"

echo ""
echo "[4] Mover ficheiro para si proprio (no-op valido)"
echo "z" > "$SANDBOX/z.txt"
$BIN "$SANDBOX/z.txt" "$SANDBOX/z.txt"; echo "  exit code: $?"

echo ""
echo "[5] Origem inexistente"
$BIN "$SANDBOX/naoexiste.txt" "$SANDBOX/destino.txt"; echo "  exit code: $?"

echo ""
echo "[6] Argumentos insuficientes"
$BIN "$SANDBOX/apenas_um.txt"; echo "  exit code: $?"

echo ""
echo "[7] Sem argumentos"
$BIN; echo "  exit code: $?"

echo ""
echo "[8] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'mv' concluidos."
echo "  Nota: testes interactivos (-i) devem ser feitos manualmente."
echo "============================================="
