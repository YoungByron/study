package httpTest;

import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;

/**
 * �߳���
 * @author Administrator
 *
 */
public class DownThread2File extends Thread{
    
    private String sURL;
    private int startPos;
    private int currentPartSize;
    private RandomAccessFile currentPart;
    
    public int length;
    
    public DownThread2File(String httpURL, int startPos, int currentPartSize,
            RandomAccessFile currentPart) {
        
        this.sURL = httpURL;
        this.startPos = startPos;
        this.currentPartSize = currentPartSize;
        this.currentPart = currentPart;
    }

    @Override
    public void run() {
        try {
            URL url = new URL(sURL);
            HttpURLConnection conn;
            conn = (HttpURLConnection)url.openConnection();
            /* ���ó�ʱʱ�� */
            conn.setConnectTimeout(5000);
            /* ���÷�������ķ��� */
            conn.setRequestMethod("GET");
            
            conn.setRequestProperty("Accept", "image/gif, image/x-xbitmap, " +
                    "image/jpeg, image/pjpeg, application/x-shockwave-flash, " +
                    "application/vnd.ms-excel, application/vnd.ms-powerpoint, " +
                    "application/msword, application/x-silverlight, */*");
            
            conn.setRequestProperty("Accept-Language", "zh-CN");
            conn.setRequestProperty("Charset", "UTF-8");
            /* ���ظ�Socket�����Ӧ����������ó���ͨ������������Socket��ȡ������ */
            InputStream inStream = conn.getInputStream();
            inStream.skip(startPos);//Ѱ����ʼ���ص�λ��
            byte[] buffer = new byte[1024];
            int hasRead = 0;
            while(length < currentPartSize && (hasRead = inStream.read(buffer)) > 0){
                currentPart.write(buffer, 0, hasRead);
                length += hasRead;
            }
            currentPart.close();
            inStream.close();
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (ProtocolException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    
}