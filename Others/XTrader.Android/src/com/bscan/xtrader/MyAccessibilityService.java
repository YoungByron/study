package com.bscan.xtrader;

import java.util.List;

import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;
import android.widget.Toast;

/**
 * ��������ǲ���Ҫ����activity��ȥ�����ģ�����ϵͳ���������� ��Ҫ��������ȥ�ֶ����� ������ƽ��app��
 * ����ʹ�õ�service ���кܴ�ͬ�� �ǳ�����
 * ������� \sdk\samples\android-23\legacy\ApiDemos ������Ŀ¼�� �ҵ�������� �������������һ��accessibility
 * �� �����й�����������demo ��Ȼ�����Ǹ�demo �ǳ����ӣ�������Ϣ���ܴ�����Ȥ�����о���ͬѧ���Զ࿴demo
 * ������ֻʵ��������Ĺ��� ��û����������쳣����ֻ�����������ܣ�������Ϊʵ��ҵ�����ߣ�
 */
public class MyAccessibilityService extends AccessibilityService {
    public MyAccessibilityService() {
    }

    /**
     * AccessibilityService ���������Թ����ܶ����ԣ���Щ���� һ�����ͨ�����������������������ã�
     * ������͵�� ����Щ�������Ե�������xml д�� ����manifest��������Ҫʹ�õ�ʱ����Ҫ���ְ汾��
     * �����ݣ����ϵİ汾�����޷�ͨ��xml�������õ� ֻ���������������д��Щ���� һ��Ҫע��.
     * ͬʱ���ҵ������ܸ��ӱ�����Ҫ��ʼ���㲥��֮��Ĺ��� �����������������д��
     */
    @Override
    protected void onServiceConnected() {
        super.onServiceConnected();
        //Configure these here for compatibility with API 13 and below.
//        AccessibilityServiceInfo config = new AccessibilityServiceInfo();
//        config.eventTypes = AccessibilityEvent.TYPES_ALL_MASK/*.TYPE_WINDOW_STATE_CHANGED*/;
//        config.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
//        //config.packageNames=["com.huaanzq.dzh"];
//        if (Build.VERSION.SDK_INT >= 16)
//            //Just in case this helps
//            config.flags = AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
//
//        setServiceInfo(config);
        
        Toast toast=Toast.makeText(this,"onServiceConnected()...���������ɹ���",Toast.LENGTH_SHORT    );
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }

    /**
     * ���������������������ʱ�򣬾Ϳ��Լ����¼��ˣ���Ȼ����ʲô�¼���������ʲô�̶� �����ɸ��������������������ģ�
     * �ҵ���Щ����д��xml���ˡ�
     */
    @Override
    public void onAccessibilityEvent(AccessibilityEvent event) {
        /**
         * �¼��Ƿֺܶ��ֵģ�����������򵥵����֣�ֻ��ʾ���Ĺ��ܣ����Ҫ����ҵ������ �����Ƽ�һ���������Կ�����������type���ԡ�
         * �����type��int ֵȡ���� ��ת��16���ƣ�Ȼ��ȥAccessibilityEvent Դ����find��˳�㿴ע�� ��������Ѹ�����type���͵ķ���
         */
            String className = event.getClassName().toString();
    	Log.i("XXOO", className + " -> " + event.eventTypeToString(event.getEventType()) );
    	switch (event.getEventType()) {
    	
        case AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED:
        //case AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED:
            //����ط�ûʲô��˵�� ������� �ҵ���ǰ���� �����а�װ ����ؼ��ʵ� ���нڵ�Ϳ����ˡ�������Щ�ڵ��list
            //ע�������find ��ʵ��contains����˼���������������2���ڵ㣬һ���ڵ������ǰ�װ1 һ���ڵ������ǰ�װ2������2���ڵ��Ƕ��᷵�ع�����
            //�����и���Text�ҽڵ�ķ��� ���и���Id�ҽڵ�ķ��������ǵ��ڶ��ֻ�rom����һ����������Ҫ��Ҷ����һ�£��е�rom packageInstall
            //���ƵıȽ����룬���ܺ͹ٷ�rom���ĺ�Զ �����Ҫ�����ദ�����Ը��ߴ��һ��С���� ��Ͱ���Щrom�� ��װ���� Ȼ��
            //ͨ��ddms�� ��view�ṹ�İ�ť ֱ�ӽ�ȥ�������ˣ�����ֱ�ӿ����Ǹ����������ĸ�������Ҳ���Կ�����Ҫ������Ǹ���ť��id��ʲô �ܷ��㣡
            if (className.equals("com.android.dazhihui.ui.screen.InitScreen")) {
                //getPacket();
            } else if (className.equals("com.android.dazhihui.ui.screen.stock.MainScreen")) {
        		

        		if(!findNodesById(event.getSource(), "com.huaanzq.dzh:id/btn_login").isEmpty())
        		{
        			Log.i("XXOO", className + " -> ��¼����..." );
	         		Log.i("XXOO", className + " -> doLogin..." );
	            	doLogin(event); 
	            	Log.i("XXOO", className + " -> doLogin..Done" );
        		}
        		else if(!findNodesById(event.getSource(), "com.huaanzq.dzh:id/ll_dbpmr").isEmpty())
        		{
        			Log.i("XXOO", className + " -> ����		����..." );
        			
        	        switch (MainActivity.iAction){
                    case R.id.b10800:
                    case R.id.b1_1:
                    case R.id.b1_2:
                    case R.id.b1_3:
                    case R.id.b1_4:
	            		//========================================================
	            		List<AccessibilityNodeInfo> listBuy  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/ll_dbpmr");
	                 	clickIt(listBuy); 
                        break;
                    case R.id.s1_1:
                    case R.id.s1_2:
                    case R.id.s1_3:
                    case R.id.s1_4:
	            		//========================================================
	            		List<AccessibilityNodeInfo> listSell  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/ll_dbpmc");
	                 	clickIt(listSell); 
                        break;
        	        }

        		
//	            	if(MainActivity.iAction == 1)
//	            	{
//	            	}
//	            	else if(MainActivity.iAction == -1)
//	            	{
//	            	}
        		}
        		else if(!findNodesById(event.getSource(), "com.huaanzq.dzh:id/bottom_menu_button5").isEmpty())
        		{
        			Log.i("XXOO", className + " -> ������..." );
	        		List<AccessibilityNodeInfo> jyBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/bottom_menu_button5");
	        		if ((null != jyBtn) && (!jyBtn.isEmpty())){
	        		    clickIt(jyBtn); //��ҳ���½ǽ��װ�ť
	        		}
        		}
                //List<AccessibilityNodeInfo> list  = event.getSource().findAccessibilityNodeInfosByText("��¼");
            } else if (className.equals("com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen")) {
        		List<AccessibilityNodeInfo> btnBS = findNodesById(event.getSource(), "com.huaanzq.dzh:id/btn_entrust");
        		if(!btnBS.isEmpty())
        			if(btnBS.get(0).getText().equals("����"))
        				Log.i("XXOO", className + " -> 		�������..." );
        			else if(btnBS.get(0).getText().equals("����"))
        				Log.i("XXOO", className + " -> 		��������..." );

        		switch (MainActivity.iAction){
                case R.id.b10800:
                case R.id.b1_1:
                case R.id.b1_2:
                case R.id.b1_3:
                case R.id.b1_4:
            		if(!btnBS.isEmpty())
            			if(!btnBS.get(0).getText().equals("����"))
            			{
		            		//========================================================
		            		List<AccessibilityNodeInfo> listBuy  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/tv_buy");
		                 	clickIt(listBuy); 
		                 	return;
            			}
            		doBuy(event); 
            		//MainActivity.iAction = 0;
                    break;
                case R.id.s1_1:
                case R.id.s1_2:
                case R.id.s1_3:
                case R.id.s1_4:
            		if(!btnBS.isEmpty())
            			if(!btnBS.get(0).getText().equals("����"))
            			{
		            		//========================================================
		            		List<AccessibilityNodeInfo> listSell  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/tv_sell");
		                 	clickIt(listSell); 
		                 	return;
            			}
            		//doSell(event); 
            		//MainActivity.iAction = 0;
                    break;
    	        }
    	        



//            	switch(MainActivity.iAction){
//            	case -1:
//                    break; 
//            	case 0:
//                    break; 
//            	case 1:
//		        	doBuy(event); 
//                    break; 
//                default : //��ѡ
//            	}
//            	MainActivity.iAction = 0;
            } else if (className.equals("android.app.Dialog")) {
            	
            	if((!event.getSource().findAccessibilityNodeInfosByText("��ȷ��������").isEmpty()) ||
            	(!event.getSource().findAccessibilityNodeInfosByText("��ȷ��������").isEmpty()))
            	{
            		MainActivity.iAction = 0;
            		List<AccessibilityNodeInfo> listConfirm  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/confirm");
		        	//clickIt(listConfirm); 
            	}

            	if(event.getSource().findAccessibilityNodeInfosByText("���ղ�����ʾ").isEmpty())
            		return;
            	Log.i("XXOO", className + " -> ClosePpup..." );
            	//List<AccessibilityNodeInfo> okBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/cancel");
            	List<AccessibilityNodeInfo> okBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/cancel");
        		if ((null != okBtn) && (!okBtn.isEmpty())){
        		    clickIt(okBtn); //�����ȷ����ť
        		}
            	Log.i("XXOO", className + " -> ClosePpup..Done" );
            }
    	}
    }

	private void doBuy(AccessibilityEvent event) {
		//========================================================
    	Log.i("XXOO", event.getClassName().toString() + " -> doBuy.......��A 0602572118." ); //Bug...
		List<AccessibilityNodeInfo> tmpCode  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/edittext");
		pasteInto(tmpCode, "��A 0602572118"); 
		
    	Log.i("XXOO", event.getClassName().toString() + " -> doBuy......159915" );
		List<AccessibilityNodeInfo> listCode  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_code");
		pasteInto(listCode, "159915"); 
		
		
    	Log.i("XXOO", event.getClassName().toString() + " -> doBuy......1.888" );
		List<AccessibilityNodeInfo> listPrice  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_price");
		pasteInto(listPrice, "1.888"); 
		//List<AccessibilityNodeInfo> listAvaNum = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/tv_ava_num");//����179800��
		
		
		//if ((null != listAvaNum) && (!listAvaNum.isEmpty()))
		{
		    //for (AccessibilityNodeInfo info : listAvaNum) 
		    {
		        //if (info.getText().toString().length()>0) 
		        {
		        	Log.i("XXOO", event.getClassName().toString() + " -> doBuy......10800" );
		List<AccessibilityNodeInfo> listNum  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_num");
		pasteInto(listNum, "10800"); 
		        	//pasteInto(listNum, info.getText().toString()); 
		List<AccessibilityNodeInfo> listEnTrust  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/btn_entrust");
		clickIt(listEnTrust); 
		        }
		    }
		}
	}

	private void doLogin(AccessibilityEvent event) {
		
		List<AccessibilityNodeInfo> list  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/tv_yzm");
		List<AccessibilityNodeInfo> listYZM  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_yzm");
		List<AccessibilityNodeInfo> listPass  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/et_passwords");
		List<AccessibilityNodeInfo> listLogin  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/btn_login");
		if(listLogin.isEmpty())
			return;
		
		pasteInto(listPass, "288603"); 
		
		if (null != list) {
		    for (AccessibilityNodeInfo info : list) {
		        if (info.getText().toString().length()>0) {
		        	
		        	pasteInto(listYZM, info.getText().toString()); 
		        }
		    }
		} 
		
		clickIt(listLogin);
	}

	private List<AccessibilityNodeInfo> waitAndFindNodes(
			AccessibilityNodeInfo nodeInfo, String sText) {
		List<AccessibilityNodeInfo>  listNodes  = null;
		while((null == listNodes) || (listNodes.isEmpty())){
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			listNodes  = findNodesById(nodeInfo, sText);
		}
		return listNodes;
	}

	private void clickIt(List<AccessibilityNodeInfo> listLogin) {
		if (null != listLogin) {
		    for (AccessibilityNodeInfo info : listLogin) {
		    	
		        //if (info2.getText().toString().equals("��¼")) {
		            //�ҵ���Ľڵ��Ժ� ��ֱ�ӵ����������
		            info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		        //}
		    }
		}
	}

	private void pasteInto(List<AccessibilityNodeInfo> listPass, String sText) {
		if (null != listPass) {
		    for (AccessibilityNodeInfo info : listPass) {
		    	
		    	//android>21 = 5.0ʱ������ACTION_SET_TEXT
		    	//android>18 3.0.1����ͨ�����Ƶ��ֶ�,��ȷ�����㣬��ճ��ACTION_PASTE
		    	//ʹ�ü��а�
		    	ClipboardManager clipboard = (ClipboardManager)this.getSystemService(Context.CLIPBOARD_SERVICE);  
		    	ClipData clip = ClipData.newPlainText("text", sText);  
		    	clipboard.setPrimaryClip(clip);  
		    	
		    	CharSequence txt = info.getText();
		    	if(txt == null) txt = "";
		    	if(txt.toString().equals("159915") )
		    	{
		    		txt = "";
		    		int xxx=88;
		    			break;
		    	}
		    	  
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                    Bundle arguments = new Bundle();
                    arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_START_INT, 0);
                    arguments.putInt(AccessibilityNodeInfo.ACTION_ARGUMENT_SELECTION_END_INT, txt.length());
                    info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
                    info.performAction(AccessibilityNodeInfo.ACTION_SET_SELECTION, arguments);
                    info.performAction(AccessibilityNodeInfo.ACTION_PASTE);
                }
                break;

//		    	//���㣨n��AccessibilityNodeInfo����  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);  
//		    	////ճ����������  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_CUT);  
//		    	clipboard.setPrimaryClip(clip);  
//		    	info.performAction(AccessibilityNodeInfo.ACTION_PASTE);  
		    	
		    	//info2.setText(info.getText());
		        //if (info2.getText().toString().equals("��¼")) {
		            //�ҵ���Ľڵ��Ժ� ��ֱ�ӵ����������
		            //info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);
		            //info.performAction(AccessibilityNodeInfo.ACTION_CLICK);
		        //}
		    }
		}
	}
    /**
     * ����View��ID�������������Ľڵ�,��ȷ������ʽ;
     * ���ֻ�������Լ�д�Ľ��棬��ΪID�����ظ�
     * apiҪ��18������
     * @param viewId
     */    
    public List<AccessibilityNodeInfo> findNodesById(AccessibilityNodeInfo nodeInfo, String viewId) {
        if (nodeInfo != null) {
            if (Build.VERSION.SDK_INT >= 18) {
                return nodeInfo.findAccessibilityNodeInfosByViewId(viewId);
            }
        }
        return null;
    }
    @Override
    public void onInterrupt() {

    }

}