#pragma once

#include "monster.h"
class c_hero
{
public:
	const static int MAX_DAMAGE = 999999999;

	c_hero();//���캯��
	void init(FILE*);
	int nextX();//��һ���ĺ�����
	int nextY();//��һ����������
	bool canMove(int);//�Ƿ���Գ�ĳ�������ƶ�
	bool moveComplete();//�ƶ��Ƿ����
	bool openRedDoor();//�򿪺�ɫ����
	bool openBlueDoor();//����ɫ����
	bool openYellowDoor();//�򿪻�ɫ����
	void upstair();//��¥
	void downstair();//��¥
	void turn(); // ת��
	void show();
	void printInfo();//�����ʿ��Ϣ
	bool canBeat(c_monster*);//�ܷ��ܹ���
	int getDamage(c_monster*);//��ܹ�����ͨ�˺�
	int getDamage(int h_atk, int h_def, int h_mdef, int m_hp, int m_atk, int m_def, int m_spe);
	int getCritical(int m_hp, int m_atk, int m_def, int m_spe);
	int getCriticalDamage(int m_hp, int m_atk, int m_def, int m_spe);
	int getDefDamage(int m_hp, int m_atk, int m_def, int m_spe);
	void beat(c_monster*);//�����
	int getNowFloor() {return now_floor;}
	int getHP() {return hp;}
	int getAtk() {return atk;}
	int getDef() {return def;}
	void useShop(int);
	void output(FILE*);
	int dir[2][4];
private:
	int hp,atk,def,mdef,money,redkey,bluekey,yellowkey;
	int x,y,move,face,now_floor;
	hgeSprite* sprites[4][4];
};
