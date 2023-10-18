#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAZE_SIZE 10000

char** maze;

//Estrutura para representar as c�lulas do algoritmo de cria��o do labirinto
struct {
    long x;
    long y;
} cells[MAZE_SIZE * MAZE_SIZE / 2];

//Estrutura de Coordenadas, utilizada no algoritmo de busca em profundidade
typedef struct {
    long x;
    long y;
} Point;

//Fun��o para criar uma inst�ncia da vari�vel do labirinto chamada de 'maze'
void inicializa_maze() {
    maze = (char**)malloc(MAZE_SIZE * sizeof(char*));
    long i;
    //#pragma omp parallel for private(i)
    for (i = 0; i < MAZE_SIZE; i++) {
        maze[i] = (char*)malloc(MAZE_SIZE * sizeof(char));
    }
}

//Fun��o para liberar a mem�ria da vari�vel 'maze', ap�s o uso
void liberar_memoria() {
    long i;
    //#pragma omp parallel for private(i)
    for (i = 0; i < MAZE_SIZE; i++) {
        free(maze[i]);
    }
    free(maze);
}

//Fun��o para criar o labirinto
void create_maze() {
    inicializa_maze();

    //#pragma omp parallel for collapse(2)
    for (long i = 0; i < MAZE_SIZE; i++) {
        for (long j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = '#';
        }
    }

    // Lista para armazenar as c�lulas candidatas para expandir
    int num_cells = 1;
    cells[0].x = 1;
    cells[0].y = 1;

    // Marca a c�lula inicial como parte do labirinto
    maze[1][1] = ' ';

    // Enquanto houverem c�lulas candidatas para expandir
    while (num_cells > 0) {
        // Escolhe uma c�lula aleat�ria da lista
        long index = rand() % num_cells;
        long x = cells[index].x;
        long y = cells[index].y;

        // Verifica as c�lulas vizinhas
        int dx[] = {-2, 2, 0, 0};
        int dy[] = {0, 0, -2, 2};
        int neighbors[4];
        int num_neighbors = 0;

        // Encontra as c�lulas vizinhas n�o marcadas
        for (int i = 0; i < 4; i++) {
            long nx = x + dx[i];
            long ny = y + dy[i];
            if (nx > 0 && nx < MAZE_SIZE && ny > 0 && ny < MAZE_SIZE && maze[nx][ny] == '#') {
                neighbors[num_neighbors++] = i;
            }
        }

        // Se houver c�lulas vizinhas n�o marcadas
        if (num_neighbors > 0) {
            // Escolhe uma outra c�lula vizinha aleat�ria
            int direction = neighbors[rand() % num_neighbors];
            long nx = x + dx[direction];
            long ny = y + dy[direction];

            // Marca a c�lula como parte do labirinto
            maze[nx][ny] = ' ';
            // Abre a parede entre a c�lula atual e a c�lula escolhida
            maze[x + dx[direction] / 2][y + dy[direction] / 2] = ' ';

            // Adiciona a c�lula escolhida � lista de c�lulas candidatas
            cells[num_cells].x = nx;
            cells[num_cells].y = ny;
            num_cells++;
        } else {
            // Se n�o houver c�lulas vizinhas n�o marcadas, remove a c�lula atual da lista
            cells[index] = cells[num_cells - 1];
            num_cells--;
        }
    }

    // Define o ponto de partida e o ponto de chegada
    maze[0][0] = 'E';
    maze[0][MAZE_SIZE - 1] = 'S';

    //Cria contornos entre a entrada e a sa�da.
    long i;
    //#pragma omp parallel for private(i)
    for(i=0; i<MAZE_SIZE; i++){
        if(maze[0][i] == 'E'){
            maze[0][i+1] = ' ';
            maze[1][i] = ' ';
        }

        maze[MAZE_SIZE-1][i] = '#';

        if(maze[i][MAZE_SIZE-1] == 'S'){
            maze[i][MAZE_SIZE-2] = ' ';
            maze[i+1][MAZE_SIZE-2] = ' ';
        }else{
            maze[i][MAZE_SIZE-1] = '#';
        }
    }

}

// Fun��o para imprimir o labirinto
void print_maze() {
    for (long i = 0; i < MAZE_SIZE; i++) {
        for (long j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

//Fun��o para validar as coordenadas do labirinto
int eh_valido(long x, long y) {
    return (x >= 0 && x < MAZE_SIZE) && (y >= 0 && y < MAZE_SIZE);
}

//Fun��o de Busca em Pronfundidade de maneira iterativa
int busca_em_profundidade(long startX, long startY) {
    // Pilha para armazenar os pontos a serem explorados
    Point *stack;
    stack = (Point*) malloc((sizeof(Point)*(MAZE_SIZE*MAZE_SIZE)));
    long top = 0;

    stack[top].x = startX;
    stack[top].y = startY;

    while (top >= 0) {
        Point current = stack[top];
        top--;

        if (maze[current.y][current.x] == 'S') {
            return 1; // Encontrou a sa�da
        }

        maze[current.y][current.x] = '-'; // Marcar como visitado

        // Vetores para representar as dire��es poss�veis (esquerda, baixo, direita, cima)
        int dx[] = {0, 1, 0, -1};
        int dy[] = {-1, 0, 1, 0};

        //Percorre todas as dire��es poss�veis
        //#pragma omp parallel for
        for (int i = 0; i < 4; i++) {
            long newX = current.x + dx[i];
            long newY = current.y + dy[i];

            // Verifica se as novas coordenadas est�o dentro dos limites do labirinto e se � um caminho vazio ou a sa�da
            if (eh_valido(newX, newY) && (maze[newY][newX] == ' ' || maze[newY][newX] == 'S')) {
                top++;
                stack[top].x = newX;
                stack[top].y = newY;
            }
        }
    }

    return 0; // Caminho n�o encontrado
}

int main() {
    srand(time(NULL));

    //omp_set_num_threads(2); // Define o n�mero de Threads

    double timei = omp_get_wtime();

    create_maze();

    //printf("Labirinto Criado!\n");
    //print_maze();

    int resp = busca_em_profundidade(0, 0);

    double timef = omp_get_wtime();
    printf("Time: %lf\n", timef-timei);

    //printf("\nCAMINHO FEITO PELO LABIRINTO: \n");
    //print_maze();

    if(resp)
        printf("Caminho encontrado! =)\n");
    else
        printf("Caminho NAO encontrado! =(\n");

    liberar_memoria();

    return 0;
}
