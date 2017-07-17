#include <opencv2/core/core.hpp>//内核头文件，包括数据结构，矩阵运算，数据变换，内存管理，文本和数学等功能
#include <opencv2/highgui/highgui.hpp>//图形界面和视频图像处理的头文件
#include <iostream>
using namespace cv;
using namespace std;
const int rt = 10;
int conv[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
int main(int argc, char const *argv[])
{

    Mat *myMats = new Mat[rt];
    Mat *Result = new Mat[rt];
    double ta = (double)getTickCount();
    #pragma omp parallel for
    for (int ii = 0; ii < rt; ++ii)
    {
        myMats[ii] = imread("../t.png");
    myMats[ii].convertTo(myMats[ii], CV_8U);
    // namedWindow("t");  
    // cout << myMats[ii].rows << endl;
    // cout << myMats[ii].cols << endl;
    // cout << (myMats[ii].depth() == CV_8U) << endl;

    Result[ii].create(myMats[ii].size(),myMats[ii].type());
    }
    ta = ((double)getTickCount() - ta)/getTickFrequency();
    int nChannels;
    double t = (double)getTickCount();

    for (int ii = 0; ii < rt; ++ii)
    {
    nChannels = myMats[ii].channels();
    #pragma omp parallel for private(j, i, previous, current, next, output)
    for(int j = 1 ; j < myMats[ii].rows-1; ++j) {
        const uchar* previous = myMats[ii].ptr<uchar>(j - 1);
        const uchar* current  = myMats[ii].ptr<uchar>(j    );
        const uchar* next     = myMats[ii].ptr<uchar>(j + 1);

        uchar* output = Result[ii].ptr<uchar>(j) + nChannels;

        for(int i= nChannels;i < nChannels*(myMats[ii].cols-1); ++i)
        {
            *output++ = saturate_cast<uchar>(current[i] * conv[4] + previous[i] * conv[1] + next[i] * conv[7]
                     + current[i-nChannels] * conv[3] + current[i+nChannels] * conv[5] 
                     + previous[i-nChannels] * conv[0] + previous[i+nChannels] * conv[2]
                     + next[i-nChannels] * conv[6] + next[i+nChannels] * conv[8]);
        }
    }
    Result[ii].row(0).setTo(Scalar(0));             // 上边界
    Result[ii].row(Result[ii].rows-1).setTo(Scalar(0)); // 下边界
    Result[ii].col(0).setTo(Scalar(0));             // 左边界
    Result[ii].col(Result[ii].cols-1).setTo(Scalar(0)); // 右边界
    }
    t = ((double)getTickCount() - t)/getTickFrequency();
    cout << "compute times passed in seconds: " << t << endl;
    cout << "read times passed in seconds: " << ta << endl;

    // imshow("t",Result);
    // waitKey(0);
     imwrite("t2.png",Result[0]);
    // Mat M(2,2, CV_8UC3, Scalar(0,0,255));
    // cout << M.rows << endl;
    // cout << M.cols << endl;
    // cout << M.isContinuous() << endl;
    // uchar* p = M.ptr<uchar>(0);
    // for (int i = 0; i < 12; ++i)
    // {
    //     cout << (int)p[i] << " ";
    // }

    // imshow("t",myMat);
    // waitKey(0);
    // imwrite("t2.png",myMat);
    return 0;
}