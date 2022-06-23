package com.example.androidex;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.util.Log;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View.OnClickListener;
import android.view.View;

public class MainActivity extends Activity implements OnClickListener {
	static boolean is_music_playing = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.fragment_main);
		View newButton = findViewById(R.id.new_button);
		newButton.setOnClickListener(this);
		View musicButton = findViewById(R.id.music_button);
		musicButton.setOnClickListener(this);
		View musicStopButton = findViewById(R.id.music_stop_button);
		musicStopButton.setOnClickListener(this);
		View exitButton = findViewById(R.id.exit_button);
		exitButton.setOnClickListener(this);		
	}
	
	private static final String TAG="Sudoku";
	
	private void startGame(){
		Log.d(TAG, "clicked on ");
		Intent intent = new Intent(this, SudokuActivity.class);
		startActivity(intent);
	}
	
	public void onClick(View v)
	{
		switch(v.getId())
		{
			case R.id.music_button:
				if(!is_music_playing){
					is_music_playing = true;
					Intent i = new Intent(getApplicationContext(), MusicService.class);
					startService(i);
				}
				break;	
			case R.id.music_stop_button:
				is_music_playing = false;
				Intent intent = new Intent(getApplicationContext(), MusicService.class);
				stopService(intent);
				break;
			case R.id.new_button:
				startGame();
				break;
			case R.id.exit_button:
				finish();
				break;
		}
	}

}
