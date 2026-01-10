#include "Renderizacao.h"
#include "Constantes.h"
#include <sstream>

void desenharTabuleiro(RenderWindow& window, Tabuleiro& tabuleiro, int offsetX, const string& titulo, 
                      bool mostrarNavios, Texturas& texturas, Font& font, int hoverX, int hoverY) {
    desenharTexto(window, titulo, offsetX, 10, 24, Color::White, font);
    
    for (int i = 0; i < TAM; i++) {
        stringstream ss;
        ss << i;
        desenharTexto(window, ss.str(), offsetX + i * CELL_SIZE + 14, OFFSET_Y - 30, 18, Color(220, 220, 220), font);
        desenharTexto(window, ss.str(), offsetX - 20, OFFSET_Y + i * CELL_SIZE + 10, 18, Color(220, 220, 220), font);
    }
    
    bool navioCelulasDesenhadas[TAM][TAM] = {};
    
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            Color cor;
            int estado = tabuleiro.g[i][j];
            int tamanhoNavio = tabuleiro.tamanhoNavio[i][j];
            
            switch (estado) {
                case AGUA: 
                    cor = Color(50, 100, 200); 
                    break;
                case NAVIO: 
                    cor = mostrarNavios ? Color(100, 200, 100) : Color(50, 100, 200);
                    estado = mostrarNavios ? NAVIO : AGUA;
                    break;
                case ACERTO: 
                    cor = Color(220, 50, 50); 
                    break;
                case ERRO: 
                    cor = Color(80, 80, 100); 
                    break;
            }
            
            if (estado == NAVIO && mostrarNavios && !navioCelulasDesenhadas[i][j] && tamanhoNavio >= 2) {
                bool horizontal = (i + 1 < TAM && tabuleiro.g[i + 1][j] == NAVIO && 
                                  tabuleiro.tamanhoNavio[i + 1][j] == tamanhoNavio);
                
                for (int k = 0; k < tamanhoNavio; k++) {
                    int nx = horizontal ? i + k : i;
                    int ny = horizontal ? j : j + k;
                    if (nx < TAM && ny < TAM) {
                        navioCelulasDesenhadas[nx][ny] = true;
                    }
                }
                
                Texture* navioTex = texturas.getNavioTexture(tamanhoNavio);
                if (navioTex) {
                    float larguraOriginal = tamanhoNavio * (CELL_SIZE - 2);
                    float alturaOriginal = (CELL_SIZE - 2);
                    
                    RectangleShape navioShape(Vector2f(larguraOriginal, alturaOriginal));
                    navioShape.setTexture(navioTex);
                    navioShape.setFillColor(Color::White);
                    navioShape.setOutlineColor(Color(30, 30, 50));
                    navioShape.setOutlineThickness(1);
                    
                    if (horizontal) {
                        navioShape.setPosition(Vector2f(i * CELL_SIZE + offsetX + 1, j * CELL_SIZE + OFFSET_Y + 1));
                    } else {
                        navioShape.setOrigin(Vector2f(0, 0));
                        navioShape.setRotation(degrees(90));
                        navioShape.setPosition(Vector2f(i * CELL_SIZE + offsetX + 1 + (CELL_SIZE - 2), 
                                                        j * CELL_SIZE + OFFSET_Y + 1));
                    }
                    
                    window.draw(navioShape);
                } else {
                    for (int k = 0; k < tamanhoNavio; k++) {
                        int nx = horizontal ? i + k : i;
                        int ny = horizontal ? j : j + k;
                        if (nx < TAM && ny < TAM) {
                            bool hover = (nx == hoverX && ny == hoverY);
                            desenharCelula(window, nx, ny, cor, offsetX, texturas, AGUA, 0, hover);
                            
                            RectangleShape cell(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                            cell.setPosition(Vector2f(nx * CELL_SIZE + offsetX + 1, ny * CELL_SIZE + OFFSET_Y + 1));
                            cell.setFillColor(cor);
                            cell.setOutlineColor(Color(30, 30, 50));
                            cell.setOutlineThickness(1);
                            window.draw(cell);
                        }
                    }
                }
            } else if (!navioCelulasDesenhadas[i][j]) {
                bool hover = (i == hoverX && j == hoverY);
                desenharCelula(window, i, j, cor, offsetX, texturas, estado, tamanhoNavio, hover);
                
                if (estado == ACERTO && !texturas.acertoCarregada) {
                    RectangleShape line1(Vector2f(CELL_SIZE * 0.7f, 2));
                    line1.setPosition(Vector2f(offsetX + i * CELL_SIZE + 5.f, OFFSET_Y + j * CELL_SIZE + CELL_SIZE / 2.f));
                    line1.setRotation(degrees(45));
                    line1.setFillColor(Color::White);
                    window.draw(line1);
                    
                    RectangleShape line2(Vector2f(CELL_SIZE * 0.7f, 2));
                    line2.setPosition(Vector2f(offsetX + i * CELL_SIZE + CELL_SIZE - 5.f, OFFSET_Y + j * CELL_SIZE + CELL_SIZE / 2.f));
                    line2.setRotation(degrees(-45));
                    line2.setFillColor(Color::White);
                    window.draw(line2);
                }
                
                if (estado == ERRO && !texturas.erroCarregada) {
                    CircleShape dot(3);
                    dot.setPosition(Vector2f(offsetX + i * CELL_SIZE + CELL_SIZE / 2.f - 3, OFFSET_Y + j * CELL_SIZE + CELL_SIZE / 2.f - 3));
                    dot.setFillColor(Color(150, 150, 150));
                    window.draw(dot);
                }
            }
        }
    }
}
