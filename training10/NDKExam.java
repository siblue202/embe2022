package org.example.ndk;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.widget.TextView;

public class NDKExam extends Activity {
	public native int add(int x, int y);
	public native int mul(int x, int y);
	public native void testString(String str);
	@Override
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.main);
        
        TextView tv = new TextView(this);
        int x = 1000;
        int y = 42;
        
        System.loadLibrary("ndk-exam");
        
        int z = add(x, y);
        int result = mul(x, y);
        
        tv.setText("The mul of " + x + " and " + y + " is " + result);
        setContentView(tv);

        testString("test");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
