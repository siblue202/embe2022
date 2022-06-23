package com.example.androidex;

import java.util.Arrays;

import android.app.Activity;
import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.widget.LinearLayout;
import android.widget.Toast;



public class SudokuActivity extends Activity{
	public native int readKey();
	public native int readSwitch();
	static{
		System.loadLibrary("device-jni");
	};
	
	
	private static final String TAG = "sudoku";
	private int puzzle[];
	private int puzzle_set[]; // 1
	private int puzzle_ori[]; // 1
	private PuzzleView puzzleView;
	private boolean terminated = false;
	private boolean switchMode = false;
	
	private final int used[][][] = new int[9][9][];
	private final String setPuzzle =
			"360000000004230800000004200" +
			"070460003820000014500013020" +
			"001900000007048300000000045";
			/*
			"360000000004230800000004200" +
			"070460003820000014500013020" +
			"001900000007048300000000045";
			 */
	
	/* for JNI */
	class SelectionThread extends Thread {
		int x;
		int y;
		
		SelectionThread(){}
		SelectionThread(int _x, int _y){
			x = _x;
			y = _y;
		}
		
		public void run(){
			while(!terminated){
				runOnUiThread(new Runnable(){
					@Override
					public void run(){
						int k = readKey();
						Log.d(TAG, "readKey() : " + k);
						int value = readSwitch();
						Log.d(TAG, "readSwitch() : " + value);
						changeMode(k, value);
						if (switchMode && value != 0){
							settingTile(value);
						} else if (!switchMode && value != 0){
							moveblock(value);
						}
						
					}
				});
				try {
					Thread.sleep(400);
				} catch (InterruptedException e){
					e.printStackTrace();
				}
			}
		}
		public void changeMode(int k, int value){
			if(k == 0 && value == 999){
				switchMode = !switchMode;
			}
		}
		
		public void moveblock(int k){
			int dx = 0;
			int dy = 0;
			
			if(k == 8){
				dy = 1;
			} else if(k == 2){
				dy = -1;
			} else if(k == 4){
				dx = -1;
			} else if(k == 6){
				dx = 1;
			}
			
			if ((x+dx > 8) || (x+dx < 0) || (y+dy > 8) || (y+dy < 0)){
				return;
			}
			
			x += dx;
			y += dy;
			puzzleView.select(x, y);
		}
		
		
		public void settingTile(int tile){
			if (tile == 999){
				return;
			}
			puzzleView.setSelectedTile(tile);
		}
		
	}
	
	
	protected int[] getUsedTiles(int x, int y){
		return used[x][y];
	}
	
	private void calculateUsedTiles(){
		for(int x=0; x<9; x++){
			for (int y=0; y<9; y++){
				used[x][y] = calculateUsedTiles(x,y);
			}
		}
	}
	
	private int[] calculateUsedTiles(int x, int y){
		int c[] = new int[9];
		
		//horizontal
		for (int i=0; i<9; i++){
			if (i == x) continue;
			int t = getTile(i,y);
			if (t != 0) c[t-1] = t;
		}
		
		//vertical
		for (int i=0; i<9; i++){
			if (i == y) continue;
			int t = getTile(x, i);
			if(t != 0) c[t-1] = t;
		}
		
		//same cell block
		int startx = (x/3) * 3;
		int starty = (y/3) * 3;
		for (int i=startx; i<startx + 3; i++){
			for (int j=starty; j<starty+3; j++){
				if (i == x && j == y) continue;
				int t = getTile(i,j);
				if (t!=0) c[t-1] = t;
			}
		}
		
		//compress
		int nused=0;
		for (int t:c){
			if (t != 0) nused++;
		}
		
		int c1[] = new int[nused];
		nused = 0;
		for (int t:c){
			if (t != 0) c1[nused++] = t;
		}
		
		return c1;
	}
	
	private int[] getPuzzle(){
		String puz;
		
		puz = setPuzzle;
		
		return fromPuzzleString(puz);
	}
	
	static private String toPuzzleString(int[] puz){
		StringBuilder buf = new StringBuilder();
		for(int element : puz){
			buf.append(element);
		}
		return buf.toString();
	}
	
	static protected int[] fromPuzzleString(String string){
		int[] puz = new int[string.length()];
		for (int i=0; i<puz.length; i++){
			puz[i] = string.charAt(i) - '0';
		}
		
		return puz;
	}
	
	private int getTile(int x, int y){
		return puzzle[9*y + x];
	}
	
	//1
	private int getReTile(int x, int y){
		return puzzle_set[9*y + x];
	}
	//2
	
	private void setTile(int x, int y, int value){
		// 1
		if(puzzle_ori[y*9 + x] != 0){
			return;
		}
		// 2
		puzzle[y*9 + x] = value;
		// 1
		puzzle_set[y*9 + x] = value;
		// 2
	}
	
	protected String getTileString(int x, int y){
		int v = getTile(x,y);
		if (v == 0)
			return "";
		else
			return String.valueOf(v);
	}
	
	//1
	protected String getReTileString(int x, int y){
		int v = getReTile(x,y);
		if (v == 0)
			return "";
		else
			return String.valueOf(v);
	}
	//2
	
	// 1
	protected void checkComplete(){
		int n = 0;
		for (int i = 0 ; i < puzzle.length; i ++){
			if (puzzle[i] != 0){
				n++;
			}
		}
		if (n >= 81){
			Toast toast = Toast.makeText(this, R.string.complition, Toast.LENGTH_SHORT);
			toast.setGravity(Gravity.CENTER, 0, 0);
			toast.show();
		}
	}
	// 2
	
	SelectionThread selectionThread;
	
	@Override
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		Log.d(TAG, "onCreate");
		puzzle = getPuzzle();
		// 1
		puzzle_set =new int[81];
		Arrays.fill(puzzle_set, 0);
		calculateUsedTiles();
		puzzle_ori = getPuzzle();
		// 2
		puzzleView = new PuzzleView(this);
		
		selectionThread = new SelectionThread(puzzleView.getSelx(), puzzleView.getSely());
		selectionThread.start();
		
		setContentView(puzzleView);
		puzzleView.requestFocus();
	}
	
	/*
	protected void showKeypadOrError(int x, int y){
		int tiles[] = getUsedTiles(x,y);
		if (tiles.length == 9){
			Toast toast = Toast.makeText(this, R.string.no_moves_label, Toast.LENGTH_SHORT);
			toast.setGravity(Gravity.CENTER, 0, 0);
			toast.show();
		}
		else {
			Log.d(TAG, "showKeypad: used=" + toPuzzleString(tiles));
			Dialog v = new Keypad(this, tiles, puzzleView);
			v.show();
		}
	}
	*/
	
	protected boolean setTileIfValid(int x, int y, int value){
		if (getReTile(x, y) == value){
			setTile(x, y, 0);
			return true;
		}
		int tiles[] = getUsedTiles(x,y);
		if (value != 0){
			for (int tile: tiles){
				if (tile == value)
					return false;
			}
		}
		setTile(x, y, value);
		calculateUsedTiles();
		return true;
	}
}
