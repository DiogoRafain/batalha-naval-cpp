#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <memory>

using namespace sf;
using namespace std;

struct Sons {
    SoundBuffer bufferAcerto;
    SoundBuffer bufferErro;
    SoundBuffer bufferVitoria;
    SoundBuffer bufferDerrota;
    SoundBuffer bufferColocarNavio;
    
    unique_ptr<Sound> somAcerto;
    unique_ptr<Sound> somErro;
    unique_ptr<Sound> somVitoria;
    unique_ptr<Sound> somDerrota;
    unique_ptr<Sound> somColocarNavio;
    
    bool acertoCarregado;
    bool erroCarregado;
    bool vitoriaCarregado;
    bool derrotaCarregado;
    bool colocarNavioCarregado;
    
    bool ativo;
    
    Sons();
    bool carregar();
    void tocarAcerto();
    void tocarErro();
    void tocarVitoria();
    void tocarDerrota();
    void tocarColocarNavio();
    void setVolume(float volume);
};

