#ifndef DETECTOR
#define DETECTOR

#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

static int init_count;

// count

static int fist_count;
static int fist_not_count;

static int rpalm_count;
static int rpaml_not_count;

static int lpalm_count;
static int lpalm_not_count;

static int right_count;
static int right_not_count;

static int left_count;
static int left_not_count;

static int no_posture_count;

static unsigned char flag_no_posture = 0;

// flag

static bool flag_lpalm = 0;
static bool flag_rpalm = 0;
static bool flag_fist = 0;
static bool flag_pright = 0;
static bool flag_pleft = 0;

static char posture = 0;
static bool change_posture = 0;

// draw

static int radius = 50;
static float scale_factor = 0.8;
static int larguraIcon = (2 * radius * scale_factor);
static int alturaIcon = (2 * radius * scale_factor);
static int timeDelay = 15;
static int timeWait = 10;

void reset_all_postures() {
    flag_fist = 0;
    flag_rpalm = 0;
    flag_lpalm = 0;
    flag_pright = 0;
    flag_pleft = 0;
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

int getSkin(cv::Mat &src, cv::Mat &output) {
    int time_count1 = 0;
    int time_count2 = 0;

    output = src;

    // detector for right palm
    cv::CascadeClassifier rpalm_cascade;
    if (!rpalm_cascade.load("./cascades/rpalm.xml"))
        std::cout << ("--(!)Error loading rpalm\n");
    std::vector<cv::Rect> rpalm;
    rpalm_cascade.detectMultiScale(src, rpalm, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    cv::Rect big_rpalm;
    if (rpalm.size() != NULL) {
        no_posture_count = 0;
        big_rpalm = rpalm[0];
        flag_rpalm = 1;
        flag_no_posture = flag_no_posture | 0x01;
    } else {
        flag_no_posture = flag_no_posture & 0xFE;
    }
    for (int i = 0; i < rpalm.size(); i++) {
        if ((rpalm[i].width * rpalm[i].height) > (big_rpalm.width * big_rpalm.height)) // detecting biggest rect
        {
            big_rpalm = rpalm[i];
        }

        cv::Point center(rpalm[i].x + rpalm[i].width * 0.5, rpalm[i].y + rpalm[i].height * 0.5);
        cv::circle(output, center, radius, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);                       // Verde
        cv::circle(output, center, (rpalm_count * 1.0 / timeDelay) * radius, cv::Scalar(0, 255, 0), -1); // Verde, -1 preenche o círculo

        int posx = center.x - larguraIcon / 2;
        int posy = center.y - alturaIcon / 2;

        drawImage(output, "./icons/hand.png", posx, posy, larguraIcon, alturaIcon);
    }

    // detector for left palm

    /*cv::CascadeClassifier lpalm_cascade;
    if (!lpalm_cascade.load("./cascades/lpalm.xml"))
        std::cout << ("--(!)Error loading lpalm\n");
    std::vector<cv::Rect> lpalm;
    lpalm_cascade.detectMultiScale(src, lpalm, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    cv::Rect big_lpalm;
    if (lpalm.size() != NULL) {
        no_posture_count = 0;
        big_lpalm = lpalm[0];
        flag_lpalm = 1;
        flag_no_posture = flag_no_posture | 0x01;
    } else {
        flag_no_posture = flag_no_posture & 0xFE;
    }

    for (int i = 0; i < lpalm.size(); i++) {
        if ((lpalm[i].width * lpalm[i].height) > (big_lpalm.width * big_lpalm.height)) // detecting biggest rect
        {
            big_lpalm = lpalm[i];
        }
        cv::Point center(lpalm[i].x + lpalm[i].width * 0.5, lpalm[i].y + lpalm[i].height * 0.5);
        // cv::rectangle(output, lpalm[i], cv::Scalar(0, 0, 0), 2); // preto
    }*/

    // detector for fist
    cv::CascadeClassifier fist_cascade;
    if (!fist_cascade.load("./cascades/fist.xml"))
        std::cout << ("--(!)Error loading fist\n");
    std::vector<cv::Rect> fists;
    fist_cascade.detectMultiScale(src, fists, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    cv::Rect big_fist;
    if (fists.size() != NULL) {
        no_posture_count = 0;
        big_fist = fists[0];
        flag_fist = 1;
        flag_no_posture = flag_no_posture | 0x02;
    } else {
        flag_no_posture = flag_no_posture & 0xFD;
    }
    for (int i = 0; i < fists.size(); i++) {
        if ((fists[i].width * fists[i].height) > (big_fist.width * big_fist.height)) // detecting biggest rect
        {
            big_fist = fists[i];
        }

        cv::Point center(fists[i].x + fists[i].width * 0.5, fists[i].y + fists[i].height * 0.5);
        cv::circle(output, center, radius, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);                      // Azul
        cv::circle(output, center, (fist_count * 1.0 / timeDelay) * radius, cv::Scalar(255, 0, 0), -1); // Azul, -1 preenche o círculo

        int posx = center.x - larguraIcon / 2;
        int posy = center.y - alturaIcon / 2;

        drawImage(output, "./icons/hand-fist.png", posx, posy, larguraIcon, alturaIcon);
    }

    // detector for right
    cv::CascadeClassifier right_cascade;
    if (!right_cascade.load("./cascades/right.xml"))
        std::cout << ("--(!)Error loading right\n");
    std::vector<cv::Rect> rect_right;
    right_cascade.detectMultiScale(src, rect_right, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    cv::Rect big_right;
    if (rect_right.size() != NULL) {
        no_posture_count = 0;
        big_right = rect_right[0];
        flag_pright = 1;
        flag_no_posture = flag_no_posture | 0x08;
    } else {
        flag_no_posture = flag_no_posture & 0xF7;
    }
    for (int i = 0; i < rect_right.size(); i++) {
        cv::Point center(rect_right[i].x + rect_right[i].width * 0.5, rect_right[i].y + rect_right[i].height * 0.5);
        cv::circle(output, center, radius, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);                       // Vermelho
        cv::circle(output, center, (right_count * 1.0 / timeDelay) * radius, cv::Scalar(0, 0, 255), -1); // Vermelho, -1 preenche o círculo

        int posx = center.x - larguraIcon / 2;
        int posy = center.y - alturaIcon / 2;

        drawImage(output, "./icons/hand-left.png", posx, posy, larguraIcon, alturaIcon);
    }

    // detector for left
    cv::CascadeClassifier left_cascade;
    if (!left_cascade.load("./cascades/left.xml"))
        std::cout << ("--(!)Error loading left\n");
    std::vector<cv::Rect> rect_left;
    left_cascade.detectMultiScale(src, rect_left, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
    cv::Rect big_left;
    if (rect_left.size() != NULL) {
        no_posture_count = 0;
        big_left = rect_left[0];
        flag_pleft = 1;
        flag_no_posture = flag_no_posture | 0x08;
    } else {
        flag_no_posture = flag_no_posture & 0xF7;
    }
    for (int i = 0; i < rect_left.size(); i++) {

        cv::Point center(rect_left[i].x + rect_left[i].width * 0.5, rect_left[i].y + rect_left[i].height * 0.5);
        cv::circle(output, center, radius, cv::Scalar(0, 255, 255), 2, cv::LINE_AA);                      // Amarelo
        cv::circle(output, center, (left_count * 1.0 / timeDelay) * radius, cv::Scalar(0, 255, 255), -1); // Amarelo, -1 preenche o círculo

        int posx = center.x - larguraIcon / 2;
        int posy = center.y - alturaIcon / 2;

        drawImage(output, "./icons/hand-right.png", posx, posy, larguraIcon, alturaIcon);
    }

    init_count++;
    if (init_count > 300)
        init_count = 0;

    // tratamento de flags

    /*
    if (flag_lpalm == 1) {
        if (lpalm_count > timeDelay) {
            std::cout << "lpalm_count: " << lpalm_count << std::endl;
            return
        } else {
            lpalm_count++;
            lpalm_not_count = 0;
        }
    } else {
        if (lpalm_not_count > timeWait)
            lpalm_count = 0;
        lpalm_not_count++;
    }
    */

    int resultSinal = -1;

    if (flag_rpalm == 1) {
        if (rpalm_count > timeDelay) {
            resultSinal = 0;
        } else {
            rpalm_count++;
            rpaml_not_count = 0;
        }
    } else {
        if (rpalm_count > timeDelay) {
            resultSinal = 0;
        }
        if (rpaml_not_count > timeWait)
            rpalm_count = 0;
        rpaml_not_count++;
    }

    if (flag_fist == 1) {
        if (fist_count > timeDelay) {
            resultSinal = 2;
        } else {
            fist_count++;
            fist_not_count = 0;
        }
    } else {
        if (fist_count > timeDelay) {
            resultSinal = 2;
        }
        if (fist_not_count > timeWait)
            fist_count = 0;
        fist_not_count++;
    }

    if (flag_pright == 1) {
        if (right_count > timeDelay) {
            resultSinal = 1;
        } else {
            right_count++;
            right_not_count = 0;
        }
    } else {
        if (right_count > timeDelay) {
            resultSinal = 1;
        }
        if (right_not_count > timeWait)
            right_count = 0;
        right_not_count++;
    }

    if (flag_pleft == 1) {
        if (left_count > timeDelay) {
            resultSinal = 3;
        } else {
            left_count++;
            left_not_count = 0;
        }
    } else {
        if (left_count > timeDelay) {
            resultSinal = 3;
        }
        if (left_not_count > timeWait)
            left_count = 0;
        left_not_count++;
    }

    reset_all_postures();

    return resultSinal;
}

#endif
