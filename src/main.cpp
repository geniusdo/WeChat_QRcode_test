#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/wechat_qrcode.hpp>
#include "opencv2/dnn.hpp"

using namespace std;
using namespace cv;

#define CLIP(x, x1, x2) max(x1, min(x, x2))
int camera_code = 2; //修改这个改变选择相机

/**
 * @brief 画二维码边界
 *
 * @param img 输入图像
 * @param points 边界四个点
 */
void Display(Mat img, Mat points)
{
    int i = 10; // i 用以微调方框位置
    line(img, Point2f(points.at<float>(0, 0) - i, points.at<float>(0, 1) - i), Point2f(points.at<float>(1, 0) + i, points.at<float>(1, 1) - i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(0, 0) - i, points.at<float>(0, 1) - i), Point2f(points.at<float>(3, 0) - i, points.at<float>(3, 1) + i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(3, 0) - i, points.at<float>(3, 1) + i), Point2f(points.at<float>(2, 0) + i, points.at<float>(2, 1) + i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(1, 0) + i, points.at<float>(1, 1) - i), Point2f(points.at<float>(2, 0) + i, points.at<float>(2, 1) + i), Scalar(0, 255, 0), 2);
}

Mat super_scale_test(Mat &img, float scale)
{
    Mat dst;
    Mat src;
    src = img;
    dnn::Net srnet_ = dnn::readNetFromCaffe("sr.prototxt", "sr.caffemodel");
    Mat blob;
    dnn::blobFromImage(src, blob, 1.0 / 255, Size(src.cols, src.rows), {0.0f}, false, false);
    srnet_.setInput(blob);
    auto prob = srnet_.forward();
    dst = Mat(prob.size[2], prob.size[3], CV_8UC1);

    for (int row = 0; row < prob.size[2]; row++)
    {
        const float *prob_score = prob.ptr<float>(0, 0, row);
        for (int col = 0; col < prob.size[3]; col++)
        {
            float pixel = prob_score[col] * 255.0;
            dst.at<uint8_t>(row, col) = static_cast<uint8_t>(CLIP(pixel, 0.0f, 255.0f));
        }
    }

    return dst;
}
int main(int argc, char *argv[])
{
    VideoCapture cap;

    cap.open(camera_code, cv::CAP_V4L2);
    cap.set(CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
    Mat frame;
    Mat img;

    Ptr<wechat_qrcode::WeChatQRCode> detector;                                              //创建类型为WeChatQRCode类的智能指针
    detector = makePtr<wechat_qrcode::WeChatQRCode>("detect.prototxt", "detect.caffemodel", //实例化该智能指针
                                                    "sr.prototxt", "sr.caffemodel");
    string prevstr = "";
    vector<Mat> points; //创建一个容器，相当于智能数组，里面的元素为Mat

    while (1)
    {
        cap >> frame;
        cvtColor(frame, img, COLOR_RGB2GRAY);
        // equalizeHist(img, img);
        if (img.empty())
            break;
        auto res = detector->detectAndDecode(img, points); //对图像进行扫描，解码
        if (res.empty())
        {
            cout << "QRcode not detected!" << endl;
        }
        if (!res.empty())
        {
            for (const auto &t : res) //对res中所有元素遍历
            {
                // if (t != prevstr)
                {
                    Mat temp;
                    cout << "QR_code:" << t << endl;
                    Display(img, points[0]);
                    // imshow("original", img);
                    Rect rect(points[0].at<float>(0, 0)-10, points[0].at<float>(0, 1)-10, points[0].at<float>(2, 0)-points[0].at<float>(0, 0)+20, points[0].at<float>(2, 1)-points[0].at<float>(0, 1)+20);
                    Mat QR_img = img(rect);
                    threshold(QR_img, temp, 100, 200.0, THRESH_BINARY);
                    imshow("QR", QR_img);
                    imshow("test", super_scale_test(temp, 2.0));
                }
            }
        }
        if (!res.empty())
            prevstr = res.back();
        imshow("image", img);
        if (waitKey(20) == 27)
            break;
    }

    cap.release();
    return 0;
}
