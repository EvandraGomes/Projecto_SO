#!/bin/bash
# Corre TODOS os testes automatizados em sequencia.
# A partir da raiz do projeto: ./tests/run_all.sh

echo ""
echo "##############################################################"
echo "#                                                            #"
echo "#         BATERIA COMPLETA DE TESTES DO PROJETO              #"
echo "#                                                            #"
echo "##############################################################"
echo ""

# Garantir que tudo esta compilado
echo "[setup] make all"
make all 2>&1 | sed 's/^/  /'
echo ""

# Sequencia: do mais simples para o mais complexo
for script in test_kill.sh test_rm.sh test_mv.sh \
              test_head.sh test_tail.sh \
              test_grep.sh test_sort.sh test_ls.sh; do
    if [ -x "tests/$script" ]; then
        bash "tests/$script"
        echo ""
        echo ""
    else
        echo "AVISO: tests/$script nao encontrado ou nao executavel."
    fi
done

echo ""
echo "##############################################################"
echo "#                                                            #"
echo "#  Testes automaticos terminados.                            #"
echo "#                                                            #"
echo "#  O islash e' interactivo - testa com:                      #"
echo "#      ./tests/test_islash.sh                                #"
echo "#                                                            #"
echo "##############################################################"
