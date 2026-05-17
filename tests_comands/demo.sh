#!/bin/bash
# Demonstracao breve para a apresentacao.
# Foca-se em mostrar UMA caracteristica representativa de cada comando.

DEMO="tests/_demo_sandbox"

clear
echo "##############################################################"
echo "#                                                            #"
echo "#                DEMO DO PROJETO - SO 2025/2026              #"
echo "#                                                            #"
echo "##############################################################"
echo ""

pausa() {
    echo ""
    read -p "[Enter para continuar]" _
    echo ""
}

# Setup
rm -rf "$DEMO"
mkdir -p "$DEMO/subdir"
seq 1 25 > "$DEMO/numeros.txt"
echo "uma linha vermelha" > "$DEMO/log.txt"
echo "uma linha verde" >> "$DEMO/log.txt"
echo "uma linha azul" >> "$DEMO/log.txt"
echo "UMA LINHA VERMELHA EM MAIUSCULAS" >> "$DEMO/log.txt"
cat > "$DEMO/palavras.txt" << 'INNER'
zebra
abacate
maca
banana
INNER

# 1. head
echo "================== head =================="
echo "Comando: head -5 -n $DEMO/numeros.txt"
echo "Funcionalidade: primeiras 5 linhas numeradas"
echo ""
./bin/head -5 -n "$DEMO/numeros.txt"
pausa

# 2. tail
echo "================== tail =================="
echo "Comando: tail -5 -n $DEMO/numeros.txt"
echo "Repara: numeros sao POSICAO REAL (21-25), nao 1-5"
echo ""
./bin/tail -5 -n "$DEMO/numeros.txt"
pausa

# 3. grep
echo "================== grep =================="
echo "Comando: grep -n -i 'vermelha' $DEMO/log.txt"
echo "Procura case-insensitive com numero de linha"
echo ""
./bin/grep -n -i "vermelha" "$DEMO/log.txt"
pausa

# 4. sort
echo "================== sort =================="
echo "Comando: sort $DEMO/palavras.txt"
echo "ATENCAO: este comando NAO imprime nada (cria .sort)"
echo ""
./bin/sort "$DEMO/palavras.txt"
echo "Conteudo gerado em palavras.txt.sort:"
cat "$DEMO/palavras.txt.sort"
pausa

# 5. ls
echo "================== ls =================="
echo "Comando: ls -l -os $DEMO  (longo, ordenado por tamanho)"
echo ""
./bin/ls -l -os "$DEMO"
pausa

# 6. rm
echo "================== rm =================="
echo "Comando: rm $DEMO/palavras.txt.sort"
echo "Conteudo antes:"
ls "$DEMO" | sed 's/^/  /'
./bin/rm "$DEMO/palavras.txt.sort"
echo "Conteudo depois:"
ls "$DEMO" | sed 's/^/  /'
pausa

# 7. mv
echo "================== mv =================="
echo "Comando: mv $DEMO/log.txt $DEMO/subdir/relatorio.txt"
./bin/mv "$DEMO/log.txt" "$DEMO/subdir/relatorio.txt"
echo "Verificacao:"
ls "$DEMO/subdir" | sed 's/^/  /'
pausa

# 8. kill
echo "================== kill =================="
echo "Comando: spawn sleep 60 -> kill PID"
sleep 60 &
PID=$!
echo "  Sleep lancado, PID=$PID"
sleep 0.5
./bin/kill $PID
sleep 1
if ps -p $PID > /dev/null; then
    echo "  FALHA - processo continua"
else
    echo "  OK - processo terminado"
fi
pausa

# 9. islash
echo "================== islash =================="
echo "O shell e' interactivo. Para demonstrar, lanco-o de seguida."
echo ""
echo "Dentro do islash, faz:"
echo "  pwd"
echo "  echo Demo a funcionar!"
echo "  ./bin/head -3 $DEMO/numeros.txt"
echo "  exit"
echo ""
read -p "[Enter para lancar o islash]" _
./bin/islash

# Cleanup
rm -rf "$DEMO"

echo ""
echo "##############################################################"
echo "#                                                            #"
echo "#              FIM DA DEMO. Obrigado!                        #"
echo "#                                                            #"
echo "##############################################################"
