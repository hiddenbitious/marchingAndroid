

package com.android.marchingcubes;

import android.app.Activity;
import android.os.Bundle;
//import android.util.Log;
//import android.view.WindowManager;

//import java.io.File;

import com.android.marchingcubes.MarchingCubesView;


public class MarchingCubesActivity extends Activity {

	 MarchingCubesView mView;

	    @Override protected void onCreate(Bundle icicle) {
	        super.onCreate(icicle);
//	        mView = new MarchingCubesView(getApplication());
	        mView = new MarchingCubesView(getApplication(), false, 8, 0);
		setContentView(mView);
	    }

	    @Override protected void onPause() {
	        super.onPause();
	        mView.onPause();
	    }

	    @Override protected void onResume() {
	        super.onResume();
	        mView.onResume();
	    }
}
