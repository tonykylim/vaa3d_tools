#include "ray_shooting.h"
#include "v3d_message.h"
#include "stackutil.h"
#include "Tip_Detection_plugin.h"
#include <iostream>
#include <cmath>

using namespace std;

#define PI 3.1415926

float square(float x){return x*x;}

void ray_shooting(int m, int n,vector<vector<float>> ray_x,vector<vector<float>> ray_y)
	{

		int num = n-1;
		float ang = 2*PI/m;
		float x_dis, y_dis;

		for(int i = 0; i < m; i++)
			{
				x_dis = cos(ang*(i+1));
				y_dis = sin(ang*(i+1));
			for(int j = 0; j<n; j++)
				{
					ray_x[i][j] = x_dis*(j+1);
					ray_y[i][j] = y_dis*(j+1);
				}
			}
        return;
	}
 
void rayinten_2D(int point_x,int point_y,int point_z,int m,int n,int threshold, vector<vector<float>> ray_x,vector<vector<float>> ray_y, Image4DSimple *p4DImage, int &count, float &max_ang)
{
	int point[3] = {point_x,point_y,point_z};
	count = max_ang = 0;  //初始化输出，因为这里使用的引用，不初始化的话会变成累加
	if(point[2] < 0||point[2] >= p4DImage->getZDim())
		return;
	int  point_coordinate[3];
	float pixe = 0.0;
	vector<int> indd(m),ind1(m);
	

	for(int i = 0; i < m; i++)
		{ 
		for(int j = 0; j<n; j++)
			{
				 pixe = 0.0; //为2D的ray像素点赋初值
				//if((point[0]+ray_x[i][j] >= 0) && (point[1]+ray_y[i][j]) >= 0 && (point[0]+ray_x[i][j]) < p4DImage->getXDim() &&  (point[1]+ray_y[i][j]) < p4DImage->getYDim())
				{  
					//pixe = p4DImage->getValueUINT8(point[0]+ray_x[i][j], point[1]+ray_y[i][j], point[2], 0);
					pixe = interp_2d(point[0]+ray_x[i][j], point[1]+ray_y[i][j], point[2],p4DImage);
					if(pixe > threshold)
						ind1[i]++;
				}
			}
		}

	
	for (int i = 0;i < m;i++)
	{
		if (ind1[i] >= n/2)
			{
				indd[count] = i;
				count++;
			}
	}
	
	float dis=0,max_dis=0; 
	if (count > 1)
		{
			for (int i = 0;i < count - 1;i++)
			{
				for (int j = i+1;j < count;j++)
				{
						dis = min(indd[j] - indd[i],m-(indd[j] - indd[i]));
					if (dis > max_dis)
						max_dis=dis;
				}
			}

			max_ang = max_dis / m;
		}
	else if (count == 1)
		max_ang = 1/m;
	else 
		{
			count = 0;
			max_ang = 0;
		}		
}

void ray_shooting_3D(float ray_X[512][8], float ray_Y[512][8], float ray_Z[512][8])
	{	

        for(int len = 0; len < 8; len++)
		{
			int mm = 0;	
            for(int n = 1; n <= 16; n++ )
				for(int k = 0; k <=31; k++ )
					{
						{
							ray_X[mm][len] = len * sin(PI * n/16) * cos(PI * k / 16);
							ray_Y[mm][len] = len * sin(PI * n/16) * sin(PI * k / 16);
							ray_Z[mm][len] = len * cos(PI * n/16);
							mm++;
						}
					}
				}
		}

void rayinten_3D(int point[3], float rayintensity[512][8], float ray_X[512][8], float ray_Y[512][8], float ray_Z[512][8], Image4DSimple *p4DImage, int &count, float &max_ang)
	{
		count = max_ang = 0;
		if(point[2] < 0||point[2] >= p4DImage->getZDim())
			return;

		int pixe, ind1[512] = {0};
		
		for (int i = 0; i < 512; i++)
			for (int j = 0; j < 8; j ++)
			{
				{
					pixe=0;
					if((point[0]+ray_X[i][j] >= 0) && ((point[1]+ray_Y[i][j]) >= 0) && ((point[0]+ray_X[i][j]) < p4DImage->getXDim()) &&  ((point[1]+ray_Y[i][j]) < p4DImage->getYDim()) 
						 && ((point[2]+ray_Z[i][j]) >= 0) &&  (point[2]+ray_Z[i][j]) < p4DImage->getZDim())
					{
						pixe = p4DImage->getValueUINT8(point[0] + ray_X[i][j], point[1] + ray_Y[i][j], point[2] + ray_Z[i][j],0);
						if(pixe > 255*0.18)
						ind1[i]++;
					}
				}
			}

		for (int i = 0; i < 512; i++)
		{
			if (ind1[i] > 8/2)
			{
				ind1[count] = i;
				count++;
			}
	}

		float dis;
		int ind1_i, ind1_j;
		if (count > 1)
		{
			for (int i = 0; i < count; i++)
				for ( int j = i+1; j <count; j++)
				{
					{
						ind1_i = ind1[i];
						ind1_j = ind1[j];
						dis = (ray_X[ind1_i][0]*ray_X[ind1_j][0] + ray_Y[ind1_i][0]*ray_Y[ind1_j][0] + ray_Z[ind1_i][0]*ray_Z[ind1_j][0])/
							sqrt(ray_X[ind1_i][0]*ray_X[ind1_i][0] + ray_Y[ind1_i][0]*ray_Y[ind1_i][0] + ray_Z[ind1_i][0]*ray_Z[i][0]) * sqrt(ray_X[ind1_j][0]*ray_X[ind1_j][0] + ray_Y[ind1_j][0]*ray_Y[ind1_j][0] + ray_Z[ind1_j][0]*ray_Z[ind1_j][0]);
						//dis = acos(dis) / PI * 180;
						if (dis < max_ang)
						{
							max_ang = dis;
						}
					}
				}
		}
		else
		{
			max_ang = 180;
		}

	}


float interp_2d(float point_x,float point_y,int point_z, Image4DSimple *p4DImage)
{
	float result;

	if(point_x < 0||point_y < 0||point_x > p4DImage->getXDim()-1||point_y > p4DImage->getYDim()-1)
		return 0.0;
	else if(point_x < 1||point_y < 1||point_x > p4DImage->getXDim()-2||point_y > p4DImage->getYDim()-2)
	{	
		result = p4DImage->getValueUINT8(point_x,point_y,point_z,0);
		return result;
	}
	else
	{
		int y1 = p4DImage->getValueUINT8(ceil(point_x), ceil(point_y), point_z,0);
		int y0 = p4DImage->getValueUINT8(floor(point_x), ceil(point_y), point_z,0);
		int x1 = p4DImage->getValueUINT8(ceil(point_x), floor(point_y), point_z,0);
		int x0 = p4DImage->getValueUINT8(floor(point_x), floor(point_y), point_z,0);

		result = x0*(point_x-floor(point_x))*(point_y-floor(point_y))+x1*(ceil(point_x)-point_x)*(point_y-floor(point_y))
			+y0*(point_x-floor(point_x))*(ceil(point_y)-point_y)+y1*(ceil(point_x)-point_x)*(ceil(point_y)-point_y);
		return result;
	}
}

bool get_slice_flag(int i,int j,int k,int nembers_2d,int length_2d,double angle_2d,double count_2d_thre,int slice_number,int threshold, vector<vector<float>> ray_x,vector<vector<float>> ray_y, Image4DSimple *p4DImage, int &count_2d, float &max_ang_2d)
{
	max_ang_2d = 0.0;
	count_2d = 0;

	float   max_ang_2d_slice1,max_ang_2d_slice2;
	int count_2d_slice1,count_2d_slice2;

	rayinten_2D(i,j,k, nembers_2d ,length_2d,threshold, ray_x, ray_y, p4DImage, count_2d, max_ang_2d);

	for(int slice = 1; slice<= slice_number;slice++)
	{

		rayinten_2D(i,j,k - slice, nembers_2d ,length_2d,threshold, ray_x, ray_y, p4DImage, count_2d_slice1, max_ang_2d_slice1);
		rayinten_2D(i,j,k + slice, nembers_2d ,length_2d,threshold, ray_x, ray_y, p4DImage, count_2d_slice2, max_ang_2d_slice2);
		if(count_2d_slice1 < count_2d_thre*nembers_2d&&max_ang_2d_slice1*360 <= angle_2d&&count_2d_slice2 < count_2d_thre*nembers_2d&&max_ang_2d_slice2*360 <= angle_2d)
			continue;
		else
			return false;

	}

	return true;
}

bool erodephase(list<cv::Point> &border, cv::Mat&Input, int neighbour[][3], const set<int>& A)
{
    auto pt = border.begin();
    bool result = false;
    while(pt!= border.end())
    {

        int weight = 0;
        for (int j = -1; j <= 1; ++j)
        for (int k = -1; k <= 1; k++)
        weight += neighbour[j + 1][k + 1] * Input.at<uchar>(pt->y + j, pt->x + k);

        if (std::find(A.begin(), A.end(), weight) != A.end())
        {
            Input.at<uchar>(pt->y , pt->x ) = 0;
            pt=border.erase(pt);
            result = true;
        }
        else
            ++pt;
    }
    return result;
}

void findborder(list<cv::Point2i>& border, const cv::Mat&Input)
{
    int cnt = 0;
    int rows = Input.rows;
    int cols = Input.cols;
    cv::Mat bordermat = Input.clone();
        for (int row = 1; row<rows - 1; ++row)
        for (int col = 1; col<cols - 1; ++col)
        {
            int weight = 0;
            for (int j = -1; j <= 1; ++j)
            for (int k = -1; k <= 1; k++)
                {
                    if (Input.at<uchar>(row + j, col + k) == 1)
                        ++cnt;
                }
            if (cnt == 9)
                bordermat.at<uchar>(row, col) = 0;
            cnt = 0;
        }

    for (int row = 1; row<rows - 1; ++row)
        for (int col = 1; col < cols - 1; ++col)
        {
            if (bordermat.at<uchar>(row, col) == 1)
                border.push_back(cv::Point2i(col, row));
        }

}

void finalerode( cv::Mat&Input ,int neighbour[][3], const set<int>& A)
{
    int rows = Input.rows;
    int cols = Input.cols;
    for (int m = 1; m<rows - 1; ++m)
        for (int n = 1; n<cols - 1; ++n)
        {
            int weight = 0;
            for (int j = -1; j <= 1; ++j)
                for (int k = -1; k <= 1; k++)
                {
                    weight += neighbour[j + 1][k + 1] * Input.at<uchar>(m + j, n + k);
                }

            if (std::find(A.begin(), A.end(), weight) != A.end())
                Input.at<uchar>(m, n) = 0;
        }
}

void thin(unsigned char* &Input) //Input?????
{
    int a0[] = { 1,2,3,4,5,6 };
    int a1[] = { 2 };
    int a2[] = { 2,3 };
    int a3[] = { 2,3,4 };
    int a4[] = { 2,3,4,5 };
    int a5[] = { 2,3,4,5,6 };
    set<int> A0 = GetAi(a0, 6);
    set<int> A1 = GetAi(a1, 1);
    set<int> A2 = GetAi(a2, 2);
    set<int> A3 = GetAi(a3, 3);
    set<int> A4 = GetAi(a4, 4);
    set<int> A5 = GetAi(a5, 5);
    list<cv::Point2i> border;
    bool continue_ = true;
    int neighbour[3][3] = {
        { 128,1,2 },
        { 64,0,4 },
        { 32,16,8 }
    };
    while (continue_)
    {
        //?????????????????????
        continue_ = false;

        findborder(border, Input);//Phase0
        //?????????????????????????
        erodephase(border, Input, neighbour,A1);//Phase1
        erodephase(border, Input, neighbour, A2);//Phase2
        erodephase(border, Input, neighbour, A3);//Phase3
        erodephase(border, Input, neighbour, A4);//Phase4
        continue_ =erodephase(border, Input, neighbour, A5);//Phase5
        border.clear();

    }
    finalerode(Input,  neighbour, A0);//????

}

set<int> GetAi(int a[], int length)//??A0~A5
{
    set<int> vec;
    int neighbour[] = { 1,2,4,8,16,32,64,128,1,2,4,8,16,32,64 };
    for (int i = 0; i<length; i++)
        for (int j = 0; j<8; j++)
        {
            int sum = 0;
            for (int k = j; k <= j + a[i]; k++)
                sum += neighbour[k];
            vec.insert(sum);
            std::cout << sum << " ";
        }
    std::cout << std::endl;
    return vec;
}
