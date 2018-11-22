//����{�ł��낢�돑�����
/*
       ��              7  0  0             ��
  ��   ��������������������������������������
  �@   ��                                  ��
   3  2��                                  ��
      0��         d r a w   A r e a        ��
      0��                                  ��
   0   ��                                  ��
       ��������������������������������������
      1��                                  ��
   0  0��         m e n u   A r e a        ��
      0��                                  ��
  ��   ��������������������������������������
  
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
	//pos1������Cpos2���E���̍��W�Ƃ��������`
	XY pos1;//������W
	XY pos2;//�E�����W
	Area();
	Area(XY pos1, XY pos2);
	bool isIn();//�}�E�X��Area���ɓ����Ă��邩
	void drawBox(unsigned int color, int FillFlag = FALSE);//�̈��color�ň͂ޑ����h��Ԃ�
};

//�{�^��
struct Button {
	Area area;		//�{�^���̈ʒu�E�傫��
	bool selected;	//true:������Ă���  false:������Ă��Ȃ�
	int type;		//���ʔԍ�
	std::string message;//�\�������镶����

	Button();
	Button(Area area, int type);
	void draw(int FillFlag = FALSE);
	void select();
	void unselect();
	bool isSelected();//������Ă��邩�ǂ���
	void setMessage(std::string message);//message�ɕ�������i�[
	void printMessage();//message�Ɋi�[���ꂽ�������\��
};

//Entry���[�h�ɂēo�^������p
struct Line{
	XY src;		//�n�_
	XY dst;		//�I�_
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
	Line entry_line;	//Entry���[�h�œo�^���ꂽLine
	Line origin_line;	//entry_line��src�����_�ɍ��킹���\��
	double length;		//�ɍ��W�ɂ�������̒���
	double theta;		//�ɍ��W�ɂ�������̊p�x(rad)
	OriginalLine();
	OriginalLine(Line line);	//�ɍ��W�\���ɕϊ�
	void draw();
};

struct ConvertLine {
	Line entry_line;	//Entry���[�h�œo�^���ꂽLine
	Line origin_line;	//entry_line��src�����_�ɍ��킹���\��
	double length_mag;	//�ɍ��W�ɂ�����OriginalLine�̒����Ƃ̔�
	double theta_deff;	//�ɍ��W�ɂ�����OriginalLine�̊p�x�Ƃ̍�(rad)
	ConvertLine();
	ConvertLine(Line line);
	//length_mag��theta_deff���v�Z(original�͊���c�Ԃ���)
	void setParam(OriginalLine original); 
	//���͍��W(pos1)�����_(0, 0)�𒆐S�Ƃ���length_mag�{���Ctheta_deff(rad)��]�������W�n��Ԃ�(original�͊���c�Ԃ���)
	XY convertPos(XY pos1, OriginalLine original);	
};

const double PI = 3.14159265;
XY cancel(98765, 98765);

//window�֌W�̃G���A
Area windowArea(XY(0, 0), XY(700, 300));
Area drawArea(XY(0, 0), XY(700, 200));
Area menuArea(XY(0, drawArea.pos2.y), windowArea.pos2);
//�}�E�X����
Mouse mouse;
Mouse mouse_old;
//�F����
unsigned int Cr;
unsigned int White;
unsigned int Black;
unsigned int Red;
unsigned int RandomColor;
//���(�Ԃ���)
OriginalLine original;
//��������ɕϊ�����Ώۂ̐�����
std::vector<ConvertLine> convline;

/********************  �֐�����  ********************/

//�������̏����� �ǂݍ��݂Ȃ�
void init();
//���j���[��� �߂�l�̓��[�h�̔ԍ�
MenuMode menu();
//mouse��pos1����pos2�̒����`���ɓ����Ă��邩
bool isIn(XY pos1, XY pos2);
/********************  �o�^��������  ********************/
MenuMode entry();
//�C�x���g�҂�
EntryMode selectWaiting(std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//�n�_�o�^
EntryMode setFirstPosition(XY &first, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//�I�_�I��
EntryMode setSecondPosition(XY &second, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM], XY first);
//�{�^���C�x���g
EntryMode entryButtonEvent(EntryMode now_mode, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]);
//�o�^�{�^���������ꂽ�ۂ̏���
EntryMode entryLine(std::vector<Line> &line);
/********************  �o�^�����܂�  ********************/
/********************  ������������  ********************/
MenuMode write();
//�{�^���C�x���g �߂�l�͎��̏��(1�Ȃ烁�j���[��)
int writeButtonEvent(Button button[WRITEMODEBUTTONNUM]);
/********************  ���������܂�  ********************/
/********************  ���������������  ********************/
MenuMode explain();
/********************  ������������܂�  ********************/
//void draw();


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	SetGraphMode( windowArea.pos2.x , windowArea.pos2.y , 32 ) ;
	ChangeWindowMode( TRUE ) ;
	

	if( DxLib_Init() == -1 )		// �c�w���C�u��������������
	{
		return -1 ;			// �G���[���N�����璼���ɏI��
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

	DxLib_End() ;			// �c�w���C�u�����g�p�̏I������

	return 0 ;				// �\�t�g�̏I�� 
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
	//pos1������pos2���E���ɂ��邩�`�F�b�N
	if (pos1.x <= pos2.x && pos1.y <= pos2.y) {
		this->pos1 = pos1;
		this->pos2 = pos2;
	}
	else {
		clsDx();
		printfDx("Error: Area::Area(pos1, pos2)  �ʒu���s���ł�\n");
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
	//Line line����̐��Ƃ��ēo�^
	entry_line = line;
	//line��src�����_�ɍ��킹���`�ɂ��ēo�^
	XY origin_src(0, 0);
	XY origin_dst(line.dst.x-line.src.x, line.dst.y - line.src.y);
	origin_line = Line(origin_src, origin_dst);
	//�����E�p�x���v�Z���Ċi�[
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
	//Line line����̐��Ƃ��ēo�^
	entry_line = line;
	//line��src�����_�ɍ��킹���`�ɂ��ēo�^
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
//���͍��W(pos1)�����_(0, 0)�𒆐S�Ƃ���length_mag�{���Ctheta_deff(rad)��]�������W�n��Ԃ�
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

//���W�n�擾
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
//���j���[ �߂�l�̓��[�h�ԍ�
MenuMode menu(){
	/*    ��    size    ����    size    ����    size    ��
	������������������������������������������������������������
	��                      s p a c e 3                       ��
	��  s  s������������s  s������������s  s������������s     ��
	��  p  p��   0    ��p  p��   1    ��p  p��   2    ��p     ��
	��  a  a��        ��a  a��        ��a  a��        ��a     ��
	��  c  c��  �o�^  ��c  c��  ����  ��c  c��  ����  ��c     ��
	��  e  e��        ��e  e��        ��e  e��        ��e     ��
	��  1  2������������2  2������������2  2������������2     ��
	��                                                        ��
	������������������������������������������������������������
	*/
	const int space1 = 50;
	const int space2 = 20;
	const int space3 = 60;
	const int size = 200;
	bool isDraw = true;
	Button button[3];
	std::string button_message[3] = { "�o�^" ,"����" , "����" };
	int putnum = -1;		//�I������Ă��郂�[�h�ԍ�
	bool isClick = false;	//�N���b�N����Ă��邩

	//�{�^���̏����Z�b�g
	for (int i = 0; i < 3; i++) {
		XY pos1(i*size + space2 + space1, space3);
		XY pos2((i + 1)*size - space2 + space1, windowArea.pos2.y - space3);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	while(ProcessMessage() == 0){
		//�`��
		if(isDraw){
			ClearDrawScreen();
			for(int i=0;i<3;i++){
				button[i].draw();
				button[i].printMessage();
			}
			ScreenFlip();
			isDraw = false;
		}

		//�N���b�N���ꂽ��
		if(( GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 && !isClick){
			isClick = true;
			//�ǂ���(�ǂ̃{�^����)�����ꂽ���𒲂ׂ�
			for(int i=0;i<3;i++){
				if (button[i].area.isIn()) {
					button[i].select();
					putnum = i;
					isDraw = true;
					break;
				}
			}
		}
		//�͂Ȃ��ꂽ��
		else if(( GetMouseInput() & MOUSE_INPUT_LEFT ) == 0 && isClick){
			isClick = false;
			//�����ꂽ�{�^���̏ꏊ�ł͂Ȃ��ꂽ�炻�̔ԍ���Ԃ�
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
//mouse��pos1(����)����pos2(�E��)�̒����`���ɓ����Ă��邩
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
	//���[�h�c 0:����҂�  1:1�_�ڑI��  2:2�_�ڑI��
	EntryMode mode = Wait;
	//�����ꂽ�{�^��  0:������  1:1�߂�  2:�S����  3:�o�^  4:���j���[��
	int putnum = -1;
	std::vector<Line> line;
	XY first, second;
	Button button[ENTRYMODEBUTTONNUM];
	//�{�^���̓o�^
	std::string button_message[ENTRYMODEBUTTONNUM] = { "������", "1�߂�", "�S����","�o�^","���j���[��" };
	int size = 100;
	int space1 = 10;
	int space2 = 10;
	for (int i = 0; i < ENTRYMODEBUTTONNUM; i++) {
		XY pos1(i*size + space2 + space1, menuArea.pos1.y + space2);
		XY pos2((i + 1)*size - space2 + space1, menuArea.pos2.y - space2);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	//���C�����[�v
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
			//�L�����Z�����Ԃ��Ă�����o�^����mode0��
			if (mode == Cancel) {
				break;
			}
			//first��second�������_�Ȃ�o�^���Ȃ�
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
//�C�x���g�҂�
EntryMode selectWaiting(std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	/*������邱�Ƃ�����΂���*/
	return First;
}
//�n�_�I��
EntryMode setFirstPosition(XY &first, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	//�N���b�N���ꂽ�Ƃ��ɓo�^����
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
		printfDx("���������Ă�������\n");
		//���łɓo�^�ς݂̕ӂ�`��
		for (int i = 0; i < line.size(); i++) {
			if (i == 0) {
				line[i].draw(Red);
			}
			else {
				line[i].draw(White);
			}
		}
		//�{�^���C�x���g
		next_mode = entryButtonEvent(First, line, button);
		if (next_mode != First) {
			return next_mode;
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
			isClick = true;
			//�̈�(drawarea)���Ń}�E�X�������ꂽ��
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
//�I�_�I��
EntryMode setSecondPosition(XY &second, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM], XY first) {
	//�N���b�N�I���(�{�^���������ꂽ��)�ɓo�^����
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
		printfDx("���������Ă�������\n");
		//���łɓo�^�ς݂̕ӂ�`��
		for (int i = 0; i < line.size(); i++) {
			if (i == 0) {
				line[i].draw(Red);
			}
			else {
				line[i].draw(White);
			}
		}
		//�{�^���C�x���g
		next_mode = entryButtonEvent(Second, line, button);
		//����Ԃƕω�����Ώ�ԑJ��
		if (next_mode != Second) {
			return next_mode;
		}
		//��_�ڂ̑I���C�`��
		mouse.getPosition();
		DrawLine(first.x, first.y, mouse.x, mouse.y, White);
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			isClick = true;
		}
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
			isClick = false;
			//�̈�(drawarea)���Ń}�E�X�������ꂽ��
			if (drawArea.isIn()) {
				second = XY(mouse.x, mouse.y);
				return Wait;
			}
		}
		ScreenFlip();
	}
	return Wait;
}
//�{�^���C�x���g
EntryMode entryButtonEvent(EntryMode now_mode, std::vector<Line> &line, Button button[ENTRYMODEBUTTONNUM]) {
	static bool isClick = false;
	static int putnum = -1;
	//�N���b�N���ꂽ��
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
		isClick = true;
		//�ǂ���(�ǂ̃{�^����)�����ꂽ���𒲂ׂ�
		for (int i = 0; i<ENTRYMODEBUTTONNUM; i++) {
			if (button[i].area.isIn()) {
				button[i].select();
				putnum = i;
				break;
			}
		}
	}
	//�͂Ȃ��ꂽ��
	else if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
		isClick = false;
		//�����ꂽ�{�^���̏ꏊ�ł͂Ȃ��ꂽ�炢�낢��Ə���������
		if (putnum != -1) {
			button[putnum].unselect();
			int putnum_buf = putnum;
			putnum = -1;
			if (button[putnum_buf].area.isIn()) {
				switch (putnum_buf) {
				//������
				case 0:
					return Cancel;
				//1�߂�
				case 1:
					if (line.size() > 0) {
						line.pop_back();
					}
					return now_mode;
				//�S����
				case 2:
					line.clear();
					return now_mode;
				//�o�^
				case 3:
					entryLine(line);
					return now_mode;
					break;
				//���j���[��
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
//�o�^�{�^���������ꂽ�ۂ̏���
EntryMode entryLine(std::vector<Line> &line) {
	//�o�^���ꂽ������x���Z�b�g
	convline.clear();
	//����������Ă��Ȃ���Γo�^���Ȃ�
	if (line.size() < 1) {
		return BackToMenu;
	}
	//���(�Ԃ���)�̓o�^
	original = OriginalLine(line[0]);
	//���̑��̐��̓o�^
	for (int i = 1; i < line.size(); i++) {
		convline.push_back(ConvertLine(line[i]));
		convline[i - 1].setParam(original);
	}
	return BackToMenu;
}
//////////////////////////////////////////// entry ////////////////////////////////////////////////
//////////////////////////////////////////// write ////////////////////////////////////////////////

//���C��
MenuMode write() {
	//���̃��[�h
	int next_mode;
	//�{�^���̐ݒ�
	int size = 100;
	int space1 = 10;
	int space2 = 10;
	Button button[WRITEMODEBUTTONNUM];
	std::string button_message[WRITEMODEBUTTONNUM] = {"�S����", "���j���[��"};
	for (int i = 0; i < WRITEMODEBUTTONNUM; i++) {
		XY pos1(i*size + space2 + space1, menuArea.pos1.y + space2);
		XY pos2((i + 1)*size - space2 + space1, menuArea.pos2.y - space2);
		button[i] = Button(Area(pos1, pos2), i);
		button[i].setMessage(button_message[i]);
	}
	//�`��
	clsDx();
	ClearDrawScreen();

	//���C�����[�v
	while (ProcessMessage() == 0) {
		mouse.getPosition();
		//���������ăh���b�O�ŕ`��
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
		//�E�������ăh���b�O�ŕ`��
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
//�{�^���C�x���g �߂�l�͎��̏��(1�Ȃ烁�j���[��)
int writeButtonEvent(Button button[WRITEMODEBUTTONNUM]) {
	static bool isClick = false;
	static int putnum = -1;
	//�N���b�N���ꂽ��
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !isClick) {
		isClick = true;
		//�ǂ���(�ǂ̃{�^����)�����ꂽ���𒲂ׂ�
		for (int i = 0; i<WRITEMODEBUTTONNUM; i++) {
			if (button[i].area.isIn()) {
				button[i].select();
				putnum = i;
				break;
			}
		}
	}
	//�͂Ȃ��ꂽ��
	else if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0 && isClick) {
		isClick = false;
		//�����ꂽ�{�^���̏ꏊ�ł͂Ȃ��ꂽ�炢�낢��Ə���������
		if (putnum != -1) {
			button[putnum].unselect();
			int putnum_buf = putnum;
			putnum = -1;
			if (button[putnum_buf].area.isIn()) {
				switch (putnum_buf) {
					//�S����
				case 0:
					ClearDrawScreen();
					return 0;
					//���j���[��
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
	button.setMessage(std::string("��������"));
	bool isClick = false;
	while (ProcessMessage() == 0) {
		clsDx();
		ClearDrawScreen();
		button.draw();
		button.printMessage();
		printfDx("��������F\n�K���ɂ���Ă�");
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