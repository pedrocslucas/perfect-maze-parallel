#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAZE_SIZE 50

char** maze;

void inicializa_maze() {
    maze = (char**)malloc(MAZE_SIZE * sizeof(char*));
    for (long long i = 0; i < MAZE_SIZE; i++) {
        maze[i] = (char*)malloc(MAZE_SIZE * sizeof(char));
    }
}

void liberar_memoria() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        free(maze[i]);
    }
    free(maze);
}

void create_maze() {
    inicializa_maze();
    // Inicialize o labirinto com todas as células preenchidas com paredes
    //#pragma omp parallel for collapse(2)
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            maze[i][j] = '#';
        }
    }

    // Lista para armazenar as células candidatas para expandir
    struct {
        int x;
        int y;
    } cells[MAZE_SIZE * MAZE_SIZE / 2];
    int num_cells = 1;
    cells[0].x = 1;
    cells[0].y = 1;

    // Marque a célula inicial como parte do labirinto
    maze[1][1] = ' ';

    // Enquanto houverem células candidatas para expandir
    while (num_cells > 0) {
        // Escolha uma célula aleatória da lista
        int index = rand() % num_cells;
        int x = cells[index].x;
        int y = cells[index].y;

        // Verifique as células vizinhas
        int dx[] = {-2, 2, 0, 0};
        int dy[] = {0, 0, -2, 2};
        int neighbors[4];
        int num_neighbors = 0;

        // Encontre as células vizinhas não marcadas
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx > 0 && nx < MAZE_SIZE && ny > 0 && ny < MAZE_SIZE && maze[nx][ny] == '#') {
                neighbors[num_neighbors++] = i;
            }
        }

        // Se houver células vizinhas não marcadas
        if (num_neighbors > 0) {
            // Escolha uma célula vizinha aleatória
            int direction = neighbors[rand() % num_neighbors];
            int nx = x + dx[direction];
            int ny = y + dy[direction];

            // Marque a célula como parte do labirinto
            maze[nx][ny] = ' ';
            // Abra a parede entre a célula atual e a célula escolhida
            maze[x + dx[direction] / 2][y + dy[direction] / 2] = ' ';

            // Adicione a célula escolhida à lista de células candidatas
            cells[num_cells].x = nx;
            cells[num_cells].y = ny;
            num_cells++;
        } else {
            // Se não houver células vizinhas não marcadas, remova a célula atual da lista
            cells[index] = cells[num_cells - 1];
            num_cells--;
        }
    }

    int ta_dentu = 0;
    int ta_fora = 0;
    for(int i=0; i<MAZE_SIZE; i++){
        if(ta_dentu == 0 && maze[0][i] == ' '){
            ta_dentu = 1;
            maze[0][i] = 'E';
        }
        if(!ta_dentu)
            maze[0][i] = '#';

        maze[i][0] = '#';

        maze[i][MAZE_SIZE-1] = '#';

        if(ta_fora)
            maze[MAZE_SIZE-1][i] = '#';
        if(ta_fora == 0 && maze[MAZE_SIZE-1][i] == ' '){
            ta_fora = 1;
            maze[MAZE_SIZE-1][i] = 'S';
        }

    }

    // Defina o ponto de partida e o ponto de chegada
    //maze[0][0] = 'E';
    //maze[0][1] = ' ';
    //maze[MAZE_SIZE - 1][MAZE_SIZE - 1] = 'S';


}

// Fun��o para imprimir o labirinto
void print_maze() {
    for (int i = 0; i < MAZE_SIZE; i++) {
        for (int j = 0; j < MAZE_SIZE; j++) {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

typedef struct {
    int x;
    int y;
} Point;

int eh_valido(int x, int y) {
    return (x >= 0 && x < MAZE_SIZE) && (y >= 0 && y < MAZE_SIZE);
}

int busca_em_profundidade(int startX, int startY) {
    Point *stack;
    stack = (Point*) malloc((sizeof(Point)*(MAZE_SIZE*MAZE_SIZE)));
    // Pilha para armazenar os pontos a serem explorados
    int top = 0;

    stack[top].x = startX;
    stack[top].y = startY;

    while (top >= 0) {
        Point current = stack[top];
        top--;

        if (maze[current.y][current.x] == 'S') {
            return 1; // Encontrou a saída
        }

        maze[current.y][current.x] = '-'; // Marcar como visitado

        // Vetores para representar as direções possíveis (cima, direita, baixo, esquerda)
        int dx[] = {0, 1, 0, -1};
        int dy[] = {-1, 0, 1, 0};

        //#pragma omp parallel for
        for (int i = 0; i < 4; i++) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            // Verifica se as novas coordenadas estão dentro dos limites do labirinto e se é um caminho válido
            if (eh_valido(newX, newY) && (maze[newY][newX] == ' ' || maze[newY][newX] == 'S')) {
                top++;
                stack[top].x = newX;
                stack[top].y = newY;
            }
        }
    }

    return 0; // Caminho não encontrado
}

int main() {
    srand(time(NULL));

    //omp_set_num_threads(2); // Define o número de Threads

    // Crie o labirinto e inicialize a matriz de caminho
    create_maze();

    //printf("Labirinto Criado!\n");
    //print_maze();

    double timei = omp_get_wtime();

    int resp = busca_em_profundidade(0, 0);

    double timef = omp_get_wtime();
    printf("Time: %lf\n", timef-timei);

    //printf("\nCAMINHO FEITO PELO LABIRINTO: \n");
    print_maze();

    if(resp)
        printf("Caminho encontrado! =)\n");
    else
        printf("Caminho NAO encontrado! =(\n");

    liberar_memoria();

    return 0;
}

