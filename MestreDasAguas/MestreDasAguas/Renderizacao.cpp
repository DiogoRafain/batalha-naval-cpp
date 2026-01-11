#include "Renderizacao.h"
#include "Constantes.h"
#include <sstream>

void desenharCelula(RenderWindow& window, int x, int y, Color cor, int offsetX, Texturas& texturas, 
                   int estado, int tamanhoNavio, bool hover) {
    RectangleShape cell(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    cell.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
    bool usouTextura = false;
    
    switch (estado) {
        case AGUA:
            // Não usar textura de água - apenas cor sólida
            cell.setFillColor(Color(50, 100, 200));
            usouTextura = true;
            break;
            
        case NAVIO:
            if (tamanhoNavio >= 2 && tamanhoNavio <= 5) {
                Texture* navioTex = texturas.getNavioTexture(tamanhoNavio);
                if (navioTex) {
                    cell.setTexture(navioTex);
                    cell.setFillColor(Color::White);
                    usouTextura = true;
                }
            }
            break;
            
        case ACERTO:
            if (texturas.acertoCarregada) {
                // Desenhar fundo azul sólido em vez de textura
                RectangleShape fundo(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                fundo.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
                fundo.setFillColor(Color(50, 100, 200));
                
                if (hover) {
                    fundo.setOutlineColor(Color::Yellow);
                    fundo.setOutlineThickness(2);
                } else {
                    fundo.setOutlineColor(Color(30, 30, 50));
                    fundo.setOutlineThickness(1);
                }
                
                window.draw(fundo);
                
                cell.setTexture(&texturas.acerto);
                cell.setFillColor(Color::White);
                usouTextura = true;
            }
            break;
            
        case ERRO:
            if (texturas.erroCarregada) {
                // Desenhar fundo azul sólido em vez de textura
                RectangleShape fundo(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                fundo.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
                fundo.setFillColor(Color(50, 100, 200));
                
                if (hover) {
                    fundo.setOutlineColor(Color::Yellow);
                    fundo.setOutlineThickness(2);
                } else {
                    fundo.setOutlineColor(Color(30, 30, 50));
                    fundo.setOutlineThickness(1);
                }
                
                window.draw(fundo);
                
                cell.setTexture(&texturas.erro);
                cell.setFillColor(Color::White);
                usouTextura = true;
            }
            break;
    }
    
    if (!usouTextura) {
        cell.setFillColor(cor);
    }
    
    if (hover) {
        cell.setOutlineColor(Color::Yellow);
        cell.setOutlineThickness(2);
    } else {
        cell.setOutlineColor(Color(30, 30, 50));
        cell.setOutlineThickness(1);
    }
    
    window.draw(cell);
}

void desenharTexto(RenderWindow& window, const string& texto, int x, int y, int tamanho, Color cor, Font& font) {
    Text text(font, texto, tamanho);
    text.setPosition(Vector2f(x, y));
    text.setFillColor(cor);
    window.draw(text);
}

void desenharNavioArrastar(RenderWindow& window, NavioArrastar& navio, int index, bool podeColocarAqui, 
                          Texturas& texturas, Font& font) {
    Color cor;
    if (navio.colocado) {
        cor = Color(100, 200, 100);
    } else if (navio.arrastando) {
        cor = podeColocarAqui ? Color(150, 255, 150) : Color(255, 100, 100);
    } else {
        cor = Color(100, 150, 200);
    }
    
    float largura = navio.horizontal ? navio.tamanho * CELL_SIZE - 4 : CELL_SIZE - 4;
    float altura = navio.horizontal ? CELL_SIZE - 4 : navio.tamanho * CELL_SIZE - 4;
    
    RectangleShape shape(Vector2f(largura, altura));
    shape.setPosition(navio.posicao);
    
    Texture* navioTex = texturas.getNavioTexture(navio.tamanho);
    if (navioTex && !navio.colocado) {
        RectangleShape texShape(Vector2f(navio.tamanho * CELL_SIZE - 4, CELL_SIZE - 4));
        texShape.setTexture(navioTex);
        texShape.setFillColor(cor);
        
        if (navio.horizontal) {
            texShape.setPosition(navio.posicao);
        } else {
            texShape.setOrigin(Vector2f(0, 0));
            texShape.setRotation(degrees(90));
            texShape.setPosition(Vector2f(navio.posicao.x + (CELL_SIZE - 4), navio.posicao.y));
        }
        
        texShape.setOutlineColor(Color(50, 50, 70));
        texShape.setOutlineThickness(2);
        window.draw(texShape);
    } else {
        shape.setFillColor(cor);
        shape.setOutlineColor(Color(50, 50, 70));
        shape.setOutlineThickness(2);
        window.draw(shape);
    }
    
    if (!navio.colocado) {
        stringstream ss;
        ss << navio.tamanho;
        desenharTexto(window, ss.str(), navio.posicao.x + 8, navio.posicao.y + 8, 18, Color::White, font);
    }
}

void desenharInfo(RenderWindow& window, int naviosJogador, int naviosPC, const string& mensagem, Font& font) {
    int yPos = OFFSET_Y + TAM * CELL_SIZE + 20;
    
    desenharTexto(window, "Teus navios: " + to_string(naviosJogador), 80, yPos, 18, Color::Green, font);
    desenharTexto(window, "Navios PC: " + to_string(naviosPC), 510, yPos, 18, Color::Red, font);
    
    if (!mensagem.empty()) {
        desenharTexto(window, mensagem, 80, yPos + 30, 16, Color::Yellow, font);
    }
}

void desenharFimDeJogo(RenderWindow& window, bool jogadorGanhou, Font& font, Botao& botaoJogarNovamente, Botao& botaoSair) {
    RectangleShape overlay(Vector2f(LARGURA_JANELA, ALTURA_JANELA));
    overlay.setFillColor(Color(0, 0, 0, 180));
    window.draw(overlay);
    
    RectangleShape caixa(Vector2f(500, 320));
    caixa.setPosition(Vector2f(200, 150));
    caixa.setFillColor(Color(30, 40, 60));
    caixa.setOutlineColor(jogadorGanhou ? Color(100, 200, 100) : Color(200, 100, 100));
    caixa.setOutlineThickness(4);
    window.draw(caixa);
    
    Text titulo(font, jogadorGanhou ? "VITORIA!" : "DERROTA!", 48);
    titulo.setFillColor(jogadorGanhou ? Color(100, 255, 100) : Color(255, 100, 100));
    titulo.setStyle(Text::Bold);
    FloatRect tituloBounds = titulo.getLocalBounds();
    titulo.setPosition(Vector2f(450 - tituloBounds.size.x / 2 - tituloBounds.position.x, 185));
    window.draw(titulo);
    
    string textoMensagem = jogadorGanhou ? 
        "Parabens! Afundaste todos os navios inimigos!" : 
        "O computador afundou todos os teus navios!";
    Text mensagem(font, textoMensagem, 18);
    mensagem.setFillColor(Color(200, 200, 200));
    FloatRect msgBounds = mensagem.getLocalBounds();
    mensagem.setPosition(Vector2f(450 - msgBounds.size.x / 2 - msgBounds.position.x, 255));
    window.draw(mensagem);
    
    botaoJogarNovamente.desenhar(window);
    botaoSair.desenhar(window);
}
