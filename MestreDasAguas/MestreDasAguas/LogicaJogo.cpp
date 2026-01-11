#include "LogicaJogo.h"
#include "Constantes.h"
#include <cstdlib>

extern int TAMANHOS[NUM_NAVIOS];

void inicializar(Tabuleiro& t) {
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++) {
            t.g[i][j] = AGUA;
            t.tamanhoNavio[i][j] = 0;
        }
}

bool podeColocar(Tabuleiro& t, int x, int y, int tam, bool horizontal) {
    if (x < 0 || y < 0) return false;
    if (horizontal && x + tam > TAM) return false;
    if (!horizontal && y + tam > TAM) return false;
    for (int i = 0; i < tam; i++) {
        int nx = horizontal ? x + i : x;
        int ny = horizontal ? y : y + i;
        if (t.g[nx][ny] != AGUA) return false;
    }
    return true;
}

void colocarNavio(Tabuleiro& t, int x, int y, int tam, bool horizontal) {
    for (int i = 0; i < tam; i++) {
        int nx = horizontal ? x + i : x;
        int ny = horizontal ? y : y + i;
        t.g[nx][ny] = NAVIO;
        t.tamanhoNavio[nx][ny] = tam;
    }
}

void colocarNaviosPC(Tabuleiro& t) {
    for (int n = 0; n < NUM_NAVIOS; n++) {
        while (true) {
            int x = rand() % TAM, y = rand() % TAM, ori = rand() % 2;
            bool horiz = (ori == 0);
            if (podeColocar(t, x, y, TAMANHOS[n], horiz)) {
                colocarNavio(t, x, y, TAMANHOS[n], horiz);
                break;
            }
        }
    }
}

int contarRestantes(Tabuleiro& t) {
    int c = 0;
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            if (t.g[i][j] == NAVIO) c++;
    return c;
}

Coordenada IA::escolherTiro(Tabuleiro& jog) {
    if (!listaAdjacentes.empty()) {
        Coordenada c = listaAdjacentes.back();
        listaAdjacentes.pop_back();
        return c;
    }
    Coordenada c;
    do {
        c.x = rand() % TAM;
        c.y = rand() % TAM;
    } while (jog.g[c.x][c.y] == ACERTO || jog.g[c.x][c.y] == ERRO);
    return c;
}

void IA::adicionarAdjacentes(Coordenada c, Tabuleiro& jog) {
    int dx[4] = { -1,1,0,0 };
    int dy[4] = { 0,0,-1,1 };
    for (int i = 0; i < 4; i++) {
        int nx = c.x + dx[i];
        int ny = c.y + dy[i];
        if (nx >= 0 && nx < TAM && ny >= 0 && ny < TAM) {
            if (jog.g[nx][ny] == AGUA || jog.g[nx][ny] == NAVIO) {
                listaAdjacentes.push_back({ nx,ny });
            }
        }
    }
}

void IA::jogadaComputador(Tabuleiro& pc, Tabuleiro& jog) {
    Coordenada c = escolherTiro(jog);
    if (jog.g[c.x][c.y] == NAVIO) {
        jog.g[c.x][c.y] = ACERTO;
        adicionarAdjacentes(c, jog);
    }
    else {
        jog.g[c.x][c.y] = ERRO;
    }
}

void IA::limpar() {
    listaAdjacentes.clear();
}
