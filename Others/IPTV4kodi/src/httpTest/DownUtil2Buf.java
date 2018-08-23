package httpTest;

import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;


public class DownUtil2Buf {
    /* ������Դ��·��--url */
    private String sURL;
    //private String targetFile;
    private int threadNum;
    /* ���߳����� */
    private DownThread2Buf[] threads;
    private int fileSize;
	private boolean bSuccess = false;
    public DownUtil2Buf(String httpURL, /*byte[] xx, */int threadNum) {
        
        this.sURL = httpURL;
//        this.targetFile = targetFile;
        threads = new DownThread2Buf[threadNum];
        this.threadNum = threadNum;
    }
    
    public void download(OutputStream os) throws Exception{
        bSuccess = false;
       URL url = new URL(sURL);
        HttpURLConnection conn;
        conn = (HttpURLConnection)url.openConnection();
    
        conn.setConnectTimeout(5000);
        
        conn.setRequestMethod("GET");
        
        conn.setRequestProperty("Accept", "image/gif, image/x-xbitmap, " +
                "image/jpeg, image/pjpeg, application/x-shockwave-flash, " +
                "application/vnd.ms-excel, application/vnd.ms-powerpoint, " +
                "application/msword, application/x-silverlight, */*");
        
        conn.setRequestProperty("Accept-Language", "zh-CN");
        conn.setRequestProperty("Charset", "UTF-8");
        conn.setRequestProperty("Connection", "Keep-Alive");
        /* �õ��ļ��Ĵ�С */
        fileSize = conn.getContentLength();

		// ��ȡ������Ӧͷ�ֶ�
        HttpRequestor.timeLog("\r\n+++++++++++++ ������Ӧͷ�ֶ�: ");

		for (int n = 0;; n++) {
			String sTmp = conn.getHeaderFieldKey(n);
			String sTmp2 = conn.getHeaderField(n);
			if (sTmp2 == null) {
				break;
			}
			if (n == 0) {
				HttpRequestor.timeLog(sTmp2);
				os.write(sTmp2.getBytes());
			} else {
				HttpRequestor.timeLog(sTmp + ": " + sTmp2);
				os.write((sTmp + ": " + sTmp2).getBytes());
			}
			os.write(13);
			os.write(10);
		}
		os.write(13);
		os.write(10);

        conn.disconnect();
        int currentPartSize = fileSize / threadNum + 1;
//        RandomAccessFile file = new RandomAccessFile(targetFile, "rw");
//        /* ���ñ����ļ��Ĵ�С */
//        file.setLength(fileSize);
//        file.close();
        byte[] xx = new byte[fileSize];
        
        for (int i = 0; i < threadNum; i++) {
            int startPos = i * currentPartSize;
            /* ÿ���߳�ʹ��һ��RandomAccessFile�������� */
//            RandomAccessFile currentPart = new RandomAccessFile(targetFile, "rw");
            /* ��λ�ļ����ص�λ��*/
//            currentPart.seek(startPos);
            threads[i] = new DownThread2Buf(sURL, startPos, Math.min(fileSize-(i * currentPartSize),currentPartSize), xx); 
            threads[i].start();
        }
        
        for (int i = 0; i < threadNum; i++) {
        	
        	while(threads[i].currentPartSize != threads[i].length)
        	{
        		Thread.sleep(50);
        	}
    		os.write(xx, threads[i].startPos, threads[i].length);
        }

//		while(!bSuccess)
//		{
//			Thread.sleep(500);
			HttpRequestor.timeLog("��������ʣ�" + String.format("%.6f",getCompleteRate()));
//		}
		

		return;
    }
    
    public double getCompleteRate(){
        int sumSize = 0;
        for (int i = 0; i < threadNum; i++) {
            sumSize += threads[i].length;
        }
        double xxx = sumSize * 1.0 / fileSize;
//        if(xxx>1)
//        {
//        	return -1;
//        }
        bSuccess = (sumSize == fileSize);
        HttpRequestor.timeLog("�����ˣ�"+ sumSize + "/"+ 	fileSize);
        return xxx;
    }
}