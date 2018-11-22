//線一本でいろいろ書くやつ
/*
       ┏              7  0  0             ┓
  ┏   ┏━━━━━━━━━━━━━━━━━┓
  　   ┃                                  ┃
   3  2┃                                  ┃
      0┃         d r a w   A r e a        ┃
      0┃                                  ┃
   0   ┃                                  ┃
       ┣━━━━━━━━━━━━━━━━━┫
      1┃                                  ┃
   0  0┃         m e n u   A r e a        ┃
      0┃                                  ┃
  ┗   ┗━━━━━━━━━━━━━━━━━┛
  
*/
#include "DxLib.h"
#include <math.h>
#include <vector>
#include <string>

#define ENTRYMODEBUTTONNUM 5
#define WRITEMODEBUTTONNUM 2

enum MenuMode{
	Entry   = 0,
	Write   = 1,
	Explain = 2,
	Menu    = 3
};

enum EntryMode {
	Wait       = 0,
	First      = 1,
	Second     = 2,
	Cancel     = 3,
	BackToMenu = 4
};

struct XY{
	int x,y;
	XY();
	XY(int x, int y);
};

struct Area {
	//pos1を左上，pos2を右下の座標とした長方形
	XY pos1;//左上座標
	XY pos2;//右下座標
	Area();
	Area(XY pos1, XY pos2);
	bool isIn();//マウスがArea内に入っているか
	void drawBox(unsigned int color, int FillFlag = FALSE);//領域をcolorで囲む他か塗りつぶす
};

//ボタン
struct Button {
	Area area;		//ボタンの位置・大きさ
	bool selected;	//true:押されている  false:押されていない
	int type;		//識別番号
	std::string message;//表示させる文字列

	Button();
	Button(Area area, int type);
	void draw(int FillFlag = FALSE);
	void select();
	void unselect();
	bool isSelected();//押されているかどうか
	void setMessage(std::string message);//messageに文字列を格納
	void printMessage();//messageに格納された文字列を表示
};

//Entryモードにて登録する線用
struct Line{
	XY src;		//始点
	XY dst;		//終点
	Line();
	Line(XY src, XY dst);
	void draw(unsigned int color);
};

struct Mouse {
	int x, y;
	void getPosition();
};

//
struct OriginalLine {
	Line entry_line;	//Entryモードで登録されたLine
	Line origin_line;	//entry_lineのsrcを原点に合わせた表示
	double length;		//極座標における線の長さ
	double theta;		//極座標における線の角度(rad)
	OriginalLine();
	OriginalLine(Line line);	//極座標表示に変換
	void draw();
};

struct ConvertLine {
	Line entry_line;	//Entryモードで登録されたLine
	Line origin_line;	//entry_lineのsrcを原点に合わせた表示
	double length_mag;	//極座標におけるOriginalLineの長さとの比
	double theta_deff;	//極座標におけるOriginalLineの角度との差(rad)
	ConvertLine();
	ConvertLine(Line line);
	//length_magとtheta_deffを計算(originalは基準線…赤い線)
	void setParam(OriginalLine original); 
	//入力座標(pos1)を原点(0, 0)を中心としてlength_mag倍し，theta_deff(rad)回転した座標地を返す(originalは基準線…赤い線)
	XY convertPos(XY pos1, OriginalLine original);	
};

const double PI = 3.14159265;
XY cancel(98765, 98765);

//window関係のエリア
Area windowArea(XY(0, 0), XY(700, 300));
Area drawArea(XY(0, 0), XY(700, 200));
Area menuArea(XY(0, drawArea.pos2.y), windowArea.pos2);
//マウスたち
Mouse mouse;
Mouse mouse_old;
//色たち
unsigned int Cr;
unsigned int White;
unsigned int Black;
unsigned int Red;
unsigned int RandomColor;
//基準線(赤い線)
OriginalLine original;
//基準線を元に変換する対象の線たち
std::vector<ConvertLine> convline;

/********************  関数たち  ********************/

//もろもろの初期化 読み込みなど
void init();
//メニュー画面 戻り値はモードの番号
MenuMode menu();
//mouseがpos1からpos2の長方形内に入っているか
bool isIn(XY pos1, XY pos2);
/********************  登録ここから  ********************/
MenuMode entry();
//イベント待ち
EntryMode selectWaiting(std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//始点登録
EntryMode setFirstPosition(XY &first, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//終点選択
EntryMode setSecondPosition(XY &second, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM], XY first);
//ボタンイベント
EntryMode entryButtonEvent(EntryMode now_mode, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//登録ボタンが押された際の処理
EntryMode entryLine(std::vector<Line> &line);
/********************  登録ここまで  ********************/
/********************  書くここから  ********************/
MenuMode write();
//ボタンイベント 戻り値は次の状態(1ならメニューへ)
int writeButtonEvent(Button button[WRITEMODEBUTTONNUM]);
/********************  書くここまで  ********************/
/********************  操作説明ここから  ********************/
MenuMode explain();
/********************  操作説明ここまで  ********************/
//void draw();


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	SetGraphMode( windowArea.pos2.x , windowArea.pos2.y , 32 ) ;
	ChangeWindowMode( TRUE ) ;
	

	if( DxLib_Init() == -1 )		// ＤＸライブラリ初期化処理
	{
		return -1 ;			// エラーが起きたら直ちに終了
	}
	init();
	
	SetDrawScreen( DX_SCREEN_BACK ) ; 

	MenuMode mode = Menu;

	mouse.getPosition();
	mouse_old = mouse;
	
	while(ProcessMessage() == 0){
		switch(mode){
		case Write:
			mode = write();
			break;
		case Entry:
			mode = entry();
			break;
		case Explain:
			mode = explain();
			break;
		case Menu:
			mode = menu();
			break;

		}
	}

	DxLib_End() ;			// ＤＸライブラリ使用の終了処理

	return 0 ;				// ソフトの終了 
}

////////////////////////////////////////// struct XY ////////////////////////////////////////////// 

XY::XY(){}
XY::XY(int x, int y){
	this->x = x;
	this->y = y;
}

////////////////////////////////////////// struct XY //////////////////////////////////////////////
////////////////////////////////////////// struct Area //////////////////////////////////////////// 

Area::Area() {}
Area::Area(XY pos1, XY pos2) {
	//pos1が左上pos2が右下にあるかチェック
	if (pos1.x <= pos2.x && pos1.y <= pos2.y) {
		this->pos1 = pos1;
		this->pos2 = pos2;
	}
	else {
		clsDx();
		printfDx("Error: Area::Area(pos1, pos2)  位置が不正です\n");
		ScreenFlip();
		WaitKey();
		exit(-1);
	}
}
bool Area::isIn() {
	mouse.getPosition();
	if (pos1.x <= mouse.x && mouse.x <= pos2.x) {
		if (pos1.y <= mouse.y && mouse.y <= pos2.y) {
			return true;
		}
	}
	return false;
}
void Area::drawBox(unsigned int color, int FillFlag) {
	DrawBox(pos1.x, pos1.y, pos2.x, pos2.y, color, FillFlag);
}
////////////////////////////////////////// struct Area ///////////////////////////////////////////
////////////////////////////////////////// struct Button /////////////////////////////////////////

Button::Button() {
	selected = false;
}
Button::Button(Area area, int type) {
	this->area = area;
	this->type = type;
	selected = false;
}
void Button::draw(int FillFlag) {
	if (selected) {
		area.drawBox(Red, FillFlag);
	}
	else {
		area.drawBox(White, FillFlag);
	}
}
void Button::select() {
	selected = true;
}
void Button::unselect() {
	selected = false;
}
bool Button::isSelected() {
	return selected;
}
void Button::setMessage(std::string message) {
	this->message = message;
}
void Button::printMessage() {
	if (selected) {
		DrawString(area.pos1.x, area.pos1.y, message.c_str(), Red);
	}
	else {
		DrawString(area.pos1.x, area.pos1.y, message.c_str(), White);
	}
}

////////////////////////////////////////// struct Button /////////////////////////////////////////
///////////////////////////////////////// struct Line ////////////////////////////////////////////

Line::Line(){}
Line::Line(XY src, XY dst){
	this->src = src;
	this->dst = dst;
}
void Line::draw(unsigned int color){
	DrawLine(src.x, src.y, dst.x, dst.y, color);
}

///////////////////////////////////////// struct Line ////////////////////////////////////////////
///////////////////////////////// struct OriginalLine ////////////////////////////////////////////
OriginalLine::OriginalLine(){}
OriginalLine::OriginalLine(Line line) {
	//Line lineを基準の線として登録
	entry_line = line;
	//lineをsrcを原点に合わせた形にして登録
	XY origin_src(0, 0);
	XY origin_dst(line.dst.x-line.src.x, line.dst.y - line.src.y);
	origin_line = Line(origin_src, origin_dst);
	//長さ・角度を計算して格納
	length = sqrt((origin_line.dst.x*origin_line.dst.x) + (origin_line.dst.y*origin_line.dst.y));
	theta = atan2(origin_line.dst.y, origin_line.dst.x);
}
void OriginalLine::draw() {
	entry_line.draw(Red);
}
///////////////////////////////// struct OriginalLine ////////////////////////////////////////////
///////////////////////////////// struct ConvertLine  //////////////////////////////////////////
ConvertLine:: ConvertLine(){}
ConvertLine::ConvertLine(Line line) {
	//Line lineを基準の線として登録
	entry_line = line;
	//lineをsrcを原点に合わせた形にして登録
	XY origin_src(0, 0);
	XY origin_dst(line.dst.x - line.src.x, line.dst.y - line.src.y);
	origin_line = Line(origin_src, origin_dst);
}
void ConvertLine::setParam(OriginalLine original) {
	double length = sqrt((origin_line.dst.x*origin_line.dst.x) + (origin_line.dst.y*origin_line.dst.y));
	double theta = atan2(origin_line.dst.y, origin_line.dst.x);
	length_mag = length / original.length;
	theta_deff = theta - original.theta;
}
//入力座標(pos1)を原点(0, 0)を中心としてlength_mag倍し，theta_deff(rad)回転した座標地を返す
XY ConvertLine::convertPos(XY pos1, OriginalLine original) {
	pos1.x -= original.entry_line.src.x;
	pos1.y -= original.entry_line.src.y;
	double pos1_length = sqrt(pos1.x*pos1.x + pos1.y*pos1.y);
	double pos1_theta = atan2(pos1.y, pos1.x);

	double new_length = pos1_length * length_mag;
	double new_theta = pos1_theta + theta_deff;

	return XY(new_length * cos(new_theta) + entry_line.src.x, new_length * sin(new_theta) + entry_line.src.y);
}
///////////////////////////////// struct ConvertLine  ////////////////////////////////////////////
//////////////////////////////////////////struct Mouse ///////////////////////////////////////////

//座標地取得
void Mouse::getPosition() {
	GetMousePoint(&x, &y);
}

//////////////////////////////////////////struct Mouse ///////////////////////////////////////////
void init(){
	White = GetColor(255, 255, 255);
	Black = GetColor(  0,   0,   0);
	Red   = GetColor(255,   0,   0);
}

//////////////////////////////////////////// menu ////////////////////////////////////////////////
//メニュー 戻り値はモード番号
MenuMode menu(){
	/*    ┏    size    ┓┏    size    ┓┏    size    ┓
	┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
	┃                      s p a c e 3                       ┃
	┃  s  s┏━━━━┓s  s┏━━━━┓s  s┏━━━━┓s     ┃
	┃  p  p┃   0    ┃p  p┃   1    ┃p  p┃   2    ┃p     ┃
	┃  a  a┃        ┃a  a┃        ┃a  a┃        ┃a     ┃
	┃  c  c┃  登録  ┃c  c┃  書く  ┃c  c┃  説明  ┃c     ┃
	┃  e  e┃        ┃e  e┃        ┃e  e┃        ┃e     ┃
	┃  1  2┗━━━━┛2  2┗━━━━┛2  2┗━━━━┛2     ┃
	┃                                                        ┃
	┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
	*/
	const int space1 = 50;
	const int space2 = 20;
	const int space3 = 60;
	const int size = 200;
	bool isDraw = true;
	Button button[3];
	std::string button_message[3] = { "登録" ,"書く" , "説明" };
	int putnum = -1;		//選択されているモード番号
	bool isClick = false;	//クリックされているか

	//ボタンの情報をセット
	for (int i = 0; i < 3; i++) {
		XY pos1(i*size + space2 + space1, space3);
		XY pos2((i + 1)*size - space2 + space1, windowArea.pos2.y - space3);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	while(ProcessMessage() == 0){
		//描画
		if(isDraw){
			ClearDrawScreen();
			for(int i=0;i<3;i++){
				button[i].draw();
				button[i].printMessage();
			}
			ScreenFlip();
			isDraw = false;
		}

		//クリックされたら
		if(( GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 && !isClick){
			isClick = true;
			//どこが(どのボタンが)押されたかを調べる
			for(int i=0;i<3;i++){
				if (button[i].area.isIn()) {
					button[i].select();
					putnum = i;
					isDraw = true;
					break;
				}
			}
		}
		//はなされたら
		else if(( GetMouseInput() & MOUSE_INPUT_LEFT ) == 0 && isClick){
			isClick = false;
			//押されたボタンの場所ではなされたらその番号を返す
			if (putnum != -1) {
				if (button[putnum].area.isIn()) {
					return (MenuMode)button[putnum].type;
				}
				else {
					button[putnum].unselect();
					putnum = -1;
					isDraw = true;
				}
			}
		}
		
	}
	return Menu;
}
//mouseがpos1(左上)からpos2(右下)の長方形内に入っているか
bool isIn(XY pos1, XY pos2){
	mouse.getPosition();
	if(pos1.x <= mouse.x && mouse.x <= pos2.x){
		if(pos1.y <= mouse.y && mouse.y <= pos2.y){
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////// menu  ////////////////////////////////////////////////
//////////////////////////////////////////// entry ////////////////////////////////////////////////
MenuMode entry() {
	//モード… 0:操作待ち  1:1点目選択  2:2点目選択
	EntryMode mode = Wait;
	//押されたボタン  0:取り消し  1:1つ戻る  2:全消し  3:登録  4:メニューへ
	int putnum = -1;
	std::vector<Line> line;
	XY first, second;
	Button button[ENTRYMODEBUTTONNUM];
	//ボタンの登録
	std::string button_message[ENTRYMODEBUTTONNUM] = { "取り消し", "1つ戻る", "全消し","登録","メニューへ" };
	int size = 100;
	int space1 = 10;
	int space2 = 10;
	for (int i = 0; i < ENTRYMODEBUTTONNUM; i++) {
		XY pos1(i*size + space2 + space1, menuArea.pos1.y + space2);
		XY pos2((i + 1)*size - space2 + space1, menuArea.pos2.y - space2);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	//メインループ
	while (ProcessMessage() == 0) {
		switch (mode) {
		case Wait:
			mode = selectWaiting(line, button);
			break;
		case First:
			mode = setFirstPosition(first, line, button);
			break;
		case Second:
			mode = setSecondPosition(second, line, button, first);
			//キャンセルが返ってきたら登録せずmode0へ
			if (mode == Cancel) {
				break;
			}
			//firstとsecondが同じ点なら登録しない
			if (first.x != second.x || first.y != second.y) {
				line.push_back(Line(first, second));
			}
			break;
		case Cancel:
			mode = Wait;
			break;
		case BackToMenu:
			return Menu;
		}
	}
	return Menu;
}
//イベント待ち
EntryMode selectWaiting(std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	/*何かやることがあればする*/
	return First;
}
//始点選択
EntryMode setFirstPosition(XY &first, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	//クリックされたときに登録する
	bool isClick = false;
	EntryMode next_mode;
	while (ProcessMessage() == 0) {
		clsDx();
		ClearDrawScreen();
		menuArea.drawBox(Red);
		for (int i = 0; i < ENTRYMODEBUTTONNUM; i++) {
			button[i].draw();
			button[i].printMessage();
		}
		printfDx("線を引いてください\n");
		//すでに登録済みの辺を描画
		for (int i = 0; i < line.size(); i++) {
			if (i == 0) {
				line[i].draw(Red);
			}
			else {
				line[i].draw(White);
			}
		}
		//ボタンイベント
		next_mode = entryButtonEvent(First, line, button);
		if (next_mode != First) {
			return next_mode;
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
			isClick = true;
			//領域(drawarea)内でマウスが押されたら
			if (drawArea.isIn()) {
				first = XY(mouse.x, mouse.y);
				return Second;
			}
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
			isClick = false;
		}
		ScreenFlip();
	}
	return Wait;
}
//終点選択
EntryMode setSecondPosition(XY &second, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM], XY first) {
	//クリック終わり(ボタンが離された時)に登録する
	bool isClick = true;
	EntryMode next_mode;
	while (ProcessMessage() == 0) {
		clsDx();
		ClearDrawScreen();
		menuArea.drawBox(Red);
		for (int i = 0; i < ENTRYMODEBUTTONNUM; i++) {
			button[i].draw();
			button[i].printMessage();
		}
		printfDx("線を引いてください\n");
		//すでに登録済みの辺を描画
		for (int i = 0; i < line.size(); i++) {
			if (i == 0) {
				line[i].draw(Red);
			}
			else {
				line[i].draw(White);
			}
		}
		//ボタンイベント
		next_mode = entryButtonEvent(Second, line, button);
		//現状態と変化すれば状態遷移
		if (next_mode != Second) {
			return next_mode;
		}
		//二点目の選択，描画
		mouse.getPosition();
		DrawLine(first.x, first.y, mouse.x, mouse.y, White);
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			isClick = true;
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
			isClick = false;
			//領域(drawarea)内でマウスが離されたら
			if (drawArea.isIn()) {
				second = XY(mouse.x, mouse.y);
				return Wait;
			}
		}
		ScreenFlip();
	}
	return Wait;
}
//ボタンイベント
EntryMode entryButtonEvent(EntryMode now_mode, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	static bool isClick = false;
	static int putnum = -1;
	//クリックされたら
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
		isClick = true;
		//どこが(どのボタンが)押されたかを調べる
		for (int i = 0; i<ENTRYMODEBUTTONNUM; i++) {
			if (button[i].area.isIn()) {
				button[i].select();
				putnum = i;
				break;
			}
		}
	}
	//はなされたら
	else if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
		isClick = false;
		//押されたボタンの場所ではなされたらいろいろと処理をする
		if (putnum != -1) {
			button[putnum].unselect();
			int putnum_buf = putnum;
			putnum = -1;
			if (button[putnum_buf].area.isIn()) {
				switch (putnum_buf) {
				//取り消し
				case 0:
					return Cancel;
				//1つ戻る
				case 1:
					if (line.size() > 0) {
						line.pop_back();
					}
					return now_mode;
				//全消し
				case 2:
					line.clear();
					return now_mode;
				//登録
				case 3:
					entryLine(line);
					return now_mode;
					break;
				//メニューへ
				case 4:
					return BackToMenu;
				}
			}
			else {
				
			}
		}
	}
	return now_mode;
}
//登録ボタンが押された際の処理
EntryMode entryLine(std::vector<Line> &line) {
	//登録された線を一度リセット
	convline.clear();
	//線が引かれていなければ登録しない
	if (line.size() < 1) {
		return BackToMenu;
	}
	//基準線(赤い線)の登録
	original = OriginalLine(line[0]);
	//その他の線の登録
	for (int i = 1; i < line.size(); i++) {
		convline.push_back(ConvertLine(line[i]));
		convline[i - 1].setParam(original);
	}
	return BackToMenu;
}
//////////////////////////////////////////// entry ////////////////////////////////////////////////
//////////////////////////////////////////// write ////////////////////////////////////////////////

//メイン
MenuMode write() {
	//次のモード
	int next_mode;
	//ボタンの設定
	int size = 100;
	int space1 = 10;
	int space2 = 10;
	Button button[WRITEMODEBUTTONNUM];
	std::string button_message[WRITEMODEBUTTONNUM] = {"全消し", "メニューへ"};
	for (int i = 0; i < WRITEMODEBUTTONNUM; i++) {
		XY pos1(i*size + space2 + space1, menuArea.pos1.y + space2);
		XY pos2((i + 1)*size - space2 + space1, menuArea.pos2.y - space2);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	//描画
	clsDx();
	ClearDrawScreen();

	//メインループ
	while (ProcessMessage() == 0) {
		mouse.getPosition();
		//左を押してドラッグで描画
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			if (drawArea.isIn()) {
				DrawLine(mouse_old.x, mouse_old.y, mouse.x, mouse.y, White);
				for (int i = 0; i < convline.size(); i++) {
					XY pos = convline[i].convertPos(XY(mouse.x, mouse.y), original);
					XY pos_old = convline[i].convertPos(XY(mouse_old.x, mouse_old.y), original);
					DrawLine(pos_old.x, pos_old.y, pos.x, pos.y, White);
				}
			}
		}
		//右を押してドラッグで描画
		else if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			if (drawArea.isIn()) {
				RandomColor = GetColor(GetRand(255), GetRand(255), GetRand(255));
				DrawLine(mouse_old.x, mouse_old.y, mouse.x, mouse.y, RandomColor);
				for (int i = 0; i < convline.size(); i++) {
					XY pos = convline[i].convertPos(XY(mouse.x, mouse.y), original);
					XY pos_old = convline[i].convertPos(XY(mouse_old.x, mouse_old.y), original);
					DrawLine(pos_old.x, pos_old.y, pos.x, pos.y, RandomColor);
				}
			}
		}
		next_mode = writeButtonEvent(button);
		switch (next_mode) {
		case 0:
			break;
		case 1:
			return Menu;
		}
		menuArea.drawBox(Black, TRUE);
		menuArea.drawBox(Red);
		original.draw();
		for (int i = 0; i < WRITEMODEBUTTONNUM; i++) {
			button[i].draw();
			button[i].printMessage();
		}
		ScreenFlip();
		mouse_old = mouse;
	}
	return Menu;
}
//ボタンイベント 戻り値は次の状態(1ならメニューへ)
int writeButtonEvent(Button button[WRITEMODEBUTTONNUM]) {
	static bool isClick = false;
	static int putnum = -1;
	//クリックされたら
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
		isClick = true;
		//どこが(どのボタンが)押されたかを調べる
		for (int i = 0; i<WRITEMODEBUTTONNUM; i++) {
			if (button[i].area.isIn()) {
				button[i].select();
				putnum = i;
				break;
			}
		}
	}
	//はなされたら
	else if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
		isClick = false;
		//押されたボタンの場所ではなされたらいろいろと処理をする
		if (putnum != -1) {
			button[putnum].unselect();
			int putnum_buf = putnum;
			putnum = -1;
			if (button[putnum_buf].area.isIn()) {
				switch (putnum_buf) {
					//全消し
				case 0:
					ClearDrawScreen();
					return 0;
					//メニューへ
				case 1:
					return 1;
				}
			}
			else {

			}
		}
	}
	return 0;
}
//////////////////////////////////////////// write ////////////////////////////////////////////////
//////////////////////////////////////////// explain //////////////////////////////////////////////
MenuMode explain() {
	Button button(menuArea, 0);
	button.setMessage(std::string("分かった"));
	bool isClick = false;
	while (ProcessMessage() == 0) {
		clsDx();
		ClearDrawScreen();
		button.draw();
		button.printMessage();
		printfDx("操作説明：\n適当にやってね");
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
			isClick = true;
			if (button.area.isIn()) {
				button.select();
			}
		}
		else if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
			isClick = false;
			if (button.area.isIn()) {
				clsDx();
				return Menu;
			}
			else {
				button.unselect();
			}
		}
		ScreenFlip();
	}
	return Menu;
}
//////////////////////////////////////////// explain //////////////////////////////////////////////