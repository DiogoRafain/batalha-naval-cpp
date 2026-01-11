#include "Sons.h"

Sons::Sons() : acertoCarregado(false), erroCarregado(false), vitoriaCarregado(false),
derrotaCarregado(false), colocarNavioCarregado(false), ativo(true) {
}

bool Sons::carregar() {
    vector<string> caminhos = {
        "C:/Users/Admin/Downloads/teste/teste/teste/audio/",
        "audio/",
        "../audio/",
        "../../audio/",
        "../../../audio/",
        "teste/audio/",
        "../teste/audio/"
    };

    // Tentar carregar primeiro como MP3, depois como WAV
    for (const auto& caminho : caminhos) {
        if (!acertoCarregado) {
            if (bufferAcerto.loadFromFile(caminho + "acerto.mp3") ||
                bufferAcerto.loadFromFile(caminho + "acerto.wav")) {
                acertoCarregado = true;
                somAcerto = make_unique<Sound>(bufferAcerto);
            }
        }
        if (!erroCarregado) {
            if (bufferErro.loadFromFile(caminho + "erro.mp3") ||
                bufferErro.loadFromFile(caminho + "erro.wav")) {
                erroCarregado = true;
                somErro = make_unique<Sound>(bufferErro);
            }
        }
        if (!vitoriaCarregado) {
            if (bufferVitoria.loadFromFile(caminho + "vitoria.mp3") ||
                bufferVitoria.loadFromFile(caminho + "vitoria.wav")) {
                vitoriaCarregado = true;
                somVitoria = make_unique<Sound>(bufferVitoria);
            }
        }
        if (!derrotaCarregado) {
            if (bufferDerrota.loadFromFile(caminho + "derrota.mp3") ||
                bufferDerrota.loadFromFile(caminho + "derrota.wav")) {
                derrotaCarregado = true;
                somDerrota = make_unique<Sound>(bufferDerrota);
            }
        }
        if (!colocarNavioCarregado) {
            if (bufferColocarNavio.loadFromFile(caminho + "colocar.mp3") ||
                bufferColocarNavio.loadFromFile(caminho + "colocar.wav")) {
                colocarNavioCarregado = true;
                somColocarNavio = make_unique<Sound>(bufferColocarNavio);
            }
        }
    }

    setVolume(30.0f);

    return true;
}

void Sons::tocarAcerto() {
    if (ativo && acertoCarregado && somAcerto) {
        somAcerto->play();
    }
}

void Sons::tocarErro() {
    if (ativo && erroCarregado && somErro) {
        somErro->play();
    }
}

void Sons::tocarVitoria() {
    if (ativo && vitoriaCarregado && somVitoria) {
        somVitoria->play();
    }
}

void Sons::tocarDerrota() {
    if (ativo && derrotaCarregado && somDerrota) {
        somDerrota->play();
    }
}

void Sons::tocarColocarNavio() {
    if (ativo && colocarNavioCarregado && somColocarNavio) {
        somColocarNavio->play();
    }
}

void Sons::setVolume(float volume) {
    if (somAcerto) somAcerto->setVolume(volume);
    if (somErro) somErro->setVolume(volume);
    if (somVitoria) somVitoria->setVolume(volume);
    if (somDerrota) somDerrota->setVolume(volume);
    if (somColocarNavio) somColocarNavio->setVolume(volume);
}