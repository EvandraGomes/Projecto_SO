#!/bin/bash
# Testes automatizados para o comando 'kill'

BIN="./bin/kill"

echo "============================================="
echo "  TESTES: kill"
echo "============================================="

echo ""
echo "[1] Matar um processo real (sleep em background)"
sleep 100 &
PID=$!
echo "  PID lancado: $PID"
$BIN $PID; echo "  kill exit code: $?"
sleep 1
if ps -p $PID > /dev/null; then
    echo "  FALHA - processo ainda existe"
else
    echo "  OK - processo terminado"
fi

echo ""
echo "[2] PID inexistente"
$BIN 999999 2>&1; echo "  exit code: $?"

echo ""
echo "[3] PID 1 (sem permissoes - systemd/init)"
$BIN 1 2>&1; echo "  exit code: $?"

echo ""
echo "[4] PID nao numerico"
$BIN abc 2>&1; echo "  exit code: $?"

echo ""
echo "[5] PID misturado"
$BIN 123abc 2>&1; echo "  exit code: $?"

echo ""
echo "[6] PID zero"
$BIN 0 2>&1; echo "  exit code: $?"

echo ""
echo "[7] Sem argumentos"
$BIN 2>&1; echo "  exit code: $?"

echo ""
echo "[8] Argumentos a mais"
$BIN 1234 5678 2>&1; echo "  exit code: $?"

echo ""
echo "[9] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

echo ""
echo "============================================="
echo "  Testes 'kill' concluidos."
echo "============================================="
