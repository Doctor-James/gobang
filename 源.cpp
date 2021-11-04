#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<time.h>
#include<easyx.h>
#include <windows.h>				// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")
/*==========================宏============================*/
#define L 700						//窗口大小700×700
#define L_START 120					//棋盘长度首坐标
#define L_END 	680					//棋盘长度末坐标
#define WIDTH 40					//线间距
#define rt_BLACK 1					//黑棋状态量为1
#define rt_WHITE 6					//白棋状态量为6
/*========================全局变量========================*/
struct coordinate					//定义存放点坐标的结构类型
{
	int x;
	int y;
	int rt;
	int score;
};
static struct coordinate crossing[15][15];	//定义存放点坐标的结构二维数组
int num = 0;								//步数
int a, b;									//坐标在数组里面的下标
MOUSEMSG m;
void start();													//游戏开始前界面
void playmusic();												//播放背景音乐
void begin();													//开始游戏
void paintboard();												//画棋盘
void definecrossing();											//定义交叉点
int Get_mouse_area();											//读取鼠标位置
int comjudge(int colour, int i, int j, int p, int q);			//电脑判断死几活几  colour颜色,i,j数组下标，p（死/活）,q（几）
int comjudges(int colour, int i, int j);						//电脑判断连成五子	colour颜色,i,j数组下标
int marks(int i, int j);										//电脑(人为黑)为每个位置打分，分最高的为落子位置	i,j数组下标
int mark(int i, int j);											//电脑（人为白）为每个位置打分，分最高的为落子位置	i,j数组下标
int judge(int colour);											//判断输赢（可用于人人和人机中的人）	colour颜色
int draw_chessp(int p, int q);												//落子（用于人机中的人落子）
int draw_chesse(int p,int q);												//落子（用于人机中的机落子）
int draw_chesspvp();											//落子（用于人人模式）
int GameStartpvp();												//开始游戏（选择人人模式）
void GameStartpve();											//开始游戏（选择人机模式，人执黑子）
void GameStartpves();											//开始游戏（选择人机模式，人执白子）
void side();													//选择所执棋色
void win_black();												//绘制黑棋胜利字样
void win_white();												//绘制白棋胜利字样
void flash();													//动画效果
void restart_or_end();											//重新开始或结束游戏

/*========================
==函数名称：flash
==功能：逐渐变暗
==参数：无
==返回值：无
========================*/
void flash()
{
	int r, g, b;
	DWORD* pbWnd = GetImageBuffer();         // 获取绘图窗口的显存指针
	for (int light = 128; light >= 100; light--)
	{
		for (int i = 0; i < 700*700; i++)
		{
			// 计算原图片每个点的颜色，实现逐渐变暗的效果
			r = (int)((GetRValue(pbWnd[i]) * light) >> 7);
			g = (int)((GetGValue(pbWnd[i]) * light) >> 7);
			b = (int)((GetBValue(pbWnd[i]) * light) >> 7);
			pbWnd[i] = RGB(r, g, b);
		}
		FlushBatchDraw();     // 使之前的显存操作生效
		Sleep(40);
	}
}
/*========================
==函数名称：playmusic
==功能：播放背景音乐
==参数：无
==返回值：无
========================*/
void playmusic()
{
	mciSendString(_T("open music.mp3 alias music"), NULL, 0, NULL);
	mciSendString(_T("play music repeat"), NULL, 0, NULL);
}
/*========================
==函数名称：restart_or_end
==功能：重新开始游戏或结束游戏
==参数：无
==返回值：无
========================*/
void restart_or_end()
{
	Sleep(400);
	flash();
	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//加载棋盘图片
	settextcolor(BLACK);
	settextstyle(60, 40, _T("华文新魏"));
	outtextxy(230, 40, _T("五子棋"));		//显示标题
	settextstyle(45, 30, _T("华文新魏"));
	int x;									//定义一个存放鼠标位置状态值的变量
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//鼠标悬停在按钮1的位置
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//鼠标悬停在按钮2的位置
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("重新开始"));
			outtextxy(230, 400, _T("退出游戏"));	//字体颜色恢复
			break;
		case 1:
			outtextxy(230, 350, _T("重新开始"));	//字体变色
			if (m.mkLButton)
			{
				num = 0;
				start();
				begin();
			}
			break;
		case 2:
			outtextxy(230, 400, _T("退出游戏"));	//字体变色
			if (m.mkLButton)
			{
				exit(1);
			}
			break;
		}
	}
}/*========================
==函数名称： paintboard
==功能：画棋盘
==参数：无
==返回值：0
========================*/
void paintboard()
{

	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//加载棋盘图片
	settextcolor(BLACK);
	outtextxy(280, 40, _T("五子棋"));				//显示标题
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 3);	// 设置线型
	for (int i = L_START; i <= L_END; i += WIDTH)	//画棋盘
	{
		line(i, L_START, i, L_END);
	}
	for (int i = L_START; i <= L_END; i += WIDTH)
	{
		line(L_START, i, L_END, i);
	}
}
/*========================
==函数名称：definecrossing
==功能：定义交叉点
==参数：无
==返回值：0
========================*/
void definecrossing()
{
	int i, j, x, y;					//定义行、列、横坐标、纵坐标
	for (i = 0, x = L_START; i < 15; i++, x += WIDTH)
	{
		for (j = 0, y = L_START; j < 15; j++, y += WIDTH)
		{
			crossing[i][j] = { x,y ,0,0 };
		}
	}
}
/*========================
==函数名称：start
==功能：游戏开始界面
==参数：无
==返回值：0
========================*/
void start()
{
	playmusic(); 
	initgraph(L, L);				//初始化绘图窗口
	loadimage(NULL, _T("board.jpg"), 700, 700, true);		//加载棋盘图片
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(60, 40, _T("华文新魏"));
	outtextxy(230, 40, _T("五子棋"));		//显示标题
	settextstyle(45, 30, _T("华文新魏"));
	outtextxy(230, 350, _T("人机模式"));	//显示“人机模式”的字样
	outtextxy(230, 400, _T("玩家对战"));	//显示“玩家对战”的字样
}
/*========================
==函数名称：win_black
==功能：黑旗胜利字样
==参数：无
==返回值：0
========================*/
void win_black()
{
	setbkcolor(RGB(125, 125, 125));			//灰色
	cleardevice();
	settextcolor(BLACK);
	settextstyle(90, 60, _T("楷体"));
	outtextxy(170, 305, _T("黑棋胜"));
	restart_or_end();
}
/*========================
==函数名称：win_white
==功能:白棋胜利字样
==参数：无
==返回值：0
========================*/
void win_white()
{
	setbkcolor(RGB(125, 125, 125));			//灰色
	cleardevice();
	settextcolor(WHITE);
	settextstyle(90, 60, _T("楷体"));
	outtextxy(170, 305, _T("白棋胜"));
	restart_or_end();
}
/*========================
==函数名：Get_mouse_area
==功能：游戏时读取鼠标所在区域范围
==参数：无
==返回值：无
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
==函数名：comjudge
==功能：电脑判断死几活几
==参数： 颜色,数组下标，p（死/活）,q（几）
==返回值：无
========================*/
int comjudge(int colour, int i, int j, int p, int q)
{
	int k;	/*用K来体现某一个方向的下一个无子而不是有另外颜色的子(活) :k=2 活
																		   k=1 一边被堵住
																		   k=0 两边被堵住*/
	int i0, j0;
	int l;	//规定方位总个数
	i0 = i;
	j0 = j;
	k = 0;
	l = 0;
	{
		//检查左边有无规定颜色的子
		while (crossing[i][j - 1].rt == colour && j - 1 >= 0)
		{
			l++;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i][j - 1].rt == 0 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查右边有无规定颜色的子
		while (crossing[i][j + 1].rt == colour && j + 1 <= 14)
		{
			l++;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i][j + 1].rt == 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}

	}	//左右


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查上面有无规定颜色的子
		while (crossing[i - 1][j].rt == colour && i - 1 >= 0)
		{
			l++;
			i -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j].rt == 0 && i - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查下面有无规定颜色的子
		while (crossing[i + 1][j].rt == colour && i + 1 <= 14)
		{
			l++;
			i += 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j].rt == 0 && i + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//上下


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查左上有无规定颜色的子
		while (crossing[i - 1][j - 1].rt == colour && i - 1 >= 0 && j - 1 >= 0)
		{
			l++;
			i -= 1;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j - 1].rt == 0 && i - 1 >= 0 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查右下有无规定颜色的子
		while (crossing[i + 1][j + 1].rt == colour && i + 1 <= 14 && j + 1 <= 14)
		{
			l++;
			i += 1;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j + 1].rt == 0 && i + 1 <= 14 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//  \方向


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查右上有无规定颜色的子
		while (crossing[i + 1][j - 1].rt == colour && i + 1 <= 14 && j - 1 >= 0)
		{
			l++;
			i += 1;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j - 1].rt == 0 && i + 1 <= 14 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查左下有无规定颜色的子
		while (crossing[i - 1][j + 1].rt == colour && i - 1 >= 0 && j + 1 <= 14)
		{
			l++;
			i -= 1;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j + 1].rt == 0 && i - 1 >= 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l == q - 1 && k == p)
		{
			return 1;
		}
	}			//  /方向
	return 0;
}

/*========================
==函数名：comjudges
==功能：电脑判断连成5子
==参数： 颜色,数组下标
==返回值：无
========================*/
int comjudges(int colour, int i, int j)
{
	int k;	/*用K来体现某一个方向的下一个无子而不是有另外颜色的子(活) :k=2 活
																		   k=1 一边被堵住
																		   k=0 两边被堵住*/
	int i0, j0;
	int l;	//规定方位总个数
	i0 = i;
	j0 = j;
	k = 0;
	l = 0;
	{
		//检查左边有无规定颜色的子
		while (crossing[i][j - 1].rt == colour && j - 1 >= 0)
		{
			l++;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i][j - 1].rt == 0 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查右边有无规定颜色的子
		while (crossing[i][j + 1].rt == colour && j + 1 <= 14)
		{
			l++;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i][j + 1].rt == 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}

	}	//左右


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查上面有无规定颜色的子
		while (crossing[i - 1][j].rt == colour && i - 1 >= 0)
		{
			l++;
			i -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j].rt == 0 && i - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查下面有无规定颜色的子
		while (crossing[i + 1][j].rt == colour && i + 1 <= 14)
		{
			l++;
			i += 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j].rt == 0 && i + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//上下


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查左上有无规定颜色的子
		while (crossing[i - 1][j - 1].rt == colour && i - 1 >= 0 && j - 1 >= 0)
		{
			l++;
			i -= 1;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j - 1].rt == 0 && i - 1 >= 0 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查右下有无规定颜色的子
		while (crossing[i + 1][j + 1].rt == colour && i + 1 <= 14 && j + 1 <= 14)
		{
			l++;
			i += 1;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j + 1].rt == 0 && i + 1 <= 14 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//  \方向


	{
		//重置数据
		i = i0;
		j = j0;
		k = 0;
		l = 0;
		//检查右上有无规定颜色的子
		while (crossing[i + 1][j - 1].rt == colour && i + 1 <= 14 && j - 1 >= 0)
		{
			l++;
			i += 1;
			j -= 1;
		}
		//检查下一个是否为无子
		if (crossing[i + 1][j - 1].rt == 0 && i + 1 <= 14 && j - 1 >= 0)
		{
			k++;
		}
		//还原落子位置
		i = i0;
		j = j0;
		//检查左下有无规定颜色的子
		while (crossing[i - 1][j + 1].rt == colour && i - 1 >= 0 && j + 1 <= 14)
		{
			l++;
			i -= 1;
			j += 1;
		}
		//检查下一个是否为无子
		if (crossing[i - 1][j + 1].rt == 0 && i - 1 >= 0 && j + 1 <= 14)
		{
			k++;
		}
		if (l >= 4)
		{
			return 1;
		}
	}			//  /方向
	return 0;
}
/*========================
==函数名：marks
==功能：对各个点进行打分
==参数： 数组下标
==返回值：无
========================*/
int marks(int i, int j)
{
	/*   主要思想
	电脑白子  自己黑子
	白子连成五子	+10000	阻碍黑子连成五子	+1000
	白子连成活四	+200	阻碍黑子连成活四	+100
	白子连成死四	+50	    阻碍黑子连成死四	+20
	白子连成活三	+30	    阻碍黑子连成活三	+10
	白子连成死三	+8	    阻碍黑子连成死三	+5
	白子连成活二	+2	    阻碍黑子连成活二	+1
	白子连成死二	+2	    阻碍黑子连成死二	+1
	白子连成活一	+1	    阻碍黑子连成活一	+0
	白子连成死一	+1	    阻碍黑子连成死一	+0
*/
	if (crossing[i][j].rt != 0)
	{
		return 0;
	}

	//白子连成五子	+10000
	else
	{
		if (comjudges(rt_WHITE, i, j))
		{
			crossing[i][j].score += 10000;
		}
		//阻碍黑子连成五子	+1000
		if (comjudges(rt_BLACK, i, j))
		{
			crossing[i][j].score += 1000;
		}
		//白子连成活四	+200      
		if (comjudge(rt_WHITE, i, j, 2, 4))
		{
			crossing[i][j].score += 200;
		}
		//阻碍黑子连成活四	+100
		if (comjudge(rt_BLACK, i, j, 2, 4))
		{
			crossing[i][j].score += 100;
		}
		//白子连成死四	+50	
		if (comjudge(rt_WHITE, i, j, 1, 4))
		{
			crossing[i][j].score += 50;
		}
		//阻碍黑子连成死四	+20
		if (comjudge(rt_BLACK, i, j, 1, 4))
		{
			crossing[i][j].score += 20;
		}
		//白子连成活三	+30	
		if (comjudge(rt_WHITE, i, j, 2, 3))
		{
			crossing[i][j].score += 30;
		}
		//阻碍黑子连成活三	+10
		if (comjudge(rt_BLACK, i, j, 2, 3))
		{
			crossing[i][j].score += 10;
		}
		//白子连成死三	+8	
		if (comjudge(rt_WHITE, i, j, 1, 3))
		{
			crossing[i][j].score += 8;
		}
		//阻碍黑子连成死三	+5
		if (comjudge(rt_BLACK, i, j, 1, 3))
		{
			crossing[i][j].score += 5;
		}
		//白子连成活二	+2	
		if (comjudge(rt_WHITE, i, j, 2, 2))
		{
			crossing[i][j].score += 2;
		}
		//阻碍黑子连成活二	+1
		if (comjudge(rt_BLACK, i, j, 2, 2))
		{
			crossing[i][j].score += 1;
		}
		//白子连成死二	+2	
		if (comjudge(rt_WHITE, i, j, 1, 2))
		{
			crossing[i][j].score += 2;
		}
		//阻碍黑子连成死二	+1
		if (comjudge(rt_BLACK, i, j, 1, 2))
		{
			crossing[i][j].score += 1;
		}
		//白子连成活一	+1	
		if (comjudge(rt_WHITE, i, j, 2, 1))
		{
			crossing[i][j].score += 1;
		}
		//白子连成死一	+1	
		if (comjudge(rt_WHITE, i, j, 1, 1))
		{
			crossing[i][j].score += 1;
		}
		/*//阻碍黑子连成活一	+1
		if(comjudge(rt_BLACK, i, j, 2, 1))
			{
					crossing[i][j].score += 1;
			}
			*/

			/* //阻碍黑子连成死一	+0
				if()
				{
						crossing[i][j].score += 0;
				}*/
		return 0;
	}
}

/*========================
==函数名：mark
==功能：对各个点进行打分
==参数： 数组下标
==返回值：无
========================*/
int mark(int i, int j)
{
	if (crossing[i][j].rt != 0)
	{
		return 0;
	}

	//黑子连成五子	+10000
	else
	{
		if (comjudges(rt_BLACK, i, j))
		{
			crossing[i][j].score += 10000;
		}
		//阻碍白子连成五子	+1000
		if (comjudges(rt_WHITE, i, j))
		{
			crossing[i][j].score += 1000;
		}
		//黑子连成活四	+200      
		if (comjudge(rt_BLACK, i, j, 2, 4))
		{
			crossing[i][j].score += 200;
		}
		//阻碍白子连成活四	+100
		if (comjudge(rt_WHITE, i, j, 2, 4))
		{
			crossing[i][j].score += 100;
		}
		//黑子连成死四	+50	
		if (comjudge(rt_BLACK, i, j, 1, 4))
		{
			crossing[i][j].score += 50;
		}
		//阻碍白子连成死四	+20
		if (comjudge(rt_WHITE, i, j, 1, 4))
		{
			crossing[i][j].score += 20;
		}
		//黑子连成活三	+30	
		if (comjudge(rt_BLACK, i, j, 2, 3))
		{
			crossing[i][j].score += 30;
		}
		//阻碍白子连成活三	+10
		if (comjudge(rt_WHITE, i, j, 2, 3))
		{
			crossing[i][j].score += 10;
		}
		//黑子连成死三	+8	
		if (comjudge(rt_BLACK, i, j, 1, 3))
		{
			crossing[i][j].score += 8;
		}
		//阻碍白子连成死三	+5
		if (comjudge(rt_WHITE, i, j, 1, 3))
		{
			crossing[i][j].score += 5;
		}
		//黑子连成活二	+2	
		if (comjudge(rt_BLACK, i, j, 2, 2))
		{
			crossing[i][j].score += 2;
		}
		//阻碍黑子连成活二	+1
		if (comjudge(rt_WHITE, i, j, 2, 2))
		{
			crossing[i][j].score += 1;
		}
		//黑子连成死二	+2	
		if (comjudge(rt_BLACK, i, j, 1, 2))
		{
			crossing[i][j].score += 2;
		}
		//阻碍白子连成死二	+1
		if (comjudge(rt_WHITE, i, j, 1, 2))
		{
			crossing[i][j].score += 1;
		}
		//黑子连成活一	+1	
		if (comjudge(rt_BLACK, i, j, 2, 1))
		{
			crossing[i][j].score += 1;
		}
		//黑子连成死一	+1	
		if (comjudge(rt_BLACK, i, j, 1, 1))
		{
			crossing[i][j].score += 1;
		}
		/*//阻碍白子连成活一	+1
		if(comjudge(rt_BLACK, i, j, 2, 1))
			{
					crossing[i][j].score += 1;
			}
			*/

			/* //阻碍白子连成死一	+0
				if()
				{
						crossing[i][j].score += 0;
				}*/
		return 0;
	}
}
/*========================
==函数名：judge
==功能：判断输赢
==参数：  coordinate crossing[][];   //坐标
		int colour;   //颜色
		int a,b	;	   //数组下标
==返回值： 无
========================*/
int judge(int colour)
{
	int i, j;
	int n1 = 0, n2 = 0;  //存放相邻相同棋子个数
	while (1)
	{
		for (i = a, j = b; i >= 0; i--)  //水平向左判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15; i++)  //水平向右判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//判断是否五子
		{
			return 1;
			break;
		}




		n1 = 0;
		n2 = 0;
		for (i = a, j = b; j >= 0; j--)  //向上判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; j<15;j++)		//向下判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//判断是否五子
		{
			return 1;
			break;
		}


		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i >= 0&& j >= 0; i--, j--)  //左上判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15&&j < 15; i++, j++)  //右下判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//判断是否五子
		{
			return 1;
			break;
		}

		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i >= 0&& j >= 0; i--, j--)  //左上判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i < 15&& j < 15; i++, j++)  //右下判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//判断是否五子
		{
			return 1;
			break;
		}

		n1 = 0;
		n2 = 0;
		for (i = a, j = b; i < 15&&j >= 0; i++, j--)  //右上判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n1++;
			else
				break;
		}
		for (i = a, j = b; i >= 0&&j < 15; i--, j++)  //左下判断棋子颜色相同个数
		{
			if (crossing[i][j].rt == colour)
				n2++;
			else
				break;
		}
		if (n1 + n2 - 1 == 5)	//判断是否五子
		{
			return 1;
			break;
		}
		return 0;
	}
}


/*========================
==函数名：draw_chesspvp
==功能：下棋子（玩家对战）
==参数 ：coordinate crossing[][];   //坐标
		 int num;					//步数
		 int a,b	;				//数组下标
==返回值：无
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
==函数名：draw_chessp
==功能：玩家下棋子（人机模式）
==参数  coordinate crossing[][];	//坐标
		int num;					//步数
		int a,b;					//数组下标
==返回值： 无
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
==函数名：draw_chesse
==功能：计算机下棋子（人机模式）
==参数  coordinate crossing[][];	//坐标
		int num;					//步数
		int a,b	;					//数组下标
==返回值： 无
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
==函数名：GameStartpvp
==功能：开始玩家对战
==参数：无
==返回值：无
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
	int pos; //鼠标状态
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
==函数名：GameStartpve
==功能：游戏开始(人机)
==参数：无
==返回值：无
========================*/
void GameStartpve()
{
	int i = 0, j = 0;
	int p, q;
	num = 0;
	paintboard();
	definecrossing();
	int pos; //鼠标状态
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
==函数名：GameStartpves
==功能：游戏开始(人机)
==参数：无
==返回值：无
========================*/
void GameStartpves()
{
	int i = 0, j = 0;
	int p, q;
	num = 1;
	paintboard();
	definecrossing();
	int pos; //鼠标状态
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
==函数名称：begin
==功能：开始界面的操作
==参数：无
==返回值：无
========================*/
void begin()
{
	int x;									//定义一个存放鼠标位置状态值的变量
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//鼠标悬停在按钮1的位置
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//鼠标悬停在按钮2的位置
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("人机模式"));
			outtextxy(230, 400, _T("玩家对战"));	//字体颜色恢复
			break;
		case 1:
			outtextxy(230, 350, _T("人机模式"));	//字体变色
			if (m.mkLButton)
			{
				side();
				return;
			}
			break;
		case 2:
			outtextxy(230, 400, _T("玩家对战"));	//字体变色
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
==函数名称：side
==功能：选择所执棋色
==参数：无
==返回值：无
========================*/
void side()
{
	int x;												//定义一个存放鼠标位置状态值的变量
	loadimage(NULL, _T("board.jpg"), 700, 700, true);
	settextcolor(BLACK);
	settextstyle(60, 40, _T("华文新魏"));
	outtextxy(230, 40, _T("五子棋"));
	settextstyle(45, 30, _T("华文新魏"));
	outtextxy(230, 350, _T("选择黑棋"));
	outtextxy(230, 400, _T("选择白棋"));
	while (1)
	{
		x = 0;
		m = GetMouseMsg();
		if (m.x > 230 && m.x < 470
			&&
			m.y > 350 && m.y < 395)					//鼠标悬停在按钮1的位置
			x = 1;
		else
			if (m.x > 230 && m.x < 470
				&&
				m.y > 400 && m.y < 445)				//鼠标悬停在按钮2的位置
				x = 2;
		settextcolor(WHITE);
		switch (x)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(230, 350, _T("选择黑棋"));
			outtextxy(230, 400, _T("选择白棋"));	//字体颜色恢复
			break;
		case 1:
			outtextxy(230, 350, _T("选择黑棋"));	//字体变色
			if (m.mkLButton)
			{
				GameStartpve();
				return;
			}
			break;
		case 2:
			outtextxy(230, 400, _T("选择白棋"));	//字体变色
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