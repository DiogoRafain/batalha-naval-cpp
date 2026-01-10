#include "Texturas.h"

Texturas::Texturas() : aguaCarregada(false), navio2Carregado(false), navio3Carregado(false), 
             navio4Carregado(false), navio5Carregado(false), acertoCarregada(false), 
             erroCarregada(false), iconeComSomCarregado(false), iconeSemSomCarregado(false), 
             carregadas(false) {}

bool Texturas::carregar() {
    vector<string> caminhos = {
        "images/",
        "../images/",
        "../../images/",
        "../../../images/"
    };
    
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
    
    carregadas = aguaCarregada && navio2Carregado && navio3Carregado && navio4Carregado && 
                 navio5Carregado && acertoCarregada && erroCarregada;
    
    return carregadas;
}

Texture* Texturas::getNavioTexture(int tamanho) {
    switch(tamanho) {
        case 2: return navio2Carregado ? &navio2 : nullptr;
        case 3: return navio3Carregado ? &navio3 : nullptr;
        case 4: return navio4Carregado ? &navio4 : nullptr;
        case 5: return navio5Carregado ? &navio5 : nullptr;
        default: return nullptr;
    }
}
