#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <math.h>
#include <sstream>
#define M_PI  3.14159265358979323846
using namespace cv;
using namespace std;
/*
HOW TO COMPILE THE CODE : g++ a2_Aditya_2016CSB1029.cpp -o output `pkg-config --cflags --libs opencv`
KEEP THE TIE POINTS OF INITIAL IMAGE IN 1.txt AND OF FINAL IMAGE IN 2.txt WITH FORMAT "Y X"

*/
int max(int k1,int k2)  {
    if (k1>k2) return k1;
    else return k2;
}
double  rad(int degree) {
    return (M_PI/180)*((double)degree);
}
Mat affine_transform(Mat input,Mat affine)  {
    //cout<<affine<<endl;
    int x0=(0)*affine.at<double>(0,0)+(0)*affine.at<double>(0,1)+(affine.at<double>(0,2));
    int x1=(0)*affine.at<double>(0,0)+((+1)*(input.cols)*affine.at<double>(0,1))+(affine.at<double>(0,2));
    int x2=((+1)*(input.rows)*affine.at<double>(0,0))+(0)*affine.at<double>(0,1)+(affine.at<double>(0,2));
    int x3=((+1)*(input.rows)*affine.at<double>(0,0))+((+1)*(input.cols)*affine.at<double>(0,1))+(affine.at<double>(0,2));
    //
    int y0=(0)*affine.at<double>(1,0)+(0)*affine.at<double>(1,1)+(affine.at<double>(1,2));
    int y1=(0)*affine.at<double>(1,0)+((+1)*(input.cols)*affine.at<double>(1,1))+(affine.at<double>(1,2));
    int y2=((+1)*(input.rows)*affine.at<double>(1,0))+(0)*affine.at<double>(1,1)+(affine.at<double>(1,2));
    int y3=((+1)*(input.rows)*affine.at<double>(1,0))+((+1)*(input.cols)*affine.at<double>(1,1))+(affine.at<double>(1,2));
    int maxx=x0,maxy=y0;
    int minx=x0,miny=y0;
    if (x1>maxx) maxx=x1;
    if (x2>maxx) maxx=x2;
    if (x3>maxx) maxx=x3;
    if (x1<minx) minx=x1;
    if (x2<minx) minx=x2;
    if (x3<minx) minx=x3;
    if (y1>maxy) maxy=y1;
    if (y2>maxy) maxy=y2;
    if (y3>maxy) maxy=y3;
    if (y1<miny) miny=x1;
    if (y2<miny) miny=x2;
    if (y3<miny) miny=x3;
    int height=maxx-minx;
    int width=maxy-miny;
    Mat output(height,width,input.type());
    Mat inv=affine.inv();
    //cout<<x0<<" "<<x1<<" "<<x2<<" "<<x3<<endl;
    //cout<<y0<<" "<<y1<<" "<<y2<<" "<<y3<<endl;
    //cout<<minx<<" "<<miny<<" "<<width<<" "<<height<<endl;
    for (int i=-height/2;i<height/2;i++)    
    {
        for (int j=-width/2;j<width/2;j++)  
        {
            output.at<Vec3b>(i+height/2,j+width/2)=0;
        }
    }
    for (int i=0;i<height;i++)  
    {
        for (int j=0;j<width;j++)   
        {
            int x=(i+minx)*inv.at<double>(0,0)+(j+miny)*inv.at<double>(0,1)+inv.at<double>(0,2);
            int y=(i+minx)*inv.at<double>(1,0)+(j+miny)*inv.at<double>(1,1)+inv.at<double>(1,2);
            if ((x)>0 && (y)>0 && (x)<input.rows && (y)<input.cols)
            output.at<Vec3b>(i,j)=input.at<Vec3b>(x,y);
        }
    }
    return output;
}

int find_index(vector<Point2f> input,int x1,int y1)	{
	for (int i=0;i<input.size();i++)	
	{
		if (input[i].x==x1 && input[i].y==y1)	
		{
			return i;
		}
	}
}
float area(int x1, int y1, int x2, int y2, int x3, int y3)
{
   return abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))/2.0);
}
 
/* A function to check whether point P(x, y) lies inside the triangle formed 
   by A(x1, y1), B(x2, y2) and C(x3, y3) */
bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{   
   /* Calculate area of triangle ABC */
   float A = area (x1, y1, x2, y2, x3, y3);
 
   /* Calculate area of triangle PBC */  
   float A1 = area (x, y, x2, y2, x3, y3);
 
   /* Calculate area of triangle PAC */  
   float A2 = area (x1, y1, x, y, x3, y3);
 
   /* Calculate area of triangle PAB */   
   float A3 = area (x1, y1, x2, y2, x, y);
   
   /* Check if sum of A1, A2 and A3 is same as A */
   return (abs(A -( A1 + A2 + A3))<0.1);
}
int main( int argc, char** argv)
{   int n=0;
    cout<<"Enter 1 for Doing Morphing given a image and affine tranformation matrix"<<endl;
    cout<<"Enter 2 for Morphing given a input image and final image"<<endl;
    cin>>n;
    if (n==2)
            {
            cout<<"Enter the number of intermedietes"<<endl;
            int number=5;
            cin>>number;
             string location1="image2.jpg";
             string location2="image1.jpg";
             char location3[6]="2.txt";
             char location4[6]="1.txt";
            cout<<"Enter the location of first Final file"<<endl;
            cin>>location1;
            cout<<"Enter the location of second Initial file"<<endl;
            cin>>location2;
            int k=0;
             for (int count=1;count<=number;count++)    {
                 float value=(float)count/number;
                //cout<<value<<endl;
                 Scalar delaunay_color(255,255,255), points_color(0, 0, 255);
                Mat img1 = imread(location1);
                Mat img2 = imread(location2);
                // imwrite("Morphed_.jpg",img1);
                Mat img1_orig = img1.clone();
                Mat img2_orig = img2.clone();
                vector<Point2f> points1;
                vector<Point2f> points2;
                vector<Point2f> points3;
                ifstream ifs1(location3);
                ifstream ifs2(location4);
                int x, y;
                while(ifs1 >> y >> x)
                {
                    points1.push_back(Point2f(x,y));
                }
                while(ifs2 >> y >> x)
                {
                    points2.push_back(Point2f(x,y));
                }
            //starting
                for (int i=0;i<points1.size();i++)  
                {
                    int tmp_x=cvRound(    (float)(value)*points1[i].x+(float)(1-value)*points2[i].x     );
                    int tmp_y=cvRound(    (float)(value)*points1[i].y+(float)(1-value)*points2[i].y     );
                    points3.push_back(Point2f(tmp_x,tmp_y));
                    //cout<<points1[i].x<<"\t"<<points2[i].x<<"\t"<<points3[i].x<<"|"<<points1[i].y<<"\t"<<points2[i].y<<"\t"<<points3[i].y<<endl;
                }

                Size size = img1.size();
                Rect rect(0, 0, size.height, size.width);
                Subdiv2D subdiv(rect);
                Subdiv2D subdiv_original(rect);
                Subdiv2D subdiv_final(rect);
                for( vector<Point2f>::iterator it = points3.begin(); it != points3.end(); it++)
                {
                    subdiv.insert(*it);
                     
                }
                for( vector<Point2f>::iterator it = points1.begin(); it != points1.end(); it++)
                {
                    subdiv_original.insert(*it);
                     
                }
                for( vector<Point2f>::iterator it = points2.begin(); it != points2.end(); it++)
                {
                    subdiv_final.insert(*it);
                     
                }
                vector<Vec6f> triangleList;
                subdiv.getTriangleList(triangleList);
                vector<Vec6f> triangleList_copy;
                for (int i=0;i<triangleList.size();i++) {
                        int x1=triangleList[i][0];
                        int y1=triangleList[i][1];
                        int x2=triangleList[i][2];
                        int y2=triangleList[i][3];
                        int x3=triangleList[i][4];
                        int y3=triangleList[i][5];
                    if (x1>=0 && y1>=0 && x2>=0 && y2>=0 && x3>=0 && y3>=0 && x1<img1.rows && x2<img1.rows && x3<img1.rows &&
                        y1<img1.cols && y2<img1.cols && y3<img1.cols)
          
                    {
                        triangleList_copy.push_back(triangleList[i]);
                        //cout<<triangleList[i]<<endl;
                    }
                }
                //cout<<triangleList_copy.size()<<endl;
                Mat output(img1.rows,img1.cols,img1.type());
                for (int i=0;i<img1.rows;i++)   
                {
                    for (int j=0;j<img1.cols;j++)   
                    {
                        int triangle_index=-1;
                        for (int k=0;k<triangleList_copy.size();k++)    
                        {
                            if (isInside(triangleList_copy[k][0],triangleList_copy[k][1],triangleList_copy[k][2],
                                triangleList_copy[k][3],triangleList_copy[k][4],triangleList_copy[k][5],i,j)==true) 
                            {
                                triangle_index=k;
                                break;
                            }
                        }
                        if (triangle_index==-1) {
                            continue;
                            //cout<<"Hello"<<endl;
                        }
                        Point2f src[3];
                        Point2f dist1[3];
                        Point2f dist2[3];
                        int x1=triangleList_copy[triangle_index][0];
                        int y1=triangleList_copy[triangle_index][1];
                        int x2=triangleList_copy[triangle_index][2];
                        int y2=triangleList_copy[triangle_index][3];
                        int x3=triangleList_copy[triangle_index][4];
                        int y3=triangleList_copy[triangle_index][5];
                        src[0]=Point2f(x1,y1);
                        src[1]=Point2f(x2,y2);
                        src[2]=Point2f(x3,y3);
                        int index1=find_index(points3,x1,y1);
                        int index2=find_index(points3,x2,y2);
                        int index3=find_index(points3,x3,y3);
                        dist1[0]=Point2f(points1[index1].x,points1[index1].y);
                        dist1[1]=Point2f(points1[index2].x,points1[index2].y);
                        dist1[2]=Point2f(points1[index3].x,points1[index3].y);
                        dist2[0]=Point2f(points2[index1].x,points2[index1].y);
                        dist2[1]=Point2f(points2[index2].x,points2[index2].y);
                        dist2[2]=Point2f(points2[index3].x,points2[index3].y);
                        Mat A=getAffineTransform(src,dist1);
                        Mat B=getAffineTransform(src,dist2);
                        
                        int i1=(i*A.at<double>(0,0)+j*A.at<double>(0,1)+A.at<double>(0,2));
                        int j1=(i*A.at<double>(1,0)+j*A.at<double>(1,1)+A.at<double>(1,2));
                        int i2=(i*B.at<double>(0,0)+j*B.at<double>(0,1)+B.at<double>(0,2));
                        int j2=(i*B.at<double>(1,0)+j*B.at<double>(1,1)+B.at<double>(1,2));

                        if (i1>=0 && j1>=0 && i2>=0 && j2>=0 && i1<output.rows && j1<output.cols && i2<output.rows && j2<output.cols)
                        {
                            output.at<Vec3b>(i,j)[0]=(value)*img1.at<Vec3b>(i1,j1)[0]+(1-value)*img2.at<Vec3b>(i2,j2)[0];
                            output.at<Vec3b>(i,j)[1]=(value)*img1.at<Vec3b>(i1,j1)[1]+(1-value)*img2.at<Vec3b>(i2,j2)[1];
                            output.at<Vec3b>(i,j)[2]=(value)*img1.at<Vec3b>(i1,j1)[2]+(1-value)*img2.at<Vec3b>(i2,j2)[2];
                        }
                    }
                }
                //ending
                string jpg=".jpg";
                stringstream convert;
                convert<< k;
                string result=convert.str();
                string name="Morphed_";
                name=name+result+jpg;
                imwrite(name,output);
                k++;
             }
            return 0;
        }
        else if (n==1)  {
            string location_file;
            cout<<"Enter input image location"<<endl;
            cin>>location_file;
            Mat input=imread(location_file,1);
            imwrite("Morphed_part0.jpg",input);
            Mat affine=(Mat_<double>(3,3)<<
                cos(rad(90)),(-1)*(sin((rad(90)))),0,
                sin((rad(90))),cos(rad(90)),0,
                0,0,1 );
            cout<<"Enter the affine matrix values"<<endl;
            for (int i=0;i<3;i++)   {
                for (int j=0;j<3;j++)   {
                    cin>>affine.at<double>(i,j);
                }
            }
            int num=10;
            cout<<"Enter the Number of intermedietes you want"<<endl;
            cin>>num;
            for (int count=1;count<=num;count++)    
            {   
                Mat temp=Mat::eye(3,3,CV_64F);
                temp.at<double>(0,0)=((double)count/num)*(affine.at<double>(0,0))+(double)(num-count)/num;
                temp.at<double>(0,1)=((double)count/num)*(affine.at<double>(0,1));
                temp.at<double>(0,2)=((double)count/num)*(affine.at<double>(0,2));
                temp.at<double>(1,0)=((double)count/num)*(affine.at<double>(1,0));
                temp.at<double>(1,1)=((double)count/num)*(affine.at<double>(1,1))+(double)(num-count)/num;
                temp.at<double>(1,2)=((double)count/num)*(affine.at<double>(1,2));
                string name="Morphedpart_";
                string jpg=".jpg";
                stringstream convert;
                convert<<count;
                string result=convert.str();
                name=name+result+jpg;
                imshow("window2",affine_transform(input,temp));
                imwrite(name,affine_transform(input,temp));
                waitKey(0);
    }
        }
        else    {
            cerr<<"Please enter a valid value"<<endl;
            exit(0);
        }
}