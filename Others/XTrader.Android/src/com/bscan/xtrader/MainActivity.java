package com.bscan.xtrader;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.provider.Settings;//����


public class MainActivity extends Activity {
	public static float fAction = 0;
	public static float fAvaMoney = 0;
	//public static float iAction000 = 0;
	private static final String ACTION = "action";
        private static final String ACTION_START_ACCESSIBILITY_SETTING = "action_start_accessibility_setting";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        jumpToSettingPage(this.getApplicationContext());

    }

        public static void jumpToSettingPage(Context context) {
            try {
//                Intent intent = new Intent(context,  MainActivity.class);
//                intent.putExtra(ACTION, ACTION_START_ACCESSIBILITY_SETTING);
//                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//                context.startActivity(intent);

            	Intent intent = new Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS);
            	intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            	intent.putExtra(ACTION, ACTION_START_ACCESSIBILITY_SETTING);
            	context.startActivity(intent);
            	} catch (Exception ignore) {
            		int xxx=999;
            	}
        }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    public void click(View v){
        Toast.makeText(MainActivity.this,((Button)findViewById(v.getId())).getText().toString(),Toast.LENGTH_LONG).show();
        fAction = Float.parseFloat(((Button)v).getText().toString());
        Log.i("XXOO", fAction + " -> 		click..." );

        PackageManager packageManager = this.getPackageManager();   
        Intent intent = packageManager.getLaunchIntentForPackage("com.huaanzq.dzh");
        startActivity(intent);
        
    }
}
