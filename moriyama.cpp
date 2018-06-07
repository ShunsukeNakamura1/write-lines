#include "DxLib.h"
#include <math.h>

const double PI = 3.14159265;

int WindowX = 700;
int WindowY = 200;
int MouseX, MouseY;
int MouseX_old,MouseY_old;
int charSize = 200;
int spaceSize = 100;
int length_hen = 80;		//��ӂ̒���
int OX = 60, OY = 20;	//���_
unsigned int Cr;
unsigned int RED;

//�������W���ɍ��W�ɕϊ�
void Convert1(double *length, double *angle);
//�ɍ��W�𒼌����W�ɕϊ�
void Convert2(double length, double angle, int &x, int &y);
//���̍��W���擾
void getPoint(int *x, int *y, int times, int devid, double length, double angle);
void DrawDebug();
void mori(double length, double angle, bool flag);
void yama(double length, double angle, bool flag);
void takumi(double length, double angle, bool flag);


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	SetGraphMode( WindowX , WindowY , 32 ) ;
	ChangeWindowMode( TRUE ) ;
	

	if( DxLib_Init() == -1 )		// �c�w���C�u��������������
	{
		return -1 ;			// �G���[���N�����璼���ɏI��
	}
	
	
	SetDrawScreen( DX_SCREEN_BACK ) ; 
	int x1,y1;
	int x1_old,y1_old;
	double length, angle;
	RED = GetColor(255,0,0);
	Cr = GetColor(255,0,0);
	GetMousePoint(&MouseX, &MouseY);
	Convert1(&length, &angle);
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		Cr = GetColor(rand()%256,rand()%256,rand()%256);
		MouseX_old = MouseX;
		MouseY_old = MouseY;

		GetMousePoint(&MouseX, &MouseY);
		Convert1(&length, &angle);
		//DrawDebug();
		DrawLine(60,20,60+length_hen,20,RED);
		//���������ăh���b�O�ŕ`��
		if( ( GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 ){
			DrawLine(MouseX_old,MouseY_old,MouseX, MouseY,Cr);
			mori(length, angle, true);
			yama(length, angle, true);
			takumi(length, angle, true);
		}
		//�E�������ăh���b�O�ŕ`��
		else if( ( GetMouseInput() & MOUSE_INPUT_RIGHT ) != 0 ){
			Cr = GetColor(255,0,0);
			DrawLine(MouseX_old,MouseY_old,MouseX, MouseY,Cr);
			mori(length, angle, true);
			yama(length, angle, true);
			takumi(length, angle, true);
		}
		else{
			mori(length, angle, false);
			yama(length, angle, false);
			takumi(length, angle, false);
		}
		//�X�y�[�X�L�[�őS����
		if(CheckHitKey(KEY_INPUT_SPACE) == 1){
			ClearDrawScreen();
		}
		ScreenFlip();
	}

	DxLib_End() ;			// �c�w���C�u�����g�p�̏I������

	return 0 ;				// �\�t�g�̏I�� 
}

//�������W���ɍ��W�ɕϊ�
void Convert1(double *length, double *angle){
	double x = (double)MouseX-(double)OX;
	double y = (double)MouseY-(double)OY;
	*length = sqrt(x*x + y*y);
	*angle  = atan2(y,x);
}
//�ɍ��W�𒼌����W�ɕϊ�
void Convert2(double length, double angle, int *x, int *y){
	*x = length * cos(angle);
	*y = -length * sin(angle);
}
void getPoint(int *x, int *y, int times, int devid, double length, double angle){
	angle = angle + PI/devid*times;
	Convert2(length, angle, x, y);
}
void DrawDebug(){
	DrawLine(charSize,0,charSize,WindowY,RED);
	DrawLine(charSize*2,0,charSize*2,WindowY,RED);
	DrawLine(charSize*2+spaceSize,0,charSize*2+spaceSize,WindowY,RED);
}
void mori(double length, double angle, bool flag){
	int x[11],y[11];
	static int x_old[11]={0}, y_old[11]={0};
	static int count=1,i;
	if(!flag){
		count=1;
		return;
	}
	getPoint(&x[0],&y[0],-1,2,length,angle);
	x[0]+=100;  y[0]+=10;
	getPoint(&x[1],&y[1],-3,4,length,angle);
	x[1]+=100;  y[1]+=20;
	getPoint(&x[2],&y[2],-1,4,length,angle);
	x[2]+=100;  y[2]+=20;
	getPoint(&x[3],&y[3],0,1,length,angle);
	x[3]+=10;  y[3]+=110;
	getPoint(&x[4],&y[4],-1,2,length,angle);
	x[4]+=50;  y[4]+=100;
	getPoint(&x[5],&y[5],-3,4,length,angle);
	x[5]+=50;  y[5]+=110;
	getPoint(&x[6],&y[6],-1,4,length,angle);
	x[6]+=50;  y[6]+=110;
	getPoint(&x[7],&y[7],0,1,length,angle);
	x[7]+=110;  y[7]+=110;
	getPoint(&x[8],&y[8],-1,2,length,angle);
	x[8]+=150;  y[8]+=100;
	getPoint(&x[9],&y[9],-3,4,length,angle);
	x[9]+=150;  y[9]+=110;
	getPoint(&x[10],&y[10],-1,4,length,angle);
	x[10]+=150;  y[10]+=110;

	if(count == 0){
		for(i=0;i<11;i++){
			DrawLine(x_old[i],y_old[i],x[i],y[i],Cr);
		}
	}
	for(i=0;i<11;i++){
		x_old[i] = x[i];
		y_old[i] = y[i];
	}
	count=0;
}
void yama(double length, double angle, bool flag){
	int x[6],y[6];
	static int x_old[6]={0}, y_old[6]={0};
	static int count=1,i;
	if(!flag){
		count=1;
		return;
	}
	getPoint(&x[0],&y[0],-1,2,length,angle);
	x[0]+=300;  y[0]+=20;
	getPoint(&x[1],&y[1],-1,2,length,angle);
	x[1]+=300;  y[1]+=100;
	getPoint(&x[2],&y[2],-1,2,length,angle);
	x[2]+=220;  y[2]+=100;
	getPoint(&x[3],&y[3],0,1,length,angle);
	x[3]+=220;  y[3]+=180;
	getPoint(&x[4],&y[4],0,1,length,angle);
	x[4]+=300;  y[4]+=180;
	getPoint(&x[5],&y[5],-1,2,length,angle);
	x[5]+=380;  y[5]+=100;

	if(count == 0){
		for(i=0;i<6;i++){
			DrawLine(x_old[i],y_old[i],x[i],y[i],Cr);
		}
	}
	for(i=0;i<6;i++){
		x_old[i] = x[i];
		y_old[i] = y[i];
	}
	count=0;
}
void takumi(double length, double angle, bool flag){
	int x[9],y[9];
	static int x_old[8]={0}, y_old[8]={0};
	static int count=1,i;
	if(!flag){
		count=1;
		return;
	}
	getPoint(&x[0],&y[0],0,1,length,angle);
	x[0]+=510;  y[0]+=60;
	getPoint(&x[1],&y[1],-1,2,length,angle);
	x[1]+=550;  y[1]+=60;
	getPoint(&x[2],&y[2],0,1,length,angle);
	x[2]+=510;  y[2]+=140;
	getPoint(&x[3],&y[3],0,1,length,angle);
	x[3]+=570;  y[3]+=20;
	getPoint(&x[4],&y[4],-1,2,length,angle);
	x[4]+=600;  y[4]+=20;
	getPoint(&x[5],&y[5],0,1,length,angle);
	x[5]+=600;  y[5]+=100;
	getPoint(&x[6],&y[6],-1,2,length,angle);
	x[6]+=680;  y[6]+=100;
	getPoint(&x[7],&y[7],1,1,length,angle);
	x[7]+=680;  y[7]+=180;

	if(count == 0){
		for(i=0;i<8;i++){
			DrawLine(x_old[i],y_old[i],x[i],y[i],Cr);
		}
	}
	for(i=0;i<8;i++){
		x_old[i] = x[i];
		y_old[i] = y[i];
	}
	count=0;
}