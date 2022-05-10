#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/wechat_qrcode.hpp>

using namespace std;
using namespace cv;

int camera_code = 0; //修改这个改变选择相机

/**
 * @brief 画二维码边界
 *
 * @param img 输入图像
 * @param points 边界四个点
 */
void Display(Mat img, Mat points)
{
    int i = 1; // i 用以微调方框位置
    line(img, Point2f(points.at<float>(0, 0) + i, points.at<float>(0, 1) + i), Point2f(points.at<float>(1, 0) + i, points.at<float>(1, 1) + i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(0, 0) + i, points.at<float>(0, 1) + i), Point2f(points.at<float>(3, 0) + i, points.at<float>(3, 1) + i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(3, 0) + i, points.at<float>(3, 1) + i), Point2f(points.at<float>(2, 0) + i, points.at<float>(2, 1) + i), Scalar(0, 255, 0), 2);
    line(img, Point2f(points.at<float>(1, 0) + i, points.at<float>(1, 1) + i), Point2f(points.at<float>(2, 0) + i, points.at<float>(2, 1) + i), Scalar(0, 255, 0), 2);
}

int main(int argc, char *argv[])
{
    VideoCapture cap;
    cap.open(camera_code, cv::CAP_V4L2);
    Mat img;
    cap >> img;
    Ptr<wechat_qrcode::WeChatQRCode> detector;                                              //创建类型为WeChatQRCode类的智能指针
    detector = makePtr<wechat_qrcode::WeChatQRCode>("detect.prototxt", "detect.caffemodel", //实例化该智能指针
                                                    "sr.prototxt", "sr.caffemodel");
    string prevstr = "";
    vector<Mat> points; //创建一个容器，相当于智能数组，里面的元素为Mat

    while (1)
    {
        cap >> img;
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
                    cout << "QR_code:" << t << endl;
                    Display(img, points[0]);
                }
            }
        }
        if (!res.empty())
            prevstr = res.back();
        imshow("image", img);
        if (waitKey(50) == 27)
            break;
    }

    cap.release();
    return 0;
}
