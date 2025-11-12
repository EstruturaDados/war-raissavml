#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

/* ------------------ Constantes Globais ------------------- */
#define MAX_STR 64
#define MISSOES_QTD 2

/* ------------------ Estrutura de Dados -------------------- */
typedef struct {
    char nome[MAX_STR];
    char dono[MAX_STR];  // cor do exército
    int tropas;
} Territorio;

/* ------------------ Protótipos ---------------------------- */
Territorio *alocarMapa(size_t n);
void inicializarTerritorios(Territorio *mapa, size_t n);
void liberarMemoria(Territorio *mapa);

void exibirMapa(const Territorio *mapa, size_t n);
void exibirMenuPrincipal(void);
void exibirMissao(int missaoID, const char *missaoTarget, int missaoNum);
void limparBufferEntrada(void);
void pausar(void);

void faseDeAtaque(Territorio *mapa, size_t n, const char *corJogador);
void atacar(Territorio *atacante, Territorio *defensor);

int sortearJogador(const Territorio *mapa, size_t n, char *corJogadorOut);
int sortearMissao(const Territorio *mapa, size_t n, const char *corJogador, char *missaoTargetOut, int *missaoNumOut);
int verificarVitoria(const Territorio *mapa, size_t n, int missaoID, const char *corJogador, const char *missaoTarget, int missaoNum);

int obterIndiceValido(const char *prompt, int min, int max);
int rolarDado(void);

/* ------------------ Implementação -------------------------- */

Territorio *alocarMapa(size_t n) {
    return calloc(n, sizeof(Territorio));
}

void inicializarTerritorios(Territorio *mapa, size_t n) {
    printf("\n=== CONFIGURAÇÃO INICIAL DO MAPA ===\n");
    for (size_t i = 0; i < n; ++i) {
        printf("\n--- Território %zu ---\n", i);
        printf("Nome: ");
        fgets(mapa[i].nome, MAX_STR, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor (exército): ");
        fgets(mapa[i].dono, MAX_STR, stdin);
        mapa[i].dono[strcspn(mapa[i].dono, "\n")] = '\0';

        printf("Número de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas <= 0) {
            printf("Valor inválido. Digite novamente: ");
            limparBufferEntrada();
        }
        limparBufferEntrada();
    }
}

void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

void exibirMapa(const Territorio *mapa, size_t n) {
    printf("\n=== MAPA ATUAL ===\n");
    printf("%-3s %-20s %-15s %-6s\n", "ID", "NOME", "COR", "TROPAS");
    printf("---------------------------------------------\n");
    for (size_t i = 0; i < n; ++i)
        printf("%-3zu %-20s %-15s %-6d\n", i, mapa[i].nome, mapa[i].dono, mapa[i].tropas);
}

void exibirMenuPrincipal(void) {
    puts("\n=== MENU ===");
    puts("1 - Fase de ataque");
    puts("2 - Verificar missão");
    puts("0 - Sair");
    printf("Escolha: ");
}

void exibirMissao(int missaoID, const char *missaoTarget, int missaoNum) {
    puts("\n--- MISSÃO SECRETA ---");
    if (missaoID == 1)
        printf("Eliminar todo o exército da cor: %s\n", missaoTarget);
    else if (missaoID == 2)
        printf("Conquistar pelo menos %d territórios.\n", missaoNum);
    else
        printf("Missão desconhecida.\n");
}

void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    limparBufferEntrada();
}

int obterIndiceValido(const char *prompt, int min, int max) {
    int x;
    while (1) {
        printf("%s (%d-%d): ", prompt, min, max);
        if (scanf("%d", &x) != 1) {
            printf("Entrada inválida.\n");
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();
        if (x < min || x > max) {
            printf("Fora do intervalo.\n");
            continue;
        }
        return x;
    }
}

int rolarDado(void) {
    return (rand() % 6) + 1;
}

/* ------------------ Fase de Ataque ------------------------- */

void faseDeAtaque(Territorio *mapa, size_t n, const char *corJogador) {
    exibirMapa(mapa, n);
    printf("\n--- FASE DE ATAQUE ---\n");

    int origem = obterIndiceValido("ID do território de origem", 0, (int)n - 1);
    if (strcmp(mapa[origem].dono, corJogador) != 0) {
        printf("O território escolhido não pertence a você (%s).\n", corJogador);
        return;
    }

    if (mapa[origem].tropas < 2) {
        printf("Precisa de ao menos 2 tropas para atacar.\n");
        return;
    }

    int destino = obterIndiceValido("ID do território alvo", 0, (int)n - 1);
    if (destino == origem) {
        printf("Origem e destino não podem ser o mesmo.\n");
        return;
    }

    if (strcasecmp(mapa[destino].dono, corJogador) == 0) {
        printf("Você não pode atacar um território seu!\n");
        return;
    }

    atacar(&mapa[origem], &mapa[destino]);
    exibirMapa(mapa, n);
}

/* ------------------ Simulação de Batalha ------------------- */

void atacar(Territorio *atacante, Territorio *defensor) {
    printf("\n--- ATAQUE: %s (%s) -> %s (%s) ---\n", 
           atacante->nome, atacante->dono, defensor->nome, defensor->dono);

    int dadoAtq = rolarDado();
    int dadoDef = rolarDado();
    printf("Atacante rola: %d | Defensor rola: %d\n", dadoAtq, dadoDef);

    if (dadoAtq > dadoDef) {
        printf("Vitória do atacante!\n");
        int tropasParaMover = atacante->tropas / 2; // metade se move
        atacante->tropas -= tropasParaMover;        // metade fica
        strcpy(defensor->dono, atacante->dono);
        defensor->tropas = tropasParaMover;         // metade ocupa o novo território
    } else {
        printf("Ataque falhou! O atacante perde 1 tropa.\n");
        if (atacante->tropas > 1) atacante->tropas--; // nunca pode ir a 0
    }
}

/* ------------------ Sorteio de Jogador e Missão ------------ */

int sortearJogador(const Territorio *mapa, size_t n, char *corJogadorOut) {
    char cores[n][MAX_STR];
    int qtdCores = 0;
    for (size_t i = 0; i < n; ++i) {
        int existe = 0;
        for (int j = 0; j < qtdCores; ++j)
            if (strcasecmp(cores[j], mapa[i].dono) == 0)
                existe = 1;
        if (!existe)
            strcpy(cores[qtdCores++], mapa[i].dono);
    }
    int indice = rand() % qtdCores;
    strcpy(corJogadorOut, cores[indice]);
    return indice;
}

int sortearMissao(const Territorio *mapa, size_t n, const char *corJogador, char *missaoTargetOut, int *missaoNumOut) {
    int id = (rand() % MISSOES_QTD) + 1;
    char cores[n][MAX_STR];
    int qtdCores = 0;
    for (size_t i = 0; i < n; ++i) {
        if (strcasecmp(mapa[i].dono, corJogador) != 0) {
            int existe = 0;
            for (int j = 0; j < qtdCores; ++j)
                if (strcasecmp(cores[j], mapa[i].dono) == 0)
                    existe = 1;
            if (!existe)
                strcpy(cores[qtdCores++], mapa[i].dono);
        }
    }
    if (id == 1 && qtdCores > 0)
        strcpy(missaoTargetOut, cores[rand() % qtdCores]);
    else {
        *missaoNumOut = (rand() % 3) + 2;
        missaoTargetOut[0] = '\0';
    }
    return id;
}

/* ------------------ Verificação de Vitória ----------------- */

int verificarVitoria(const Territorio *mapa, size_t n, int missaoID, const char *corJogador, const char *missaoTarget, int missaoNum) {
    if (missaoID == 1) {
        for (size_t i = 0; i < n; ++i)
            if (strcasecmp(mapa[i].dono, missaoTarget) == 0)
                return 0;
        return 1;
    } else if (missaoID == 2) {
        int contador = 0;
        for (size_t i = 0; i < n; ++i)
            if (strcasecmp(mapa[i].dono, corJogador) == 0)
                contador++;
        return (contador >= missaoNum);
    }
    return 0;
}

/* ------------------ Função Principal ----------------------- */

int main(void) {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    int n;
    printf("Informe o número de territórios (mínimo 5): ");
    while (scanf("%d", &n) != 1 || n < 5) {
        printf("Valor inválido. Digite novamente (>=5): ");
        limparBufferEntrada();
    }
    limparBufferEntrada();

    Territorio *mapa = alocarMapa(n);
    if (!mapa) {
        fprintf(stderr, "Erro: falha ao alocar memória.\n");
        return EXIT_FAILURE;
    }

    inicializarTerritorios(mapa, n);

    char corJogador[MAX_STR];
    sortearJogador(mapa, n, corJogador);

    char missaoTarget[MAX_STR];
    int missaoNum = 0;
    int missaoID = sortearMissao(mapa, n, corJogador, missaoTarget, &missaoNum);

    int opcao, venceu = 0;
    do {
        system("clear");
        printf("=== WAR MINI ===\nJogador: %s\n", corJogador);
        exibirMapa(mapa, n);
        exibirMissao(missaoID, missaoTarget, missaoNum);
        exibirMenuPrincipal();

        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, n, corJogador);
                break;
            case 2:
                if (verificarVitoria(mapa, n, missaoID, corJogador, missaoTarget, missaoNum)) {
                    printf("\nParabéns! Você cumpriu sua missão!\n");
                    venceu = 1;
                } else {
                    printf("\nMissão ainda não cumprida.\n");
                }
                break;
            case 0:
                printf("Encerrando o jogo.\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
        pausar();
    } while (opcao != 0 && !venceu);

    liberarMemoria(mapa);
    return 0;
}