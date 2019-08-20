//自带库
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//elsdc文件
#include "misc.h"
#include "pgm.h"
#include "svg.h"
#include "polygon.h"
#include "ring.h"
#include "elsdc.h"

//parameter
//参数

//define the size of image
//固定输入图片大小 比较低的尺度大小
//用来检测圆（分辨率过高算法可能会检测不出来，另外分辨率过高速度会慢）
#define HEIGHT_RING 800
#define WIDTH_RING 800

//固定大小，比较高的尺寸大小
//用来截取比较清晰图片
#define HEIGHT_RING_H 2400
#define WIDTH_RING_H 2400

//HEIGHT_RING_H/HEIGHT_RING or WIDTH_RING_H/WIDTH_RING
#define SCALE 3

//剪切图片大小
//公章 （OUT_SIZE_S，OUT_SIZE_S）
//公章和签字 (OUT_SIZE_L,OUT_SIZE_L)
#define OUT_SIZE_S 600
#define OUT_SIZE_L 1200


//define red thre of the hsv image
//hsv参数阈值
//h
#define HUE_LOW_RING 110.0
#define HUE_HIGH_RING 130.0
//s
#define SATU_LOW_RING 25.5
#define SATU_HIGH_RING 255.0
//v
#define VALUE_LOW_RING 2.55
#define VALUE_HIGH_RING 255


/**
 *a ring out should larger than this value;
 *圆弧对应弧度最低阈值，过小的弧度对应的圆弧被认为是错检。
 */
#define MIN_RADIAN 3.14*0.15

/**
 *a ring out should larger than min_radius_ratio*max_radius;
 *取最大的圆半径为基准，过滤半径小于0.5倍的其他圆弧。
 */
#define MIN_RADIUS_RATIO 0.5

/**
 *a ring out should also larger than min_radius;
 *设置最低圆半径，过滤一些半径过小的圆。
 */
#define MIN_RADIUES 55.0

/**
 *threshold of ax/bx
 *长短轴之比，一般都是正着拍的，不会倾斜太多，过滤一些错检的椭圆。
 */
#define AXBX 0.9


//projection thre
//后处理步骤中 红色像素投影的阈值
#define PROJECTION_THRE 0.5

using namespace std;
using namespace cv;

void outputFunc(int ell_count, Ring *ell_out, int *ell_labels,int xsize, int ysize,string output_path,Mat &img,Mat &img_H){
    
    /**
     *compute the radians of all ell_out
     *计算所有圆弧对于的弧度
     */
    double ell_radian[ell_count];
    for(int i=0;i<ell_count;i++){
        double tmp1 =ell_out[i].ang_start;
        double tmp2 = ell_out[i].ang_end;
        double tmp3 = angle_diff( tmp1, tmp2 );
        double ring_angle = tmp1-tmp3;
        if(ring_angle<-M_PI)
            ring_angle=ring_angle+2*M_PI;
        if(double_equal(ring_angle, tmp2))
            ell_radian[i]=2*M_PI-tmp3;
        else
            ell_radian[i]=tmp3;
    }
    
    /**
     *compute the min_radius base on min_raian and ring`s randians
     *计算最大圆的半径
     */
    double max_radius=0;
    for(int i=0; i<ell_count; i++){
        if(ell_radian[i]>MIN_RADIAN&&ell_out[i].ax>max_radius)
            max_radius = ell_out[i].ax;
    }
    
    /**
     *select the ring from ell_out,which rdian>min_raidan && ax>min_radius
     *从原始结果中筛选弧度、半径满足要求的
     */
    Ring* ring_temp=( Ring *) calloc( ell_count,  sizeof(Ring) );
    int temp_count = 0;
    double temp_radian[ell_count];
    
    for(int i=0; i<ell_count;i++){
        if(ell_radian[i]>MIN_RADIAN && ell_out[i].ax>(MIN_RADIUS_RATIO*max_radius) && ell_out[i].ax>MIN_RADIUES&&(ell_out[i].ax/ell_out[i].bx)>AXBX){
            copy_ring( &ell_out[i], &ring_temp[temp_count]);
            //算弧长
            temp_radian[temp_count]=ell_radian[i]*ell_out[i].ax*ell_out[i].ax;
            temp_count++;
        }
    }
    
    /**
     *when two circle`s center is nearly,chose only one depend on the radian.
     *两段圆弧对应的圆心很接近的情况，认为弧度大对应的圆弧可靠性高。
     */
    Ring* ring_out=( Ring *) calloc( temp_count,  sizeof(Ring) );
    int ring_count = 0;
    for(int i=0; i<temp_count;i++){
        int flag=1;
        if(temp_radian[i]<0) continue;
        for(int j=i+1;j<temp_count;j++){
            double dx = ring_temp[i].cx - ring_temp[j].cx;
            double dy = ring_temp[i].cy - ring_temp[j].cy;
            double distance = dx*dx+dy*dy;
            double para1 = ring_temp[i].bx<ring_temp[j].bx?ring_temp[i].bx:ring_temp[j].bx;
            para1 = para1*para1;
            if(distance>para1)
                continue;
            else{
                if(temp_radian[i]>temp_radian[j])
                    temp_radian[j]=-1;
                else
                    flag = 0;
            }
        }
        if(flag == 1){
            copy_ring(&ring_temp[i],&ring_out[ring_count]);
            ring_count++;
        }
    }
    
    
    /**
     *cut target pic and write img
     *根据得到的圆弧将对应的图片切割出来，如果是椭圆弧，那么取长轴截取图片,并且对输出图像的红色区域判断。
     */
    int out_count = 0;
    if(1){
        for(int it=0;it<ring_count;it++){
            /*扩大1.1倍*/
            int size_x = round(ring_out[it].ax*2*1.1);
            int size_y = round(ring_out[it].ax*2*1.1);
            Mat outIm(size_y,size_x,CV_8UC3,Scalar(255,255,255));
            for(int x=0;x<size_x;x++)
                for(int y=0;y<size_y;y++){
                    int x_target = round(ring_out[it].cx-ring_out[it].ax*1.1+x);
                    int y_target = round(ring_out[it].cy-ring_out[it].ax*1.1+y);
                    if(x_target>=0&&y_target>=0&&x_target<img.cols&&y_target<img.rows){
                        outIm.at<Vec3b>(y,x) = img.at<Vec3b>(y_target,x_target);
                    }
                }
            
            /*对准备输出的图片进行判断 用红色区域投影 即检测出的圆弧中线的垂线方向投影 红色像素区域是否有0.5*/
            bool flag = true;
            //水平和垂直投影
            //红色区域掩码图
            Mat mask;
            inRange(outIm, Scalar(HUE_LOW_RING,SATU_LOW_RING,VALUE_LOW_RING), Scalar(HUE_HIGH_RING,SATU_HIGH_RING,VALUE_HIGH_RING), mask);
            double min_len = size_y * PROJECTION_THRE;
            Mat ma(outIm.cols, 1, CV_8UC1, Scalar(0));
            Mat mb(outIm.cols, 1, CV_8UC1, Scalar(0));
            //mask取值是255不是1
            reduce(mask, ma, 1, REDUCE_MAX);
            reduce(mask, mb, 0, REDUCE_MAX);
            Scalar sum_s1 = sum(ma);
            Scalar sum_s2 = sum(mb);
            double sum = min(sum_s1.val[0],sum_s2.val[0]);
            sum = sum/255;
            if(sum>min_len)
                flag = true;
            else
                flag = false;
            
            //            if(flag){
            //                out_count++;
            //                //小图
            //                int size_s = OUT_SIZE_S;
            //                Mat outIm_s(size_s,size_s,CV_8UC3,Scalar(255,255,255));
            //                for(int x=0;x<size_s;x++)
            //                    for(int y=0;y<size_s;y++){
            //                        int x_target = round(ring_out[it].cx-0.5*size_s+x);
            //                        int y_target = round(ring_out[it].cy-0.5*size_s+y);
            //                        if(x_target>=0&&y_target>=0&&x_target<img.cols&&y_target<img.rows){
            //                            outIm_s.at<Vec3b>(y,x) = img.at<Vec3b>(y_target,x_target);
            //                        }
            //                    }
            //                imwrite(output_path+to_string(out_count)+"_S.jpg", outIm_s);
            //                //大图
            //                int size_l = OUT_SIZE_L;
            //                Mat outIm_l(size_l,size_l,CV_8UC3,Scalar(255,255,255));
            //                for(int x=0;x<size_l;x++)
            //                    for(int y=0;y<size_l;y++){
            //                        int x_target = round(ring_out[it].cx-0.5*size_l+x);
            //                        int y_target = round(ring_out[it].cy-0.5*size_l+y);
            //                        if(x_target>=0&&y_target>=0&&x_target<img.cols&&y_target<img.rows){
            //                            outIm_l.at<Vec3b>(y,x) = img.at<Vec3b>(y_target,x_target);
            //                        }
            //                    }
            //                imwrite(output_path+to_string(out_count)+"_L.jpg", outIm_l);
            //            }
            if(flag){
                out_count++;
                //小图 因为清晰度问题，所以需要在img_H上截取
                int size_s = OUT_SIZE_S;
                Mat outIm_s(size_s,size_s,CV_8UC3,Scalar(255,255,255));
                for(int x=0;x<size_s;x++)
                    for(int y=0;y<size_s;y++){
                        int x_target = round( ring_out[it].cx*SCALE - 0.5*size_s + x );
                        int y_target = round( ring_out[it].cy*SCALE - 0.5*size_s + y );
                        if(x_target>=0&&y_target>=0&&x_target<img_H.cols&&y_target<img_H.rows){
                            outIm_s.at<Vec3b>(y,x) = img_H.at<Vec3b>(y_target,x_target);
                        }
                    }
                imwrite(output_path+to_string(out_count)+"_S.jpg", outIm_s);
                //大图
                int size_l = OUT_SIZE_L;
                Mat outIm_l(size_l,size_l,CV_8UC3,Scalar(255,255,255));
                for(int x=0;x<size_l;x++)
                    for(int y=0;y<size_l;y++){
                        int x_target = round( ring_out[it].cx*SCALE - 0.5*size_l + x );
                        int y_target = round( ring_out[it].cy*SCALE - 0.5*size_l + y );
                        if(x_target>=0&&y_target>=0&&x_target<img_H.cols&&y_target<img_H.rows){
                            outIm_l.at<Vec3b>(y,x) = img_H.at<Vec3b>(y_target,x_target);
                        }
                    }
                imwrite(output_path+to_string(out_count)+"_L.jpg", outIm_l);
            }
        }
        
    }
    
    printf("Number of rings detected = %d\n",out_count);
    
    if(ring_out !=NULL) free(ring_out);
}

void process(Mat &inputImg,String output_path){
    PImageDouble in;     /* input image */
    PImageInt    out;    /* output image having the same size as 'in'; the pixels
                          supporting a certain geometric primitive are marked
                          with the same label */
    
    int ell_count = 0;   /* number of detected ellipses */
    int *ell_labels=NULL;/* the pixels supporting a certain ellipse are marked
                          with the same unique label */
    Ring *ell_out = NULL;/* array containing the parameters of the detected
                          ellipses; correlated with ell_labels, i.e. the i-th
                          element of ell_labels is the label of the pixels
                          supporting the ellipse defined by the parameters
                          ell[i] */
    
    int poly_count = 0;  /* number of detected polygons */
    int *poly_labels=NULL;/* the pixels supporting a certain polygon are marked
                           with the same unique label */
    Polygon *poly_out=NULL;/* array containing the parameters of the detected
                            polygons; correlated with poly_labels, i.e. the i-th
                            element of ell_labels is the label of the pixels
                            supporting the polygon defined by the parameters
                            poly[i] */
    int i,j;
    
    Mat resizeImg,resizeImg_H;
    Mat hsvImg;
    Mat mask;
    int xsize = inputImg.cols;
    int ysize = inputImg.rows;
    
    if(inputImg.channels()!=3)
        error("Please input color image");
    
    //固定设置输入图片短边为800像素，长边随比例缩放。
    if(xsize>ysize){
        resize(inputImg, resizeImg, Size(round(xsize*HEIGHT_RING/double(ysize)),int(HEIGHT_RING)));
        resize(inputImg, resizeImg_H, Size(round(xsize*HEIGHT_RING_H/double(ysize)),int(HEIGHT_RING_H)));
    }else{
        resize(inputImg, resizeImg, Size(int(WIDTH_RING),round(ysize*WIDTH_RING/double(xsize))));
        resize(inputImg, resizeImg_H, Size(int(WIDTH_RING_H),round(ysize*WIDTH_RING_H/double(xsize))));
    }
    
    xsize = resizeImg.cols;
    ysize = resizeImg.rows;
    
    cvtColor(resizeImg, hsvImg, COLOR_RGB2HSV);
    
    //hsv阈值
    inRange(hsvImg, Scalar(HUE_LOW_RING,SATU_LOW_RING,VALUE_LOW_RING), Scalar(HUE_HIGH_RING,SATU_HIGH_RING,VALUE_HIGH_RING), mask);
    
    Mat grayImg(ysize,xsize,CV_8UC3,Scalar(255,255,255));
    
    resizeImg.copyTo(grayImg, mask);
    
    cvtColor(grayImg, grayImg, COLOR_RGB2GRAY);
    
    /*
     *腐蚀操作 grayImg
     */
    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
    erode(grayImg, grayImg, element);
    
    /*
     *转变数据格式，传给elsdc算法
     */
    in = new_PImageDouble( xsize, ysize );
    for(int x=0;x<xsize;x++)
        for(int y=0;y<ysize;y++){
            in->data[x+y*xsize] = double(grayImg.data[x+y*xsize]);
        }
    
    /* create and initialize with 0 output label image */
    out = new_PImageInt_ini( in->xsize, in->ysize, 0 );
    
    /*
     call detection procedure
     ell_count 检测圆弧个数
     ell_out 圆弧数据
     */
    ELSDc( in, &ell_count, &ell_out, &ell_labels, &poly_count, &poly_out,
          &poly_labels, out );
    
    /*
     *后处理步骤，对圆弧进行处理，并且剪切输出图片
     */
    outputFunc(ell_count, ell_out, ell_labels,xsize,ysize,output_path,resizeImg,resizeImg_H);
    
    
    /*
     *释放内存
     */
    free_PImageInt(out);
    if( ell_out != NULL ) {free(ell_out); free(ell_labels);}
    if( poly_out != NULL )
    {
        for( i=0; i<poly_count; i++ )
            free(poly_out[i].pts);
        free(poly_out);
        free(poly_labels);
    }
    return;
}

int main(int argc, const char * argv[]) {
    String im_path;
    String output_path;
    if( argc < 2 )
        error("usage: ./elsdc image_name out_put_path");
    else if(argc == 2){
        im_path = argv[1];
        output_path ="";
    }else if(argc == 3){
        im_path = argv[1];
        output_path =argv[2];
    }else{
        error("usage: ./elsdc image_name out_put_path");
    }
    //read img
    Mat inputImg = imread(im_path);
    //main process
    process(inputImg,output_path);
}


