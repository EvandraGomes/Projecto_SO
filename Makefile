# ============================================================
#  Makefile - Comandos UNIX em C
#  Sistemas Operativos - IADE 2025/2026
# ============================================================

# --- Compilador e flags ---
CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c99 -Iinclude
LDFLAGS  :=

# --- Estrutura de diretorios ---
SRCDIR   := src
INCDIR   := include
BINDIR   := bin

# --- Biblioteca partilhada ---
LIB_SRC  := $(SRCDIR)/utilitarios_comuns.c
LIB_HDR  := $(INCDIR)/utilitarios_comuns.h

# --- Lista de comandos ---
COMANDOS := head tail rm mv kill grep sort ls islash
ALVOS    := $(addprefix $(BINDIR)/, $(COMANDOS))

# --- Empacotamento para entrega ---
GROUP    := G00
ZIP_NAME := $(GROUP)-TrabSO.zip


# ============================================================
#  Alvos principais
# ============================================================

# Default: compilar tudo
all: $(ALVOS)

# Aliases: 'make head' -> compila 'bin/head'
$(COMANDOS): %: $(BINDIR)/%

# Recompilar tudo do zero
rebuild: clean all

# Apagar binarios e ZIP
clean:
	rm -rf $(BINDIR) $(ZIP_NAME)

# Modo debug: simbolos para gdb e sem otimizacao
debug: CFLAGS += -g -O0 -DDEBUG
debug: rebuild

# Criar ZIP de entrega (basta mudar GROUP no topo)
dist: clean
	@echo "  PACK  $(ZIP_NAME)"
	@zip -r $(ZIP_NAME) Makefile README.md $(INCDIR)/ $(SRCDIR)/ docs/ \
		-x "*.DS_Store" -x ".git/*" -x "bin/*" -x "*.o"


# ============================================================
#  Regra de padrao: cada comando depende do seu .c e da lib
# ============================================================

$(BINDIR)/%: $(SRCDIR)/%.c $(LIB_SRC) $(LIB_HDR) | $(BINDIR)
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) $< $(LIB_SRC) -o $@ $(LDFLAGS)

# Criar bin/ se nao existir
$(BINDIR):
	@mkdir -p $@


# ============================================================
#  Ajuda
# ============================================================

help:
	@echo "Targets disponiveis:"
	@echo "  make           - compila todos os comandos (default)"
	@echo "  make <nome>    - compila so um comando (ex: make head)"
	@echo "  make clean     - apaga binarios e ZIP de entrega"
	@echo "  make rebuild   - limpa e recompila tudo"
	@echo "  make debug     - compila com simbolos de debug (-g -O0)"
	@echo "  make dist      - cria $(ZIP_NAME) para entrega"
	@echo "  make help      - mostra esta mensagem"
	@echo ""
	@echo "Comandos disponiveis:"
	@echo "  $(COMANDOS)"


.PHONY: all clean rebuild debug dist help $(COMANDOS)