public class Main {

    public static void main(String[] args) {
        long startTime = System.currentTimeMillis();   //获取开始时间
//        String outpath="/Users/applezhj/Desktop/";
        String outpath="";
        RingDetector ringDetector = new RingDetector("in.jpg",outpath);
        long endTime = System.currentTimeMillis(); //获取结束时间
        System.out.println("程序运行时间： " + (endTime - startTime) + "ms");
    }
}
