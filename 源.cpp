#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<time.h>
#include<easyx.h>
#include <windows.h>				// ���� Windows Multimedia API
#pragma comment(lib,"Winmm.lib")
/*==========================��============================*/
#define L 700						//���ڴ�С700��700
#define L_START 120					//���̳���������
#define L_END 	680					//���̳���ĩ����
#define WIDTH 40					//�߼��
#define rt_BLACK 1					//����״̬��Ϊ1
#define rt_WHITE 6					//����״̬��Ϊ6
/*========================ȫ�ֱ���========================*/
struct coordinate					//�����ŵ�����Ľṹ����
{
	int x;
	int y;
	int rt;
	int score;
};
static struct coordinate crossing[15][15];	//�����ŵ�����Ľṹ��ά����
int num = 0;								//����
int a, b;									//����������������±�
MOUSEMSG m;
void start();													//��Ϸ��ʼǰ����
void playmusic();												//���ű�������
void begin();													//��ʼ��Ϸ
void paintboard();												//������
void definecrossing();											//���彻���
int Get_mouse_area();											//��ȡ���λ��
int comjudge(int colour, int i, int j, int p, int q);			//�����ж������  colour��ɫ,i,j�����±꣬p����/�,q������
int comjudges(int colour, int i, int j);						//�����ж���������	colour��ɫ,i,j�����±�
int marks(int i, int j);										//����(��Ϊ��)Ϊÿ��λ�ô�֣�����ߵ�Ϊ����λ��	i,j�����±�
int mark(int i, int j);											//���ԣ���Ϊ�ף�Ϊÿ��λ�ô�֣�����ߵ�Ϊ����λ��	i,j�����±�
int judge(int colour);											//�ж���Ӯ�����������˺��˻��е��ˣ�	colour��ɫ
int draw_chessp(int p, int q);												//���ӣ������˻��е������ӣ�
int draw_chesse(int p,int q);												//���ӣ������˻��еĻ����ӣ�
int draw_chesspvp();											//���ӣ���������ģʽ��
int GameStartpvp();												//��ʼ��Ϸ��ѡ������ģʽ��
void GameStartpve();											//��ʼ��Ϸ��ѡ���˻�ģʽ����ִ���ӣ�
void GameStartpves();											//��ʼ��Ϸ��ѡ���˻�ģʽ����ִ���ӣ�
void side();													//ѡ����ִ��ɫ
void win_black();												//���ƺ���ʤ������
void win_white();												//���ư���ʤ������
void flash();													//����Ч��
void restart_or_end();											//���¿�ʼ�������Ϸ

/*========================
==�������ƣ�flash
==���ܣ��𽥱䰵
==��������
==����ֵ����
========================*/
void flash()
{
	int r, g, b;
	DWORD* pbWnd = GetImageBuffer();         // ��ȡ��ͼ���ڵ��Դ�ָ��
	for (int light = 128; light >= 100; light--)
	{
		for (int i = 0; i < 700*700; i++)
		{
			// ����ԭͼƬÿ�������ɫ��ʵ���𽥱䰵��Ч��
			r = (int)((GetRValue(pbWnd[i]) * light) >> 7);
			g = (int)((GetGValue(pbWnd[i]) * light) >> 7);
			b = (int)((GetBValue(pbWnd[i]) * light) >> 7);
			pbWnd[i] = RGB(r, g, b);
		}
		FlushBatchDraw();     // ʹ֮ǰ���Դ������Ч
		Sleep(40);
	}
}
/*========================
==�������ƣ�playmusic
==���ܣ����ű�������
==��������
==����ֵ����
========================*/
void playmusic()
{
	mciSendString(_T("open music.mp3 alias music"), NULL, 0, NULL);
	mciSendString(_T("play music repeat"), NULL, 0, NULL);
}
/*========================
==�������ƣ�restart_or_end
==���ܣ����¿�ʼ��Ϸ�������Ϸ
==��������
==����ֵ����
========================*/
void restart_or_end()
{
	Sleep(400);
	flash();
	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//��������ͼƬ
	settextcolor(BLACK);
	settextstyle(60, 40, _T("������κ"));
	outtextxy(230, 40, _T("������"));		//��ʾ����
	settextstyle(45, 30, _T("������κ"));
	int x;									//����һ��������λ��״ֵ̬�ı���
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//�����ͣ�ڰ�ť1��λ��
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//�����ͣ�ڰ�ť2��λ��
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("���¿�ʼ"));
			outtextxy(230, 400, _T("�˳���Ϸ"));	//������ɫ�ָ�
			break;
		case 1:
			outtextxy(230, 350, _T("���¿�ʼ"));	//�����ɫ
			if (m.mkLButton)
			{
				num = 0;
				start();
				begin();
			}
			break;
		case 2:
			outtextxy(230, 400, _T("�˳���Ϸ"));	//�����ɫ
			if (m.mkLButton)
			{
				exit(1);
			}
			break;
		}
	}
}/*========================
==�������ƣ� paintboard
==���ܣ�������
==��������
==����ֵ��0
========================*/
void paintboard()
{

	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//��������ͼƬ
	settextcolor(BLACK);
	outtextxy(280, 40, _T("������"));				//��ʾ����
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 3);	// ��������
	for (int i = L_START; i <= L_END; i += WIDTH)	//������
	{
		line(i, L_START, i, L_END);
	}
	for (int i = L_START; i <= L_END; i += WIDTH)
	{
		line(L_START, i, L_END, i);
	}
}
/*========================
==�������ƣ�definecrossing
==���ܣ����彻���
==��������
==����ֵ��0
========================*/
void definecrossing()
{
	int i, j, x, y;					//�����С��С������ꡢ������
	for (i = 0, x = L_START; i < 15; i++, x += WIDTH)
	{
		for (j = 0, y = L_START; j < 15; j++, y += WIDTH)
		{
			crossing[i][j] = { x,y ,0,0 };
		}
	}
}
/*========================
==�������ƣ�start
==���ܣ���Ϸ��ʼ����
==��������
==����ֵ��0
========================*/
void start()
{
	playmusic(); 
	initgraph(L, L);				//��ʼ����ͼ����
	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//��������ͼƬ
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(60, 40, _T("������κ"));
	outtextxy(230, 40, _T("������"));		//��ʾ����
	settextstyle(45, 30, _T("������κ"));
	outtextxy(230, 350, _T("�˻�ģʽ"));	//��ʾ���˻�ģʽ��������
	outtextxy(230, 400, _T("��Ҷ�ս"));	//��ʾ����Ҷ�ս��������
}
/*========================
==�������ƣ�win_black
==���ܣ�����ʤ������
==��������
==����ֵ��0
========================*/
void win_black()
{
	setbkcolor(RGB(125, 125, 125));			//��ɫ
	cleardevice();
	settextcolor(BLACK);
	settextstyle(90, 60, _T("����"));
	outtextxy(170, 305, _T("����ʤ"));
	restart_or_end();
}
/*========================
==�������ƣ�win_white
==����:����ʤ������
==��������
==����ֵ��0
========================*/
void win_white()
{
	setbkcolor(RGB(125, 125, 125));			//��ɫ
	cleardevice();
	settextcolor(WHITE);
	settextstyle(90, 60, _T("����"));
	outtextxy(170, 305, _T("����ʤ"));
	restart_or_end();
}
/*========================
==��������Get_mouse_area
==���ܣ���Ϸʱ��ȡ�����������Χ
==��������
==����ֵ����
========================*/
int Get_mouse_area()
{
	int i, j, x1, y1;
	for (i = 1; i < 16; i++)
	{
		for (j = 1; j < 16; j++)
		{
			x1 = 80 + 40 * i;
			y1 = 80 + 40 * j;
			if (m.x <= x1 + 10 && m.x >= x1 - 10)
			{
				if (m.y <= y1 + 10 && m.y >= y1 - 10)
				{
					if (m.uMsg == WM_LBUTTONDOWN)
					{
						a = i - 1;
						b = j - 1;
						return 1;
					}
				}
			}

		}
	}
	return 0;
}
/*========================
==��������comjudge
==���ܣ������ж������
==������ ��ɫ,�����±꣬p����/�,q������
==����ֵ����
========================*/
int comjudge(int colour, int i, int j, int p, int q)
{
	int k;	/*��K������ĳһ���������һ�����Ӷ�������������ɫ����(��) :k=2 ��
																		   k=1 һ�߱���ס
																		   k=0 ���߱���ס*/
	int i0, j0;
	int l;	//�涨��λ�ܸ���
	i0 = i;
	j0 = j;
	k = 0;
	l = 0;
	{
		//���������޹涨��ɫ����
		while (crossing[i][j - 1].rt == colour && j - 1 >= 0)
		{
			l++;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i][j - 1].rt == 0 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����ұ����޹涨��ɫ����
		while (crossing[i][j + 1].rt == colour && j + 1 <= 14)
		{
			l++;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i][j + 1].rt == 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}

	}	//����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j].rt == colour && i - 1 >= 0)
		{
			l++;
			i -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j].rt == 0 && i - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j].rt == colour && i + 1 <= 14)
		{
			l++;
			i += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j].rt == 0 && i + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j - 1].rt == colour && i - 1 >= 0 && j - 1 >= 0)
		{
			l++;
			i -= 1;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j - 1].rt == 0 && i - 1 >= 0 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j + 1].rt == colour && i + 1 <= 14 && j + 1 <= 14)
		{
			l++;
			i += 1;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j + 1].rt == 0 && i + 1 <= 14 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//  \����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j - 1].rt == colour && i + 1 <= 14 && j - 1 >= 0)
		{
			l++;
			i += 1;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j - 1].rt == 0 && i + 1 <= 14 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j + 1].rt == colour && i - 1 >= 0 && j + 1 <= 14)
		{
			l++;
			i -= 1;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j + 1].rt == 0 && i - 1 >= 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//  /����
	return 0;
}

/*========================
==��������comjudges
==���ܣ������ж�����5��
==������ ��ɫ,�����±�
==����ֵ����
========================*/
int comjudges(int colour, int i, int j)
{
	int k;	/*��K������ĳһ���������һ�����Ӷ�������������ɫ����(��) :k=2 ��
																		   k=1 һ�߱���ס
																		   k=0 ���߱���ס*/
	int i0, j0;
	int l;	//�涨��λ�ܸ���
	i0 = i;
	j0 = j;
	k = 0;
	l = 0;
	{
		//���������޹涨��ɫ����
		while (crossing[i][j - 1].rt == colour && j - 1 >= 0)
		{
			l++;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i][j - 1].rt == 0 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����ұ����޹涨��ɫ����
		while (crossing[i][j + 1].rt == colour && j + 1 <= 14)
		{
			l++;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i][j + 1].rt == 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}

	}	//����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j].rt == colour && i - 1 >= 0)
		{
			l++;
			i -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j].rt == 0 && i - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j].rt == colour && i + 1 <= 14)
		{
			l++;
			i += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j].rt == 0 && i + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j - 1].rt == colour && i - 1 >= 0 && j - 1 >= 0)
		{
			l++;
			i -= 1;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j - 1].rt == 0 && i - 1 >= 0 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j + 1].rt == colour && i + 1 <= 14 && j + 1 <= 14)
		{
			l++;
			i += 1;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j + 1].rt == 0 && i + 1 <= 14 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//  \����


	{
		//��������
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//����������޹涨��ɫ����
		while (crossing[i + 1][j - 1].rt == colour && i + 1 <= 14 && j - 1 >= 0)
		{
			l++;
			i += 1;
			j -= 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i + 1][j - 1].rt == 0 && i + 1 <= 14 && j - 1 >= 0)
		{
			k++;
		}
		//��ԭ����λ��
		i = i0;
		j = j0;
		//����������޹涨��ɫ����
		while (crossing[i - 1][j + 1].rt == colour && i - 1 >= 0 && j + 1 <= 14)
		{
			l++;
			i -= 1;
			j += 1;
		}
		//�����һ���Ƿ�Ϊ����
		if (crossing[i - 1][j + 1].rt == 0 && i - 1 >= 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//  /����
	return 0;
}
/*========================
==��������marks
==���ܣ��Ը�������д��
==������ �����±�
==����ֵ����
========================*/
int marks(int i, int j)
{
	/*   ��Ҫ˼��
	���԰���  �Լ�����
	������������	+10000	�谭������������	+1000
	�������ɻ���	+200	�谭�������ɻ���	+100
	������������	+50	    �谭������������	+20
	�������ɻ���	+30	    �谭�������ɻ���	+10
	������������	+8	    �谭������������	+5
	�������ɻ��	+2	    �谭�������ɻ��	+1
	������������	+2	    �谭������������	+1
	�������ɻ�һ	+1	    �谭�������ɻ�һ	+0
	����������һ	+1	    �谭����������һ	+0
*/
	if (crossing[i][j].rt != 0)
	{
		return 0;
	}

	//������������	+10000
	else
	{
		if (comjudges(rt_WHITE, i, j))
		{
			crossing[i][j].score += 10000;
		}
		//�谭������������	+1000
		if (comjudges(rt_BLACK, i, j))
		{
			crossing[i][j].score += 1000;
		}
		//�������ɻ���	+200      
		if (comjudge(rt_WHITE, i, j, 2, 4))
		{
			crossing[i][j].score += 200;
		}
		//�谭�������ɻ���	+100
		if (comjudge(rt_BLACK, i, j, 2, 4))
		{
			crossing[i][j].score += 100;
		}
		//������������	+50	
		if (comjudge(rt_WHITE, i, j, 1, 4))
		{
			crossing[i][j].score += 50;
		}
		//�谭������������	+20
		if (comjudge(rt_BLACK, i, j, 1, 4))
		{
			crossing[i][j].score += 20;
		}
		//�������ɻ���	+30	
		if (comjudge(rt_WHITE, i, j, 2, 3))
		{
			crossing[i][j].score += 30;
		}
		//�谭�������ɻ���	+10
		if (comjudge(rt_BLACK, i, j, 2, 3))
		{
			crossing[i][j].score += 10;
		}
		//������������	+8	
		if (comjudge(rt_WHITE, i, j, 1, 3))
		{
			crossing[i][j].score += 8;
		}
		//�谭������������	+5
		if (comjudge(rt_BLACK, i, j, 1, 3))
		{
			crossing[i][j].score += 5;
		}
		//�������ɻ��	+2	
		if (comjudge(rt_WHITE, i, j, 2, 2))
		{
			crossing[i][j].score += 2;
		}
		//�谭�������ɻ��	+1
		if (comjudge(rt_BLACK, i, j, 2, 2))
		{
			crossing[i][j].score += 1;
		}
		//������������	+2	
		if (comjudge(rt_WHITE, i, j, 1, 2))
		{
			crossing[i][j].score += 2;
		}
		//�谭������������	+1
		if (comjudge(rt_BLACK, i, j, 1, 2))
		{
			crossing[i][j].score += 1;
		}
		//�������ɻ�һ	+1	
		if (comjudge(rt_WHITE, i, j, 2, 1))
		{
			crossing[i][j].score += 1;
		}
		//����������һ	+1	
		if (comjudge(rt_WHITE, i, j, 1, 1))
		{
			crossing[i][j].score += 1;
		}
		/*//�谭�������ɻ�һ	+1
		if(comjudge(rt_BLACK, i, j, 2, 1))
			{
					crossing[i][j].score += 1;
			}
			*/

			/* //�谭����������һ	+0
				if()
				{
						crossing[i][j].score += 0;
				}*/
		return 0;
	}
}

/*========================
==��������mark
==���ܣ��Ը�������д��
==������ �����±�
==����ֵ����
========================*/
int mark(int i, int j)
{
	if (crossing[i][j].rt != 0)
	{
		return 0;
	}

	//������������	+10000
	else
	{
		if (comjudges(rt_BLACK, i, j))
		{
			crossing[i][j].score += 10000;
		}
		//�谭������������	+1000
		if (comjudges(rt_WHITE, i, j))
		{
			crossing[i][j].score += 1000;
		}
		//�������ɻ���	+200      
		if (comjudge(rt_BLACK, i, j, 2, 4))
		{
			crossing[i][j].score += 200;
		}
		//�谭�������ɻ���	+100
		if (comjudge(rt_WHITE, i, j, 2, 4))
		{
			crossing[i][j].score += 100;
		}
		//������������	+50	
		if (comjudge(rt_BLACK, i, j, 1, 4))
		{
			crossing[i][j].score += 50;
		}
		//�谭������������	+20
		if (comjudge(rt_WHITE, i, j, 1, 4))
		{
			crossing[i][j].score += 20;
		}
		//�������ɻ���	+30	
		if (comjudge(rt_BLACK, i, j, 2, 3))
		{
			crossing[i][j].score += 30;
		}
		//�谭�������ɻ���	+10
		if (comjudge(rt_WHITE, i, j, 2, 3))
		{
			crossing[i][j].score += 10;
		}
		//������������	+8	
		if (comjudge(rt_BLACK, i, j, 1, 3))
		{
			crossing[i][j].score += 8;
		}
		//�谭������������	+5
		if (comjudge(rt_WHITE, i, j, 1, 3))
		{
			crossing[i][j].score += 5;
		}
		//�������ɻ��	+2	
		if (comjudge(rt_BLACK, i, j, 2, 2))
		{
			crossing[i][j].score += 2;
		}
		//�谭�������ɻ��	+1
		if (comjudge(rt_WHITE, i, j, 2, 2))
		{
			crossing[i][j].score += 1;
		}
		//������������	+2	
		if (comjudge(rt_BLACK, i, j, 1, 2))
		{
			crossing[i][j].score += 2;
		}
		//�谭������������	+1
		if (comjudge(rt_WHITE, i, j, 1, 2))
		{
			crossing[i][j].score += 1;
		}
		//�������ɻ�һ	+1	
		if (comjudge(rt_BLACK, i, j, 2, 1))
		{
			crossing[i][j].score += 1;
		}
		//����������һ	+1	
		if (comjudge(rt_BLACK, i, j, 1, 1))
		{
			crossing[i][j].score += 1;
		}
		/*//�谭�������ɻ�һ	+1
		if(comjudge(rt_BLACK, i, j, 2, 1))
			{
					crossing[i][j].score += 1;
			}
			*/

			/* //�谭����������һ	+0
				if()
				{
						crossing[i][j].score += 0;
				}*/
		return 0;
	}
}
/*========================
==��������judge
==���ܣ��ж���Ӯ
==������  coordinate crossing[][];   //����
		int colour;   //��ɫ
		int a,b	;	   //�����±�
==����ֵ�� ��
========================*/
int judge(int colour)
{
	int i, j;
	int n1 = 0, n2 = 0;  //���������ͬ���Ӹ���
	while (1)
	{
		for (i = a, j = b; i >= 0; i--)  //ˮƽ�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15; i++)  //ˮƽ�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//�ж��Ƿ�����
		{
			return 1;
			break;
		}




		n1 = 0;
		n2 = 0;
		for (i = a, j = b; j >= 0; j--)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; j<15;j++)		//�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//�ж��Ƿ�����
		{
			return 1;
			break;
		}


		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i >= 0&& j >= 0; i--, j--)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15&&j < 15; i++, j++)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//�ж��Ƿ�����
		{
			return 1;
			break;
		}

		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i >= 0&& j >= 0; i--, j--)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15&& j < 15; i++, j++)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//�ж��Ƿ�����
		{
			return 1;
			break;
		}

		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i < 15&&j >= 0; i++, j--)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i >= 0&&j < 15; i--, j++)  //�����ж�������ɫ��ͬ����
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//�ж��Ƿ�����
		{
			return 1;
			break;
		}
		return 0;
	}
}


/*========================
==��������draw_chesspvp
==���ܣ������ӣ���Ҷ�ս��
==���� ��coordinate crossing[][];   //����
		 int num;					//����
		 int a,b	;				//�����±�
==����ֵ����
========================*/
int draw_chesspvp()
{
	if (num % 2 == 0)
	{
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		fillcircle(crossing[a][b].x, crossing[a][b].y, 15);
		crossing[a][b].rt = rt_BLACK;
	}
	if (num % 2 != 0)
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		fillcircle(crossing[a][b].x, crossing[a][b].y, 15);
		crossing[a][b].rt = rt_WHITE;
	}
	return 0;
}
/*========================
==��������draw_chessp
==���ܣ���������ӣ��˻�ģʽ��
==����  coordinate crossing[][];	//����
		int num;					//����
		int a,b;					//�����±�
==����ֵ�� ��
========================*/
int draw_chessp(int p,int q)
{
	if (num % 2 == 0)
	{
		setlinecolor(BLACK);
		setfillcolor(BLACK);
		fillcircle(crossing[a][b].x, crossing[a][b].y, 15);
		crossing[a][b].rt = rt_BLACK;
	}
	return 0;
}

/*========================
==��������draw_chesse
==���ܣ�����������ӣ��˻�ģʽ��
==����  coordinate crossing[][];	//����
		int num;					//����
		int a,b	;					//�����±�
==����ֵ�� ��
========================*/
int draw_chesse(int p, int q)
{
	if (num % 2 != 0)
	{
		setlinecolor(WHITE);
		setfillcolor(WHITE);
		fillcircle(crossing[p][q].x, crossing[p][q].y, 15);
		crossing[p][q].rt = rt_WHITE;
	}
	return 0;
}

/*========================
==��������GameStartpvp
==���ܣ���ʼ��Ҷ�ս
==��������
==����ֵ����
========================*/
int GameStartpvp()
{
	paintboard();
	definecrossing();
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	fillcircle(60, 250, 15);
	setlinecolor(WHITE);
	setfillcolor(WHITE);
	int pos; //���״̬
	while (1)
	{
		m = GetMouseMsg();
		pos = Get_mouse_area();
		switch (m.uMsg)
		{
		case WM_LBUTTONDOWN:
			if (pos)
			{
				if (crossing[a][b].rt == rt_BLACK || crossing[a][b].rt == rt_WHITE)
					continue;
				else
				{
					fillcircle(60, 250, 15);
					draw_chesspvp();
					num++;
				}
				if (judge(rt_BLACK))
				{
					win_black();
					return 0;
				}
				else if (judge(rt_WHITE))
				{
					win_white();
					return 0;
				}

			}
		}
	}

}

/*========================
==��������GameStartpve
==���ܣ���Ϸ��ʼ(�˻�)
==��������
==����ֵ����
========================*/
void GameStartpve()
{
	int i = 0, j = 0;
	int p, q;
	num = 0;
	paintboard();
	definecrossing();
	int pos; //���״̬
	while (1)
	{
		p = 0, q = 0;
		m = GetMouseMsg();
		pos = Get_mouse_area();
		if (num % 2 == 0)
		{
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (pos == 1)
				{
					if (crossing[a][b].rt == rt_BLACK || crossing[a][b].rt == rt_WHITE)
						continue;
					else
					{
						draw_chessp(a,b);
						num++;
					}
					if (judge(rt_BLACK))
					{
						win_black();
						return;
					}
				}
			}
		}
		else if (num % 2 != 0)
		{
			for (i = 0; i < 15; i++)
			{
				for (j = 0; j < 15; j++)
				{
					crossing[i][j].score = 0;
				}
			}
			for (i = 0; i < 15; i++)
			{
				for (j = 0; j < 15; j++)
				{
					marks(i, j);
				}
			}
			for (j = 0; j < 15; j++)
			{
				for (i = 0; i < 15; i++)
				{

					if (crossing[p][q].score < crossing[i][j].score)
					{
						p = i, q = j;
					}
				}
			}
			a = p, b = q;
			draw_chesse(p, q);
			num++;
			if (judge(rt_WHITE))
			{
				win_white();
				return;
			}
		}


	}
}

/*========================
==��������GameStartpves
==���ܣ���Ϸ��ʼ(�˻�)
==��������
==����ֵ����
========================*/
void GameStartpves()
{
	int i = 0, j = 0;
	int p, q;
	num = 1;
	paintboard();
	definecrossing();
	int pos; //���״̬
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	fillcircle(crossing[7][7].x, crossing[7][7].y, 15);
	crossing[7][7].rt = rt_BLACK;
	while (1)
	{
		p = 0, q = 0;
		m = GetMouseMsg();
		pos = Get_mouse_area();
		if (num % 2 != 0)
		{
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (pos == 1)
				{
					if (crossing[a][b].rt == rt_BLACK || crossing[a][b].rt == rt_WHITE)
						continue;
					else
					{
						p = a, q = b;
						draw_chesse(p,q);
						num++;
					}
					if (judge(rt_WHITE))
					{
						win_white();
						return;
					}
				}
			}
		}
		else if (num % 2 == 0)
		{
			for (i = 0; i < 15; i++)
			{
				for (j = 0; j < 15; j++)
				{
					crossing[i][j].score = 0;
				}
			}
			for (i = 0; i < 15; i++)
			{
				for (j = 0; j < 15; j++)
				{
					mark(i, j);
				}
			}
			for (j = 0; j < 15; j++)
			{
				for (i = 0; i < 15; i++)
				{

					if (crossing[p][q].score < crossing[i][j].score)
					{
						p = i, q = j;
					}
				}
			}
			a = p, b = q;
			draw_chessp(p, q);
			num++;
			if (judge(rt_BLACK))
			{
				win_black();
				return;
			}
		}


	}

}


/*========================
==�������ƣ�begin
==���ܣ���ʼ����Ĳ���
==��������
==����ֵ����
========================*/
void begin()
{
	int x;									//����һ��������λ��״ֵ̬�ı���
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//�����ͣ�ڰ�ť1��λ��
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//�����ͣ�ڰ�ť2��λ��
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("�˻�ģʽ"));
			outtextxy(230, 400, _T("��Ҷ�ս"));	//������ɫ�ָ�
			break;
		case 1:
			outtextxy(230, 350, _T("�˻�ģʽ"));	//�����ɫ
			if (m.mkLButton)
			{
				side();
				return;
			}
			break;
		case 2:
			outtextxy(230, 400, _T("��Ҷ�ս"));	//�����ɫ
			if (m.mkLButton)
			{
				GameStartpvp();
				return;
			}
			break;
		}
	}

}
/*========================
==�������ƣ�side
==���ܣ�ѡ����ִ��ɫ
==��������
==����ֵ����
========================*/
void side()
{
	int x;												//����һ��������λ��״ֵ̬�ı���
	loadimage(NULL, _T("board.jpg"), 700, 700, true);
	settextcolor(BLACK);
	settextstyle(60, 40, _T("������κ"));
	outtextxy(230, 40, _T("������"));
	settextstyle(45, 30, _T("������κ"));
	outtextxy(230, 350, _T("ѡ�����"));
	outtextxy(230, 400, _T("ѡ�����"));
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//�����ͣ�ڰ�ť1��λ��
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//�����ͣ�ڰ�ť2��λ��
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("ѡ�����"));
			outtextxy(230, 400, _T("ѡ�����"));	//������ɫ�ָ�
			break;
		case 1:
			outtextxy(230, 350, _T("ѡ�����"));	//�����ɫ
			if (m.mkLButton)
			{
				GameStartpve();
				return;
			}
			break;
		case 2:
			outtextxy(230, 400, _T("ѡ�����"));	//�����ɫ
			if (m.mkLButton)
			{
				GameStartpves();
				return;
			}
			break;
		}
	}

}




int main()
{
	start();
	begin();
	closegraph();
}