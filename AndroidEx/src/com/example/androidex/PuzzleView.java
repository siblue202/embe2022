package com.example.androidex;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Paint.FontMetrics;
import android.graphics.Paint.Style;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.Toast;

public class PuzzleView extends View {
	private static final String TAG = "Sudoku";
	private final SudokuActivity game;
	private float width;
	private float height; 
	private int selX;
	private int selY;
	private final Rect selRect = new Rect();

	public PuzzleView(Context context){
		super(context);
		this.game = (SudokuActivity) context;
		setFocusable(true);
		setFocusableInTouchMode(true);
	}
	
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh){
		width = w / 9f;
		height = h / 9f;
		getRect(selX, selY, selRect);
		Log.d(TAG, "onSizeChanged: width" + width + ",height" + height);
		super.onSizeChanged(w, h, oldw, oldh);
	}

	private void getRect(int x, int y, Rect rect){
		// Log.d(TAG, "getRect: x" + x + ",y" + y);
		rect.set((int)(x * width), (int)(y * height), (int)(x * width + width), (int)(y * height + height));
	}

	@Override 
	protected void onDraw(Canvas canvas){
		
		//Draw the background
		Paint background = new Paint();
		background.setColor(getResources().getColor(R.color.puzzle_background));
		canvas.drawRect(0,0, getWidth(), getHeight(), background);
		
		//Draw the board..
		//Define colors for the grid lines	
		
		Paint dark = new Paint();
		dark.setColor(getResources().getColor(R.color.puzzle_dark));
		Paint hilite = new Paint();
		hilite.setColor(getResources().getColor(R.color.puzzle_hilite));
		Paint light = new Paint();
		light.setColor(getResources().getColor(R.color.puzzle_light));
		
		for (int i = 0; i < 9; i++){
			canvas.drawLine(0, i*height, getWidth(), i*height, light);
			canvas.drawLine(0, i*height + 1, getWidth(), i*height + 1, hilite);
			canvas.drawLine(i*width, 0, i*width, getHeight(), light);
			canvas.drawLine(i*width + 1, 0, i*width + 1, getHeight(), hilite);		
		}
		
		//Draw the major grid lines
		for (int i = 0; i < 9; i++){
			if (i % 3 != 0)
				continue;
			canvas.drawLine(0, i*height, getWidth(), i*height, dark);
			canvas.drawLine(0, i*height + 1, getWidth(), i*height + 1, hilite);
			canvas.drawLine(i*width, 0, i*width, getHeight(), dark);
			canvas.drawLine(i*width + 1, 0, i*width + 1, getHeight(), hilite);		
		}
		
		//Draw the numbers...
		//Define color and style for numbers
		Paint foreground = new Paint(Paint.ANTI_ALIAS_FLAG);
		foreground.setColor(getResources().getColor(R.color.puzzle_foreground));
		foreground.setStyle(Style.FILL);
		foreground.setTextSize(height * 0.75f);
		foreground.setTextScaleX(width/height);
		foreground.setTextAlign(Paint.Align.CENTER);
		
		//Draw the number in the center of the tile
		FontMetrics fm = foreground.getFontMetrics();
		
		//Centering in X:use alignment (and X at midpoint)
		float x = width / 2;
		
		//Centering in Y: measure ascent/descent first
		float y = height / 2 - (fm.ascent + fm.descent) / 2;
		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++){
				canvas.drawText(this.game.getTileString(i, j), i * width + x, j * height + y, foreground);
			}
		}
		
		//1
		Paint reground = new Paint(Paint.ANTI_ALIAS_FLAG);
		reground.setColor(getResources().getColor(R.color.BLUE));
		reground.setStyle(Style.FILL);
		reground.setTextSize(height * 0.75f);
		reground.setTextScaleX(width/height);
		reground.setTextAlign(Paint.Align.CENTER);
		
		//Draw the number in the center of the tile
		FontMetrics fm2 = reground.getFontMetrics();
		
		//Centering in X:use alignment (and X at midpoint)
		float x2 = width / 2;
		
		//Centering in Y: measure ascent/descent first
		float y2 = height / 2 - (fm2.ascent + fm2.descent) / 2;
		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++){
				if(this.game.getReTileString(i,j).equals("0")){
					continue;
				}
				canvas.drawText(this.game.getReTileString(i, j), i * width + x2, j * height + y2, reground);
			}
		}
		//2
		
		//Draw the selection...
		Log.d(TAG, "selRect=" + selRect);
		Paint selected = new Paint();
		selected.setColor(getResources().getColor(R.color.puzzle_selected));
		canvas.drawRect(selRect, selected);		
    }
	
	public int getSelx(){
		return this.selX;
	}
	
	public int getSely(){
		return this.selY;
	}
	
	public void select(int x, int y){
		invalidate();
		selX = Math.min(Math.max(x, 0), 8);
		selY = Math.min(Math.max(y, 0), 8);
		getRect(selX, selY, selRect);
		invalidate();
	}
	
	public void setSelectedTile(int tile){
		if (game.setTileIfValid(selX, selY, tile)){
			invalidate();
			game.checkComplete();
		}
		else {
			//Number is not valid for this tile
			Log.d(TAG, "setSelectedTile: invalid: " + tile);
			startAnimation(AnimationUtils.loadAnimation(game, R.anim.shake));
		}
	}
	
}

