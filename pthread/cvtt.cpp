#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>
#include <pthread.h>

using namespace cv;
using namespace std;
int conv[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
const int nthread = 1000;
const int rt = 10;
Mat * myMat;
Mat * rel;

void *pfunc(void* arg)
{
    const int nChannels = myMat->channels();
    for(int j = 1 + (long)arg ; j < myMat->rows-1; j+=nthread) {
        const uchar* previous = myMat->ptr<uchar>(j - 1);
        const uchar* current  = myMat->ptr<uchar>(j    );
        const uchar* next     = myMat->ptr<uchar>(j + 1);

        uchar* output = rel->ptr<uchar>(j)+nChannels;

        for(int i= nChannels;i < nChannels*(myMat->cols-1); ++i)//卷积操作
        {
            *output++ = saturate_cast<uchar>(current[i] * conv[4] + previous[i] * conv[1] + next[i] * conv[7]
                     + current[i-nChannels] * conv[3] + current[i+nChannels] * conv[5] 
                     + previous[i-nChannels] * conv[0] + previous[i+nChannels] * conv[2]
                     + next[i-nChannels] * conv[6] + next[i+nChannels] * conv[8]);
        }
    }

}


int main(int argc, char const *argv[])
{

    Mat *myMats = new Mat[rt];
    Mat *Result = new Mat[rt];
    double ta = (double)getTickCount();//表示以毫秒为单位的计算机启动后经历的时间间隔
    for (int ii = 0; ii < rt; ++ii)
    {
        myMats[ii] = imread("../t.png");
		myMats[ii].convertTo(myMats[ii], CV_8U);
		Result[ii].create(myMats[ii].size(),myMats[ii].type());
    }
    ta = ((double)getTickCount() - ta)/getTickFrequency();

    pthread_t threads[nthread];
    int rc;
    struct st *s;
    double t = (double)getTickCount();

    for (int ii = 0; ii < rt; ++ii)
    {
        myMat = &myMats[ii];
        rel = &Result[ii];
    for (int i = 0; i < nthread; ++i)
    {
        rc = pthread_create(&threads[i], NULL, pfunc, (void *)i);//创建线程
        if(rc)
        {
            cout << "error";
            return -1;
        }
    }
    for (int i = 0; i < nthread; ++i)
    {
        rc = pthread_join(threads[i], NULL);
        if(rc)
        {
            cout << "error2";
            return -1;
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

     imwrite("t2.png",Result[0]);
    return 0;
}