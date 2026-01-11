#pragma once

const int TAM = 10;
const int NUM_NAVIOS = 8;
const int CELL_SIZE = 35;
const int OFFSET_Y = 100;
const int SPACING = 50;
const int LARGURA_JANELA = 900;
const int ALTURA_JANELA = 600;

enum Estado { AGUA = 0, NAVIO = 1, ERRO = 2, ACERTO = 3 };
enum FaseJogo { MENU, COLOCACAO_NAVIOS, JOGANDO, FIM };
