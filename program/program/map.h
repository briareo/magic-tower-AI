#pragma once
#include "hero.h"

class c_map_door
{
public:
	void init(int);//��ʼ��
	void show(int,int);//��Ⱦ
	bool open();//��ĳ����ͨ��
	int getType() {return type;} //�ŵ�����
private:
	int type;
	int state;
	hgeSprite *door[4];
};

class c_map_point
{
public:
	void init(int);//��ʼ��
	int getId();
	void show(GfxFont*,int,int);//��Ⱦ
	void animation() {monster.changeState();}//npc״̬�ı�
	void printMonsterInfo() {monster.printInfo();} //���������Ϣ
	bool canMove(int);//�жϸõ��Ƿ���ƶ�
	void distroyItem() {item=0;} //��ȡ�õ���Ʒ
	int getItem() {return item;}
	hgeSprite* getTypeSprite();
	hgeSprite* getItemSprite();
private:
	int type;
	int item;
	c_monster monster;//��������
	c_map_door door;//������
};

class c_map_floor
{
public:
	void init(int,int[30][30]);//��ʼ��
	void show();//��Ⱦ
	void animation();//״̬�ı�
	void printMonsterInfo(int,int);//���������Ϣ
	bool canMove(int,int,int);//�жϸõ�ͼ��ĳ���Ƿ���ƶ�
	int getItem(int,int);//��ȡ��ͼĳ�����Ʒ
	void getDownPosition(int&,int&);//�ҵ���ͼ����¥¥��
	void getUpPosition(int&,int&);//�ҵ���ͼ����¥¥��
	c_map_point* getinfo(int x, int y) {return &info[x][y];}
	void output(FILE*);
private:
	int id;//¥����
	int dx,dy,ux,uy;//¥������
	c_map_point info[30][30];//ÿ����Ϣ
};