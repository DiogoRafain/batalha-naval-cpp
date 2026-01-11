#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class Botao {
private:
    RectangleShape shape;
    Text texto;
    bool hover;
    
public:
    Botao(Vector2f posicao, Vector2f tamanho, const string& textoStr, Font& font);
    void atualizar(Vector2i mousePos);
    bool foiClicado(Vector2i mousePos);
    void desenhar(RenderWindow& window);
};

class BotaoMusica {
private:
    CircleShape shape;
    bool hover;
    Texture* texturaComSom;
    Texture* texturaSemSom;
    
public:
    bool ativo;
    
    BotaoMusica(Vector2f posicao, float raio, Texture* texComSom = nullptr, Texture* texSemSom = nullptr);
    void atualizar(Vector2i mousePos);
    bool foiClicado();
    void desenhar(RenderWindow& window);
};
