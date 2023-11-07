#include "Detector.h"
#include "VProcessor.h"

int main() {
    VideoProcessor processor;
    processor.iniciarJogo();
    processor.setInput(0);
    processor.displayOutput("Output Video");
    processor.setDelay(1000. / processor.getFrameRate());
    processor.setFrameProcessor(getSkin);

    processor.run();
    cv::waitKey();
}
