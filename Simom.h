#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Simom {
  public:
    Simom() {
        srand(time(0)); // Inicializa a semente para números aleatórios

        this->readHighscore();
    }

    void iniciarJogo() {
        sequence.clear();
        entradaJogador.clear();

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
                    this->setNewHighscore(this->pontos);
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
    std::vector<int> entradaJogador;
    int pontos = 0;
    int highscore = 0;

    void setNewHighscore(int pontos) {
        if (pontos <= highscore) {
            return;
        }
        highscore = pontos;
        const std::string nomeArquivo = "highscoreSave.txt";
        std::ofstream arquivo(nomeArquivo);
        if (arquivo.is_open()) {
            arquivo << highscore;
            arquivo.close();
            std::cout << "Número escrito com sucesso em " << nomeArquivo << std::endl;
        } else {
            std::cerr << "Erro ao abrir o arquivo salvar" << nomeArquivo << std::endl;
        }
    }

    void readHighscore() {
        int numero = 0;
        const std::string nomeArquivo = "highscoreSave.txt";
        std::ifstream arquivo(nomeArquivo);
        if (arquivo.is_open()) {
            arquivo >> numero;
            arquivo.close();
            std::cout << "Número lido com sucesso de " << nomeArquivo << std::endl;
        } else {
            std::cerr << "Erro ao abrir o arquivo ler" << nomeArquivo << std::endl;
        }
        this->highscore = numero;
    }

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
};