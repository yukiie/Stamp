import org.opencv.core.*;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.io.IOException;
import java.util.*;

import static org.opencv.core.CvType.CV_8UC1;
import static org.opencv.core.CvType.CV_8UC3;

public class RingDetector {

    //elsdc 直接移动到工程所在目录下，或者加路径,然后修改runElsdc函数
//    private String elsdcPath="../../ELSDC/";


    private ArrayList<RingData> rings = new ArrayList<>();
    //input image 保存输入图片
    private Mat img;
    /*
    resize img
    resizeImg是个分辨率低的图片，给elsdc程序
    resizeImg是个高分辨率图片，用来截取输出结果
     */
    private Mat resizeImg;
    private Mat grayImg;//红色区域转灰度
    private Mat resizeImg_h;
    /*
    为了记录缩放比例，scale1是img/resizeImg,scale2是resizeImg_h/resizeImg
     */
    private double scale1;
    private double scale2;
    /*
    图片参数
     */
    private double height = 800;
    private double width=800;
    private double height_h=2400;
    private double width_h=2400;
    /*
    HSV阈值
     */
    private double hue_low=110;
    private double hue_high=130;
    private double saturation_low =25.5;
    private double saturation_high =255;
    private double value_low = 2.55;
    private double value_high = 255;
    /**
     *a ring out should larger than this value;
     *圆弧对应弧度最低阈值，过小的弧度对应的圆弧被认为是错检。
     */
    private double min_radian=3.14*0.15;
    /**
     *a ring out should larger than min_radius_ratio*max_radius;
     *取最大的圆半径为基准，过滤半径小于0.5倍的其他圆弧。
     */
     private double min_radius_ratio = 0.5;
    /**
     *a ring out should also larger than min_radius;
     *设置最低圆半径，过滤一些半径过小的圆。
     */
     private double min_radius = width/800*55.0;

    /**
     *threshold of ax/bx
     *长短轴之比，一般都是正着拍的，不会倾斜太多，过滤一些错检的椭圆。
     */
    private double axbx = 0.9;

    //projection thre
    //后处理步骤中 红色像素投影的阈值
    private double projection_thre = 0.5;
    /*
    输出图片大小
     */
    private double out_size_s = 600;
    private double out_size_l = 1200;

    //接受参数 输入图片路径，输出路径
    RingDetector(String imgPath, String outPath) {
        run(imgPath,outPath);
    }

    private void run(String imgPath,String outPath){
        //load opencv lib
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        /*
        step1 extract red region and convert img to pgm fomat file in.pgm
         */
        converImg2Pgm(imgPath);
        /*
        step2 run elsdc,and get ellipse.txt file
         */
        runElsdc("in.pgm");
        /*
        step3 读取文本数据
         */
        readRings();
        /*
        step4 处理文本数据
         */
        dealData();
        /*
        step5 输出图片
         */
        outPic(outPath);
    }
    //elsdc need pgm input format
    //trans rgb2hsv to extract the red region, then write pgm use mask.
    private void converImg2Pgm(String imgPath)  {
        img = Imgcodecs.imread(imgPath);
        int ysize = img.rows();
        int xsize = img.cols();
        resizeImg = new Mat();
        resizeImg_h = new Mat();
        if(xsize>ysize) {
            Imgproc.resize(img, resizeImg, new Size((int) Math.round(xsize * height / (double) ysize), (int) height));
            Imgproc.resize(img, resizeImg_h, new Size((int) Math.round(xsize * height_h / (double) ysize), (int) height_h));
        }
        else{
            Imgproc.resize(img, resizeImg, new Size((int)width, (int) Math.round(xsize * width / (double) xsize)));
            Imgproc.resize(img, resizeImg_h, new Size((int)width_h, (int) Math.round(xsize * width_h / (double) xsize)));
        }
        scale1 = img.rows()/resizeImg.rows();
        scale2 = resizeImg_h.rows()/resizeImg.rows();

        Mat hsvImg = new Mat();
        //first draw the red background then convert to gray img.
        Imgproc.cvtColor(resizeImg, hsvImg, Imgproc.COLOR_RGB2HSV);
        Mat mask = new Mat();

        Core.inRange(hsvImg,new Scalar(hue_low,saturation_low,value_low),new Scalar(hue_high,saturation_high,value_high),mask);

        grayImg = new Mat(resizeImg.rows(),resizeImg.cols(),CV_8UC3,new Scalar(255,255,255));
        resizeImg.copyTo(grayImg,mask);
        Imgproc.cvtColor(grayImg, grayImg, Imgproc.COLOR_RGB2GRAY);

        //腐蚀操作
        Mat structImage = Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3));
        Mat temp=new Mat();
        Imgproc.erode(grayImg, temp, structImage, new Point(-1, -1), 2);
//        grayImg=temp;

        Imgcodecs.imwrite("in.pgm",temp);
    }

    //run elsdc by command elsdc
    //源程序默认在目录下生成out_ellipse.txt
    private void runElsdc(String imgPath) {
        List<String> command = new ArrayList<String>();
        String os = System.getProperty("os.name");
        if (os.toLowerCase().startsWith("win")) {
            //windows
            command.add("elsdc.exe");
            command.add(imgPath);
        } else {
            //linux & mac
            command.add("./elsdc");
            command.add(imgPath);
        }
        try {
            ProcessBuilder builder = new ProcessBuilder(command);
            builder.command(command);
            Process p = builder.start();
            //need to wait
            if (p.isAlive()) {
                p.waitFor();
            }
//            p.waitFor();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //read elsdc result
    private void readRings() {
        //the elsdc output
        String filePath = "out_ellipse.txt";
        List<Double> doubles = new ArrayList<Double>();
        RingData inData;
        try {
            Scanner sc = new Scanner(new File(filePath));
            while (sc.hasNextDouble()) {
                doubles.add(sc.nextDouble());
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
        }
        Iterator<Double> it = doubles.iterator();
        while (it.hasNext()) {
            double[] readData = new double[11];
            int count = 0;
            while (it.hasNext() && count < 11) {
                readData[count++] = it.next();
            }
            inData = new RingData(readData);
            rings.add(inData);
        }
    }


    //处理elsdc得到圆弧
    private void dealData(){
        /**
         *compute the radians of all ell_out
         *计算所有圆弧对于的弧度
         */
        double radian=0;
        double max_radius=0;
        for(int i=0;i<rings.size();i++){
            RingData r= rings.get(i);
            double tmp1 = r.ang_start;
            double tmp2 = r.ang_end;
            double tmp3 = angle_diff(tmp1,tmp2);
            double ring_angle = tmp1 - tmp3;
            if(ring_angle<-Math.PI)
                ring_angle = ring_angle + 2*Math.PI;
            if(angle_diff(ring_angle,tmp2)<0.01)
                radian = 2*Math.PI-tmp3;
            else
                radian = tmp3;
            //弧度过小就删除 顺带删除长短轴之比小于阈值的
            if((radian<min_radian) || ((r.ax/r.bx)<axbx )){
                rings.remove(i);
                i--;
            }else{
                max_radius=max_radius>r.ax?max_radius:r.ax;
                r.ring_length = radian*r.ax;
            }
        }
        /*
        删除半径过小的
         */
        for(int i=0;i<rings.size();i++) {
            RingData r= rings.get(i);
            if(r.ax<(max_radius*min_radius_ratio)){
                rings.remove(i);
                i--;
            }
        }
        /**
         *when two circle`s center is nearly,chose only one depend on the radian.
         *两段圆弧对应的圆心很接近的情况，认为弧长大对应的圆弧可靠性高。
         */
        //根据弧长排序 降序排列
        Collections.sort(rings, new Comparator<RingData>() {
            @Override
            public int compare(RingData r1, RingData r2) {
                double diff = r1.ring_length - r2.ring_length;
                if (diff < 0.0) {
                    return 1;
                } else if (diff > 0.0) {
                    return -1;
                }
                return 0; //相等为0
            }
        });
        double dx,dy,distance,para;
        for(int i=0;i<rings.size()-1;i++){
            RingData r1 = rings.get(i);
            for(int j=i+1;j<rings.size();j++){
                RingData r2 = rings.get(j);
                dx = r1.cx - r2.cx;
                dy = r1.cy - r2.cy;
                distance = dx*dx+dy*dy;
                para = r1.bx<r2.bx?r1.bx:r2.bx;
                para = para*para;
                if(distance>para)
                    continue;
                else{
                    rings.remove(j);
                    j--;
                }
            }
        }

        /*
        判断红色区域阈值
         */
        for(int i=0;i<rings.size();i++){
            RingData r = rings.get(i);
            int size_r = (int)(r.ax*2);
            int size_r_2 = (int)Math.round(r.ax);
            Mat tempImg=new Mat(size_r,size_r,CV_8UC3,new Scalar(255,255,255));
            for(int x=0;x<size_r;x++)
                for(int y=0;y<size_r;y++){
                    int target_x = (int)r.cx + x - size_r_2;
                    int target_y = (int)r.cy + y - size_r_2;
                    if(target_x>=0&&target_y>=0&&target_x<resizeImg.rows()&&target_y<resizeImg.cols())
                        tempImg.put(y,x, resizeImg.get(target_y,target_x));
                }
            Mat mask=new Mat();
            Core.inRange(tempImg,new Scalar(hue_low,saturation_low,value_low),new Scalar(hue_high,saturation_high,value_high),mask);
            double min_len = size_r * projection_thre ;
            Mat ma = new Mat(tempImg.cols(),1,CV_8UC1,new Scalar(0));
            Mat mb = new Mat(tempImg.cols(),1,CV_8UC1,new Scalar(0));
            Core.reduce(mask,ma,1,Core.REDUCE_MAX);
            Core.reduce(mask,mb,0,Core.REDUCE_MAX);
            Scalar sum_s1 = Core.sumElems(ma);
            Scalar sum_s2 = Core.sumElems(mb);
            double sum_s = Math.min(sum_s1.val[0],sum_s2.val[0]);
            sum_s = sum_s/255;
            if(sum_s<min_len) {
                rings.remove(i);
                i--;
            }
        }
    }

    //根据rings，输出圆图片。
    private void outPic(String outPath){

        //_S.jpg
        for(int i=0;i<rings.size();i++){
            RingData r = rings.get(i);
            int size_r = (int)(out_size_s*0.5);
            int c_x = (int)Math.round(r.cx*scale2);
            int c_y = (int)Math.round(r.cy*scale2);
            Mat outImg = new Mat((int)out_size_s,(int)out_size_s,CV_8UC3,new Scalar(255,255,255));
            for(int x=0;x<(size_r*2);x++)
                for(int y=0;y<(size_r*2);y++){
                    int target_x = c_x + x - size_r;
                    int target_y = c_y + y - size_r;
                    if(target_x>=0&&target_y>=0&&target_x<resizeImg_h.rows()&&target_y<resizeImg_h.cols())
                        outImg.put(y,x, resizeImg_h.get(target_y,target_x));
;                }
            String out_name;
            if(outPath=="")
                out_name = Integer.toString(i) + "_S.jpg";
            else
                out_name =outPath + File.separator + Integer.toString(i) + "_S.jpg";
            Imgcodecs.imwrite(out_name, outImg);
        }
        //_L.jpg
        for(int i=0;i<rings.size();i++){
            RingData r = rings.get(i);
            int size_r = (int)(out_size_l*0.5);
            int c_x = (int)Math.round(r.cx*scale2);
            int c_y = (int)Math.round(r.cy*scale2);
            Mat outImg = new Mat((int)out_size_l,(int)out_size_l,CV_8UC3,new Scalar(255,255,255));
            for(int x=0;x<(size_r*2);x++)
                for(int y=0;y<(size_r*2);y++){
                    int target_x = c_x + x - size_r;
                    int target_y = c_y + y - size_r;
                    if(target_x>=0&&target_y>=0&&target_x<resizeImg_h.rows()&&target_y<resizeImg_h.cols())
                        outImg.put(y,x, resizeImg_h.get(target_y,target_x));
                }
            String out_name;
            if(outPath=="")
                out_name = Integer.toString(i) + "_L.jpg";
            else
                out_name =outPath + File.separator + Integer.toString(i) + "_L.jpg";
            Imgcodecs.imwrite(out_name, outImg);
        }
        System.out.println("detect "+rings.size()+" rings.");
    }

    //计算角度差
    private double angle_diff(double a,double b){
        a -=b;
        while(a<=-Math.PI) a += (Math.PI*2);
        while(a>Math.PI) a-=(Math.PI*2);
        if(a<0.0) a=-a;
        return a;
    }
}

class RingData{
    double x1,y1,x2,y2,cx,cy,ax,bx,theta,ang_start,ang_end;
    double ring_length=0;
    RingData(double[] data){
        this.x1 = data[0];
        this.y1 = data[1];
        this.x2 = data[2];
        this.y2 = data[3];
        this.cx = data[4];
        this.cy = data[5];
        this.ax = data[6];
        this.bx = data[7];
        this.theta = data[8];
        this.ang_start = data[9];
        this.ang_end = data[10];
    }
}