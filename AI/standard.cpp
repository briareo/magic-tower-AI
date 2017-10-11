/*	
 *	��Ʒ��ħ��AI
 *	���飺������word��
 *	��Ա�����������𳯡�����͡��β���
 *	
 */
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<algorithm>
#include<queue>
#include<stack>
#include<ctime>
using namespace std;

string rev(string s, int sx, int sy, int ex, int ey, int type = 0);
int getDamage(int h_atk, int h_def, int h_mdef, int m_hp, int m_atk, int m_def, int m_spe);
bool is_connected(int sx, int sy, int ex, int ey);
string cal_trace(int sx, int sy, int ex, int ey);

class Node;
fstream fin, fout;



/*��ͼ*/
int vis[15][15];
int Mov[4][2] = { { -1,0 },{ 1,0 },{ 0,-1 },{ 0,1 } };
char str[5] = "wsad";
struct Path {	//��·���ṹ
	int x, y;
	char sp;
}path[15][15];
int i = 1;
Node *graph[200];	//��ͼ���
/*����*/
int layer;
int row, col;
int map[15][15];
string ans;  //output string
string bestway;  //best output
int monster[100][6] = { 0 };  //exist, hp, attack, defence, special skill
int hero_hp, hero_attack, hero_defence, hero_mdefence, sx, sy;
int node_num = 0;	//�������
int power_of_Redgem, power_of_Bluegem, power_of_Greengem, capacity_of_Smallhpbottle, capacity_of_Largehpbottle;
int hero_yellow_key = 0, hero_red_key = 0, hero_blue_key = 0;
int anshp = 0, monster_defeat = 0;

/*�߽ṹ*/
class Edge {
public:
	Node* to;	//Ŀ�ĵؽ��
	string move;	//ȥ��Ŀ�ĵص�·���ַ���
	Edge* nextedge;	//�����ͬ����һ����

	Edge() {
		to = NULL;
		move = "";
		nextedge = NULL;
	}
	Edge(Node* g, string s) {
		to = g;
		move = s;
		nextedge = NULL;
	}

	Edge(Edge *s, Edge *e) {
		to = e->to;
		move = s->move + e->move;
		nextedge = NULL;
	}

	Edge(Edge *e) {
		to = e->to;
		move = e->move;
		nextedge = NULL;
	}
};

/*	ͼ���
 *
 *	���ڴ洢��ͼ��ÿ����ǽ�ǵ�·��㣬����graphָ������
 *	graph[0]����ʼ�Ľ�㣬���Ҽ�¼Ӣ�۵���Ϣ
 *	ÿ���ж�ʱ���ϲ���㣬����ǰ�����£������ؼ���㡣
 */
class Node {
public:
	int hp;	//��¼���������/���������/��ʯ�ӳ�
	int attack_increase;
	int defence_increase;
	int magicdefence_increase;
	int yellow_key, blue_key, red_key;	//����Կ�׵���������Ϊ-1
	int type_num;	//1�����>50����֣�ÿ���ж��̳�Ŀ�ĵؽ����type_num
	int visited;	//����Ƿ��Ѿ��߹�
	int x, y;	//��ǰ����ڵ�ͼ�ϵ�����
	int id;	//��¼��ǰλ�õĽ����graph����洢��λ��
	Edge* firstedge;	//�ڽӱ�ָ��
	Node() {
		hp = attack_increase = defence_increase = magicdefence_increase = blue_key = yellow_key = red_key = 0;
		type_num = visited = x = y = 0;
		firstedge = NULL;
	}
	Node(int h, int a, int d, int m, int type, int xx = 0, int yy = 0, int i = -1) {
		hp = h;
		attack_increase = a;
		defence_increase = d;
		magicdefence_increase = m;
		firstedge = NULL;
		yellow_key = blue_key = red_key = 0;
		type_num = type;
		visited = 0;
		x = xx;
		y = yy;
		id = i;
	}

	/*���ڽӱ��м���һ����*/
	Edge* SetEdge(Node* g, string s) {
		Edge* e = new Edge(g, s);
		if (this->firstedge == NULL) {			
			this->firstedge = e;			
		}
		else {
			Edge* temp = this->firstedge;
			while (temp->nextedge != NULL) {
				temp = temp->nextedge;
			}
			temp->nextedge = e;
		}
		return this->firstedge;
	}

	/*Ѱ�ҵ���g���ı�*/
	Edge* ToNode(Node* g) {
		Edge* tmp;
		for (tmp = this->firstedge; tmp != NULL; tmp = tmp->nextedge) {
			if (tmp->to == g)
				break;
		}
		return tmp;
	}

	/*ɾ������g���ıߣ�������û���õ���*/
	bool DelEdge(Node *g) {
		Edge* target = ToNode(g);
		if (target == NULL)
			return false;
		Edge* tmp = this->firstedge;
		if (tmp == target) {
			this->firstedge = target->nextedge;
			delete target;
		}
		else {
			while (tmp->nextedge != target && tmp->nextedge != NULL)
				tmp = tmp->nextedge;
			tmp->nextedge = target->nextedge;
			delete target;
		}
		return true;
	}

	//����һ���µĺϲ���㣬����������Χ�����±ߣ�ֱ�Ӽ���ԭͼ֮�ϣ�������ԭͼ�����������ݺ�ָ��ṹ
	Node* createunite(Node *g) {   //û�и�g��visit���
		//����һ���µĽ�㣬
		Node *tmp;

		//�����µĺϲ��ڵ㣬��������ֵ
		if (g->type_num < 50)
		{
			tmp = new Node(hp + g->hp, attack_increase + g->attack_increase, defence_increase + g->defence_increase, magicdefence_increase + g->magicdefence_increase, g->type_num, g->x, g->y, g->id);
		}
		else {
			int damage = getDamage(attack_increase, defence_increase, magicdefence_increase, g->hp, g->attack_increase, g->defence_increase, g->magicdefence_increase);
			tmp = new Node(hp - damage, attack_increase, defence_increase, magicdefence_increase, g->type_num, g->x, g->y, g->id);
		}
		tmp->yellow_key = yellow_key + g->yellow_key;
		tmp->blue_key = blue_key + g->blue_key;
		tmp->red_key = red_key + g->red_key;

		Edge *e;
		Edge *tmpe = new Edge();	//����βָ��
		Edge *head = tmpe;			//����ͷָ��

		/*����������Ŀ�ĵؽ����Χ���ӵ����н������������ı߶������ڽӱ�ֻ����ߣ����ı����*/

		//�Ƚ�Ŀ�Ľ������бߣ�ֱ�Ӹ��Ʊ߼���
		for (e = g->firstedge; e != NULL; e = e->nextedge) {	
			if (e->to->visited == 1)	//���������ı����1����ʾ֮ǰ���ʹ�������������һ����
				continue;
			e->to->visited = 1;	//������Ѿ����ʹ����Է������رߣ���-1�������Ӧ
			if (e->to == this)
				continue;
			tmpe->nextedge = new Edge(e);
			tmpe = tmpe->nextedge;
			
		}

		Edge *ee;
		//�ٽӳ������ıߣ�ÿһ�ζ���ҪѰ·һ��
		for (e = firstedge; e != NULL; e = e->nextedge) {
			if (e->to->visited == 1)
				continue;
			e->to->visited = 1;
			if (e->to == g)
				continue;
			if ((ee = g->ToNode(e->to)) != NULL)	//���Ŀ�Ľ��Ҳ�е����������·����ֱ�Ӹ��ƽ���
			{
				tmpe->nextedge = new Edge(ee);
			}
			else	//����Ϊ���Ż�·�����ٴ�Ѱ·һ��
			{
				if (is_connected(g->x, g->y, e->to->x, e->to->y)) {
					tmpe->nextedge = new Edge(e->to, cal_trace(g->x,g->y,e->to->x,e->to->y));
				}				
			}
			tmpe = tmpe->nextedge;
		}
		
		tmp->firstedge = head->nextedge;	//��������ڽ����
		for (e = tmp->firstedge; e != NULL; e = e->nextedge) {  //����visitedΪ0
			e->to->visited = 0;
		}
		delete head;
		return tmp;
	}

	//ɾ��e��֮������б�
	void delete_afteredge(Edge *e) {
		if (e == NULL)
			return;
		delete_afteredge(e->nextedge);
		delete(e);
		return;
	}

	//�⹹��ɾ������
	void deunite() {
		Edge *tmp = this->firstedge;
		delete_afteredge(tmp);
		delete this;
	}

	//�ı�Ӣ��״̬�����İ汾��ʹ�ã�
	void change_hero()
	{
		hero_hp += hp;
		hero_attack += attack_increase;
		hero_defence += defence_increase;
		hero_mdefence += magicdefence_increase;
		hero_yellow_key += yellow_key;
		hero_blue_key += blue_key;
		hero_red_key += red_key;
		visited = 1;
	}

	//�ָ�Ӣ��״̬
	void unchange_hero()
	{
		hero_hp -= hp;
		hero_attack -= attack_increase;
		hero_defence -= defence_increase;
		hero_mdefence -= magicdefence_increase;
		hero_yellow_key -= yellow_key;
		hero_blue_key -= blue_key;
		hero_red_key -= red_key;
		visited = 0;
	}
};

/*���Ѵ洢�Ľṹ*/
struct state {
	Node* g;	//��ǰ���
	Node* prev;	//������ǰ����previous���
	string answer;	//�ߵ���ǰ״̬��·��
	bool visit[200];	//���浱ǰ��Щ����Ѿ��߹�
	state(Node *gg, string str, Node* p) :g(gg), answer(str), prev(p) {
		memset(visit, 0, sizeof(visit));
	}
	state(Node *gg, string str, bool v[200], Node* p) :g(gg), answer(str), prev(p) {
		memcpy(visit, v, sizeof(visit));
	}

	//��s�Ļ����ϣ��־���move�ַ����ߵ���graph[mark]
	state(const state &s, Node *cur, string move, Node *p, int mark = 0) {
		g = cur;
		answer = s.answer + move;
		memcpy(visit, s.visit, sizeof(visit));
		visit[mark] = true;
		prev = p;
	}

	//���ȶ��й�ֵ������������Ч��
	bool operator<(const state &a) const {
		int val1, val2;
		val1 = g->hp + 1500 * (g->attack_increase + g->defence_increase) + 60 * g->magicdefence_increase + 300 * (g->yellow_key + g->blue_key + g->red_key);
		val2 = a.g->hp + 1500 * (a.g->attack_increase + a.g->defence_increase) + 60 * a.g->magicdefence_increase + 300 * (a.g->yellow_key + a.g->blue_key + a.g->red_key);
		return val1 < val2;
		//return g->hp - getDamage(g->attack_increase, g->defence_increase, g->magicdefence_increase, monster[99][1], monster[99][2], monster[99][3], monster[99][4])
	//< a.g->hp - getDamage(a.g->attack_increase, a.g->defence_increase, a.g->magicdefence_increase, monster[99][1], monster[99][2], monster[99][3], monster[99][4]);
	}
};

/*�����ļ�����*/
void init(fstream &fin) {
	fin >> layer >> row >> col;
	for (int i = 1; i <= row; ++i)
		for (int j = 1; j <= col; ++j)
		{
			fin >> map[i][j];
			if (map[i][j] != 10 && map[i][j] != 11)
				node_num++;
		}
	fin >> power_of_Redgem >> power_of_Bluegem >> power_of_Greengem >> capacity_of_Smallhpbottle >> capacity_of_Largehpbottle;
	int k;
	fin >> k;
	while (k--) {
		int id, hp, att, def, spe;
		fin >> id >> hp >> att >> def >> spe;
		monster[id][0] = 1;
		monster[id][1] = hp;
		monster[id][2] = att;
		monster[id][3] = def;
		monster[id][4] = spe;
	}
	fin >> hero_hp >> hero_attack >> hero_defence >> hero_mdefence >> sx >> sy;
	graph[0] = new Node(hero_hp, hero_attack, hero_defence, hero_mdefence, 0, sx + 1, sy + 1);
}

/*�˹��������д�����������*/
void initbyhuman() {
	cin >> layer >> row >> col;
	for (int i = 1; i <= row; ++i)
		for (int j = 1; j <= col; ++j)
		{
			cin >> map[i][j];
			if (map[i][j] != 10 && map[i][j] != 11)
				node_num++;
		}
	cin >> power_of_Redgem >> power_of_Bluegem >> power_of_Greengem >> capacity_of_Smallhpbottle >> capacity_of_Largehpbottle;
	int k;
	cin >> k;
	while (k--) {
		int id, hp, att, def, spe;
		cin >> id >> hp >> att >> def >> spe;
		monster[id][0] = 1;
		monster[id][1] = hp;
		monster[id][2] = att;
		monster[id][3] = def;
		monster[id][4] = spe;
	}
	cin >> hero_hp >> hero_attack >> hero_defence >> hero_mdefence >> sx >> sy;
	graph[0] = new Node(hero_hp, hero_attack, hero_defence, hero_mdefence, 0, sx + 1, sy + 1);
}
/*
�˺�������һ���������ʿ��ɵ��˺�
����˵����
h_atk: ��ʿ����
h_def: ��ʿ����
h_mdef: ��ʿħ��
m_hp: ��������ֵ
m_atk: ���﹥��
m_def: �������
m_spe: ������Ч
����ֵ��
һ�������������˺�ֵ��
�����ʿ�޷��Թ�������˺�������999999999

������Ч˵����
0: ����Ч
1: ħ��������������ʿ�ķ�����
2: ��̣��������=��ʿ����-1��
3: �ȹ���ս��ǰ�������ȹ�����ʿһ�Σ�
4: 2������ÿ�غϹ������ʿ���Σ�
*/
int getDamage(int h_atk, int h_def, int h_mdef, int m_hp, int m_atk, int m_def, int m_spe)
{
	if (m_spe == 1) // ħ��
		h_def = 0;
	if (m_spe == 2) // ���
		m_def = h_atk - 1;

	if (h_atk <= m_def) // �޷�ս��
		return 999999999;
	if (h_def >= m_atk) // �޷�����˺�
		return 0;

	int damage = 0;
	if (m_spe == 3) // �ȹ�
		damage += m_atk - h_def;

	int m_times = 1;
	if (m_spe == 4) // 2����
		m_times = 2;

	damage += (m_hp - 1) / (h_atk - m_def)*(m_atk - h_def)*m_times; // �����˺����㹫ʽ
	damage -= h_mdef;

	return damage <= 0 ? 0 : damage;
}

/*����ÿһ�����*/
void build_node()
{
	for (int x = 1; x <= 13; x++)
		for (int y = 1; y <= 13; y++)
			if (map[x][y] > 20)
			{
				if (map[x][y] == 21)
				{
					graph[i] = new Node(0, 0, 0, 0, 1, x, y, i);
					graph[i++]->yellow_key = 1;
				}
				if (map[x][y] == 22)
				{
					graph[i] = new Node(0, 0, 0, 0, 1, x, y, i);
					graph[i++]->blue_key = 1;
				}
				if (map[x][y] == 23)
				{
					graph[i] = new Node(0, 0, 0, 0, 1, x, y, i);
					graph[i++]->red_key = 1;
				}
				if (map[x][y] == 24)
					graph[i++] = new Node(0, power_of_Redgem, 0, 0, 1, x, y, i);
				if (map[x][y] == 25)
					graph[i++] = new Node(0, 0, power_of_Bluegem, 0, 1, x, y, i);
				if (map[x][y] == 26)
					graph[i++] = new Node(0, 0, 0, power_of_Greengem, 1, x, y, i);
				if (map[x][y] == 27)
					graph[i++] = new Node(capacity_of_Smallhpbottle, 0, 0, 0, 1, x, y, i);
				if (map[x][y] == 28)
					graph[i++] = new Node(capacity_of_Largehpbottle, 0, 0, 0, 1, x, y, i);
				if (map[x][y] == 31)
				{
					graph[i] = new Node(0, 0, 0, 0, 31, x, y, i);
					graph[i++]->yellow_key = -1;
				}
				if (map[x][y] == 32)
				{
					graph[i] = new Node(0, 0, 0, 0, 32, x, y, i);
					graph[i++]->blue_key = -1;
				}
				if (map[x][y] == 33)
				{
					graph[i] = new Node(0, 0, 0, 0, 33, x, y, i);
					graph[i++]->red_key = -1;
				}
				if (map[x][y] > 50 && map[x][y] <= 70 || map[x][y] == 99)
					graph[i++] = new Node(monster[map[x][y]][1], monster[map[x][y]][2], monster[map[x][y]][3], monster[map[x][y]][4], map[x][y], x, y, i);
			}
}

/*����������ڵ�ͼ���Ƿ����ߵ�*/
bool is_connected(int sx, int sy, int ex, int ey)
{
	if (sx == ex && sy == ey)
		return true;
	Node now;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 15; j++)
		{
			path[i][j].x = path[i][j].y = -1;
			path[i][j].sp = '?';
		}
	now.x = sx;
	now.y = sy;
	queue<Node> q;
	q.push(now);
	while (q.size() > 0)
	{
		now = q.front();
		
		q.pop();
		Node next = now;
		for (int i = 0; i < 4; i++)
		{
			next.x = now.x + Mov[i][0];
			next.y = now.y + Mov[i][1];
			if (next.x == ex && next.y == ey)
			{
				vis[next.x][next.y] = 1;
				path[next.x][next.y].x = now.x;
				path[next.x][next.y].y = now.y;
				path[next.x][next.y].sp = str[i];
				return true;
			}
			if ((next.x > 0 && next.x <= row &&next.y > 0 && next.y <= col) && map[next.x][next.y] == 10 && (!vis[next.x][next.y])) {
				vis[next.x][next.y] = 1;
				path[next.x][next.y].x = now.x;
				path[next.x][next.y].y = now.y;
				path[next.x][next.y].sp = str[i];
				q.push(next);
			}
		}
	}
	return false;
}

/*�ڼ����������ã�����·���ַ���*/
string cal_trace(int sx, int sy, int ex, int ey)  //����·��
{
	string step = "";
	int nx = ex, ny = ey;
	int nextx = nx, nexty = ny;
	while (1) {
		nx = nextx;
		ny = nexty;
		if (nx == sx && ny == sy)
			break;
		step = path[nx][ny].sp + step;
		nextx = path[nx][ny].x;
		nexty = path[nx][ny].y;
	}
	if (map[ex][ey] > 50 && map[ex][ey] <= 70 || map[ex][ey] == 99)
	{
		step += path[ex][ey].sp;
	}
	return step;
}

/*�������֮��ı�*/
void switch_node()
{
	for (int i = 0; i <= node_num; i++)
		for (int j = i + 1; j <= node_num; j++)
		{
			if (is_connected(graph[i]->x, graph[i]->y, graph[j]->x, graph[j]->y))
			{
				string s = cal_trace(graph[i]->x, graph[i]->y, graph[j]->x, graph[j]->y);
				graph[i]->SetEdge(graph[j], s);
				graph[j]->SetEdge(graph[i], rev(s, graph[i]->x, graph[i]->y, graph[j]->x, graph[j]->y));
			}
		}
}

/*��ת�ַ��������İ汾û���õ�����sx,sy��s����㣬ex��ey��s���յ�*/
string rev(string s, int sx, int sy, int ex, int ey, int type)
{
	string temp = "";
	int l = s.length();
	if (map[ex][ey] > 50 && map[ex][ey] <= 70 || map[ex][ey] == 99)
		l--;
	for (int i = l - 1; i >= 0; i--)
	{
		if (s[i] == 'w')
			temp += 's';
		if (s[i] == 's')
			temp += 'w';
		if (s[i] == 'a')
			temp += 'd';
		if (s[i] == 'd')
			temp += 'a';
	}
	if (type == 0 && map[sx][sy] > 50 && map[sx][sy] <= 70 || map[sx][sy] == 99)
	{
		temp += temp[l-1];
	}
	return temp;
}

/*��ͼ*/
void build_map()
{
	build_node();
	switch_node();
}

/*���ȶ��еĿ���
 *
 *����Ҫ�����ĺϲ���㰴��һ���Ĺ�ֵ�ŵ����ȶ��У���㱣����Ӣ�۵ĵ�ǰ״̬
 *��ÿ������һ�����ʱ������������Ǳ��ֱ�ӳԵ�������������ȶ��У��ȴ���һ����������������ʹ����ÿ�λ�ȡ�Ĵ�����Χֻ���Ż��߹�����
 *����ڿ��Ż���ս���󣬲��ᷢ���κεı����ô�ұ�����������·����̽����ֱ���б�����߲���̽��Ϊֹ�������Ե�����·��ȥ���������������·��������ʱ��ȥ��������������·�ϵĽ�㣬���ǻ�ȡ������������������㣬����һ����õ������·������֦��
 *�����������boss�����ܹ�սʤ�����һ���սʤ����һ������������ʱ�������У����ҵò������棬��˿�������ʱ�������еõ�������״̬�����
 */
void BFS(Node *g) {
	//�����ͼ����
	int temp_map[15][15];	
	memcpy(temp_map, map, sizeof(map));

	//���ȶ���
	priority_queue<state> q;
	//������ʼstate�ṹ������ʼ״̬
	state cur(g, ans, NULL);
	cur.g->id = 0;
	cur.visit[0] = true;
	int sum = 1;
	//Ѱ�������Χ������
	//�п������ߵ���������ַ����µ������㣬������whileѭ��ֱ��������������
	while (sum) {
		sum = 0;
		for (Edge *e = cur.g->firstedge; e != NULL; e = e->nextedge)
		{
			if ((e->to->type_num == 1
				|| (e->to->type_num > 50 && e->to->type_num < 99 && getDamage(cur.g->attack_increase, cur.g->defence_increase, cur.g->magicdefence_increase, e->to->hp, e->to->attack_increase, e->to->defence_increase, e->to->magicdefence_increase) == 0))
				&& cur.visit[e->to->id] == false)
			{
				sum++;	//�������������ͼ�иĶ����п��ܷ����µ�����
				e->to->visited = 1;
				Node *tmp = cur.g->createunite(e->to);
				//ֱ�Ӹ���cur����
				cur.answer += cur.g->ToNode(e->to)->move;
				cur.g = tmp;
				cur.visit[e->to->id] = true;
				map[e->to->x][e->to->y] = 10;
			}
		}
	}
	q.push(cur);	//������ʼ���
	while (!q.empty())
	{
		memcpy(map, temp_map, sizeof(map));
		cur = q.top();
		q.pop();

		/*����ʱ��*/
		//clock_t t = clock();
		//if (t > 300000)
		//{
		//	cerr << "Time Out��" << t <<endl;
		//	break;
		//}


		//�ָ�����¼�ĵ�ͼ״̬
		for (int i = 0; i <= node_num; i++)
		{
			if (cur.visit[i] == true)
			{
				graph[i]->visited = 1;
				map[graph[i]->x][graph[i]->y] = 10;
			}
			else
				graph[i]->visited = 0;
		}

		/*Ѱ�ҵ�ǰ�������·��*/
		/*int msum = 0;
		for (int i = 1; i <= node_num; i++)
			if (graph[i]->visited == 1 && graph[i]->type_num > 50)
				msum++;
		if (msum > monster_defeat)
		{
			monster_defeat = msum;
			ans = cur.answer;
		}*/

		/*�����boss���������ʵ���ᵽ��һ����*/
		if (cur.g->type_num == 99)
		{
			/*Ѱ�����Ž�*/
			//if (cur.g->hp > anshp)
			//{
			//	anshp = cur.g->hp;
			//	bestway = cur.answer;
			//}
			//cout << cur.answer << endl;
			fout << cur.answer << endl;
			//continue;
			//break;
			return;

		}

		//fout << cur.answer << endl;

		queue<state> sNo;	//��ʱ����

		//Ѱ�������ߵ�·��,������ʱ������
		for (Edge *e = cur.g->firstedge; e != NULL; e = e->nextedge)
		{

			if (cur.visit[e->to->id] == false)
			{
				//�ִ򲻹�
				if (e->to->type_num > 50 && cur.g->hp <= getDamage(cur.g->attack_increase, cur.g->defence_increase, cur.g->magicdefence_increase, e->to->hp, e->to->attack_increase, e->to->defence_increase, e->to->magicdefence_increase))
					continue;

				//Կ�ײ���
				if (e->to->type_num == 31 && cur.g->yellow_key == 0)
					continue;
				if (e->to->type_num == 32 && cur.g->blue_key == 0)
					continue;
				if (e->to->type_num == 33 && cur.g->red_key == 0)
					continue;

				//e����·����
				e->to->visited = 1;
				Node *tmp = cur.g->createunite(e->to);	//�ϲ���㣬������ʱ����
				//���б�������Ҫ����Ľ��״̬
				sNo.push(state(cur, tmp, e->move, cur.g, e->to->id));
				e->to->visited = 0;
			}
		}
		int get_up = 0;

		//����ʱ���еĽ��������ȶ��У���ȡ������߼�����ǰ��
		while (!sNo.empty()) {
			state cur_state = sNo.front();
			sNo.pop();

			//�����ǰ����Ѿ������boss��������
			if (cur_state.g->type_num == 99)
			{
				//Ѱ�����Ž⣨��Ҫ��������ͼ����δ����Ч��֦�����޷�Ѱ�ң�
				//if (cur_state.g->hp > anshp)
				//{
				//	anshp = cur_state.g->hp;
				//	bestway = cur_state.answer;
				//}
				fout << cur_state.answer << endl;
				//cout << cur_state.answer << endl;
				//continue;
				//break;
				return;
			}

			//�ָ���ǰ��ͼ�����ʹ��Ľ���ɵ�·[10]������1��ǣ����ڵ��ִܵ�Ľ����-1�����ڼ�֦
			memcpy(map, temp_map, sizeof(map));
			for (int i = 0; i <= node_num; i++)
			{
				if (cur_state.visit[i] == true)
				{
					graph[i]->visited = 1;
					map[graph[i]->x][graph[i]->y] = 10;
				}
				else
					graph[i]->visited = 0;
			}

			for (Edge *ee = cur_state.prev->firstedge; ee != NULL; ee = ee->nextedge)
			{
				ee->to->visited = -1;
			}
			graph[cur.g->id]->visited = 1;


			get_up = 0;
			sum = 1;
			//��ȡ����
			//�п������ߵ���������ַ����µ������㣬������whileѭ��ֱ��������������
			while (sum)
			{
				sum = 0;
				for (Edge *ee = cur_state.g->firstedge; ee != NULL; ee = ee->nextedge)
				{
					if ((ee->to->type_num == 1
						|| (ee->to->type_num > 50 && ee->to->type_num < 99 && getDamage(cur_state.g->attack_increase, cur_state.g->defence_increase, cur_state.g->magicdefence_increase, ee->to->hp, ee->to->attack_increase, ee->to->defence_increase, ee->to->magicdefence_increase) == 0))
						&& cur_state.visit[ee->to->id] == false)
					{
						sum++;
						if (ee->to->type_num == 1)	//�Ǳ����㣬��ȡ������
							get_up = 1;
						ee->to->visited = 1;	//��Ǻϲ���㣨���ݺϲ���ԭ����һ���ܹ��ҵ���ڵ㵽���ڷ��ֵ����������·��
						Node *uni = cur_state.g->createunite(ee->to);
						cur_state.answer += cur_state.g->ToNode(ee->to)->move;	//��¼·��
						cur_state.g = uni;	//����ڵ�
						cur_state.visit[ee->to->id] = true;	//����visit״̬
						map[ee->to->x][ee->to->y] = 10;		//���ĵ�ͼ״̬
					}
				}
			}
			//�����û�ҵ����棬�ͼ�����������·ǰ��һ����㣬���뵽��ǰ��Ҫ̽������ʱ���У�ֱ���������Ϊֹ��
			if (get_up == 0)
			{
				for (Edge *ee = graph[cur_state.g->id]->firstedge; ee != NULL; ee = ee->nextedge)
				{
					if (ee->to->visited == 0)
					{
						if (ee->to->type_num > 50 && cur_state.g->hp <= getDamage(cur_state.g->attack_increase, cur_state.g->defence_increase, cur_state.g->magicdefence_increase, ee->to->hp, ee->to->attack_increase, ee->to->defence_increase, ee->to->magicdefence_increase))
							continue;
						if (ee->to->type_num == 31 && cur_state.g->yellow_key == 0)
							continue;
						if (ee->to->type_num == 32 && cur_state.g->blue_key == 0)
							continue;
						if (ee->to->type_num == 33 && cur_state.g->red_key == 0)
							continue;
						//����һ��Ҫ�ߵĽ�������ʱ����
						ee->to->visited = 1;
						Node *uni = cur_state.g->createunite(ee->to);
						sNo.push(state(cur_state, uni, ee->move, cur_state.g, ee->to->id));
						//���ݣ��ָ���ͼ״̬
						for (Edge *Ee = cur_state.prev->firstedge; Ee != NULL; Ee = Ee->nextedge)
						{
							Ee->to->visited = -1;
						}
						graph[cur_state.g->id]->visited = 1;
						ee->to->visited = 0;
					}
				}
			}
			else {
				//�������Ľ��������ȶ��У����е�״̬�仯�Ѿ�����cur_state���ˣ�
				q.push(cur_state);
			}

		}
	}
	return;
}

/*���ѳ��ԣ����İ汾���ã�*/
void search(Node* g)
{
	//cout << g->x << ' ' << g->y << endl;
	//cout << ans << endl;
	//system("pause");
	Node*tmp = NULL;//next node
	Node *uni = NULL;  //ָ��ϲ���Ľ��
	int tmp_type;
	string tmpstr = ans;
	//treasure
	for (Edge*temp = g->firstedge; temp != NULL; temp = temp->nextedge)//if finding a treasure node,get that and change position
	{
		tmp = temp->to;
		if (tmp->type_num != 1 || tmp->visited == 1)//if it is not treasure or it has been visited
			continue;

		tmp->change_hero();//change hero status
		tmp_type = map[tmp->x][tmp->y];
		uni = g->createunite(tmp);
		map[tmp->x][tmp->y] = 10;
		ans += temp->move;
		
		search(uni);//unite two node into next position node and go there

		tmp->unchange_hero();//��ʼ�����ˣ�
		ans = tmpstr;
		map[tmp->x][tmp->y] = tmp_type;
		uni->deunite();//change to its original looks
		uni = NULL;

		return;
	}

	//door or monster
	for (Edge*temp = g->firstedge; temp != NULL; temp = temp->nextedge)
	{
		tmp = temp->to;
		if (tmp->visited)
			continue;
		
		if (tmp->type_num == 99)
		{
			//cout << ans << endl;
			//system("pause");
			int damage;
			int m_hp = tmp->hp, m_atk = tmp->attack_increase, m_def = tmp->defence_increase, m_spe = tmp->magicdefence_increase;
			damage = getDamage(hero_attack, hero_defence, hero_mdefence, m_hp, m_atk, m_def, m_spe);//calculate damage
			if (hero_hp > damage)
			{
				if (anshp < hero_hp - damage)
				{
					anshp = hero_hp - damage;
					bestway = ans + temp->move;
				}
				//cout << "answer: " << ans + temp->move << endl;
				//system("pause");
			}
		}
		else if (tmp->type_num == 31 && hero_yellow_key > 0//yellow
			|| tmp->type_num == 32 && hero_blue_key > 0//blue
			|| tmp->type_num == 33 && hero_red_key > 0)//red
		{
			tmp->change_hero();//change hero status
			tmp_type = map[tmp->x][tmp->y];
			uni = g->createunite(tmp);
			ans += temp->move;
			map[tmp->x][tmp->y] = 10;
			
			search(uni);//unite two node into next position node and go there

			tmp->unchange_hero();//��ʼ�����ˣ�
			map[tmp->x][tmp->y] = tmp_type;
			ans = tmpstr;
			uni->deunite();//change to its original looks
			uni = NULL;
		}
		else if (tmp->type_num <= 70 && tmp->type_num >= 51)//monster
		{
			int damage;
			int m_hp = tmp->hp, m_atk = tmp->attack_increase, m_def = tmp->defence_increase, m_spe = tmp->magicdefence_increase;
			damage = getDamage(hero_attack, hero_defence, hero_mdefence, m_hp, m_atk, m_def, m_spe);//calculate damage
			
			if (hero_hp <= damage)
				continue;

			hero_hp -= damage;
			tmp->visited = 1;
			tmp_type = map[tmp->x][tmp->y];
			uni = g->createunite(tmp);
			map[tmp->x][tmp->y] = 10;
			ans += temp->move;

			search(uni);//unite two node into next position node and go there

			map[tmp->x][tmp->y] = tmp_type;
			tmp->visited = 0;
			hero_hp += damage;
			ans = tmpstr;
			tmp->visited = 0;
			uni->deunite();

		}
	}

}

int main() {
	fin.open("input.txt");
	fout.open("output.txt");
	fout.clear();

	//initbyhuman();
	init(fin);
	build_map();
	graph[0]->visited = 1;
	//search(graph[0]);
	BFS(graph[0]);
	
	//if (anshp > 0)
	//	fout << "Can beat boss : " << bestway << endl;	//�������·��
	//else
	//	fout << "Can't beat boss : " << ans << endl;
	fin.close();
	fout.close();
	//system("pause");
	return 0;
}

/*
�����ļ�Ϊinput.txt������ļ�Ϊoutput.txt

--------------------------------------------------------------

����˵����

��һ��һ������Ϊ�������ڱ���Ŀ�к�Ϊ1
�ڶ���������m��n�������ͼ�����ڱ���Ŀ��m��n����Ϊ13��

��������һ��m*n�ľ���ÿ����һ�����������ͼ�����ӡ�

���У�
10     ·
11     ǽ

21    ��Կ��
22    ��Կ��
23    ��Կ��
24    �챦ʯ���ӹ�����
25    ����ʯ���ӷ�����
26    �̱�ʯ����ħ����
27    СѪƿ����������
28    ��Ѫƿ����������

31    ����
32    ����
33    ����

51~70 ��ͨ����
99    boss

������һ������������ֱ����챦ʯ�ӹ�������ֵ������ʯ�ӷ�������ֵ���̱�ʯ��ħ������ֵ��СѪƿ����������ֵ�ʹ�Ѫƿ����������ֵ��
����  1 1 50 50 100  ����ʰȡ�챦ʯ���Լ�1�㹥����ʰȡСѪƿ���Լ�50���������Դ����ơ�


������һ����k������ͬ����Ĺ��������
֮����k�У�ÿ��5�������ֱ��������ID����������������������Ч��

�磺52 200 15 5 2  ����52�Ź��������ֵΪ200������15������5����Ϊ��̹��

�й�ÿ��������Ч��˵�����Լ�����ʿ���˺����㹫ʽ���Բμ� damage.cpp ��ֱ�ӵ��ü��ɣ�


���һ����6�������ֱ�Ϊ��ʿ�ĳ�ʼ������������������ħ������ʼλ�õĺ����ꡢ��ʼλ�õ������ꡣ



--------------------------------------------------------------

���˵����

ֱ�����һ����w,a,s,d��ɵ��ַ������Ի��н�����


��ע��

����ʿǰ������Ϊ���������﷢��ս������ʿ��ǰ����
����ʿǰ������Ϊ�ţ�����п��Ų�������ʿǰ��
����ʿǰ������Ϊ�����ʰȡ�����ʿǰ��


�������������output.txt�ļ����ŵ�mota.exeĿ¼�¿����Զ�����������в�����


*/
