# Makefile — Projeto SO N2: Exclusão Mútua
# Uso: make all       → compila tudo
#      make run       → compila e salva saídas em output/ (5x cada)
#      make clean     → remove os binários e a pasta output/

CC      = gcc
CFLAGS  = -Wall -Wextra -lpthread
BIN     = ./bin
OUTPUT  = ./output
TARGETS = problema_concorrencia \
          solucao1_inibicao     \
          solucao2_busy         \
          solucao3_turn         \
          solucao4_peterson     \
          solucao5_atomica

all: $(TARGETS)

%: %.c
	@mkdir -p $(BIN)
	$(CC) $< -o $(BIN)/$@ $(CFLAGS)

run: all
	@mkdir -p $(OUTPUT)
	@for prog in $(TARGETS); do \
		out=$(OUTPUT)/$$prog.txt; \
		echo "========================================" | tee $$out; \
		echo " $$prog"                                  | tee -a $$out; \
		echo " Data/hora: $$(date)"                     | tee -a $$out; \
		echo "========================================"  | tee -a $$out; \
		for i in 1 2 3 4 5; do \
			echo "--- Execucao $$i ---"                 | tee -a $$out; \
			$(BIN)/$$prog                               | tee -a $$out; \
			echo ""                                     | tee -a $$out; \
		done; \
	done
	@echo "Saidas salvas em $(OUTPUT)/"
	@ls -lh $(OUTPUT)/

clean:
	rm -rf $(BIN)
	rm -rf $(OUTPUT)