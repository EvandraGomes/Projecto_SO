#!/bin/bash
# Testes automatizados para o comando 'sort'

SANDBOX="tests/_sandbox_sort"
BIN="./bin/sort"

echo "============================================="
echo "  TESTES: sort"
echo "============================================="

rm -rf "$SANDBOX"
mkdir -p "$SANDBOX"

cat > "$SANDBOX/frutas.txt" << 'INNER_EOF'
banana
cherry
apple
date
elderberry
INNER_EOF

cat > "$SANDBOX/numeros.txt" << 'INNER_EOF'
10
2
1
20
3
INNER_EOF

touch "$SANDBOX/vazio.txt"

echo ""
echo "[1] Crescente (default - nao deve ter output no terminal)"
$BIN "$SANDBOX/frutas.txt"
echo "  Conteudo de frutas.txt.sort:"
cat "$SANDBOX/frutas.txt.sort" | sed 's/^/    /'

echo ""
echo "[2] Decrescente (-d)"
$BIN -d "$SANDBOX/frutas.txt"
echo "  Conteudo de frutas.txt.sort:"
cat "$SANDBOX/frutas.txt.sort" | sed 's/^/    /'

echo ""
echo "[3] Ordenacao lexicografica (numeros como strings)"
$BIN "$SANDBOX/numeros.txt"
echo "  Conteudo (esperado: 1, 10, 2, 20, 3 em ordem lex):"
cat "$SANDBOX/numeros.txt.sort" | sed 's/^/    /'

echo ""
echo "[4] CRITICO: verificar que NAO escreve para stdout"
SAIDA=$($BIN "$SANDBOX/frutas.txt" 2>&1)
echo "  Bytes em stdout: ${#SAIDA}  (deveria ser 0)"

echo ""
echo "[5] Multiplos ficheiros (cria multiplos .sort)"
$BIN "$SANDBOX/frutas.txt" "$SANDBOX/numeros.txt"
ls "$SANDBOX"/*.sort 2>/dev/null

echo ""
echo "[6] Ficheiro vazio"
$BIN "$SANDBOX/vazio.txt"
[ -f "$SANDBOX/vazio.txt.sort" ] && echo "  OK - .sort vazio criado"

echo ""
echo "[7] Ficheiro inexistente"
$BIN "$SANDBOX/naoexiste.txt"; echo "  exit code: $?"

echo ""
echo "[8] Sem argumentos"
$BIN; echo "  exit code: $?"

echo ""
echo "[9] Ajuda (-h)"
$BIN -h; echo "  exit code: $?"

rm -rf "$SANDBOX"
echo ""
echo "============================================="
echo "  Testes 'sort' concluidos."
echo "============================================="
