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
    
    // PRIMEIRA PASSAGEM: Desenhar água e navios
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            int estado = tabuleiro.g[i][j];
            int tamanhoNavio = tabuleiro.tamanhoNavio[i][j];
            
            // Desenhar navio completo (apenas se não estiver completamente destruído)
            if ((estado == NAVIO || estado == ACERTO) && mostrarNavios && !navioCelulasDesenhadas[i][j] && tamanhoNavio >= 2) {
                // Verificar orientação considerando células com NAVIO ou ACERTO
                bool horizontal = (i + 1 < TAM && 
                                  (tabuleiro.g[i + 1][j] == NAVIO || tabuleiro.g[i + 1][j] == ACERTO) && 
                                  tabuleiro.tamanhoNavio[i + 1][j] == tamanhoNavio);
                
                // Verificar se o navio está completamente destruído
                bool navioDestruido = true;
                for (int k = 0; k < tamanhoNavio; k++) {
                    int nx = horizontal ? i + k : i;
                    int ny = horizontal ? j : j + k;
                    if (nx < TAM && ny < TAM) {
                        if (tabuleiro.g[nx][ny] == NAVIO) {
                            navioDestruido = false;
                            break;
                        }
                    }
                }
                
                // Marcar células como desenhadas
                for (int k = 0; k < tamanhoNavio; k++) {
                    int nx = horizontal ? i + k : i;
                    int ny = horizontal ? j : j + k;
                    if (nx < TAM && ny < TAM) {
                        navioCelulasDesenhadas[nx][ny] = true;
                    }
                }
                
                // Só desenhar o navio se NÃO estiver completamente destruído
                if (!navioDestruido) {
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
                        // Fallback sem textura
                        for (int k = 0; k < tamanhoNavio; k++) {
                            int nx = horizontal ? i + k : i;
                            int ny = horizontal ? j : j + k;
                            if (nx < TAM && ny < TAM) {
                                RectangleShape cell(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                                cell.setPosition(Vector2f(nx * CELL_SIZE + offsetX + 1, ny * CELL_SIZE + OFFSET_Y + 1));
                                cell.setFillColor(Color(100, 200, 100));
                                cell.setOutlineColor(Color(30, 30, 50));
                                cell.setOutlineThickness(1);
                                window.draw(cell);
                            }
                        }
                    }
                }
            } 
            // Desenhar células de água e navios não mostrados
            else if (!navioCelulasDesenhadas[i][j]) {
                Color cor;
                int estadoDesenho = estado;
                
                if (estado == NAVIO && mostrarNavios) {
                    cor = Color(100, 200, 100);
                    estadoDesenho = NAVIO;
                } else if (estado == NAVIO && !mostrarNavios) {
                    cor = Color(50, 100, 200);
                    estadoDesenho = AGUA;
                } else if (estado == AGUA) {
                    cor = Color(50, 100, 200);
                    estadoDesenho = AGUA;
                } else if (estado == ACERTO) {
                    // Não desenhar ainda, será desenhado na segunda passagem
                    cor = Color(50, 100, 200);
                    estadoDesenho = AGUA;
                } else if (estado == ERRO) {
                    // Não desenhar ainda, será desenhado na segunda passagem
                    cor = Color(50, 100, 200);
                    estadoDesenho = AGUA;
                }
                
                // Desenhar fundo (água)
                desenharCelula(window, i, j, cor, offsetX, texturas, 
                              (estado == ACERTO || estado == ERRO) ? AGUA : estadoDesenho, 
                              tamanhoNavio, false);
            }
        }
    }
    
    // SEGUNDA PASSAGEM: Desenhar ícones de acerto e erro por cima
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            int estado = tabuleiro.g[i][j];
            bool hover = (i == hoverX && j == hoverY);
            
            if (estado == ACERTO) {
                // Desenhar ícone de acerto por cima
                if (texturas.acertoCarregada) {
                    RectangleShape acertoShape(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                    acertoShape.setPosition(Vector2f(i * CELL_SIZE + offsetX + 1, j * CELL_SIZE + OFFSET_Y + 1));
                    acertoShape.setTexture(&texturas.acerto);
                    acertoShape.setFillColor(Color::White);
                    
                    if (hover) {
                        acertoShape.setOutlineColor(Color::Yellow);
                        acertoShape.setOutlineThickness(2);
                    }
                    
                    window.draw(acertoShape);
                } else {
                    // Fallback: desenhar X
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
            }
            else if (estado == ERRO) {
                // Desenhar ícone de erro por cima
                if (texturas.erroCarregada) {
                    RectangleShape erroShape(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                    erroShape.setPosition(Vector2f(i * CELL_SIZE + offsetX + 1, j * CELL_SIZE + OFFSET_Y + 1));
                    erroShape.setTexture(&texturas.erro);
                    erroShape.setFillColor(Color::White);
                    
                    if (hover) {
                        erroShape.setOutlineColor(Color::Yellow);
                        erroShape.setOutlineThickness(2);
                    }
                    
                    window.draw(erroShape);
                } else {
                    // Fallback: desenhar ponto
                    CircleShape dot(3);
                    dot.setPosition(Vector2f(offsetX + i * CELL_SIZE + CELL_SIZE / 2.f - 3, OFFSET_Y + j * CELL_SIZE + CELL_SIZE / 2.f - 3));
                    dot.setFillColor(Color(150, 150, 150));
                    window.draw(dot);
                }
            }
            
            // Desenhar borda amarela no hover
            if (hover && estado != ACERTO && estado != ERRO) {
                RectangleShape hoverBorder(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                hoverBorder.setPosition(Vector2f(i * CELL_SIZE + offsetX + 1, j * CELL_SIZE + OFFSET_Y + 1));
                hoverBorder.setFillColor(Color::Transparent);
                hoverBorder.setOutlineColor(Color::Yellow);
                hoverBorder.setOutlineThickness(2);
                window.draw(hoverBorder);
            }
        }
    }
}
