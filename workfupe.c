#include <stdio.h>
#include <string.h>

#define MAX_DORAMAS 100

typedef struct {
    int id;
    int ano;
    float nota;
    char titulo[50];
} Dorama;

Dorama doramas[MAX_DORAMAS];
int total_doramas = 0;
int prox_id = 1;

void salvarEmArquivo() {
    FILE *arquivo = fopen("doramas.txt", "w");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo para salvar");
        return;
    }
    for (int i = 0; i < total_doramas; i++) {
        fprintf(arquivo, "%d\n%s\n%d\n%.1f\n", doramas[i].id, doramas[i].titulo, doramas[i].ano, doramas[i].nota);
    }
    fflush(arquivo);  
    fclose(arquivo);
    printf("Dados salvos no arquivo!\n");
}

void carregarDeArquivo() {
    FILE *arquivo = fopen("doramas.txt", "r");
    if (!arquivo) {
        printf("Nenhum arquivo encontrado, iniciando sem doramas cadastrados.\n");
     return;
    }

    total_doramas = 0;
    prox_id = 1;
    int aux = -1;

    while ((aux = fscanf(arquivo, "%d %49[^\n] %d %f", &doramas[total_doramas].id, doramas[total_doramas].titulo, &doramas[total_doramas].ano, &doramas[total_doramas].nota)) == 4) {
       // printf("%d, %s, %d, %.1f\n", doramas[total_doramas].id, doramas[total_doramas].titulo, doramas[total_doramas].ano, doramas[total_doramas].nota);
        total_doramas++;

        if (doramas[total_doramas - 1].id >= prox_id) {
         prox_id = doramas[total_doramas - 1].id + 1;
        }
        //printf("Dorama: %s\n", doramas[total_doramas-1].titulo);
    }
    //printf("FSCANF: %d\n", aux);
    fclose(arquivo);
    printf("Dados carregados do arquivo.\n");
}
void bubbleSortDoramas() {
    for (int i = 0; i < total_doramas - 1; i++) {
        for (int j = 0; j < total_doramas - i - 1; j++) {
         if (doramas[j].nota < doramas[j + 1].nota) {  
          Dorama temp = doramas[j];
         doramas[j] = doramas[j + 1];
          doramas[j + 1] = temp;
            }
        }
    }
}

void listarDoramas() {
    if (total_doramas == 0) {
    printf("Nenhum dorama cadastrado.\n");
        return;
    }
    bubbleSortDoramas();
    printf("\n%-4s | %-30s | %-4s | %-6s\n", "ID", "Nome", "Ano", "Nota");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < total_doramas; i++) {
        printf("%-4d | %-30s | %-6d | %.1f\n", doramas[i].id, doramas[i].titulo, doramas[i].ano, doramas[i].nota);
    }
} 

/*void listarDoramas() {
    for (int i = 0; i < total_doramas; i++) {
        printf("ID: %d | Titulo: %s | Ano: %d | Nota: %.1f\n", doramas[i].id, doramas[i].titulo, doramas[i].ano, doramas[i].nota);
    }
    if (total_doramas == 0) printf("Nenhum dorama cadastrado.\n");
}
*/


void adicionarDorama() {
    if (total_doramas >= MAX_DORAMAS) {
        printf("Limite de doramas atingido!\n");
        return;
    }
    Dorama novo;
    novo.id = prox_id++;

    printf("Digite o titulo: ");
    scanf(" %49[^\n]", novo.titulo);

    printf("Digite o ano: ");
    while (1) {
        if (scanf("%d", &novo.ano) == 1) {
            break;
        } else {
            printf("Entrada invalida! Digite um numero valido.\n");
            while (getchar() != '\n');
        }
    }

    printf("Digite a nota: ");
    while (1) {
        if (scanf("%f", &novo.nota) == 1 && novo.nota >= 0 && novo.nota <= 10) {
            break;
        } else {
            printf("Entrada invalida! A nota deve estar entre 0 e 10.\n");
            while (getchar() != '\n');
        }
    }

    doramas[total_doramas] = novo;
    total_doramas++;
    salvarEmArquivo();

    printf("Dorama adicionado!\n");
}
void removerDorama() {
    int id, i, j;
    printf("Digite o ID do dorama para remover: ");
    scanf("%d", &id);
    for (i = 0; i < total_doramas; i++) {
        if (doramas[i].id == id) {
            for (j = i; j < total_doramas - 1; j++) {
                doramas[j] = doramas[j + 1];
            }
            total_doramas--;
            salvarEmArquivo();
            printf("Dorama removido!\n");
            return;
        }
    }
    printf("Dorama nao encontrado.\n");
}

int main() {
    carregarDeArquivo();
    int opcao;

    while (1) {
        printf("\n-------------- Sua agenda funcional de doramas ---------------\n");
        printf("\n <3 Escolha uma das opcoes a seguir, e digite seu respectivo numero <3\n\n1 - Listar\n2 - Adicionar\n3 - Remover\n4 - Sair\n");
        printf("\n-->");
        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida! Digite um numero.\n");
            while (getchar() != '\n');
            continue;
        }

        if (opcao == 1) { 
            listarDoramas();
        }
         else if (opcao == 2) {
             adicionarDorama();
        } 
         else if (opcao == 3) {
            removerDorama();
        } 
         else if (opcao == 4) {
            break;
        }
         else {
            printf("Opcao invalida!\n");
        }
    }
    return 0;
}
