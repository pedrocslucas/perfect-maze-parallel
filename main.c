#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAZE_SIZE 1000

char maze[MAZE_SIZE][MAZE_SIZE];

// Função para imprimir o labirinto
void print_maze() {
    #pragma omp parallel for
    for (int i = 0; i < MAZE_SIZE; i++) {
        #pragma omp parallel for
        for (int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

// Função para criar um labirinto com caminhos internos e alternativos
void create_maze() {
    // Inicializa o labirinto com todas as células preenchidas com paredes
    #pragma omp parallel for
    for (int i = 0; i < MAZE_SIZE; i++) {
        #pragma omp parallel for
        for (int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = '#';
        }
    }

    // Define o ponto de partida e o ponto de chegada
    maze[0][0] = 'E';
    maze[0][1] = ' ';
    maze[MAZE_SIZE - 1][MAZE_SIZE - 1] = 'S';
    maze[MAZE_SIZE - 1][MAZE_SIZE - 2] = ' ';

    // Caminho principal para a saída
    int x = 1;
    int y = 0;
    while (x < MAZE_SIZE - 1 || y < MAZE_SIZE - 2) {
        #pragma omp parallel for
        for (int i = x; i < x + 1; i++) {
            #pragma omp parallel for
            for (int j = y; j < y + 1; j++) {
                #pragma omp atomic write
                maze[i][j] = ' ';
            }
        }

        if (x < MAZE_SIZE - 1 && rand() % 2 == 0) {
            x++;
        } else if (y < MAZE_SIZE - 2) {
            y++;
        }
    }

    #pragma omp parallel for
    for (int i = 1; i < MAZE_SIZE - 1; i += 2) {
        #pragma omp parallel for
        for (int j = 1; j < MAZE_SIZE - 2; j++) {
            if (rand() % 2 == 0) {
                #pragma omp atomic write
                maze[i][j] = ' ';
            }
        }
    }

    #pragma omp parallel for
    for (int i = 2; i < MAZE_SIZE - 1; i += 2) {
        #pragma omp parallel for
        for (int j = 2; j < MAZE_SIZE - 2; j++) {
            if (rand() % 2 == 0) {
                #pragma omp atomic write
                maze[i][j] = ' ';
            }
        }
    }
}

typedef struct {
    int x, y;
} Coordenada;

typedef struct {
    Coordenada item[MAZE_SIZE*MAZE_SIZE];
    int topo;
} Pilha;

void inicializa_pilha(Pilha *p){
    p->topo = -1;
}

void push(Pilha *p, Coordenada coord){
    p->topo++;
    p->item[p->topo] = coord;
}

Coordenada pop(Pilha *p){
    Coordenada coord = p->item[p->topo];
    p->topo--;
    return coord;
}

int eh_valido(int x, int y){
    return (x >= 0 && x < MAZE_SIZE) && (y >= 0 && y < MAZE_SIZE);
}

int busca_em_profundidade(int inicio_x, int inicio_y) {
    Pilha pilha;
    inicializa_pilha(&pilha);

    Coordenada inicio = {inicio_x, inicio_y};
    push(&pilha, inicio);

    while (pilha.topo != -1) {
        Coordenada atual;
        #pragma omp critical
        {
            atual = pop(&pilha);
        }

        int x = atual.x;
        int y = atual.y;

        // Condição de Parada
        if (maze[x][y] == 'S') {
            return 1;
        }

        // Marcando a célula como visitada
        #pragma omp atomic write
        maze[x][y] = '.';

        // Todos os movimentos possíveis
        int movimentosX[4] = {0, 0, -1, 1};
        int movimentosY[4] = {-1, 1, 0, 0};

        #pragma omp parallel for nowait
        for (int i = 0; i < 4; i++) {
            int novoX = x + movimentosX[i];
            int novoY = y + movimentosY[i];

            if (eh_valido(novoX, novoY) && (maze[novoX][novoY] == ' ' || maze[novoX][novoY] == 'S')) {
                Coordenada novoCoord = {novoX, novoY};
                #pragma omp critical
                {
                    push(&pilha, novoCoord);
                }
            }
        }
    }

    // Se a pilha estiver vazia e não chegarmos à saída, então não há caminho
    return 0;
}

int main() {
    srand(time(NULL));

    omp_set_num_threads(2); //Definir o número de threads

    // Crie o labirinto e inicialize a matriz de caminho
    create_maze();

    printf("Labirinto Criado:\n");
    print_maze();

    int resp = busca_em_profundidade(0, 0);
    printf("\nCAMINHO FEITO PELO LABIRINTO: \n");
    print_maze();
    if(resp == 1)
        printf("Caminho encontrado! =)\n");
    else
        printf("Caminho NAO encontrado! =(\n");

    return 0;
}
