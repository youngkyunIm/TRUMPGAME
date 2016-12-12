#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <time.h>
#include <windows.h>

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
void clrscr();
void gotoxy(int x, int y);
int wherex();
int wherey();
void setcursortype(CURSOR_TYPE c);

#define delay(n) Sleep(n)							// n/1000초만큼 시간 지연
#define randomize() srand((unsigned)time(NULL))		// 난수 발생기 초기화
#define random(n) (rand() % (n))					//0~n까지의 난수 발생

// 화면을 모두 지운다.
void clrscr() {
	system("cls");
}

// 커서를 x,y좌표로 이동시킨다.
void gotoxy(int x, int y) {
	COORD Cur;
	Cur.X=x;
	Cur.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),Cur);
}

// 커서의 x 좌표를 조사한다.
int wherex()
{
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&BufInfo);
	return BufInfo.dwCursorPosition.X;
}

// 커서의 y좌표를 조사한다.
int wherey()
{
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&BufInfo);
	return BufInfo.dwCursorPosition.Y;
}

// 커서를 숨기거나 다시 표시한다.
void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize=1;
		CurInfo.bVisible=FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize=100;
		CurInfo.bVisible=TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize=20;
		CurInfo.bVisible=TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&CurInfo);
}

using namespace std;



const int MaxCard = 52;

const int CardGap = 4;

const int Speed = 1000;

const int PromptSpeed = 2000;

struct SCard {
	char Name[4];
	SCard() { Name[0] = 0; }
	SCard(const char *pName) {
		strcpy(Name, pName);
	}
	int GetNumber() const {
		if (isdigit(Name[0])) return Name[0] - '0';
		if (Name[0] == 'T') return 10;
		if (Name[0] == 'J') return 11;
		if (Name[0] == 'Q') return 12;
		return 13;
	};
	int GetKind() const {
		if (strcmp(Name + 1, "♠") == 0) return 0;
		else if (strcmp(Name + 1, "♥") == 0) return 1;
		else if (strcmp(Name + 1, "♣") == 0) return 2;
		else return 3;
	}
	friend ostream &operator <<(ostream &c, const SCard &C) {
		return c << C.Name;
	}
	bool operator ==(const SCard &Other) const {
		return (strcmp(Name, Other.Name) == 0);
	}
	bool operator <(const SCard &Other) const {
		if (GetNumber() < Other.GetNumber()) return true;
		if (GetNumber() > Other.GetNumber()) return false;
		if (GetKind() < Other.GetKind()) return true;
		return false;
	}
};

SCard GCard[MaxCard] = {
	"1♠","2♠","3♠","4♠","5♠","6♠","7♠","8♠","9♠","T♠","J♠","Q♠","K♠",
	"1♥","2♥","3♥","4♥","5♥","6♥","7♥","8♥","9♥","T♥","J♥","Q♥","K♥",
	"1♣","2♣","3♣","4♣","5♣","6♣","7♣","8♣","9♣","T♣","J♣","Q♣","K♣",
	"1◆","2◆","3◆","4◆","5◆","6◆","7◆","8◆","9◆","T◆","J◆","Q◆","K◆"
};

// 카드의 집합을 관리하는 클래스
class CCardSet {
protected:
	SCard Card[MaxCard];
	int Num;
	const int sx,sy;
	CCardSet(int asx,int asy) : sx(asx), sy(asy) { Num=0; }

public:
	int GetNum() { return Num; }
	SCard GetCard(int idx) { return Card[idx]; }
	void Reset() { 
		for (int i=0;i<MaxCard;i++) Card[i].Name[0]=0;
		Num=0; 
	}
	void InsertCard(SCard C);
	SCard RemoveCard(int idx);
//	int FindSameCard(SCard C,int *pSame);
//	int FindFirstCard(const char *pName);
};

void CCardSet::InsertCard(SCard C) {
	int i;

	if (C.Name[0] == 0) return;
	for (i=0;i<Num;i++) {
		if (C < Card[i]) break;
	}
	memmove(&Card[i+1],&Card[i],sizeof(SCard)*(Num-i));
	Card[i]=C;
	Num++;
}

SCard CCardSet::RemoveCard(int idx) {
	assert(idx < Num);
	SCard C=Card[idx];
	memmove(&Card[idx],&Card[idx+1],sizeof(SCard)*(Num-idx-1));
	Num--;
	return C;
}
/*
int CCardSet::FindSameCard(SCard C,int *pSame) {
	int i,num;
	int *p=pSame;

	for (i=0,num=0;i<Num;i++) {
		if (Card[i].GetNumber() == C.GetNumber()) {
			num++;
			*p++=i;
		}
	}
	*p=-1;
	return num;
}
*/
/*
int CCardSet::FindFirstCard(const char *pName) {
	int i;
	for (i=0;i<Num;i++) {
		if (strstr(Card[i].Name,pName) != NULL) {
			return i;
		}
	}
	return -1;
}
*/
// 담요에 카드를 쌓아 놓는 데크
class CDeck : public CCardSet {
public:
	CDeck(int asx,int asy) : CCardSet(asx,asy) { ; }
	void Shuffle() {
		int i,n;
	    	for (i=0;i<MaxCard;i++) {
	           	do {
	               	n=random(MaxCard);
	       		} while (Card[n].Name[0] != NULL);
	        Card[n]=GCard[i];
	        Num++;
		   	}
	}
	SCard Pop() { return RemoveCard(Num-1); }
	bool IsEmpty() { return Num==0; }
	bool IsNotLast() { return Num > 1; }
	void Draw(bool bFlip) {
		gotoxy(18,15);
		cout << "??? " << (bFlip ? Card[Num-1].Name:"   ");
	}
};

// 게임을 하는 플레이어
class CPlayer : public CCardSet {
public:
	CPlayer(int asx,int asy) : CCardSet(asx,asy) { ; }
	void Draw(bool MyTurn) {
		int i,x;
		for (i=0,x=sx;i<Num;i++,x+=CardGap) {
			gotoxy(x,sy);
			cout << Card[i];
			if (MyTurn) {
				gotoxy(x,sy+1);
                cout << '[' << i+1 << ']';
			}
		}
	}
};
class placeset {
protected:
	SCard Card[MaxCard];
	int Num;
	const int sx,sy;
	placeset(int asx,int asy) : sx(asx), sy(asy) { Num=0; }

public:
	int GetNum() { return Num; }
	SCard GetCard(int idx) { return Card[idx]; }
	void Reset() { 
		for (int i=0;i<MaxCard;i++) Card[i].Name[0]=0;
		Num=0; 
	}
	void InsertCard(SCard C);
//	SCard RemoveCard(int idx);
//	int FindSameCard(SCard C,int *pSame);
//	int FindFirstCard(const char *pName);
};
void placeset::InsertCard(SCard C) {
	int i;

	if (C.Name[0] == 0) return;
	for (i=0;i<Num;i++) {
//		if (C < Card[i]) break;
	}
	memmove(&Card[i+1],&Card[i],sizeof(SCard)*(Num-i));
	Card[i]=C;
	Num++;
}
class place : public placeset {
public:
	place(int asx,int asy) :placeset(asx,asy) { ; }
	void Draw(bool MyTurn) {
		int i,x;
		for (i=0,x=sx;i<Num;i++,x+=CardGap) {
			gotoxy(sx,sy);
			cout << Card[i];
		}

	}
};

// 게임이 진행되는 담요
class CBlanket: public place {
public:
	CBlanket(int asx,int asy) : place(asx,asy) { ; }
	void Draw() {
		place::Draw(false);
	}
	void DrawSelNum(int *pSame) {
		int n;
		int *p;
		for (n=1,p=pSame;*p!=-1;p++,n++) {
			gotoxy(sx+*p*CardGap,sy-1);
			cout << '[' << n << ']';

		}
	}
	void DrawTempCard(int idx,SCard C) {
		gotoxy(sx+idx*CardGap,sy+1);
		cout << C;
	}
};
// 게임이 진행되는 담요2
class CBlanket2: public place {
public:
	CBlanket2(int asx,int asy) : place(asx,asy) { ; }
	void Draw() {
		place::Draw(false);

	}
	void DrawSelNum(int *pSame) {
		int n;
		int *p;
		for (n=1,p=pSame;*p!=-1;p++,n++) {
			gotoxy(sx+*p*CardGap,sy-1);
			cout << '[' << n << ']';
		}
	}
	void DrawTempCard(int idx,SCard C) {
		gotoxy(sx+idx*CardGap,sy+1);
		cout << C;
	}
};
// 게임이 진행되는 담요3
class CBlanket3: public place {
public:
	CBlanket3(int asx,int asy) : place(asx,asy) { ; }
	void Draw() {
		place::Draw(false);
	}
	void DrawSelNum(int *pSame) {
		int n;
		int *p;
		for (n=1,p=pSame;*p!=-1;p++,n++) {
			gotoxy(sx+*p*CardGap,sy-1);
			cout << '[' << n << ']';
		}
	}
	void DrawTempCard(int idx,SCard C) {
		gotoxy(sx+idx*CardGap,sy+1);
		cout << C;
	}
};
// 게임이 진행되는 담요4
class CBlanket4: public place {
public:
	CBlanket4(int asx,int asy) : place(asx,asy) { ; }
	void Draw() {
		place::Draw(false);
	}
	void DrawSelNum(int *pSame) {
		int n;
		int *p;
		for (n=1,p=pSame;*p!=-1;p++,n++) {
			gotoxy(sx+*p*CardGap,sy-1);
			cout << '[' << n << ']';
		}
	}
	void DrawTempCard(int idx,SCard C) {
		gotoxy(sx+idx*CardGap,sy+1);
		cout << C;
	}
};

// 함수 원형
void Initialize();
void DrawScreen();
void OutPrompt(const char *Mes,int Wait=0);
int InputInt(const char *Mes, int start, int end);

// 전역 변수
CDeck Deck(5,9);
CPlayer South(5,20), North(5,2);
CBlanket Blanket(5,11);
CBlanket Blanket2(10,11);
CBlanket Blanket3(5,16);
CBlanket Blanket4(10,16);
bool SouthTurn;

// 프로그램을 총지휘하는 main 함수
int main() {
	int i,ch,a;
	int arSame[4],SameNum;
	char Mes[256];
	CPlayer *Turn;
	int UserIdx,UserSel,DeckSel;
	SCard UserCard, DeckCard;
	bool UserTriple, DeckTriple;
	int nSnatch;
	int NewScore;

	randomize();
	Initialize();
	DrawScreen();

	for (SouthTurn=true;!Deck.IsEmpty();SouthTurn=!SouthTurn) {
		DrawScreen();
		if (SouthTurn) {
			Turn=&South;
		} else {
			Turn=&North;
		}
		sprintf(Mes,"내고 싶은 화투를 선택하세요(1~%d,0:종료) ",Turn->GetNum());
  		ch=InputInt(Mes,0,Turn->GetNum());
		if (ch == 0) {
			if (InputInt("정말 끝낼겁니까?(0:예,1:아니오)",0,1)==0) 
				return 0;
			else 
				continue;
		}
		
		// 플레이어가 카드를 한장 낸다.
		UserTriple=DeckTriple=false;
		UserIdx=ch-1;
		UserCard=Turn->GetCard(UserIdx);
		//SameNum=Blanket.FindSameCard(UserCard,arSame);
		DrawScreen();
		sprintf(Mes,"카드를 올리고 싶은 택의 번호를 선택하세요(1~4) ",Turn->GetNum());
		a=InputInt(Mes,0,Turn->GetNum());
		switch (a) {
		case 1:
			UserSel=-1;
			Blanket.InsertCard(Turn->RemoveCard(UserIdx));
			DrawScreen();
			break;
		case 2:
			UserSel=-1;
			Blanket2.InsertCard(Turn->RemoveCard(UserIdx));
			DrawScreen();
			break;
		case 3:
			UserSel=-1;
			Blanket3.InsertCard(Turn->RemoveCard(UserIdx));
			DrawScreen();
			break;
		case 4:
			UserSel=-1;
			Blanket4.InsertCard(Turn->RemoveCard(UserIdx));
			DrawScreen();
			break;
		}
		// 데크에서 한장을 뒤집는다.
		Deck.Draw(true);
		delay(Speed);
		DeckCard=Deck.Pop();
		//SameNum=Blanket.FindSameCard(DeckCard,arSame);
		switch (SameNum) {
		case 0:
			DeckSel=-1;
			break;
/*		case 1:
			DeckSel=arSame[0];
			if (DeckSel == UserSel) {
				if (Deck.IsNotLast()) {
					Blanket.InsertCard(DeckCard);
					Blanket.InsertCard(Turn->RemoveCard(UserIdx));
					OutPrompt("설사했습니다.",PromptSpeed);
					continue;
				} else {
					DeckSel=-1;
				}
			}
			break;
*/
		case 2:
			if (UserSel == arSame[0]) {
				DeckSel=arSame[1];
			} else if (UserSel == arSame[1]) {
				DeckSel=arSame[0];
			} else {
				if (Blanket.GetCard(arSame[0]) == Blanket.GetCard(arSame[1])) {
					DeckSel=arSame[0];
				} else {
					Blanket.DrawSelNum(arSame);
					sprintf(Mes,"어떤 카드를 선택하시겠습니까?(1~%d)",SameNum);
					DeckSel=arSame[InputInt(Mes,1,SameNum)-1];
				}
			}
			break;
		case 3:
			DeckSel=arSame[1];
			DeckTriple=true;
			break;
		}
		if (DeckSel != -1) {
			Blanket.DrawTempCard(DeckSel,DeckCard);
		}
		Deck.Draw(false);
		delay(Speed);
	}
	DrawScreen();
	OutPrompt("게임이 끝났습니다.",0);
}

void Initialize() {
	int i;

	Deck.Shuffle();
	for (i=0;i<8;i++) {
		South.InsertCard(Deck.Pop());
		North.InsertCard(Deck.Pop());
		if (i < 1) Blanket.InsertCard(Deck.Pop());
		if (i < 1) Blanket2.InsertCard(Deck.Pop());
		if (i < 1) Blanket3.InsertCard(Deck.Pop());
		if (i < 1) Blanket4.InsertCard(Deck.Pop());
	}
}

void DrawScreen() {
	clrscr();
	South.Draw(SouthTurn);
	North.Draw(!SouthTurn);
	Blanket.Draw();
	gotoxy(5,12);
	cout << '[' << 1 << ']';
	Blanket2.Draw();
	gotoxy(10,12);
	cout << '[' << 2 << ']';
	Blanket3.Draw();
	gotoxy(5,17);
	cout << '[' << 3 << ']';
	Blanket4.Draw();
	gotoxy(10,17);
	cout << '[' << 4 << ']';
//lanket.Draw();
	Deck.Draw(false);
}

void OutPrompt(const char *Mes,int Wait/*=0*/) {
	gotoxy(5,23);
	for (int i=5;i<79;i++) { cout << ' '; }
	gotoxy(5,23);
	cout << Mes;
	delay(Wait);
}

int InputInt(const char *Mes, int start, int end) {
	int ch;

	OutPrompt(Mes);
	for (;;) {
		ch=tolower(getch());
		if (ch == 0xE0 || ch == 0) {
			ch=getch();
			continue;
		}
		if (!(isdigit(ch) || ch=='a')) continue;
		if (ch=='a') ch=10; else ch=ch-'0';
		if (ch >= start && ch <= end) {
			return ch;
		}
		OutPrompt("무효한 번호입니다. 지정한 범위에 맞게 다시 입력해 주세요.");
	}
}


