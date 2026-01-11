#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

struct Texturas {
    Texture agua;
    Texture navio2, navio3, navio4, navio5;
    Texture acerto;
    Texture erro;
    Texture iconeComSom;
    Texture iconeSemSom;
    bool aguaCarregada;
    bool navio2Carregado, navio3Carregado, navio4Carregado, navio5Carregado;
    bool acertoCarregada;
    bool erroCarregada;
    bool iconeComSomCarregado;
    bool iconeSemSomCarregado;
    bool carregadas;
    
    Texturas();
    bool carregar();
    Texture* getNavioTexture(int tamanho);
};
