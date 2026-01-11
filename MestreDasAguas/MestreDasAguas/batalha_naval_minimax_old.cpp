#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(lib, "sfml-audio.lib")
#endif

using namespace std;
using namespace sf;

const int TAM = 10;
const int NUM_NAVIOS = 8;
int TAMANHOS[NUM_NAVIOS] = { 5,4,4,3,3,2,2,2 };
const int CELL_SIZE = 35;
const int OFFSET_Y = 100;
const int SPACING = 50;

enum Estado { AGUA = 0, NAVIO = 1, ERRO = 2, ACERTO = 3 };
enum FaseJogo { MENU, COLOCACAO_NAVIOS, JOGANDO, FIM };

struct Tabuleiro { 
    int g[TAM][TAM]; 
    int tamanhoNavio[TAM][TAM]; // Armazena o tamanho do navio em cada célula
};

struct Coordenada { int x, y; };
vector<Coordenada> listaAdjacentes;

struct NavioArrastar {
    int tamanho;
    bool horizontal;
    bool colocado;
    Vector2f posicao;
    bool arrastando;
};

// Estrutura para armazenar texturas
struct Texturas {
    Texture agua;
    Texture navio2, navio3, navio4, navio5; // Diferentes tamanhos de navios
    Texture acerto;
    Texture erro;
    Texture iconeComSom;  // Ícone de música ligada
    Texture iconeSemSom;  // Ícone de música desligada
    bool aguaCarregada;
    bool navio2Carregado, navio3Carregado, navio4Carregado, navio5Carregado;
    bool acertoCarregada;
    bool erroCarregada;
    bool iconeComSomCarregado;
    bool iconeSemSomCarregado;
    bool carregadas; // Todas carregadas
    
    Texturas() : aguaCarregada(false), navio2Carregado(false), navio3Carregado(false), 
                 navio4Carregado(false), navio5Carregado(false), acertoCarregada(false), 
                 erroCarregada(false), iconeComSomCarregado(false), iconeSemSomCarregado(false), 
                 carregadas(false) {}
    
    bool carregar() {
        vector<string> caminhos = {
            "images/",
            "../images/",
            "../../images/",
            "../../../images/"
        };
        
        // Tenta carregar cada textura individualmente
        for (const auto& caminho : caminhos) {
            if (!aguaCarregada) aguaCarregada = agua.loadFromFile(caminho + "agua.png");
            if (!navio2Carregado) navio2Carregado = navio2.loadFromFile(caminho + "navio2.png");
            if (!navio3Carregado) navio3Carregado = navio3.loadFromFile(caminho + "navio3.png");
            if (!navio4Carregado) navio4Carregado = navio4.loadFromFile(caminho + "navio4.png");
            if (!navio5Carregado) navio5Carregado = navio5.loadFromFile(caminho + "navio5.png");
            if (!acertoCarregada) acertoCarregada = acerto.loadFromFile(caminho + "acerto.png");
            if (!erroCarregada) erroCarregada = erro.loadFromFile(caminho + "erro.png");
            if (!iconeComSomCarregado) iconeComSomCarregado = iconeComSom.loadFromFile(caminho + "Com-som.png");
            if (!iconeSemSomCarregado) iconeSemSomCarregado = iconeSemSom.loadFromFile(caminho + "Sem-som.png");
        }
        
        // Verifica se todas foram carregadas
        carregadas = aguaCarregada && navio2Carregado && navio3Carregado && navio4Carregado && 
                     navio5Carregado && acertoCarregada && erroCarregada;
        
        return carregadas;
    }
    
    // Função auxiliar para pegar textura do navio baseado no tamanho
    Texture* getNavioTexture(int tamanho) {
        switch(tamanho) {
            case 2: return navio2Carregado ? &navio2 : nullptr;
            case 3: return navio3Carregado ? &navio3 : nullptr;
            case 4: return navio4Carregado ? &navio4 : nullptr;
            case 5: return navio5Carregado ? &navio5 : nullptr;
            default: return nullptr;
        }
    }
};

// ----------------- FUNÇÕES BASE -----------------
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

// ----------------- JOGADAS DO COMPUTADOR -----------------
Coordenada escolherTiro(Tabuleiro& jog) {
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

void adicionarAdjacentes(Coordenada c, Tabuleiro& jog) {
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

void jogadaComputador(Tabuleiro& pc, Tabuleiro& jog) {
    Coordenada c = escolherTiro(jog);
    if (jog.g[c.x][c.y] == NAVIO) {
        jog.g[c.x][c.y] = ACERTO;
        adicionarAdjacentes(c, jog);
    }
    else {
        jog.g[c.x][c.y] = ERRO;
    }
}

// ----------------- FUNÇÕES GRÁFICAS -----------------
void desenharCelula(RenderWindow& window, int x, int y, Color cor, int offsetX, Texturas& texturas, 
       int estado, int tamanhoNavio, bool hover = false) {
RectangleShape cell(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
cell.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
bool usouTextura = false;
    
    switch (estado) {
        case AGUA:
            if (texturas.aguaCarregada) {
                cell.setTexture(&texturas.agua);
                cell.setFillColor(Color::White);
                usouTextura = true;
            }
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
                if (texturas.aguaCarregada) {
                    // Desenhar água como fundo
                    RectangleShape fundo(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                    fundo.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
                    fundo.setTexture(&texturas.agua);
                    fundo.setFillColor(Color::White);
                    
                    if (hover) {
                        fundo.setOutlineColor(Color::Yellow);
                        fundo.setOutlineThickness(2);
                    } else {
                        fundo.setOutlineColor(Color(30, 30, 50));
                        fundo.setOutlineThickness(1);
                    }
                    
                    window.draw(fundo);
                }
                
                cell.setTexture(&texturas.acerto);
                cell.setFillColor(Color::White);
                usouTextura = true;
            }
            break;
            
        case ERRO:
            if (texturas.erroCarregada) {
                if (texturas.aguaCarregada) {
                    // Desenhar água como fundo
                    RectangleShape fundo(Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
                    fundo.setPosition(Vector2f(x * CELL_SIZE + offsetX + 1, y * CELL_SIZE + OFFSET_Y + 1));
                    fundo.setTexture(&texturas.agua);
                    fundo.setFillColor(Color::White);
                    
                    if (hover) {
                        fundo.setOutlineColor(Color::Yellow);
                        fundo.setOutlineThickness(2);
                    } else {
                        fundo.setOutlineColor(Color(30, 30, 50));
                        fundo.setOutlineThickness(1);
                    }
                    
                    window.draw(fundo);
                }
                
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
    
    // Usar textura específica do tamanho se disponível
    Texture* navioTex = texturas.getNavioTexture(navio.tamanho);
    if (navioTex && !navio.colocado) {
        // Criar shape com largura/altura da imagem original (horizontal)
        RectangleShape texShape(Vector2f(navio.tamanho * CELL_SIZE - 4, CELL_SIZE - 4));
        texShape.setTexture(navioTex);
        texShape.setFillColor(cor);
        
        if (navio.horizontal) {
            // Horizontal: posição normal
            texShape.setPosition(navio.posicao);
        } else {
            // Vertical: rodar 90 graus
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

void desenharTabuleiro(RenderWindow& window, Tabuleiro& tabuleiro, int offsetX, const string& titulo, 
          bool mostrarNavios, Texturas& texturas, Font& font, int hoverX = -1, int hoverY = -1) {
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
            
            // Se for um navio e mostrar navios, desenhar como peça única
            if (estado == NAVIO && mostrarNavios && !navioCelulasDesenhadas[i][j] && tamanhoNavio >= 2) {
                // Descobrir orientação do navio (horizontal ou vertical)
                // i é coluna (x), j é linha (y)
                // Horizontal: próxima coluna tem navio (i+1)
                // Vertical: próxima linha tem navio (j+1)
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

void desenharInfo(RenderWindow& window, int naviosJogador, int naviosPC, const string& mensagem, Font& font) {
int yPos = OFFSET_Y + TAM * CELL_SIZE + 20;
    
desenharTexto(window, "Teus navios: " + to_string(naviosJogador), 80, yPos, 18, Color::Green, font);
desenharTexto(window, "Navios PC: " + to_string(naviosPC), 510, yPos, 18, Color::Red, font);
    
    if (!mensagem.empty()) {
        desenharTexto(window, mensagem, 20, yPos + 30, 16, Color::Yellow, font);
    }
}

// Estrutura do botão
struct Botao {
    RectangleShape shape;
    Text texto;
    bool hover;
    
    Botao(Vector2f posicao, Vector2f tamanho, const string& textoStr, Font& font) 
        : texto(font, textoStr, 28) {
        shape.setSize(tamanho);
        shape.setPosition(posicao);
        shape.setFillColor(Color(60, 80, 120));
        shape.setOutlineColor(Color(100, 150, 200));
        shape.setOutlineThickness(3);
        
        texto.setFillColor(Color::White);
        
        FloatRect textBounds = texto.getLocalBounds();
        texto.setPosition(Vector2f(
            posicao.x + (tamanho.x - textBounds.size.x) / 2 - textBounds.position.x,
            posicao.y + (tamanho.y - textBounds.size.y) / 2 - textBounds.position.y
        ));
        
        hover = false;
    }
    
    void atualizar(Vector2i mousePos) {
        bool dentroX = mousePos.x >= shape.getPosition().x && 
                      mousePos.x <= shape.getPosition().x + shape.getSize().x;
        bool dentroY = mousePos.y >= shape.getPosition().y && 
                      mousePos.y <= shape.getPosition().y + shape.getSize().y;
        
        hover = dentroX && dentroY;
        
        if (hover) {
            shape.setFillColor(Color(80, 120, 180));
            shape.setOutlineColor(Color(150, 200, 255));
            shape.setOutlineThickness(4);
        } else {
            shape.setFillColor(Color(60, 80, 120));
            shape.setOutlineColor(Color(100, 150, 200));
            shape.setOutlineThickness(3);
        }
    }
    
    bool foiClicado(Vector2i mousePos) {
        return hover;
    }
    
    void desenhar(RenderWindow& window) {
        window.draw(shape);
        window.draw(texto);
    }
};

// Botão de música com ícone
struct BotaoMusica {
    CircleShape shape;
    bool hover;
    bool ativo;
    Texture* texturaComSom;
    Texture* texturaSemSom;
    
    BotaoMusica(Vector2f posicao, float raio, Texture* texComSom = nullptr, Texture* texSemSom = nullptr) 
        : ativo(false), hover(false), texturaComSom(texComSom), texturaSemSom(texSemSom) {
        shape.setRadius(raio);
        shape.setPosition(posicao);
        shape.setFillColor(Color(60, 80, 120));
        shape.setOutlineColor(Color(100, 150, 200));
        shape.setOutlineThickness(2);
    }
    
    void atualizar(Vector2i mousePos) {
        Vector2f centro(shape.getPosition().x + shape.getRadius(), 
                       shape.getPosition().y + shape.getRadius());
        float dx = mousePos.x - centro.x;
        float dy = mousePos.y - centro.y;
        float distancia = sqrt(dx * dx + dy * dy);
        
        hover = distancia <= shape.getRadius();
        
        if (hover) {
            shape.setFillColor(Color(80, 120, 180));
            shape.setOutlineColor(Color(150, 200, 255));
            shape.setOutlineThickness(3);
        } else {
            if (ativo) {
                shape.setFillColor(Color(50, 150, 80));
            } else {
                shape.setFillColor(Color(60, 80, 120));
            }
            shape.setOutlineColor(Color(100, 150, 200));
            shape.setOutlineThickness(2);
        }
    }
    
    bool foiClicado() {
        return hover;
    }
    
    void desenhar(RenderWindow& window) {
        window.draw(shape);
        
        Vector2f centro(shape.getPosition().x + shape.getRadius(), 
                       shape.getPosition().y + shape.getRadius());
        float raio = shape.getRadius();
        
        Texture* texturaAtual = ativo ? texturaComSom : texturaSemSom;
        
        if (texturaAtual) {
            Sprite icone(*texturaAtual);
            
            Vector2u texSize = texturaAtual->getSize();
            float escala = (raio * 1.6f) / max(texSize.x, texSize.y);
            icone.setScale(Vector2f(escala, escala));
            
            FloatRect bounds = icone.getLocalBounds();
            icone.setOrigin(Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
            icone.setPosition(centro);
            
            window.draw(icone);
        } else {
            RectangleShape haste(Vector2f(2.5f, raio * 1.3f));
            haste.setPosition(Vector2f(centro.x + raio * 0.15f, centro.y - raio * 0.4f));
            haste.setFillColor(Color::White);
            window.draw(haste);
            
            CircleShape cabeca(raio * 0.35f);
            cabeca.setScale(Vector2f(1.3f, 1.0f));
            cabeca.setPosition(Vector2f(centro.x - raio * 0.45f, centro.y + raio * 0.05f));
            cabeca.setFillColor(Color::White);
            window.draw(cabeca);
            
            ConvexShape bandeira;
            bandeira.setPointCount(4);
            bandeira.setPoint(0, Vector2f(centro.x + raio * 0.15f, centro.y - raio * 0.4f));
            bandeira.setPoint(1, Vector2f(centro.x + raio * 0.55f, centro.y - raio * 0.15f));
            bandeira.setPoint(2, Vector2f(centro.x + raio * 0.55f, centro.y - raio * 0.05f));
            bandeira.setPoint(3, Vector2f(centro.x + raio * 0.15f, centro.y - raio * 0.25f));
            bandeira.setFillColor(Color::White);
            window.draw(bandeira);
            
            if (!ativo) {
                RectangleShape linha1(Vector2f(raio * 1.8f, 3));
                linha1.setPosition(Vector2f(centro.x - raio * 0.65f, centro.y));
                linha1.setRotation(degrees(45));
                linha1.setFillColor(Color(255, 80, 80));
                window.draw(linha1);
                
                RectangleShape linha2(Vector2f(raio * 1.8f, 3));
                linha2.setPosition(Vector2f(centro.x - raio * 0.65f, centro.y));
                linha2.setRotation(degrees(-45));
                linha2.setFillColor(Color(255, 80, 80));
                window.draw(linha2);
            }
        }
    }
};


// Desenhar overlay de fim de jogo
void desenharFimDeJogo(RenderWindow& window, bool jogadorGanhou, Font& font, Botao& botaoJogarNovamente, Botao& botaoSair) {
RectangleShape overlay(Vector2f(900, 600));
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
    
    // Mensagem
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


// ----------------- MAIN GRÁFICO -----------------
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
    
    Music musicaFundo;
    bool musicaCarregada = false;
    
    if (musicaFundo.openFromFile("C:/Users/Admin/Downloads/teste/teste/teste/audio/musica.mp3")) {
        musicaCarregada = true;
        musicaFundo.setLooping(true);
        musicaFundo.setVolume(50);
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
                musicaFundo.setVolume(50);
                break;
            }
        }
    }
    
    colocarNaviosPC(pc);

    RenderWindow window(VideoMode({900, 600}), "Batalha Naval - # Mestre das Águas #");
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
                            for (int i = 0; i < navios.size(); i++) {
                                if (!navios[i].colocado) {
                                    float largura = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4 : CELL_SIZE - 4;
                                    float altura = navios[i].horizontal ? CELL_SIZE - 4 : navios[i].tamanho * CELL_SIZE - 4;
                                    
                                    bool dentroX = mousePos.x >= navios[i].posicao.x && mousePos.x <= navios[i].posicao.x + largura;
                                    bool dentroY = mousePos.y >= navios[i].posicao.y && mousePos.y <= navios[i].posicao.y + altura;
                                    
                                    if (dentroX && dentroY) {
                                        navioSelecionado = i;
                                        navios[i].arrastando = true;
                                        offsetArrastar = Vector2f(mousePos.x - navios[i].posicao.x, 
                                                                 mousePos.y - navios[i].posicao.y);
                                        break;
                                    }
                                }
                            }
                        } else if (mouseEvent->button == Mouse::Button::Right) {
                            for (int i = 0; i < navios.size(); i++) {
                                if (!navios[i].colocado) {
                                    float larguraAtual = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4 : CELL_SIZE - 4;
                                    float alturaAtual = navios[i].horizontal ? CELL_SIZE - 4 : navios[i].tamanho * CELL_SIZE - 4;
                                    
                                    bool dentroX = mousePos.x >= navios[i].posicao.x && mousePos.x <= navios[i].posicao.x + larguraAtual;
                                    bool dentroY = mousePos.y >= navios[i].posicao.y && mousePos.y <= navios[i].posicao.y + alturaAtual;
                                    
                                    if (dentroX && dentroY) {
                                        float centroX = navios[i].posicao.x + larguraAtual / 2.0f;
                                        float centroY = navios[i].posicao.y + alturaAtual / 2.0f;
                                        
                                        navios[i].horizontal = !navios[i].horizontal;
                                        
                                        float novaLargura = navios[i].horizontal ? navios[i].tamanho * CELL_SIZE - 4 : CELL_SIZE - 4;
                                        float novaAltura = navios[i].horizontal ? CELL_SIZE - 4 : navios[i].tamanho * CELL_SIZE - 4;
                                        
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
                                
                                navios[navioSelecionado].posicao = Vector2f(
                                    offsetX + gridX * CELL_SIZE + 1, 
                                    OFFSET_Y + gridY * CELL_SIZE + 1
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
                                    mensagem = "ACERTASTE!";
                                } else {
                                    tirosPC.g[x][y] = ERRO;
                                    mensagem = "ERROU!";
                                }
                                
                                if (contarRestantes(pc) == 0) {
                                    mensagem = "GANHASTE! Afundaste todos os navios!";
                                    jogadorGanhou = true;
                                    fase = FIM;
                                } else {
                                    jogadaComputador(pc, jogador);
                                    
                                    if (contarRestantes(jogador) == 0) {
                                        mensagem = "PERDESTE! O PC afundou todos os teus navios!";
                                        jogadorGanhou = false;
                                        fase = FIM;
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
                            listaAdjacentes.clear();
                            
                            for (int i = 0; i < NUM_NAVIOS; i++) {
                                navios[i].colocado = false;
                                navios[i].arrastando = false;
                                navios[i].horizontal = true;
                                navios[i].posicao = Vector2f(50 + (i % 4) * 180, startY + (i / 4) * 50);
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
            titulo.setPosition(Vector2f((900 - tituloBounds.size.x) / 2 - tituloBounds.position.x, 100));
            window.draw(titulo);
            
            Text subtitulo(font, "Mestre das Águas", 24);
            subtitulo.setFillColor(Color(150, 180, 220));
            FloatRect subtituloBounds = subtitulo.getLocalBounds();
            subtitulo.setPosition(Vector2f((900 - subtituloBounds.size.x) / 2 - subtituloBounds.position.x, 175));
            window.draw(subtitulo);
            
            botaoJogar.desenhar(window);
            botaoSair.desenhar(window);
            
            Text instrucoes(font, "Arrasta navios | Clique direito para rodar", 16);
            instrucoes.setFillColor(Color(120, 140, 180));
            FloatRect instBounds = instrucoes.getLocalBounds();
            instrucoes.setPosition(Vector2f((900 - instBounds.size.x) / 2 - instBounds.position.x, 520));
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
            
            for (int i = 0; i < navios.size(); i++) {
                if (!navios[i].colocado || navios[i].arrastando) {
                    desenharNavioArrastar(window, navios[i], i, 
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
