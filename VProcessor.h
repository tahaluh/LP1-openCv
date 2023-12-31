#if !defined VPROCESSOR
#define VPROCESSOR

#include "Simom.h"
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <vector>


class VideoProcessor {

  private:
    Simom simomGame;
    bool mostrandoSequencia = false;
    int nPiscadas = 7;
    int timePiscada = 200;
    int nInterval = 4;
    int timeInterval = 150;

    cv::VideoCapture capture;
    int (*process)(cv::Mat &, cv::Mat &);
    bool callIt;
    std::string windowNameInput;
    std::string windowNameOutput;
    int delay;
    bool stop;

    std::vector<std::string> images;
    std::vector<std::string>::const_iterator itImg;

    int currentIndex;
    int digits;
    std::string extension;

    void tocaSom(int sinalJogada) {
        std::string filePath = "./sounds/beep";
        switch (sinalJogada) {
        case 0:
            filePath += "01.wav";
            break;
        case 1:
            filePath += "02.wav";
            break;
        case 2:
            filePath += "03.wav";
            break;
        case 3:
            filePath += "04.wav";
            break;
        default:
            filePath += "01.wav";
            break;
        }
        tocarAudio(filePath.c_str());
    }

    bool tocarAudio(const char *filePath) {
        if (PlaySoundA(filePath, NULL, SND_FILENAME | SND_ASYNC) == 0) {
            std::cerr << "Erro ao reproduzir o som." << std::endl;
            return false;
        }
        return true;
    }

    bool readNextFrame(cv::Mat &frame) {

        if (images.size() == 0)
            return capture.read(frame);
        else {

            if (itImg != images.end()) {

                frame = cv::imread(*itImg);
                itImg++;
                return frame.data != 0;
            }
        }
    }

    void sequenciaPerdeu(cv::Mat &frame) {
        std::cout << "Você perdeu!" << std::endl;
        tocarAudio("./sounds/lose1.wav");

        // Define o texto e a posição no centro da imagem
        std::string mensagem = "Voce perdeu!";
        std::string mensagem2 = "Pontuacao: " + std::to_string(simomGame.getPontos());
        std::string mensagem3 = "Maior: " + std::to_string(simomGame.getHighscore());
        int fontSize = 1;  // Tamanho da fonte
        int thickness = 2; // Espessura do texto
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        cv::Size textSize = cv::getTextSize(mensagem, fontFace, fontSize, thickness, 0);
        cv::Point textOrg((frame.cols - textSize.width) / 2, (frame.rows + textSize.height) / 2 - textSize.height * 2);
        cv::Point textOrg2((frame.cols - textSize.width) / 2, (frame.rows + textSize.height) / 2);
        cv::Point textOrg3((frame.cols - textSize.width) / 2, (frame.rows + textSize.height) / 2 + textSize.height * 2);

        for (int i = 0; i < nInterval * 5; ++i) {
            if (i % 2 == 0) {
                drawButton(frame, 0, 0.8);
                drawButton(frame, 1, 0.8);
                drawButton(frame, 2, 0.8);
                drawButton(frame, 3, 0.8);
            }

            cv::putText(frame, mensagem, textOrg, fontFace, fontSize, cv::Scalar(255, 255, 255), thickness);
            cv::putText(frame, mensagem2, textOrg2, fontFace, fontSize, cv::Scalar(255, 255, 255), thickness);
            cv::putText(frame, mensagem3, textOrg3, fontFace, fontSize, cv::Scalar(255, 255, 255), thickness);

            cv::imshow(windowNameOutput, frame); // Exibe o frame atualizado na mesma janela
            if (i != nInterval - 1)
                cv::waitKey(timeInterval);    // Aguarde o tempo de piscada
            frame.setTo(cv::Scalar(0, 0, 0)); // Limpe o frame para preto novamente
        }

        simomGame.iniciarJogo();

        mostrandoSequencia = true;
    }

    void piscaSequencia(cv::Mat &frame) {
        std::vector<int> sequencia = simomGame.getSequencia();

        std::string mensagem = "Pontuacao: " + std::to_string(simomGame.getPontos());
        int fontSize = 1;  // Tamanho da fonte
        int thickness = 2; // Espessura do texto
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        cv::Size textSize = cv::getTextSize(mensagem, fontFace, fontSize, thickness, 0);
        cv::Point textOrg((frame.cols - textSize.width) / 2, (frame.rows + textSize.height) / 2);

        if (sequencia.size() > 1)
            cv::waitKey(1000); // Aguarde o tempo de piscada

        for (size_t j = 0; j < sequencia.size(); ++j) {
            frame.setTo(cv::Scalar(0, 0, 0)); // Limpe o frame para preto

            tocaSom(sequencia[j]);
            for (int i = 0; i < nPiscadas; ++i) {
                if (i % 2 == 0) {
                    drawButton(frame, sequencia[j], 0.8); // Desenha o botão apenas nas piscadas pares
                }

                cv::putText(frame, mensagem, textOrg, fontFace, fontSize, cv::Scalar(255, 255, 255), thickness);

                cv::imshow(windowNameOutput, frame); // Exibe o frame atualizado na mesma janela
                cv::waitKey(timePiscada);            // Aguarde o tempo de piscada
                frame.setTo(cv::Scalar(0, 0, 0));    // Limpe o frame para preto novamente
            }

            for (int i = 0; i < nInterval; ++i) {
                if (i % 2 == 0) {
                    drawButton(frame, 0, 0.8);
                    drawButton(frame, 1, 0.8);
                    drawButton(frame, 2, 0.8);
                    drawButton(frame, 3, 0.8);
                }

                cv::putText(frame, mensagem, textOrg, fontFace, fontSize, cv::Scalar(255, 255, 255), thickness);
                cv::imshow(windowNameOutput, frame); // Exibe o frame atualizado na mesma janela
                if (i != nInterval - 1)
                    cv::waitKey(timeInterval);    // Aguarde o tempo de piscada
                frame.setTo(cv::Scalar(0, 0, 0)); // Limpe o frame para preto novamente
            }
        }

        mostrandoSequencia = false;
    }

    void drawButtons(cv::Mat frame) {
        float alpha = 0.5;

        drawButton(frame, 0, alpha);
        drawButton(frame, 1, alpha);
        drawButton(frame, 2, alpha);
        drawButton(frame, 3, alpha);
    }

    void drawButton(cv::Mat frame, int colorId, float alpha) {
        int frameWidth = frame.cols;      // Obtém a largura do frame
        int frameHeight = frame.rows;     // Obtém a altura do frame
        int buttonWidth = frameWidth / 5; // Largura do botão como uma quinta parte da largura do frame
        int padding = 10;                 // Padding entre os botões e a borda do frame

        // Botão verde
        if (colorId == 0) {
            drawTransRect(frame, cv::Scalar(0, 255, 0), alpha, cv::Rect(frameWidth / 2 - buttonWidth / 2, padding, buttonWidth, buttonWidth));
            drawImage(frame, "./icons/hand.png", frameWidth / 2 - buttonWidth / 2, padding, buttonWidth, buttonWidth);
        }

        // Botão azul
        if (colorId == 2) {
            drawTransRect(frame, cv::Scalar(255, 0, 0), alpha, cv::Rect(frameWidth / 2 - buttonWidth / 2, frameHeight - buttonWidth - padding, buttonWidth, buttonWidth));
            drawImage(frame, "./icons/hand-fist.png", frameWidth / 2 - buttonWidth / 2, frameHeight - buttonWidth - padding, buttonWidth, buttonWidth);
        }

        // Botão amarelo
        if (colorId == 3) {
            drawTransRect(frame, cv::Scalar(0, 255, 255), alpha, cv::Rect(padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth));
            drawImage(frame, "./icons/hand-left.png", padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth);
        }

        // Botão vermelho
        if (colorId == 1) {
            drawTransRect(frame, cv::Scalar(0, 0, 255), alpha, cv::Rect(frameWidth - buttonWidth - padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth));
            drawImage(frame, "./icons/hand-right.png", frameWidth - buttonWidth - padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth);
        }
    }

    void drawTransRect(cv::Mat frame, cv::Scalar color, double alpha, cv::Rect region) {
        cv::Mat roi = frame(region);
        cv::Mat rectImg(roi.size(), CV_8UC3, color);
        addWeighted(rectImg, alpha, roi, 1.0 - alpha, 0, roi);
    }

    void drawImage(cv::Mat frame, const std::string &imagePath, int x, int y, int width, int height) {

        cv::Mat image = cv::imread(imagePath, cv::IMREAD_UNCHANGED);

        cv::Mat mask;
        std::vector<cv::Mat> layers;

        cv::resize(image, image, cv::Size(width, height));

        cv::split(image, layers); // seperate channels
        cv::Mat rgb[3] = {layers[0], layers[1], layers[2]};
        mask = layers[3];     // png's alpha channel used as mask
        merge(rgb, 3, image); // put together the RGB channels, now transp insn't transparent
        image.copyTo(frame.rowRange(y, y + image.rows).colRange(x, x + image.cols), mask);
    }

  public:
    VideoProcessor() : callIt(false), delay(-1), stop(false), digits(0),
                       process(0){}

    void iniciarJogo() {
        simomGame.iniciarJogo();
        this->mostrandoSequencia = true;
    }

    bool setInput(int id) {
        capture.release();
        images.clear();

        return capture.open(id);
    }

    void setFrameProcessor(int (*frameProcessingCallback)(cv::Mat &, cv::Mat &)) {
        process = frameProcessingCallback;
        callIt = true;
    }

    void displayOutput(std::string wn) {

        windowNameOutput = wn;
        cv::namedWindow(windowNameOutput);
    }

    void setDelay(int d) {

        delay = d;
    }

    cv::Size getFrameSize() {

        if (images.size() == 0) {

            int w = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
            int h = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

            return cv::Size(w, h);

        } else {

            cv::Mat tmp = cv::imread(images[0]);
            if (!tmp.data)
                return cv::Size(0, 0);
            else
                return tmp.size();
        }
    }

    double getFrameRate() {

        if (images.size() != 0)
            return 0;

        double r = capture.get(cv::CAP_PROP_FPS);

        if (r <= 0)
            r = 30;

        return r;
    }

    void stopIt() {

        stop = true;
    }

    bool isStopped() {

        return stop;
    }

    bool isOpened() {

        return capture.isOpened() || !images.empty();
    }

    void run() {

        cv::Mat frame;
        cv::Mat output;

        if (!isOpened())
            return;

        stop = false;

        int sinalJogada = -1;
        int sinalAnterior = -1;

        while (!isStopped()) {

            if (!readNextFrame(frame))
                break;

            if (windowNameInput.length() != 0)
                cv::imshow(windowNameInput, frame);

            if (callIt) {
                if (!mostrandoSequencia) {
                    sinalJogada = process(frame, output);

                    cv::flip(frame, output, 1); // flipa a imagem

                    drawButtons(frame);

                    cv::resize(output, output, cv::Size(), 1.5, 1.5);

                    // trata ação de jogo
                    if (sinalJogada != -1 && sinalJogada != sinalAnterior) {
                        std::cout << "sinalJogada: " << sinalJogada << std::endl;
                        sinalAnterior = sinalJogada;

                        int resultadoJogada = simomGame.realizaJogada(sinalJogada);

                        if (resultadoJogada == 1) {
                            tocaSom(sinalJogada);
                            mostrandoSequencia = true;
                        } else if (resultadoJogada == -1) {
                            sequenciaPerdeu(output);
                        } else {
                            tocaSom(sinalJogada);
                        }

                    } else if (sinalJogada == -1) {
                        sinalAnterior = -1;
                    }
                } else {

                    cv::resize(frame, frame, cv::Size(), 1.5, 1.5);
                    piscaSequencia(frame);
                    output = frame;
                }
            } else {
                output = frame;
            }

            if (windowNameOutput.length() != 0)
                cv::imshow(windowNameOutput, output);

            if (delay >= 0 && cv::waitKey(delay) >= 0)
                stopIt();
        }
    }
};

#endif