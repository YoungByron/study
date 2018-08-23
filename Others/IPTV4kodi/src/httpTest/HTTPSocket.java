package httpTest;

import java.io.*;
import java.net.*;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class HTTPSocket
{
private static final int BLOCKSIZE = 10240;
//private static OutputStream mLocalOS = null;
private static Socket mSocket;
private static BufferedWriter out;
private static BufferedReader in;
private static String sHeader = "";
private static String sHeaderBuf = "";
private static List<String> listFileStr = new LinkedList<String>();
private static List<String> listHtmlStr = new LinkedList<String>();
private static Thread bufThread = null;
private static boolean bStopThread = false;
private static int doneSize = 0;
private static byte[] bufBytes = null;
private static int iPort = 0;
private static String sHost = "";
private static int iPackages = 0;
public static Map<String, Integer> mTimeMap = new HashMap<String, Integer>();

//    public static void main(String[ ] args) throws UnknownHostException, IOException
//    {
//        String host = "111.13.42.8";
//        int port = 80;
//        newHttpRequest(host, port, "/PLTV/88888888/224/3221225684/index.m3u8");
//    }

	public static void newHttpRequest(String host, int port, String webPath, boolean bDownToBuf, OutputStream mLocalOS)
			throws UnknownHostException, IOException, MalformedURLException {
		if(sHost.length() == 0)
		{
			sHost = host;
			iPort = port;
		}
		String line = "";
		int iDataLen = 0;
		boolean bIsStream = false;
		if(/*(!bDownToBuf) && */(sHeaderBuf.length() == 0) || !webPath.contains(".ts"))
		{
			connectAndRequest(sHost, iPort, webPath);

			// ��ȡ��Ӧ ͷ��
			sHeader = "";
			while ((line = in.readLine()) != null) {
				if (line.contains("Content-Length: ")) {
					iDataLen = Integer.parseInt((line.split(": "))[1]);
					HttpRequestor.timeLog("" + iDataLen);
				}
				if (line.contains("Content-Type: video/mpeg")) {
					bIsStream = true;
				}
				if (line.contains("Connection: Close")) {
					HttpRequestor.timeLog("=====================Connection: Close=========================");;
				}
				HttpRequestor.timeLog(line);
				if (line.contains("Location: ")) {
					HttpRequestor.timeLog("=========================" + line);;
					break;
				}
				if(sHeader.isEmpty() && !line.contains("HTTP"))
				{
					HttpRequestor.timeLog("=====================����������ͷ��һ�в���=\"HTTP\"���ж����ӣ���=======================");;
					break;
				}
				sHeader += line + "\r\n";
				if (line.length() == 0) {
					HttpRequestor.timeLog("=======================line.length() == 0=======================");;
					//break;
				}
			}
		}
		else
		{
			sHeader = sHeaderBuf; 
			bIsStream = true;
			iDataLen = bufBytes.length;
		}

		if (line.length() > 0) {
			destroyIPTVConnection();
			HttpRequestor.timeLog(line.substring(10));
			jump302(line, mLocalOS);
		} else {
			if(bDownToBuf){
				sHeaderBuf = sHeader; //buffering
			}
			else if(mLocalOS != null)
				mLocalOS.write(sHeader.getBytes());
			
			sHeader = "";

			
			// ��ȡweb body
			if (!bIsStream) {//�ı�html
				String sBody = "";
				String sLastTs = "";
				int iDuaration = 0;
				mTimeMap.clear();
				while ((iDataLen != 0) && (line = in.readLine()) != null) {
					iDataLen -= line.length() + 2;
					sBody += line + "\r\n";
					listHtmlStr.add(line + "\r\n");
					HttpRequestor.timeLog(line);
					if (line.contains("#EXTINF:")) {
						String tm = line.split(":")[1];
						iDuaration = (int) Float.parseFloat(tm.split(",")[0]);
					}
					if (line.contains(".ts")) {
						sLastTs = line + "\r\n";
						// HttpRequestor.timeLog("GET " + webPath.substring(0,
						// webPath.lastIndexOf("/") + 1) + line + "
						// HTTP/1.1\r\n");

//						if (listFileStr.contains(line) == false)
//							listFileStr.add(line);
					    mTimeMap.put(line, iDuaration);
					}
						
					/*
					 * if (line.contains(".ts")) { out.write("GET " +
					 * webFile.substring(0, webFile.lastIndexOf("/") + 1) + line
					 * + " HTTP/1.1\r\n");
					 * out.write("User-Agent: Daum PotPlayer\r\n");
					 * out.write("Cache-Control: no-cache\r\n\r\n");
					 * out.flush(); }
					 */
				}
//				�滻Ϊ���һ��ts�ļ���
/*				for(int i = 0; i<listHtmlStr.size(); i++)
				{
					if(listHtmlStr.get(i).contains(".ts"))
					{
//						listHtmlStr.set(i, sLastTs);
						mLocalOS.write(sLastTs.getBytes());
					}
					else
					{
						mLocalOS.write(listHtmlStr.get(i).getBytes());
					}
				}
				listHtmlStr.clear();*/
				if(mLocalOS != null)
					mLocalOS.write(sBody.getBytes());

			} else {//������������
				byte[] cbuf = new byte[BLOCKSIZE];
				int off = 0;
				int len = 0;
				int iTT = 0;

				//buffering
				if(bDownToBuf){
					bufBytes = new byte[iDataLen];
				}
				
				if(doneSize > 0)
				{
					if(mLocalOS != null)
						mLocalOS.write(bufBytes, 0, doneSize);
					iDataLen -= doneSize;
					doneSize = 0;
				}
				
				Date d = new Date();
				int sec = d.getSeconds();

				byte[] buffer = new byte[BLOCKSIZE];
//				int len = 0;
//
//				long tlen = 0;
				long lastsecTlen = 0;

				HttpRequestor.timeLog("*************** ���أ� ��ʼ");
				
				int iReturnSec = (int) mTimeMap.get(webPath.split("/")[webPath.split("/").length-1]);
//				mTimeMap.remove(webPath.split("/")[webPath.split("/").length-1]);
				
				iReturnSec = ((new Date()).getSeconds() + (iReturnSec + 2)) % 60;
				while ((len != -1) && !bStopThread){
					// mSocket.getInputStream().read(cbuf);
					int count = 0;
//					int iSec = ((new Date()).getSeconds() + 5) % 60;
					while (count == 0) {
//						if (iSec == (new Date()).getSeconds()) {
//							HttpRequestor.timeLog( "<==         destroyIPTVConnection()          -->");
//							destroyIPTVConnection();
//							return;
//						}
						
						if (iReturnSec == (new Date()).getSeconds()) {
							break;
						}
						count = mSocket.getInputStream().available();
						if(count == 0) {
						try {
							Thread.sleep(5);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						}
					}
					
					if  (iReturnSec == (new Date()).getSeconds()) {
						HttpRequestor.timeLog("*************** ʱ���ѵ�����ֹ����");
						break;
					}
					 //buffering
					if(bDownToBuf){
						len = mSocket.getInputStream().read(bufBytes, doneSize, Math.min(count, BLOCKSIZE));
						doneSize += len;

					}
					else {
						len = mSocket.getInputStream().read(cbuf, 0, Math.min(count, BLOCKSIZE));
						if(mLocalOS != null)
							//HttpRequestor.timeLog("================================ write()" + len);
							mLocalOS.write(cbuf, 0, len);
							//HttpRequestor.timeLog("*************** flush(" +iTT);
							mLocalOS.flush();
							//HttpRequestor.timeLog("*************** flush)" +iTT);
					}
					
					iTT += len;
					iDataLen -= len;
					if (iDataLen <= 10000)
						HttpRequestor.timeLog("��ʣ���ֽ�=" + iDataLen + " ���һ�ζ�ȡ-->" + len + " Ŀ���ܳ�-->" + iTT);
					
					d = new Date();
					if ((sec) != (d.getSeconds())) {
						double fSpeed = (iTT - lastsecTlen) / 1024.0 / 1024.0;
						
						String sss = "";
						for(int x=0;x<(fSpeed*20);x++)
						{
							if(x>=20)
								sss += "#";
							else
								sss += "=";
						}
						HttpRequestor.timeLog("*************** �ٶȣ� " + String.format("%.2f", fSpeed) + "M/s " + sss);
						if ((fSpeed < 0.01) && (lastsecTlen != 0)) {
							HttpRequestor.timeLog("*************** �ٶȣ� ���ͣ�������������");
//							break;
						}
						lastsecTlen = iTT;
						sec = d.getSeconds();
					}
					;
					if (iDataLen <= 0)
					{
						HttpRequestor.timeLog("*************** ���أ� �ɹ�����������");
						break;
					}
				}

				// while ((iDataLen != 0) && ((len = in..read(cbuf)) > 0)) {
				// iDataLen -= len;
				// //sb1.append(cbuf);
				// }
				// HttpRequestor.timeLog(String.valueOf(cbuf));
				// String ccc = cbuf;
				// cbuf.toString()
				
				iPackages++;
				if (iPackages > 0) {
					iPackages = 0;
					destroyIPTVConnection();
				}
			}
		}
	}

	public static void connectAndRequest(String host, int port, String webPath)
			throws UnknownHostException, IOException {
		if(port==-1) port=80;
		
		if((mSocket == null) || (mSocket.isConnected() == false))
		{
			mSocket = new Socket(host, port);
			HttpRequestor.timeLog("�½����ӣ� ���ض˿�=" + mSocket.getLocalPort() + " ������=" + host + ":" + port);
	        
	        out = new BufferedWriter(new OutputStreamWriter(mSocket.getOutputStream()));
	        in = new BufferedReader(new InputStreamReader(mSocket.getInputStream()));
		}
		else
		{
			
			HttpRequestor.timeLog("ʹ�õ����ӣ� ���ض˿�=" + mSocket.getLocalPort() + " ������=" + mSocket.getInetAddress().getHostAddress() + ":" + port);
		}
        StringBuffer sb = new StringBuffer();
        //��������ͷ
        sb.append("GET "+ webPath +" HTTP/1.1\r\n")
          .append("Host:"+ host + "\r\n")
          .append("\r\n");
        
		out.write(sb.toString());
		out.flush();
		HttpRequestor.timeLog("��������ͷ�� " + sb.toString());
	}

	protected static void writeOS(String line) {
		// TODO Auto-generated method stub
		
	}

	public static void jump302(String sUrl, OutputStream localOS) throws MalformedURLException, UnknownHostException, IOException {
		URL localURL = new URL(sUrl.substring(10));
		HttpRequestor.timeLog("\r\njump302 " + localURL.getHost());
		HttpRequestor.timeLog("jump302 " + ""+localURL.getPort());
		HttpRequestor.timeLog("jump302 " + localURL.getPath());
		HttpRequestor.timeLog("jump302 " + localURL.getFile());
		HttpRequestor.timeLog("jump302 " + "\r\n");
		
		sHost = localURL.getHost();
		iPort = localURL.getPort();
		newHttpRequest(localURL.getHost(), localURL.getPort(), localURL.getFile(), false, localOS);
	}

	public static void newUrl(String sIPTV_URL, OutputStream localOS) {
		HttpRequestor.timeLog(sIPTV_URL + " newUrl***************** \r\n");
//		mLocalOS = localOS;
		try {
			URL localURL = new URL(sIPTV_URL);
			try {
				if (!localURL.getFile().contains(".ts")) {
					if((bufThread != null) && bufThread.isAlive()) // ��ֹ���ң�����buffering��ܾ�ҳ������
						return;
					
					if((bufBytes != null) && (doneSize != bufBytes.length))
					{
						HttpRequestor.timeLog(sIPTV_URL + " ������********** \r\n");
					}
					newHttpRequest(localURL.getHost(), localURL.getPort(), localURL.getFile(), false, localOS);
					return;
				}
				// ��ֹbuf�߳�
				while ((bufThread != null) && bufThread.isAlive()) {
					bStopThread = true;
					try {
						Thread.sleep(10);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}

				bStopThread = false;
				// ��ȡbufд��+����
				newHttpRequest(localURL.getHost(), localURL.getPort(), localURL.getFile(), false, localOS);
				// �����߳���ʷ����
				resetBufData();
				bufThread = null;

				String webPath = localURL.getPath();
				// webPath.substring(0, webPath.lastIndexOf("/") + 1) + line;
				String webPath2 = "";
				if ((webPath2 = getNextFileName(webPath.substring(webPath.lastIndexOf("/") + 1))).length() > 0) {
					final String tmp = webPath.substring(0, webPath.lastIndexOf("/") + 1) + webPath2;

//					bufThread = new Thread(new Runnable() {
//						@Override
//						public void run() {
//							try {
//								HttpRequestor.timeLog("�������˻����߳� ==========> " + tmp);
//								newHttpRequest(localURL.getHost(), localURL.getPort(), tmp, true);
//								if(doneSize != bufBytes.length)
//								{
//									HttpRequestor.timeLog(doneSize + "��==========> " + bufBytes.length);
//								}
//								HttpRequestor.timeLog("�����߳� ��ֹ��==========> " + tmp);
//							} catch (IOException e) {
//								// TODO Auto-generated catch block
//								e.printStackTrace();
//							}
//						}
//					});
//					bufThread.start();

				}
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				destroyIPTVConnection();

				e.printStackTrace();
			}
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// TODO Auto-generated method stub

	}

	private static void resetBufData() {
		bStopThread = false;
		doneSize = 0;
		sHeaderBuf = "";
	}

	private static String getNextFileName(String substring) {
		String str = "";
		try {
			str = listFileStr.get(listFileStr.indexOf(substring)+1);
		} catch (IndexOutOfBoundsException  e) {
			// TODO Auto-generated catch block
//			e.printStackTrace();
			return str;
		}
		return str;
	}

	public static void destroyIPTVConnection() {
		try {
			mSocket.close();
			mSocket = null;
			out.close();
			in.close();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
}