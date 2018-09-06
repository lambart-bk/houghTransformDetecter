#include "houghtransformdetecter/houghtransformdetecter.h"
#include"iostream"
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"stdio.h"
using namespace std;

//#define DEBUG
using namespace std;
void detecter(cv::Mat img,char *filename)
{
  cv::Mat dst;
  cv::Size sz(cv::Size(img.cols/4,img.rows/4));
  cv::resize(img,img,sz);
#if  defined(DEBUG)       
  cv::imshow( "img", img );
  cv::waitKey(0);
#endif     
  // smooth it, otherwise a lot of false circles may be detected
  cv::GaussianBlur( img, img, cv::Size(9, 9), 2, 2 );
  
  
  cv::Mat edge;
  cv::Canny(img,edge,100,20);
#if defined(DEBUG)  
  cv::imshow( "edgef", edge );
  cv::waitKey(0);
#endif  
  
  vector<vector<cv::Point> > contours;
  vector<cv::Vec4i> hierarchy;   
  cv::findContours(edge,contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
  int maxSize=0;
  cv::RotatedRect max_box;
  for(int i=0;i<contours.size();i++)
  {
    cv::RotatedRect box=cv::minAreaRect(contours[i]); 
    
#if  defined(DEBUG)      
    cv::circle(edge,box.center,max(box.size.height,box.size.width)/2,cv::Scalar(255,0,0),5);
#endif       
    
    if((abs(box.size.height-box.size.width)<=200)&&(max(box.size.height,box.size.width)>maxSize))
    {
      maxSize=max(box.size.height,box.size.width);
      max_box=box;
    }
  }
#if  defined(DEBUG)    
  cout<<max_box.center.x<<","<<max_box.center.y<<endl;
  cout<<max_box.size.height<<","<<max_box.size.width<<endl;
  cv::circle(edge,max_box.center,max(max_box.size.height,max_box.size.width)/2,cv::Scalar(255,0,0),5);
  cv::imshow( "edge", edge );
  cv::waitKey(0);
#endif   
  cv::Mat img_roi;
  //cv::Rect rect(300,210,690,550);
  
  cv::Rect rect(max_box.center.x-max_box.size.width/2-10,
		max_box.center.y-max_box.size.height/2-10,
		max_box.size.width+20,
		max_box.size.height+20);
  
  img_roi=img(rect);
#if  defined(DEBUG)     
  cv::imshow( "img_roi", img_roi );
  cv::waitKey(0);
#endif 
  
  cv::Mat kernel;
  kernel=(cv::Mat_<float>(3,3)<<0,-1,0,-1,5,-1,0,-1,0);
  cv::filter2D(img_roi,img_roi,CV_8UC1,kernel);
#if  defined(DEBUG) 
  cv::imshow("laplice:img_roi",img_roi);
  cv::waitKey(0);
    
  
  cv::Canny(img_roi,edge,60,30);	
  cv::imshow( "edge", edge );
  cv::waitKey(0);
#endif   
  
  vector<cv::Vec3f> circles;
  cv::HoughCircles(img_roi, circles, CV_HOUGH_GRADIENT,
		2, max(img_roi.rows,img_roi.cols)/1.5, 60, 20 );
  
  int thresh_canny=0;
  while(circles.size()!=1)
  {
    if(circles.size()>1)
    {
      thresh_canny+=10;
      cv::HoughCircles(img_roi, circles, CV_HOUGH_GRADIENT,
		  2, max(img_roi.rows,img_roi.cols)/1.5, 60-thresh_canny, 20 );
    }
    else{
      
    }
  }
  
  for( size_t i = 0; i < circles.size(); i++ )
  {
	//cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//int radius = cvRound(circles[i][2]);
	// draw the circle center
	//cv::circle( img_roi, center, 3, cv::Scalar(255,0,0), -1, 8, 0 );
	// draw the circle outline
	//cv::circle( img_roi, center, radius, cv::Scalar(0,255,0), 3, 8, 0 );
	
	cv::Point center(cvRound(circles[i][0])+rect.x, cvRound(circles[i][1])+rect.y);
	cv::circle( img, center, 3, cv::Scalar(255,0,0), -1, 8, 0 );
  }
#if  defined(DEBUG)     
  cv::imshow( "circles", img_roi);    
  cv::waitKey(0);
#else
  //cv::imwrite(filename,img_roi);
  cv::imwrite(filename,img);
  std::cout<<"save to "<<filename<<std::endl;
#endif      
}
int main(int argc, char** argv)
{
  char ch[20];
  if(argc!=2)
  {
    std::cout<<"usage: ./exec path_to_imageDir"<<std::endl;
    exit(0);
  }
  for(int i=1;i<2;i++)
  {
    sprintf(ch,"%s/%d.jpg",argv[1],i);
    cv::Mat  img=cv::imread(ch,CV_LOAD_IMAGE_GRAYSCALE);
    sprintf(ch,"%s/res/%d.jpg",argv[1],i);
    detecter(img,ch);
  }
  

   
  return 0;
}

