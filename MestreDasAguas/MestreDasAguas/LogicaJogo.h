#pragma once
#include "Estruturas.h"
#include <vector>

using namespace std;

void inicializar(Tabuleiro& t);
bool podeColocar(Tabuleiro& t, int x, int y, int tam, bool horizontal);
void colocarNavio(Tabuleiro& t, int x, int y, int tam, bool horizontal);
void colocarNaviosPC(Tabuleiro& t);
int contarRestantes(Tabuleiro& t);

class IA {
private:
    vector<Coordenada> listaAdjacentes;
    
public:
    Coordenada escolherTiro(Tabuleiro& jog);
    void adicionarAdjacentes(Coordenada c, Tabuleiro& jog);
    void jogadaComputador(Tabuleiro& pc, Tabuleiro& jog);
    void limpar();
};
