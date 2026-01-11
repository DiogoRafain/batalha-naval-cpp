#pragma once
#include <SFML/Graphics.hpp>
#include "Constantes.h"

using namespace sf;

struct Tabuleiro { 
    int g[TAM][TAM]; 
    int tamanhoNavio[TAM][TAM];
};

struct Coordenada { 
    int x, y; 
};

struct NavioArrastar {
    int tamanho;
    bool horizontal;
    bool colocado;
    Vector2f posicao;
    bool arrastando;
};
