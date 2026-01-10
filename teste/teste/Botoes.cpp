#include "Botoes.h"
#include <cmath>
#include <algorithm>

Botao::Botao(Vector2f posicao, Vector2f tamanho, const string& textoStr, Font& font) 
    : texto(font, textoStr, 28), hover(false) {
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
}

void Botao::atualizar(Vector2i mousePos) {
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

bool Botao::foiClicado(Vector2i mousePos) {
    return hover;
}

void Botao::desenhar(RenderWindow& window) {
    window.draw(shape);
    window.draw(texto);
}

BotaoMusica::BotaoMusica(Vector2f posicao, float raio, Texture* texComSom, Texture* texSemSom) 
    : ativo(false), hover(false), texturaComSom(texComSom), texturaSemSom(texSemSom) {
    shape.setRadius(raio);
    shape.setPosition(posicao);
    shape.setFillColor(Color(60, 80, 120));
    shape.setOutlineColor(Color(100, 150, 200));
    shape.setOutlineThickness(2);
}

void BotaoMusica::atualizar(Vector2i mousePos) {
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

bool BotaoMusica::foiClicado() {
    return hover;
}

void BotaoMusica::desenhar(RenderWindow& window) {
    window.draw(shape);
    
    Vector2f centro(shape.getPosition().x + shape.getRadius(), 
                   shape.getPosition().y + shape.getRadius());
    float raio = shape.getRadius();
    
    Texture* texturaAtual = ativo ? texturaComSom : texturaSemSom;
    
    if (texturaAtual) {
        Sprite icone(*texturaAtual);
        
        Vector2u texSize = texturaAtual->getSize();
        float escala = (raio * 1.6f) / std::max(texSize.x, texSize.y);
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
