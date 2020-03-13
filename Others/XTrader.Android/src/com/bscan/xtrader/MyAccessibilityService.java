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
        AccessibilityServiceInfo config = new AccessibilityServiceInfo();
        config.eventTypes = AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED;
        config.feedbackType = AccessibilityServiceInfo.FEEDBACK_GENERIC;
        //config.packageNames=["com.huaanzq.dzh"];
        if (Build.VERSION.SDK_INT >= 16)
            //Just in case this helps
            config.flags = AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;

        setServiceInfo(config);
        
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
        case AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED:
            //����ط�ûʲô��˵�� ������� �ҵ���ǰ���� �����а�װ ����ؼ��ʵ� ���нڵ�Ϳ����ˡ�������Щ�ڵ��list
            //ע�������find ��ʵ��contains����˼���������������2���ڵ㣬һ���ڵ������ǰ�װ1 һ���ڵ������ǰ�װ2������2���ڵ��Ƕ��᷵�ع�����
            //�����и���Text�ҽڵ�ķ��� ���и���Id�ҽڵ�ķ��������ǵ��ڶ��ֻ�rom����һ����������Ҫ��Ҷ����һ�£��е�rom packageInstall
            //���ƵıȽ����룬���ܺ͹ٷ�rom���ĺ�Զ �����Ҫ�����ദ�����Ը��ߴ��һ��С���� ��Ͱ���Щrom�� ��װ���� Ȼ��
            //ͨ��ddms�� ��view�ṹ�İ�ť ֱ�ӽ�ȥ�������ˣ�����ֱ�ӿ����Ǹ����������ĸ�������Ҳ���Կ�����Ҫ������Ǹ���ť��id��ʲô �ܷ��㣡
            if (className.equals("com.android.dazhihui.ui.screen.InitScreen")) {
                //getPacket();
            } else if (className.equals("com.android.dazhihui.ui.screen.stock.MainScreen")) {
                //List<AccessibilityNodeInfo> list  = event.getSource().findAccessibilityNodeInfosByText("��¼");
            	Log.i("XXOO", className + " -> doLogin..." );
            	doLogin(event); 
            	Log.i("XXOO", className + " -> doLogin..Done" );
            } else if (className.equals("com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen")) {
               //close();
	        	//doBuy(event); 
            } else if (className.equals("android.app.Dialog")) {
            	Log.i("XXOO", className + " -> ClosePpup..." );
        		List<AccessibilityNodeInfo> okBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/cancel");
        		if ((null != okBtn) && (!okBtn.isEmpty())){
        		    clickIt(okBtn); //�����ȷ����ť
        		}
            	Log.i("XXOO", className + " -> ClosePpup..Done" );
        		
            }

            //
          //
          //com.android.dazhihui.ui.delegate.screen.margin.MarginCommonScreen
            //
            //
            

    	}
    }

	private void doBuy(AccessibilityEvent event) {
		//========================================================
		List<AccessibilityNodeInfo> listBuy  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/ll_dbpmr");
		
//
     	clickIt(listBuy); 
		//========================================================
		List<AccessibilityNodeInfo> listCode  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/et_code");
		List<AccessibilityNodeInfo> listNum  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/et_num");
		pasteInto(listCode, "159915"); 
		
		List<AccessibilityNodeInfo> listAvaNum = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/tv_ava_num");//����179800��
		
		List<AccessibilityNodeInfo> listEnTrust  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/btn_entrust");
		List<AccessibilityNodeInfo> listConfirm  = waitAndFindNodes(event.getSource(), "com.huaanzq.dzh:id/confirm");
		
		if ((null != listAvaNum) && (!listAvaNum.isEmpty())){
		    for (AccessibilityNodeInfo info : listAvaNum) {
		        if (info.getText().toString().length()>0) {
		        	
		        	pasteInto(listNum, "10800"); 
		        	//pasteInto(listNum, info.getText().toString()); 
		        	clickIt(listEnTrust); 
		        	//clickIt(listConfirm); 
		        }
		    }
		}
	}

	private void doLogin(AccessibilityEvent event) {
		
		List<AccessibilityNodeInfo> jyBtn  = findNodesById(event.getSource(), "com.huaanzq.dzh:id/bottom_menu_button5");
		if ((null != jyBtn) && (!jyBtn.isEmpty())){
		    clickIt(jyBtn); //��ҳ���½ǽ��װ�ť
		}
		
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
		    	//���㣨n��AccessibilityNodeInfo����  
		    	info.performAction(AccessibilityNodeInfo.ACTION_FOCUS);  
		    	////ճ����������  
		    	info.performAction(AccessibilityNodeInfo.ACTION_CUT);  
		    	clipboard.setPrimaryClip(clip);  
		    	info.performAction(AccessibilityNodeInfo.ACTION_PASTE);  
		    	
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