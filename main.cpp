#include "Detector.h"
#include "VProcessor.h"

void draw(cv::Mat &img, cv::Mat &out) {

    img.copyTo(out);
    cv::circle(out, cv::Point(100, 100), 5, cv::Scalar(255, 0, 0), 2);
}

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
