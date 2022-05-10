# 基于CNN卷积神经网络的二维码识别

## 介绍

**dnn库**使用训练好了的神经网络模型。

---

## 函数用法

 **cv::VideoCapture**  
* **open（int `cam`, int `Code`）** `cam`填入0，2，4等，表示需要打开的相机端口号；`Code`填入cv库中定义好的宏，这里我们填入`cv::CAP_V4L2`，表示使用linux上的视频接口。

* **set(`CAP_PROP_FRAME_WIDTH`, int `resolution`)** 设置捕获视频大小。

---

## 方案效果比对