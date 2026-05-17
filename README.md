# Relatório do Projeto

**Disciplina:** Sistemas Operativos
**Curso:** Engenharia Informática — IADE (Universidade Europeia)
**Ano lectivo:** 2025 / 2026
**Docente:** Pedro Rosa / Duarte Cavaleiro

---

## 1. Identificação do Grupo

| Nome | Nº de aluno |
|------|-------------|
| [José Morais] | [20230315] |
| [Evandra Gomes] | [20230416] |


---

## 2. Visão Geral

O projeto consiste na implementação, em Linguagem C sobre Linux, de **9 comandos** ao estilo UNIX. Cumprem-se os 3 comandos obrigatórios (`sort`, `ls`, `UEsh/islash`) e 6 dos 8 opcionais (`tail`, `head`, `cp`/`mv`, `rm`, `kill`, `grep`).

Os comandos podem ser compilados individualmente ou em conjunto através do `Makefile`. Toda a funcionalidade comum aos vários comandos foi extraída para uma **biblioteca partilhada** (`utilitarios_comuns`), conforme exigido no briefing.

---

## 3. Comandos Implementados

| # | Comando | Pontuação | Descrição breve |
|---|---------|-----------|-----------------|
| 1 | `head`    | 2 | Lista as primeiras linhas de um ficheiro |
| 2 | `tail`    | 2 | Lista as últimas linhas de um ficheiro |
| 3 | `rm`      | 1 | Apaga ficheiros (com verificação prévia) |
| 4 | `mv`      | 1 | Move ou renomeia um ficheiro |
| 5 | `kill`    | 1 | Termina um processo activo via SIGTERM |
| 6 | `grep`    | 2 | Procura strings em ficheiros |
| 7 | `sort`    | 2 | Ordena ficheiros de texto (output em `.sort`) |
| 8 | `ls`      | 3 | Lista o conteúdo de um diretório |
| 9 | `islash`  | 3 | Shell interpretador de comandos (UEsh) |
| **Total** | | **17 valores** | |

---

## 4. Estrutura do Projeto

```
Projecto_SO/
├── Makefile
├── README.md
├── RELATORIO.md
├── include/
│   └── utilitarios_comuns.h     # API da biblioteca partilhada
├── src/
│   ├── utilitarios_comuns.c     # Implementação da biblioteca
│   ├── head.c
│   ├── tail.c
│   ├── rm.c
│   ├── mv.c
│   ├── kill.c
│   ├── grep.c
│   ├── sort.c
│   ├── ls.c
│   └── islash.c
├── bin/                          # Executáveis (gerado pelo make)
├── docs/
│   └── manuais/                  # Manuais de cada comando (.txt)
│   └── Relatorio.pdf/            # Relatório do projeto (.pdf)
└── tests/                        # Scripts de teste (.sh)
```

---

## 5. Biblioteca Partilhada (`utilitarios_comuns`)

Para evitar repetição de código entre os 9 comandos, foi criada uma biblioteca estática com as funções comuns:

### Funções expostas

```c
void verificar_ajuda(int argc, char *argv[], const char *msg_de_ajuda);
void tratar_erro(const char *msg_de_erro);
```

- **`verificar_ajuda`**: percorre `argv` à procura da flag `-h`. Se encontrar, imprime a mensagem de ajuda e termina o processo com código 0. É chamada no início do `main()` de cada comando, antes de qualquer outro parsing.
- **`tratar_erro`**: imprime mensagem de erro com `perror()` (que anexa a descrição do `errno`) e termina o processo com código 1. Usada para erros fatais.

Esta abordagem cumpre o requisito do briefing ("utilização de funções genéricas, sempre que possível, é obrigatória"), mantém o código de cada comando focado na sua lógica específica, e facilita futuras manutenções (basta alterar uma vez para afectar todos os comandos).

---

## 6. Decisões Técnicas Notáveis

### 6.1 Parsing de argumentos manual em vez de `getopt`
Optámos por implementar o parsing à mão em cada comando, em vez de usar `getopt()` da stdlib. A razão prática: o `head` e `tail` aceitam a sintaxe `-NUM` (ex.: `head -5`), que o `getopt` não suporta directamente sem hacks. Para consistência, todos os comandos seguem o mesmo padrão (loop por `argv`).

### 6.2 Memória dinâmica em `sort` e `ls`
Os comandos que precisam de juntar e ordenar coleções de itens (linhas no `sort`, entradas no `ls`) usam um **array dinâmico com crescimento por duplicação** (`realloc` quando atinge capacidade). É a estratégia clássica que dá amortized O(1) por insert.

### 6.3 Sinais no `islash`
O shell instala `signal(SIGINT, SIG_IGN)` para que o `Ctrl+C` não o termine. Antes de cada `execvp()` num filho, restaura o default com `signal(SIGINT, SIG_DFL)`. Resultado: o `Ctrl+C` interrompe apenas o comando externo em execução, deixando o shell em pé.

### 6.4 `rename()` + EXDEV fallback no `mv`
O `mv` tenta primeiro `rename()` — operação atómica e instantânea no mesmo filesystem. Quando esta falha com `EXDEV` (filesystems diferentes), faz cópia byte-a-byte seguida de `unlink()` da origem. É o que o `mv` real do GNU coreutils faz internamente.

### 6.5 `lstat()` no `ls`
Para obter metadados das entradas, usa-se `lstat()` em vez de `stat()`. A diferença: `lstat` não segue links simbólicos — se uma entrada for um link para um ficheiro inexistente, ainda assim consegue-se listar o link (em vez de falhar com `ENOENT`).

### 6.6 Builtins no `islash`
Os comandos `cd`, `pwd`, `echo`, `sleep`, `exit` foram implementados como **builtins** (executam dentro do processo do shell, sem `fork`). É obrigatório para o `cd` e `exit`, e por consistência fizemos o mesmo aos outros três. Para o `cd`, em particular, fazer `fork+exec` seria inútil — a mudança de directório aconteceria num processo filho que termina logo a seguir.

---

## 7. Como Compilar

A partir da raiz do projeto:

```bash
make              # compila os 9 comandos
make <nome>       # compila apenas um (ex: make head)
make clean        # remove os binários
make rebuild      # limpa e recompila
make help         # mostra todos os targets
```

Os executáveis são gerados em `bin/`. A compilação usa `-Wall -Wextra -Wpedantic -std=c99` para garantir zero warnings.

Para empacotar para entrega:

```bash
# Editar o Makefile e mudar GROUP := G00 para o número do grupo
make dist         # gera Gxx-TrabSO.zip pronto para Canvas
```

---

## 8. Como Testar

Os scripts de teste estão em `tests/`. Para correr a bateria completa:

```bash
chmod +x tests/*.sh
./tests/run_all.sh
```

Para testar um comando individualmente:

```bash
./tests/test_head.sh
```

Para a apresentação, o `tests/demo.sh` faz uma demonstração breve e visual dos comandos principais.

O `islash` é interactivo — o `tests/test_islash.sh` apresenta um guião dos comandos a executar dentro do shell.

---

## 9. Notas Adicionais

- Todos os comandos suportam a flag `-h` para mostrar ajuda, conforme exigido.
- Os erros são sempre escritos para `stderr` e devolvem código de saída ≠ 0.
- O código foi escrito sem warnings com `-Wall -Wextra -Wpedantic`.
- Algumas funções POSIX (ex.: `getline`, `strncasecmp`) requerem o macro `_POSIX_C_SOURCE 200809L` definido antes dos includes.

---

## 10. Conclusão

O projeto permitiu consolidar conhecimentos sobre o sistema operativo Linux, syscalls fundamentais (`fork`, `exec`, `wait`, `open`, `read`, `write`, `stat`, `unlink`, `rename`, `kill`), gestão dinâmica de memória, e organização de código C em projetos com múltiplos módulos. A implementação do `islash` foi particularmente formativa, ao integrar gestão de processos, sinais, e parsing de input.