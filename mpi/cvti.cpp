#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <mpi.h>
using namespace cv;
using namespace std;
const int rt = 10;
int conv[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
int main(int argc, char *argv[])
{

    Mat *myMats = new Mat[rt];
    Mat *Result = new Mat[rt];
    double ta = (double)getTickCount();
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
    int rank, tot;
    uchar *buf;
    MPI_Status state;
    double t = (double)getTickCount();
    MPI_Init(&argc, &argv); //初始化MPI环境
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tot);
    for (int ii = 0; ii < rt; ++ii)
    {
    nChannels = myMats[ii].channels();
    buf = new uchar[nChannels*(myMats[ii].cols-2)];

    if(rank == 0)
    {
        if(tot == 1) return 0;
        uchar* output;
        for (int i = 1; i < myMats[ii].rows-1; ++i)
        {
            MPI_Recv(buf, nChannels*(myMats[ii].cols-2), MPI_UNSIGNED_CHAR, (i % (tot - 1))?(i % (tot - 1)):(tot - 1), i, MPI_COMM_WORLD, &state);
            output = Result[ii].ptr<uchar>(i) + nChannels;
            for (int j = 0; j < nChannels*(myMats[ii].cols-2); ++j)
            {
                *output++ = buf[j];
            }
        }
        Result[ii].row(0).setTo(Scalar(0));             // 上边界
        Result[ii].row(Result[ii].rows-1).setTo(Scalar(0)); // 下边界
        Result[ii].col(0).setTo(Scalar(0));             // 左边界
        Result[ii].col(Result[ii].cols-1).setTo(Scalar(0)); // 右边界
    }
    else{
        for(int j = rank ; j < myMats[ii].rows-1; j += (tot - 1)) {
            const uchar* previous = myMats[ii].ptr<uchar>(j - 1);
            const uchar* current  = myMats[ii].ptr<uchar>(j    );
            const uchar* next     = myMats[ii].ptr<uchar>(j + 1);

            for(int i= nChannels;i < nChannels*(myMats[ii].cols-1); ++i)
            {
                buf[i-nChannels] = saturate_cast<uchar>(current[i] * conv[4] + previous[i] * conv[1] + next[i] * conv[7]
                         + current[i-nChannels] * conv[3] + current[i+nChannels] * conv[5] 
                         + previous[i-nChannels] * conv[0] + previous[i+nChannels] * conv[2]
                         + next[i-nChannels] * conv[6] + next[i+nChannels] * conv[8]);
            }
            MPI_Send(buf, nChannels*(myMats[ii].cols-2), MPI_UNSIGNED_CHAR, 0, j, MPI_COMM_WORLD);
        }
    }
    }
    MPI_Finalize(); //结束MPI环境
    if(rank == 0) {
    t = ((double)getTickCount() - t)/getTickFrequency();
    cout << "compute times passed in seconds: " << t << endl;
    cout << "read times passed in seconds: " << ta << endl;

    // imshow("t",Result);
    // waitKey(0);
     imwrite("t2.png",Result[0]);
    }
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