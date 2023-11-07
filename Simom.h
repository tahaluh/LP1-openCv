#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

class Simom {
  public:
    Simom() {
        srand(time(0)); // Inicializa a semente para números aleatórios
    }

    void iniciarJogo() {
        sequence.clear();
        pontos = 0;
        adicionarCorAleatoria();
    }

    int realizaJogada(int jogada) {
        this->entradaJogador.push_back(jogada);

        if (this->verificaJogada()) {
            if (this->entradaJogador.size() == this->sequence.size()) { // acertou a sequência toda
                this->entradaJogador.clear();
                this->adicionarCorAleatoria();
                this->pontos++;

                if (this->pontos > this->highscore) {
                    this->highscore = this->pontos;
                }

                return 1;
            } else { // acertou parte da sequência
                return 0;
            }
        } else { // errou
            return -1;
        }
    }

    int getPontos() {
        return pontos;
    }

    int getHighscore() {
        return highscore;
    }

    std::vector<int> getSequencia() {
        return sequence;
    }

  private:
    std::vector<int> sequence;
    int pontos = 0;
    int highscore = 0;

    void adicionarCorAleatoria() {
        int corAleatoria = rand() % 4;
        sequence.push_back(corAleatoria);
    }

    bool verificaJogada() {
        if (entradaJogador.size() == 0) {
            return false;
        }
        for (size_t i = 0; i < entradaJogador.size(); ++i) {
            if (entradaJogador[i] != sequence[i]) {

                return false;
            }
        }
        return true;
    }

    std::vector<int> entradaJogador;
};