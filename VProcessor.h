#if !defined VPROCESSOR
#define VPROCESSOR

#include <iomanip>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <string>
#include <vector>

class FrameProcessor {

  public:
    virtual void process(cv::Mat &input, cv::Mat &output) = 0;
};

class VideoProcessor {

  private:
    cv::VideoCapture capture;
    int (*process)(cv::Mat &, cv::Mat &);
    FrameProcessor *frameProcessor;
    bool callIt;
    std::string windowNameInput;
    std::string windowNameOutput;
    int delay;
    long fnumber;
    long frameToStop;
    bool stop;

    std::vector<std::string> images;
    std::vector<std::string>::const_iterator itImg;

    cv::VideoWriter writer;
    std::string outputFile;

    int currentIndex;
    int digits;
    std::string extension;

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

    void writeNextFrame(cv::Mat &frame) {

        if (extension.length()) {

            std::stringstream ss;
            ss << outputFile << std::setfill('0') << std::setw(digits) << currentIndex++ << extension;
            cv::imwrite(ss.str(), frame);

        } else {

            writer.write(frame);
        }
    }

  public:
    VideoProcessor() : callIt(false), delay(-1),
                       fnumber(0), stop(false), digits(0), frameToStop(-1),
                       process(0), frameProcessor(0) {}

    bool setInput(std::string filename) {

        fnumber = 0;
        capture.release();
        images.clear();

        return capture.open(filename);
    }

    bool setInput(int id) {

        fnumber = 0;
        capture.release();
        images.clear();

        return capture.open(id);
    }

    bool setInput(const std::vector<std::string> &imgs) {

        fnumber = 0;
        capture.release();

        images = imgs;
        itImg = images.begin();

        return true;
    }

    bool setOutput(const std::string &filename, int codec = 0, double framerate = 0.0, bool isColor = true) {

        outputFile = filename;
        extension.clear();

        if (framerate == 0.0)
            framerate = getFrameRate();

        char c[4];
        if (codec == 0) {
            codec = getCodec(c);
        }

        return writer.open(outputFile,     // filename
                           codec,          // codec to be used
                           framerate,      // frame rate of the video
                           getFrameSize(), // frame size
                           isColor);       // color video?
    }

    bool setOutput(const std::string &filename, // filename prefix
                   const std::string &ext,      // image file extension
                   int numberOfDigits = 3,      // number of digits
                   int startIndex = 0) {        // start index

        if (numberOfDigits < 0)
            return false;

        outputFile = filename;
        extension = ext;

        digits = numberOfDigits;
        currentIndex = startIndex;

        return true;
    }

    void setFrameProcessor(int (*frameProcessingCallback)(cv::Mat &, cv::Mat &)) {

        frameProcessor = 0;
        process = frameProcessingCallback;
        callProcess();
    }

    void setFrameProcessor(FrameProcessor *frameProcessorPtr) {

        process = 0;
        frameProcessor = frameProcessorPtr;
        callProcess();
    }

    void stopAtFrameNo(long frame) {

        frameToStop = frame;
    }

    void callProcess() {

        callIt = true;
    }

    void dontCallProcess() {

        callIt = false;
    }

    void displayInput(std::string wn) {

        windowNameInput = wn;
        cv::namedWindow(windowNameInput);
    }

    void displayOutput(std::string wn) {

        windowNameOutput = wn;
        cv::namedWindow(windowNameOutput);
    }

    void dontDisplay() {

        cv::destroyWindow(windowNameInput);
        cv::destroyWindow(windowNameOutput);
        windowNameInput.clear();
        windowNameOutput.clear();
    }

    void setDelay(int d) {

        delay = d;
    }

    long getNumberOfProcessedFrames() {

        return fnumber;
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

    long getFrameNumber() {

        if (images.size() == 0) {

            long f = static_cast<long>(capture.get(cv::CAP_PROP_POS_FRAMES));
            return f;

        } else {

            return static_cast<long>(itImg - images.begin());
        }
    }

    double getPositionMS() {

        if (images.size() != 0)
            return 0.0;

        double t = capture.get(cv::CAP_PROP_POS_MSEC);
        return t;
    }

    double getFrameRate() {

        if (images.size() != 0)
            return 0;

        double r = capture.get(cv::CAP_PROP_FPS);

        if (r <= 0)
            r = 30;

        return r;
    }

    long getTotalFrameCount() {

        if (images.size() != 0)
            return images.size();

        long t = capture.get(cv::CAP_PROP_FRAME_COUNT);
        return t;
    }

    int getCodec(char codec[4]) {

        if (images.size() != 0)
            return -1;

        union {
            int value;
            char code[4];
        } returned;

        returned.value = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));

        if (returned.value <= 0) {

            returned.value = cv::VideoWriter::fourcc('P', 'I', 'M', '1');
        }

        codec[0] = returned.code[0];
        codec[1] = returned.code[1];
        codec[2] = returned.code[2];
        codec[3] = returned.code[3];

        return returned.value;
    }

    bool setFrameNumber(long pos) {

        if (images.size() != 0) {

            itImg = images.begin() + pos;
            if (pos < images.size())
                return true;
            else
                return false;

        } else {

            return capture.set(cv::CAP_PROP_POS_FRAMES, pos);
        }
    }

    bool setPositionMS(double pos) {

        if (images.size() != 0)
            return false;
        else
            return capture.set(cv::CAP_PROP_POS_MSEC, pos);
    }

    bool setRelativePosition(double pos) {

        if (images.size() != 0) {

            long posI = static_cast<long>(pos * images.size() + 0.5);
            itImg = images.begin() + posI;
            if (posI < images.size())
                return true;
            else
                return false;

        } else {

            return capture.set(cv::CAP_PROP_POS_AVI_RATIO, pos);
        }
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

                if (process) {
                    sinalJogada = process(frame, output);
                } else if (frameProcessor)
                    frameProcessor->process(frame, output);
                fnumber++;

                cv::flip(frame, output, 1); // flipa a imagem

                drawButtons(frame);

                // faz a jogada de acordo com o sinal
                if (sinalJogada != -1 && sinalJogada != sinalAnterior) {
                    std::cout << "sinalJogada: " << sinalJogada << std::endl;
                    sinalAnterior = sinalJogada;
                } else if (sinalJogada == -1) {
                    sinalAnterior = -1;
                }
            } else {

                output = frame;
            }

            if (outputFile.length() != 0)
                writeNextFrame(output);

            if (windowNameOutput.length() != 0)
                cv::imshow(windowNameOutput, output);

            if (delay >= 0 && cv::waitKey(delay) >= 0)
                stopIt();

            if (frameToStop >= 0 && getFrameNumber() == frameToStop)
                stopIt();
        }
    }

    void drawButtons(cv::Mat frame) {
        double alpha = 0.3;
        int frameWidth = frame.cols;      // Obtém a largura do frame
        int frameHeight = frame.rows;     // Obtém a altura do frame
        int buttonWidth = frameWidth / 5; // Largura do botão como uma quinta parte da largura do frame
        int padding = 10;                 // Padding entre os botões e a borda do frame

        // Botão verde
        drawTransRect(frame, cv::Scalar(0, 255, 0), alpha, cv::Rect(frameWidth / 2 - buttonWidth / 2, padding, buttonWidth, buttonWidth));
        drawImage(frame, "./icons/hand.png", frameWidth / 2 - buttonWidth / 2, padding, buttonWidth, buttonWidth);

        // Botão azul
        drawTransRect(frame, cv::Scalar(255, 0, 0), alpha, cv::Rect(frameWidth / 2 - buttonWidth / 2, frameHeight - buttonWidth - padding, buttonWidth, buttonWidth));
        drawImage(frame, "./icons/hand-fist.png", frameWidth / 2 - buttonWidth / 2, frameHeight - buttonWidth - padding, buttonWidth, buttonWidth);

        // Botão amarelo
        drawTransRect(frame, cv::Scalar(0, 255, 255), alpha, cv::Rect(padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth));
        drawImage(frame, "./icons/hand-left.png", padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth);

        // Botão vermelho
        drawTransRect(frame, cv::Scalar(0, 0, 255), alpha, cv::Rect(frameWidth - buttonWidth - padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth));
        drawImage(frame, "./icons/hand-right.png", frameWidth - buttonWidth - padding, frameHeight / 2 - buttonWidth / 2, buttonWidth, buttonWidth);
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
};

#endif