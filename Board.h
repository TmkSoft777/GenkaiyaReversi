#pragma once


enum BlockStatus { EMPTY , WHITE , BLACK , AVAILABLE };

class Board {
public:

	BlockStatus(*getBoard())[8];
	bool isGameOver;
	bool isFull;
	bool isnoAvailable;

	void mouseClick(int x,int y);
	void Set();
	void Reverse(int i, int j);
	void robot();
	void setAvailable();
	void Test(int i,int j,int m,int n);
	void checkifOver();
	bool checkifAvailable(int i,int j,int m,int n,int cmd);
	int countPoint(int s,int r);
	void copymap();
	bool checkmodelifAvailable(int i,int j,int m,int n);
	void modelTest(int i,int j,int m,int n);

	Board() { Set(); }
private:
	BlockStatus m_board[8][8];
};
