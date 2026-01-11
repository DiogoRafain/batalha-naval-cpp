#pragma once
#include <SFML/Graphics.hpp>
#include "Estruturas.h"
#include "Texturas.h"
#include "Botoes.h"
#include <string>

using namespace sf;
using namespace std;

void desenharCelula(RenderWindow& window, int x, int y, Color cor, int offsetX, Texturas& texturas, 
                   int estado, int tamanhoNavio, bool hover = false);

void desenharTexto(RenderWindow& window, const string& texto, int x, int y, int tamanho, Color cor, Font& font);

void desenharNavioArrastar(RenderWindow& window, NavioArrastar& navio, int index, bool podeColocarAqui, 
                          Texturas& texturas, Font& font);

void desenharTabuleiro(RenderWindow& window, Tabuleiro& tabuleiro, int offsetX, const string& titulo, 
                      bool mostrarNavios, Texturas& texturas, Font& font, int hoverX = -1, int hoverY = -1);

void desenharInfo(RenderWindow& window, int naviosJogador, int naviosPC, const string& mensagem, Font& font);

void desenharFimDeJogo(RenderWindow& window, bool jogadorGanhou, Font& font, Botao& botaoJogarNovamente, Botao& botaoSair);
