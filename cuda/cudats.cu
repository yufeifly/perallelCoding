#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace std;
using namespace cv;
using namespace cv::gpu;
int conv[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
const int w = 16 + 3 - 1;

__global__ void conv_kernel(const PtrStepSz<uchar3> gpuin,PtrStep<uchar3> gpuout, int* __restrict__ mask)
{
    __shared__ unsigned char mx[w][w];
    __shared__ unsigned char my[w][w];
    __shared__ unsigned char mz[w][w];
    int rs = blockIdx.x * blockDim.x;
    int cs = blockIdx.y * blockDim.y;
    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            uchar3 vi = gpuin(rs + i,cs + j);
            mx[i][j] =  vi.x;
            my[i][j] =  vi.y;
            mz[i][j] =  vi.z;
        }
    }
    __syncthreads();
    uchar3 v;
    int vx = 0, vy = 0, vz = 0;
    int r = threadIdx.x + blockIdx.x * blockDim.x + 1;
    int c = threadIdx.y + blockIdx.y * blockDim.y + 1;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int ind = j+i*3;

            vx += mx[threadIdx.x + i][threadIdx.y + j] * mask[ind];
            vy += my[threadIdx.x + i][threadIdx.y + j] * mask[ind];
            vz += mz[threadIdx.x + i][threadIdx.y + j] * mask[ind];
        }
    }

	if(vx < 0)
		vx = 0;
	else if(vx > 255)
		vx = 255;
	if(vy < 0)
		vy = 0;
	else if(vy > 255)
		vy = 255;
	if(vz < 0)
		vz = 0;
	else if(vz > 255)
		vz = 255;
	v.x = vx;
	v.y = vy;
	v.z = vz;
    gpuout(r,c) = make_uchar3(v.x,v.y,v.z);
}

int main()
{
    cout << getCudaEnabledDeviceCount() << endl;
    DeviceInfo dev_inf(0);
    cout << dev_inf.isCompatible() << endl;
    setDevice(0);
    cout << dev_inf.multiProcessorCount() << endl;
    cout << dev_inf.totalMemory() << endl;
   
    Mat myMat = imread("../t.png");
    myMat.convertTo(myMat, CV_8U);
    Mat result;
    result.create(myMat.size(), myMat.type());
    GpuMat gpuin,gpuout;
    int *mask;
    gpuin.upload(myMat);
    gpuout.create(gpuin.size(), gpuin.type());
    dim3 block(16,16);
    dim3 grid((myMat.rows-2)/16,(myMat.cols-2)/16);
    cudaMalloc((void **)&mask, sizeof(int)*9);
    cudaMemcpy(mask, conv, sizeof(int)*9, cudaMemcpyHostToDevice);

	double ta = (double)getTickCount();

    conv_kernel<<<grid,block>>>(gpuin, gpuout, mask);

	ta = ((double)getTickCount() - ta)/getTickFrequency();

    gpuout.download(result);
    result.row(0).setTo(Scalar(0));             // 上边界
    result.row(result.rows-1).setTo(Scalar(0)); // 下边界
    result.col(0).setTo(Scalar(0));             // 左边界
    result.col(result.cols-1).setTo(Scalar(0)); // 右边界
   
    cout << "times passed in seconds: " << ta << endl;
    cudaFree(mask);
    imwrite("t3.png",result);

    return 0;
}