#include <iostream>
#include <bangtal>
#include <cstdio>
#include <time.h>
#include <Windows.h>

using namespace bangtal;
using namespace std;

ObjectPtr board[8][8];

enum class State {
	BLANK,
	PASSIBLE,
	BLACK,
	WHITE
};

State state[8][8];

enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK;

void setState(int x, int y, State s) {
	switch(s) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::PASSIBLE: board[y][x]->setImage(turn ==Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	state[y][x] = s; //현재 보드의 상태를 저장하는 변수
}

bool checkPossible(int x, int y, int dx, int dy) {//x,y 현재위치, dx,dy 가고자 하는 방향 오버
	//내가 놓을 턴이랑 위치 확인. 즉, 내돌 다음에 상대돌로 끝나야함.
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	//내가 체크하는 곳부터 시작해서 other이 나오다가 self인게 나와야지 놓을 수 잇음.
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy){ //x는 놓여지는 위치에서 dx,dy만큼 움직인것을 체크해야함. 언제까지? 보드를 벗어날때까지, 다시 증가시켜줌.
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) {
			return possible;
		}
		else {//공백일경우
			return false;
		}
	}
	return false;
}

bool checkPossible(int x, int y) {
	//배열이용해서 체크
	int delta[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };

	if (state[y][x] == State::BLACK) return false;
	if (state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK); // 각 턴에 맞는 돌의 위치만 보여주고, 다른 턴은 블랭크로 바꿔주기.

	/*for (int i = 0; i < 8, i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);
	}*/

	bool possible = false; //내가 움질일수 있는 지 없는지, 없으면 게임 패배가 됨.
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;
	}

	return possible;
}
void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;

	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) { //x는 놓여지는 위치에서 dx,dy만큼 움직인것을 체크해야함. 언제까지? 보드를 벗어날때까지, 다시 증가시켜줌.
		if (state[y][x] == other) {
			possible = true;
		}
		else if (state[y][x] == self) {
			if (possible) {
				//x,y를 증가시키면서 왔으니까. 반대로 감소시키면서 거꾸로 바꿔줌.
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) { //판안에 있는건 이미 확인했으니 사시 렁ㅄ어도 가능은 함
					if (state[y][x] == other) {
						setState(x, y, self);
					}
					else return;
				}
			}
		}
		else {//공백일경우
			return;
		}
	}
}



void reverse(int x, int y) {
	int delta[8][2] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };

	bool possible = false; //내가 움질일수 있는 지 없는지, 없으면 게임 패배가 됨.
	for (auto d : delta) {
		reverse(x, y, d[0], d[1]);
	}
}

bool setPossible() //8x8믜 모든 위치에 대해서 놓여질 수 있으면 상태를 possible로 바꾼다.
{
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) { //체크하는 함수
				setState(x,y,State::PASSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	auto scene = Scene::create("오델로", "Images/background.png");


	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			//마우스 x, y는 사실 없어도됨. 그리고 사용할 x,y도 각각 값 call by value으로 가져와야하니까 참조할때 써줌.
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr, int, int, MouseAction)->bool {
				if (state[y][x] == State::PASSIBLE) {
					if (turn == Turn::BLACK) {
						setState(x, y, State::BLACK);
						reverse(x, y); // 돌 바꾸기 함수
						turn = Turn::WHITE;
					}
					else {
						setState(x, y, State::WHITE);
						reverse(x, y); // 돌 바꾸기 함수
						turn = Turn::BLACK;
					}
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK; // toggle
						if (!setPossible) {
							showMessage("게임이 끝났습니다.");
						 }
					}; // 턴이 다음턴이 됐을때, 그 턴에 맞는 게 보임. 이걸 통해서 승패를 알 수 있어야함.
				}
				return true;
			});
			state[y][x] = State::BLANK;
		}
	}
	
	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();

	startGame(scene); 
}