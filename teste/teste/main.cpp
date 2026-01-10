#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <cstdlib>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(lib, "sfml-audio.lib")
#endif

#include "Constantes.h"
#include "Estruturas.h"
#include "Texturas.h"
#include "Sons.h"
#include "LogicaJogo.h"
#include "Botoes.h"
#include "Renderizacao.h"

using namespace std;
using namespace sf;

int TAMANHOS[NUM_NAVIOS] = { 5,4,4,3,3,2,2,2 };

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#else
int main() {
#endif
    srand(time(NULL));
    
    Tabuleiro jogador, pc, tirosPC;
    inicializar(jogador);
    inicializar(pc);
    inicializar(tirosPC);
    
    Texturas texturas;
    texturas.carregar();
    
    Sons sons;
    sons.carregar();
    
    Music musicaFundo;
    bool musicaCarregada = false;
    
    if (musicaFundo.openFromFile("C:/Users/Admin/Downloads/teste/teste/teste/audio/musica.mp3")) {
        musicaCarregada = true;
        musicaFundo.setLooping(true);
        musicaFundo.setVolume(5);
    } else {
        vector<string> caminhosMusicais = {
            "teste/audio/musica.mp3",
            "audio/musica.mp3",
            "../teste/audio/musica.mp3",
            "../audio/musica.mp3",
            "../../teste/audio/musica.mp3"
        };
        
        for (const auto& caminho : caminhosMusicais) {
            if (musicaFundo.openFromFile(caminho)) {
                musicaCarregada = true;
                musicaFundo.setLooping(true);
                musicaFundo.setVolume(20);
                break;
            }
        }
    }
    
    colocarNaviosPC(pc);

    RenderWindow window(VideoMode({LARGURA_JANELA, ALTURA_JANELA}), "Batalha Naval - # Mestre das Águas #");
    window.setFramerateLimit(60);
    
    Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return 1;
    }
    
    FaseJogo fase = MENU;
    string mensagem = "Arrasta os navios para o tabuleiro! Clique direito para rodar.";
    int hoverX = -1, hoverY = -1;
    bool jogadorGanhou = false;
    
    Botao botaoJogar(Vector2f(350, 275), Vector2f(200, 60), "JOGAR", font);
    Botao botaoSair(Vector2f(350, 355), Vector2f(200, 60), "SAIR", font);
    
    Botao botaoJogarNovamente(Vector2f(300, 305), Vector2f(300, 60), "JOGAR NOVAMENTE", font);
    Botao botaoSairFim(Vector2f(300, 380), Vector2f(300, 60), "SAIR", font);
    
    BotaoMusica botaoMusica(Vector2f(830, 10), 25,
                           texturas.iconeComSomCarregado ? &texturas.iconeComSom : nullptr,
                           texturas.iconeSemSomCarregado ? &texturas.iconeSemSom : nullptr);
    
    if (musicaCarregada) {
        botaoMusica.ativo = true;
        musicaFundo.play();
    }

    vector<NavioArrastar> navios;
    int startY = 450;
    for (int i = 0; i < NUM_NAVIOS; i++) {
        NavioArrastar navio;
        navio.tamanho = TAMANHOS[i];
        navio.horizontal = true;
        navio.colocado = false;
        navio.arrastando = false;
        navio.posicao = Vector2f(50 + (i % 4) * 180, startY + (i / 4) * 50);
        navios.push_back(navio);
    }
    
    int navioSelecionado = -1;
    Vector2f offsetArrastar;
    
    IA ia;

    while (window.isOpen()) {
        Vector2i mousePos = Mouse::getPosition(window);
        
        botaoMusica.atualizar(mousePos);
        
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
            
            if (event->is<Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<Event::MouseButtonPressed>();
                if (mouseEvent && mouseEvent->button == Mouse::Button::Left && botaoMusica.foiClicado()) {
                    botaoMusica.ativo = !botaoMusica.ativo;
                    sons.ativo = botaoMusica.ativo;
                    if (musicaCarregada) {
                        botaoMusica.ativo ? musicaFundo.play() : musicaFundo.pause();
                    }
                }
            }
            
            if (fase == MENU) {
                botaoJogar.atualizar(mousePos);
                botaoSair.atualizar(mousePos);
                
                if (event->is<Event::MouseButtonPressed>()) {
                    auto mouseEvent = event->getIf<Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == Mouse::Button::Left) {
                        if (botaoJogar.foiClicado(mousePos)) {
                            fase = COLOCACAO_NAVIOS;
                            mensagem = "Arrasta os navios para o tabuleiro! Clique direito para rodar.";
                        } else if (botaoSair.foiClicado(mousePos)) {
                            window.close();
                        }
                    }
                }
            }
            
            else if (fase == COLOCACAO_NAVIOS) {
                if (event->is<Event::MouseButtonPressed>()) {
                    auto mouseEvent = event->getIf<Event::MouseButtonPressed>();
                    if (mouseEvent) {
                        if (mouseEvent->button == Mouse::Button::Left) {
                            for (size_t i = 0; i < navios.size(); i++) {
                                if (!navios[i].colocado) {
                                    float largura = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4.0f : CELL_SIZE - 4.0f;
                                    float altura = navios[i].horizontal ? CELL_SIZE - 4.0f : navios[i].tamanho * CELL_SIZE - 4.0f;
                                    
                                    float mousePosX = static_cast<float>(mousePos.x);
                                    float mousePosY = static_cast<float>(mousePos.y);
                                    
                                    bool dentroX = mousePosX >= navios[i].posicao.x && mousePosX <= navios[i].posicao.x + largura;
                                    bool dentroY = mousePosY >= navios[i].posicao.y && mousePosY <= navios[i].posicao.y + altura;
                                    
                                    if (dentroX && dentroY) {
                                        navioSelecionado = static_cast<int>(i);
                                        navios[i].arrastando = true;
                                        offsetArrastar = Vector2f(mousePosX - navios[i].posicao.x, 
                                                                 mousePosY - navios[i].posicao.y);
                                        break;
                                    }
                                }
                            }
                        } else if (mouseEvent->button == Mouse::Button::Right) {
                            for (size_t i = 0; i < navios.size(); i++) {
                                if (!navios[i].colocado) {
                                    float larguraAtual = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4.0f : CELL_SIZE - 4.0f;
                                    float alturaAtual = navios[i].horizontal ? CELL_SIZE - 4.0f : navios[i].tamanho * CELL_SIZE - 4.0f;
                                    
                                    bool dentroX = mousePos.x >= navios[i].posicao.x && mousePos.x <= navios[i].posicao.x + larguraAtual;
                                    bool dentroY = mousePos.y >= navios[i].posicao.y && mousePos.y <= navios[i].posicao.y + alturaAtual;
                                    
                                    if (dentroX && dentroY) {
                                        float centroX = navios[i].posicao.x + larguraAtual / 2.0f;
                                        float centroY = navios[i].posicao.y + alturaAtual / 2.0f;
                                        
                                        navios[i].horizontal = !navios[i].horizontal;
                                        
                                        float novaLargura = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4.0f : CELL_SIZE - 4.0f;
                                        float novaAltura = navios[i].horizontal ? CELL_SIZE - 4.0f : navios[i].tamanho * CELL_SIZE - 4.0f;
                                        
                                        navios[i].posicao.x = centroX - novaLargura / 2.0f;
                                        navios[i].posicao.y = centroY - novaAltura / 2.0f;
                                        
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (event->is<Event::MouseButtonReleased>()) {
                    auto mouseEvent = event->getIf<Event::MouseButtonReleased>();
                    if (mouseEvent && mouseEvent->button == Mouse::Button::Left && navioSelecionado != -1) {
                        int offsetX = 80;
                        if (mousePos.x >= offsetX && mousePos.x < offsetX + TAM * CELL_SIZE &&
                            mousePos.y >= OFFSET_Y && mousePos.y < OFFSET_Y + TAM * CELL_SIZE) {
                            
                            int gridX = (mousePos.x - offsetX) / CELL_SIZE;
                            int gridY = (mousePos.y - OFFSET_Y) / CELL_SIZE;
                            
                            if (podeColocar(jogador, gridX, gridY, navios[navioSelecionado].tamanho, 
                                          navios[navioSelecionado].horizontal)) {
                                colocarNavio(jogador, gridX, gridY, navios[navioSelecionado].tamanho, 
                                           navios[navioSelecionado].horizontal);
                                navios[navioSelecionado].colocado = true;
                                
                                sons.tocarColocarNavio();
                                
                                navios[navioSelecionado].posicao = Vector2f(
                                    offsetX + gridX * CELL_SIZE + 1.0f, 
                                    OFFSET_Y + gridY * CELL_SIZE + 1.0f
                                );
                                
                                bool todosColocados = true;
                                for (auto& n : navios) {
                                    if (!n.colocado) {
                                        todosColocados = false;
                                        break;
                                    }
                                }
                                if (todosColocados) {
                                    fase = JOGANDO;
                                    mensagem = "Clica no tabuleiro da direita para atacar!";
                                }
                            }
                        }
                        navios[navioSelecionado].arrastando = false;
                        navioSelecionado = -1;
                    }
                }
            }
            
            else if (fase == JOGANDO) {
                if (event->is<Event::MouseButtonPressed>()) {
                    auto mouseEvent = event->getIf<Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == Mouse::Button::Left) {
                        int offsetX = 460 + SPACING;
                        if (mousePos.x >= offsetX && mousePos.x < offsetX + TAM * CELL_SIZE &&
                            mousePos.y >= OFFSET_Y && mousePos.y < OFFSET_Y + TAM * CELL_SIZE) {
                            
                            int x = (mousePos.x - offsetX) / CELL_SIZE;
                            int y = (mousePos.y - OFFSET_Y) / CELL_SIZE;
                            
                            if (tirosPC.g[x][y] == ACERTO || tirosPC.g[x][y] == ERRO) {
                                mensagem = "Ja atiraste ai!";
                            } else {
                                if (pc.g[x][y] == NAVIO) {
                                    pc.g[x][y] = ACERTO;
                                    tirosPC.g[x][y] = ACERTO;
                                    sons.tocarAcerto();
                                } else {
                                    tirosPC.g[x][y] = ERRO;
                                    sons.tocarErro();
                                }
                                
                                if (contarRestantes(pc) == 0) {
                                    mensagem = "GANHASTE! Afundaste todos os navios!";
                                    jogadorGanhou = true;
                                    fase = FIM;
                                    sons.tocarVitoria();
                                } else {
                                    ia.jogadaComputador(pc, jogador);
                                    
                                    if (contarRestantes(jogador) == 0) {
                                        mensagem = "PERDESTE! O PC afundou todos os teus navios!";
                                        jogadorGanhou = false;
                                        fase = FIM;
                                        sons.tocarDerrota();
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (event->is<Event::MouseMoved>()) {
                    auto mouseEvent = event->getIf<Event::MouseMoved>();
                    if (mouseEvent) {
                        Vector2i pos(mouseEvent->position.x, mouseEvent->position.y);
                        int offsetX = 460 + SPACING;
                        
                        if (pos.x >= offsetX && pos.x < offsetX + TAM * CELL_SIZE &&
                            pos.y >= OFFSET_Y && pos.y < OFFSET_Y + TAM * CELL_SIZE) {
                            hoverX = (pos.x - offsetX) / CELL_SIZE;
                            hoverY = (pos.y - OFFSET_Y) / CELL_SIZE;
                        } else {
                            hoverX = -1;
                            hoverY = -1;
                        }
                    }
                }
            }
            
            else if (fase == FIM) {
                botaoJogarNovamente.atualizar(mousePos);
                botaoSairFim.atualizar(mousePos);
                
                if (event->is<Event::MouseButtonPressed>()) {
                    auto mouseEvent = event->getIf<Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == Mouse::Button::Left) {
                        if (botaoJogarNovamente.foiClicado(mousePos)) {
                            inicializar(jogador);
                            inicializar(pc);
                            inicializar(tirosPC);
                            colocarNaviosPC(pc);
                            ia.limpar();
                            
                            for (int i = 0; i < NUM_NAVIOS; i++) {
                                navios[i].colocado = false;
                                navios[i].arrastando = false;
                                navios[i].horizontal = true;
                                navios[i].posicao = Vector2f(50 + (i % 4) * 180.0f, startY + (i / 4) * 50.0f);
                            }
                            
                            navioSelecionado = -1;
                            fase = COLOCACAO_NAVIOS;
                            mensagem = "Arrasta os navios para o tabuleiro! Clique direito para rodar.";
                            jogadorGanhou = false;
                        } else if (botaoSairFim.foiClicado(mousePos)) {
                            window.close();
                        }
                    }
                }
            }
        }
        
        if (navioSelecionado != -1 && navios[navioSelecionado].arrastando) {
            navios[navioSelecionado].posicao = Vector2f(
                static_cast<float>(mousePos.x) - offsetArrastar.x, 
                static_cast<float>(mousePos.y) - offsetArrastar.y
            );
        }

        window.clear(Color(20, 30, 50));
        
        if (fase == MENU) {
            Text titulo(font, "BATALHA NAVAL", 56);
            titulo.setFillColor(Color(100, 200, 255));
            titulo.setStyle(Text::Bold);
            FloatRect tituloBounds = titulo.getLocalBounds();
            titulo.setPosition(Vector2f((LARGURA_JANELA - tituloBounds.size.x) / 2 - tituloBounds.position.x, 100));
            window.draw(titulo);
            
            Text subtitulo(font, "Mestre das Águas", 24);
            subtitulo.setFillColor(Color(150, 180, 220));
            FloatRect subtituloBounds = subtitulo.getLocalBounds();
            subtitulo.setPosition(Vector2f((LARGURA_JANELA - subtituloBounds.size.x) / 2 - subtituloBounds.position.x, 175));
            window.draw(subtitulo);
            
            botaoJogar.desenhar(window);
            botaoSair.desenhar(window);
            
            Text instrucoes(font, "Arrasta navios | Clique direito para rodar", 16);
            instrucoes.setFillColor(Color(120, 140, 180));
            FloatRect instBounds = instrucoes.getLocalBounds();
            instrucoes.setPosition(Vector2f((LARGURA_JANELA - instBounds.size.x) / 2 - instBounds.position.x, 520));
            window.draw(instrucoes);
            
        } else if (fase == COLOCACAO_NAVIOS) {
            desenharTabuleiro(window, jogador, 80, "COLOCA OS TEUS NAVIOS", true, texturas, font);
            
            bool podeColocarAqui = false;
            if (navioSelecionado != -1) {
                int offsetX = 80;
                if (mousePos.x >= offsetX && mousePos.x < offsetX + TAM * CELL_SIZE &&
                    mousePos.y >= OFFSET_Y && mousePos.y < OFFSET_Y + TAM * CELL_SIZE) {
                    int gridX = (mousePos.x - offsetX) / CELL_SIZE;
                    int gridY = (mousePos.y - OFFSET_Y) / CELL_SIZE;
                    podeColocarAqui = podeColocar(jogador, gridX, gridY, 
                                                  navios[navioSelecionado].tamanho,
                                                  navios[navioSelecionado].horizontal);
                }
            }
            
            for (size_t i = 0; i < navios.size(); i++) {
                if (!navios[i].colocado || navios[i].arrastando) {
                    desenharNavioArrastar(window, navios[i], static_cast<int>(i), 
                                        i == navioSelecionado ? podeColocarAqui : false, texturas, font);
                }
            }
            
            desenharTexto(window, mensagem, 80, OFFSET_Y + TAM * CELL_SIZE + 20, 16, Color::Yellow, font);
            
        } else if (fase == JOGANDO || fase == FIM) {
            desenharTabuleiro(window, jogador, 80, "TEU TABULEIRO", true, texturas, font);
            desenharTabuleiro(window, tirosPC, 460 + SPACING, "TABULEIRO INIMIGO", false, texturas, font, hoverX, hoverY);
            desenharInfo(window, contarRestantes(jogador), contarRestantes(pc), mensagem, font);
            
            if (fase == FIM) {
                desenharFimDeJogo(window, jogadorGanhou, font, botaoJogarNovamente, botaoSairFim);
            }
        }
        
        botaoMusica.desenhar(window);
        
        window.display();
    }

    return 0;
}