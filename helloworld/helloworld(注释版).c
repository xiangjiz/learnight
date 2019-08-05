#include <stdio.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <mgi/mgi.h>                    //头文件都是必须的不出问题不用改。


// #define TRANSPARENT_COLOR		MakeRGBA(0x00,0x80,0xC0,0x00)
// #define TRANSPARENT_COLOR		RGB2Pixel(255, 224, 255)
#define TRANSPARENT_COLOR		MakeRGBA(0x00,0x00,0x00,0x00)   //定义TRANSPARENT_COLOR参数表示透明色


#define IDC_PROG_NAME				0x1001   //大概是端口设置
#define IDC_DATE_TIME				0x1002   
#define IDC_HEART_BEAT			0x1003     
#define IDC_VERSION_INFO		0x1004
#define IDC_IP_INFO					0x1005
#define IDC_NAME_INFO				0x1006
#define IDC_EDT_ADDR                0x1007

#define ID_TIMER1						0x0001
#define ID_TIMER2						0x0002


static CTRLDATA ctrl_main_window[] =    //结构 CTRLDATA 用来定义控件,DLGTEMPLATE 用来定义对话框本身。应该先用CTRLDATA定义对话框中所有控件, 
{                                       //用数组表示控件在该数组中的顺序,也就是对话框中用户按 TAB 键时的控件切换顺序。
    {                                   //然后定义对话框,指定对话框中的控件数目,并指定 DLGTEMPLATE 结构中的 controls 指针 指向定义控件的数组。
		"static",                       //静态框控件，宏定义  CTRL_STATIC                   
		WS_VISIBLE | SS_CENTER | SS_NOPREFIX | SS_NOWORDWRAP,  //创建初始可见的窗口；可显示多行的静态框，中对齐显示文本；
		//	   禁止对字符“&”进行解释，通常字符“&”会被解释成在下一个字符加一个下画线，“&&”会被解释成一个字符“&”，用户可以使用SS_NOPREFIX风格来禁止这项解释；
		// 超过控件宽度部份将被截去，不进行自动换行处理。                                                   
		10, 10, 100, 20,     //(x,y,w,h)
		IDC_PROG_NAME,       //控件的标识符>>显示程序名称
		"DOOR SYSTEM",       //显示内容
		0,                   //参数 add_data 用来向控件传递其特有数据的指针,该指针所指向的数据结构随控件类的不同而不同，0
	},

	// 显示时间
	{
		"static",
		WS_VISIBLE | SS_SIMPLE | SS_NOPREFIX | SS_NOWORDWRAP, // ；用来显示单行文本，永远左对齐不会自动换行；；；
		300, 50, 180, 20,            //（x,y,w,h）
		IDC_DATE_TIME,             //时间
		//"2019-01-01 00:00:00 MON",
		"2019-01-01 00:00:00 星期天",      //初始显示时间
		WS_EX_TRANSPARENT | WS_EX_AUTOSECONDARYDC,  //使窗口透明无法接收操作 | 窗口自动双缓冲刷新
	},

	// 心跳标志
	{
		"static",
		WS_VISIBLE | SS_SIMPLE | SS_NOPREFIX | SS_NOWORDWRAP,
		10, 750, 100, 40,          //(x,y,w,h)
		IDC_HEART_BEAT,
		"OK",                      //初始显示OK
		WS_EX_TRANSPARENT,          //使窗口透明无法接收操作
	},

	// IP地址
	{
		"static",
		WS_VISIBLE | SS_SIMPLE | SS_NOPREFIX | SS_NOWORDWRAP,    
		200, 750, 100, 40,
		IDC_IP_INFO,
		"192.168.1.100",
		0,
	},

	// 版本信息
	{
		"static",
		WS_VISIBLE | SS_SIMPLE | SS_NOPREFIX | SS_NOWORDWRAP,
		350, 750, 100, 40,
		IDC_VERSION_INFO,
		"v1.01.00",
		WS_EX_TRANSPARENT,
	},

	// 姓名信息
	{
		"static",
		WS_VISIBLE | SS_CENTER | SS_NOPREFIX | SS_NOWORDWRAP,
		154, 500, 300, 80,
		IDC_NAME_INFO,
		// 需要使用GB2312编码
		// "\xD0\xD5\xC3\xFB\x31\x32\x33",
		//"Name",
		"陈君",
		0,
	},
	//编辑框
	{
		"sledit",     //控件类型查看mini里的控件png，这条表示单行编辑框，宏定义CTRL_SLEDIT，多行编辑框mledit
		WS_VISIBLE | WS_TABSTOP | WS_BORDER,  //；用户可以通过使用Tab键移动，TAB键移动到下一个有WS_TABSTOP标志的控件；创建一个有边框的窗口
		130, 40, 199, 30,
		IDC_EDT_ADDR,
		"",
		0
	}
};

static DLGTEMPLATE dialog_main_window =   //定义对话框模板dialog_main_window
{
	WS_VISIBLE,
	WS_EX_NONE | WS_EX_AUTOSECONDARYDC,   //窗口无扩展风格 | 窗口自动双缓冲刷新
	0/*656*/, 0, 608, 1080,       //（x,y,w,h）
	"",
	0, 0,
	sizeof(ctrl_main_window)/sizeof(ctrl_main_window[0]), ctrl_main_window,    //应该是和控件一样大小的对话框。
	0
};

static PLOGFONT font_main = NULL;  //用于编辑框的字体，使用自定义的逻辑字体font_main
static PLOGFONT font_tips1 = NULL; //用于编辑框的字体，使用自定义的逻辑字体font_tips


static int OnInitDialog(HWND hWnd)    //定义窗口过程函数static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
//第一个参数 hWnd 是接收消息的窗口的句柄,它与 CreateMainWindow 函数的返回值相同,该值标识了接收该消息的特定窗口。
//第二个参数与 MSG 结构中的 message 域相同,它是一个标识窗口所收到消息的整数值。最后两个参数都是 32 位的消息参数,它提供和消息相关的特定信息。
{
	// 设置主窗口背景色为透明色
	SetWindowBkColor(hWnd, TRANSPARENT_COLOR);     //SetWindowBKColor
	
	// 设置标签背景色
	SetWindowBkColor((HWND)GetDlgItem(hWnd, IDC_PROG_NAME), MakeRGBA(255, 255, 255, 255));  //设置IDC_PROG_NAME标签背景色为黑色
	
	// 设置标签文本颜色
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_NAME_INFO), WE_FGC_WINDOW, MakeRGBA(0, 255, 0, 0));     //SetWindowElementAttr
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_DATE_TIME), WE_FGC_WINDOW, MakeRGBA(255, 0, 0, 0)); //WE_FGC_WINDOW	|窗口前景色
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_HEART_BEAT), WE_FGC_WINDOW, MakeRGBA(255, 0, 0, 0));
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_IP_INFO), WE_FGC_WINDOW, MakeRGBA(0, 0, 255, 0));
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_VERSION_INFO), WE_FGC_WINDOW, MakeRGBA(255, 0, 0, 0));
	SetWindowElementAttr((HWND)GetDlgItem(hWnd, IDC_EDT_ADDR), WE_FGC_WINDOW, MakeRGBA(0, 255, 255, 0));
	
	// 设置标签透明
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_DATE_TIME), WS_EX_TRANSPARENT);  
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_HEART_BEAT), WS_EX_TRANSPARENT);
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_IP_INFO), WS_EX_TRANSPARENT);
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_VERSION_INFO), WS_EX_TRANSPARENT);
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_NAME_INFO), WS_EX_TRANSPARENT);
	SetWindowExStyle((HWND)GetDlgItem(hWnd, IDC_EDT_ADDR), WS_EX_TRANSPARENT);
	
	// 设置字体
	font_main = CreateLogFont("ttf", "song", FONT_CHARSET_UTF8, 
                       FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                       FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                       72,0);  //CreatLogFont创建逻辑字体，宋体、ttf字体，UTF-8编码，不做特殊处理，不做特殊处理，等字体渲染风格。
	if (NULL == font_main)
	{
		printf("CreateLogFont return NULL\n");
	}
	else
	{
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_NAME_INFO), font_main); //姓名信息使用这种字体
	}

	font_tips1 = CreateLogFont("ttf", "song", FONT_CHARSET_UTF8, 
                       FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                       FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                       28,0); //大小为28像素高，旋转0度。
	if (NULL == font_tips1)
	{
		printf("CreateLogFont return NULL\n");
	}
	else
	{
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_PROG_NAME), font_tips1);  //以下这些使用这种字体
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_DATE_TIME), font_tips1);
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_HEART_BEAT), font_tips1);
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_IP_INFO), font_tips1);
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_VERSION_INFO), font_tips1);
		SetWindowFont((HWND)GetDlgItem(hWnd, IDC_EDT_ADDR), font_tips1);
	
	}
	
	// 调整标签位置
	HWND hw;  //获得窗口句柄
	HDC hdc;  //获得设备描述表句柄 
	SIZE sz;  // 
	RECT rc;
	char text[128] = {0};   //初始化128位数组为0
	
	hw = GetDlgItem(hWnd, IDC_PROG_NAME); // GetDlgTtem根据控件标识符获取控件句柄
	hdc = GetDC(hw);   //GetDC获取针对整个窗口的上下文句柄
	GetWindowText(hw, text, sizeof(text));    //GetWindowText获取编辑框中的文本
	GetTextExtent(hdc, text, strlen(text), &sz); //GetTextExtent计算文本的输出高度和宽度
	ReleaseDC(hdc);     //ReleaseDC释放上面三个函数获取的设备上下文
	GetWindowRect(hw, &rc);   //GetWindowRect获取窗口矩形
	MoveWindow(hw, rc.left, rc.top, sz.cx + 10, sz.cy, FALSE);  //MoveWindow移动窗口或者改变窗口大小
	
	hw = GetDlgItem(hWnd, IDC_DATE_TIME);   //就这个模板吧，具体怎么操作实验几次
	hdc = GetDC(hw);
	GetWindowText(hw, text, sizeof(text));
	GetTextExtent(hdc, text, strlen(text), &sz);
	GetWindowRect(hw, &rc);
	ReleaseDC(hdc);
	MoveWindow(hw, rc.right - sz.cx, rc.top, sz.cx, sz.cy, FALSE);
	
	hw = GetDlgItem(hWnd, IDC_HEART_BEAT);
	hdc = GetDC(hw);
	GetWindowText(hw, text, sizeof(text));
	GetTextExtent(hdc, text, strlen(text), &sz);
	GetWindowRect(hw, &rc);
	ReleaseDC(hdc);
	MoveWindow(hw, rc.left, rc.bottom - sz.cy, sz.cx, sz.cy, FALSE);
	
	hw = GetDlgItem(hWnd, IDC_VERSION_INFO);
	hdc = GetDC(hw);
	GetWindowText(hw, text, sizeof(text));
	GetTextExtent(hdc, text, strlen(text), &sz);
	GetWindowRect(hw, &rc);
	ReleaseDC(hdc);
	MoveWindow(hw, rc.right - sz.cx, rc.bottom - sz.cy, sz.cx, sz.cy, FALSE);
	
	int verinfo_left = rc.right - sz.cx;

	hw = GetDlgItem(hWnd, IDC_IP_INFO);
	hdc = GetDC(hw);
	GetWindowText(hw, text, sizeof(text));
	GetTextExtent(hdc, text, strlen(text), &sz);
	GetWindowRect(hw, &rc);
	ReleaseDC(hdc);
	MoveWindow(hw, verinfo_left - 10 - sz.cx, rc.bottom - sz.cy, sz.cx, sz.cy, FALSE);
	
	hw = GetDlgItem(hWnd, IDC_EDT_ADDR);
	hdc = GetDC(hw);
	GetWindowText(hw, text, sizeof(text));
	GetTextExtent(hdc, text, strlen(text), &sz);
	ReleaseDC(hdc);
	GetWindowRect(hw, &rc);
	MoveWindow(hw, rc.left, rc.top, sz.cx + 10, sz.cy, FALSE);
	
	// 设置定时器
	SetTimer(hWnd, ID_TIMER1, 100); //SetTimer创建定时器，指定的窗口会收到MSG_TIMER消息
	SetTimer(hWnd, ID_TIMER2, 1);  //间隔为1个10ms
	
	srand(time(NULL)); //以现在的系统时间作为随机数的种子来产生随机数

	return 0;
}

static int OnCloseDialog(HWND hWnd)   //关闭窗口时返回0
{
	return 0;	
}

static int ShowCurTime(HWND hWnd)  //窗口过程函数，标签颜色
{
	//static const char * const week_name[] = {"SUN", "MON", "TUE", "WED", "FRI", "MON", "SAT"};
	static const char * const week_name[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "M星期五", "星期六"};
	static time_t last_time = 0;
	time_t time_now;
	struct tm tm_now;
	char time_str[36];
	HWND hw;
	HDC hdc;
	RECT rc;
	time(&time_now);
	if (last_time == time_now)
		return 0;
	
	last_time = time_now;
	localtime_r(&time_now, &tm_now);
	
	sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d %s",    //显示时间
		tm_now.tm_year + 1900, tm_now.tm_mon+1, tm_now.tm_mday, 
		tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, week_name[tm_now.tm_wday]);
	
	hw = GetDlgItem(hWnd, IDC_DATE_TIME);  //根据控件标识符获取控件句柄
	GetWindowRect(hw, &rc);
	// ClientToScreen(hw, &rc.left, &rc.top);
	// ClientToScreen(hw, &rc.right, &rc.bottom);
	
	hdc = GetClientDC(hWnd); //获取的设备上下文是针对窗口客戶区
	SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ReleaseDC(hdc);
	
	SetWindowText(hw, time_str);     //设置编辑框中文本内容

#if 1
	hw = GetDlgItem(hWnd, IDC_NAME_INFO);
	GetWindowRect(hw, &rc);
			
	hdc = GetClientDC(hWnd);
	SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ReleaseDC(hdc);
	SetWindowText(hw, "名字陈君");


	hw = GetDlgItem(hWnd, IDC_HEART_BEAT);
	GetWindowRect(hw, &rc);
			
	hdc = GetClientDC(hWnd);
	SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ReleaseDC(hdc);
	SetWindowText(hw, "OK");


	hw = GetDlgItem(hWnd, IDC_IP_INFO);
	GetWindowRect(hw, &rc);
			
	hdc = GetClientDC(hWnd);
	SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ReleaseDC(hdc);
	SetWindowText(hw, "192.168.1.100");
	
	
	hw = GetDlgItem(hWnd, IDC_VERSION_INFO);
	GetWindowRect(hw, &rc);
			
	hdc = GetClientDC(hWnd);
	SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	ReleaseDC(hdc);
	SetWindowText(hw, "v1.01.01");

#endif

	return 0;
}

static RECT g_cur_face = {100, 100, 200, 200};   //设定各个分量的初值赋予g_cur_face
static RECT g_pre_face = {100, 100, 200, 200};

static int dir = 0;
static int dx = 0;
static int dy = 0;


static int PaintCurFacebox(HWND hWnd, HDC hdc)    //定义窗口过程函数PaintCurFacebox
{
	RECT rc = g_pre_face;
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	int lw = 10;	// line width

	SetBrushColor(hdc, DWORD2Pixel(hdc, (TRANSPARENT_COLOR)));  //设置刷的类型，双字节像素

	FillBox(hdc, rc.left, rc.top, w / 3, lw);      //FillBox填充指定矩形颜色
	FillBox(hdc, rc.left, rc.top + lw, lw, h / 3 - lw);

	FillBox(hdc, rc.right - w / 3, rc.top, w / 3, lw);
	FillBox(hdc, rc.right - lw, rc.top + lw, lw, h / 3 - lw);

	FillBox(hdc, rc.left, rc.bottom - h / 3, lw, h / 3 - lw);
	FillBox(hdc, rc.left, rc.bottom - lw, w / 3, lw);

	FillBox(hdc, rc.right - lw, rc.bottom - h / 3, lw, h / 3 - lw);
	FillBox(hdc, rc.right - w / 3, rc.bottom - lw, w / 3, lw);

	rc = g_cur_face;
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;
	lw = 10;	// line width
	SetBrushColor(hdc, dir ? COLOR_darkmagenta : COLOR_cyan);//获取系统RGB颜色：青色

	FillBox(hdc, rc.left, rc.top, w/3, lw);
	FillBox(hdc, rc.left, rc.top + lw, lw, h/3 - lw);

	FillBox(hdc, rc.right - w/3, rc.top, w/3, lw);
	FillBox(hdc, rc.right - lw, rc.top + lw, lw, h/3 - lw);

	FillBox(hdc, rc.left, rc.bottom - h/3, lw, h/3 - lw);
	FillBox(hdc, rc.left, rc.bottom - lw, w/3, lw);

	FillBox(hdc, rc.right - lw, rc.bottom - h/3, lw, h/3 - lw);
	FillBox(hdc, rc.right - w/3, rc.bottom - lw, w/3, lw);

	return 0;
}

static int RandomFacebox()  //让这个框放大缩小一直动
{
	memcpy(&g_pre_face, &g_cur_face, sizeof(RECT));//memcpy内存拷贝

	if (dir)
	{
		dx++;
		dy++;
		
		if (dx > 100)
			dir = 0;
	}
	else
	{
		dx--;
		dy--;
		if (dx <= 0)
			dir = 1;
	}
	
	g_cur_face.left = 150+dx;
	g_cur_face.top = 250+dy;
	g_cur_face.right = 250+2*dx;
	g_cur_face.bottom = 350+2*dy;
	
	return 0;
}

static char* mk_time(char* buff) //静态字符数组缓冲结构体
{
    static const char * const week_name[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    static time_t last_time = 0;
    time_t time_now;
    struct tm tm_now;
    time(&time_now);
    if (last_time == time_now)
        return 0;
    last_time = time_now;
    localtime_r(&time_now, &tm_now);
//                
    sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d %s",
        tm_now.tm_year + 1900, tm_now.tm_mon+1, tm_now.tm_mday, 
        tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, week_name[tm_now.tm_wday]);
    return buff;
}




static LRESULT MainWindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//定义一个长整型的主窗口回调函数
{
	HDC hdc;
    HWND hw;
    static RECT rc_test;
    static status = 0;
    char time_str[50];
	switch (message) {       //选择语句，碰上什么消息执行什么动作
		case MSG_CREATE:   //消息在窗口成功创建并添加到 MiniGUI 的窗口管理器之后发送到窗口过程
			break;         

		case MSG_CLOSE:      //当用戶点击窗口上的 “ 关闭 ” 按钮时 , MiniGUI 向窗口过程发 送 MSG_CLOSE 消息
			OnCloseDialog(hWnd);
			DestroyMainWindow (hWnd);   //销毁主窗口
			PostQuitMessage (hWnd);//在消息队列中投入一个MSG_QUIT消息，当GetMessage函数取出 MSG_QUIT 消息时将返回0,最终导致程序退出消息循环。
			return 0;

		case MSG_ERASEBKGND:     //当系统需要清除窗口背景时 , 将发送该消息到窗口过程
			hdc = GetClientDC(hWnd);
			//SetMemDCColorKey(HDC_SCREEN, MEMDC_FLAG_SRCALPHA, TRANSPARENT_COLOR);
			SetMemDCColorKey(HDC_SCREEN, MEMDC_FLAG_SRCCOLORKEY, TRANSPARENT_COLOR);
			ReleaseDC(hdc);
			return 0;
                
		case MSG_INITDIALOG:   //消息是在 MiniGUI 根据对话框模板建立对话框以及控件之后 , 发送到对话框回调函数的
			OnInitDialog(hWnd);
			return 0;
			
		case MSG_TIMER:    //创建定时器后触发的消息
			if (wParam == ID_TIMER1)
			{
			    status = 1;
                hw = GetDlgItem(hWnd, IDC_DATE_TIME);
                GetWindowRect(hw, &rc_test);
                InvalidateRect(hWnd, &rc_test, FALSE);    //InvalidateRect 函数来更新窗口的无效区域 , 这将产生一个 MSG_PAINT 消息
                SetDlgItemText(hWnd, IDC_DATE_TIME,mk_time(time_str)); //SetDlgItemText设置子控件文本
				//ShowCurTime(hWnd);
			}
			else if (wParam == ID_TIMER2)
			{
			    status = 0;
				//RandomFacebox();
				InvalidateRect(hWnd, NULL, FALSE); //InvalidateRect 函数来更新窗口的无效区域 , 这将产生一个 MSG_PAINT 消息
			}
			break;

		case MSG_PAINT:  //该消息在需要进行窗口重绘时发送到窗口过程
			hdc = BeginPaint (hWnd); 
            if(status == 0)
            {
                //printf("eason.chen 500\n");
                PaintCurFacebox(hWnd, hdc);
            }
            else
            {
                SetBrushColor(hdc, DWORD2Pixel(hdc, TRANSPARENT_COLOR));
                FillBox(hdc, rc_test.left, rc_test.top, rc_test.right - rc_test.left, rc_test.bottom - rc_test.top);
                //SetDlgItemText(hWnd, IDC_DATE_TIME,time_str);
            
            }


			EndPaint (hWnd, hdc);
			return 0;
	}

	return DefaultMainWinProc (hWnd, message, wParam, lParam); //主窗口过程通常调用此函数来完成消息的默认处理工作并返回该函数的返回值
}


int MiniGUIMain (int argc, const char* argv[])    //设置程序入口点
{
        MSG Msg;
        HWND hMainWnd;
        MAINWINCREATE CreateInfo;//CreateMainWindow函数创建一个主窗口,参数是一个指向MAINWINCREATE结构的指针CreateInfo,返回值为主窗口的句柄。


        #ifdef _MGRM_PROCESSES   //进程模式：每个程序是单独的进程,每个进程可建立多个窗口，实现了多进程窗口系统适用完整UNIX特性的嵌入式系统。
        JoinLayer (NAME_DEF_LAYER , "helloworld" , 0 , 0);//MiniGUI-Processes模式下加入层（客户端）
        
        // 初始化矢量字体功能
        if (!InitVectorialFonts())  //如果没调用InitVectorialFonts函数构建矢量字体
        {
        	printf("InitVectorialFonts failed!\n");
        }
        #endif

				hMainWnd = CreateMainWindowIndirect(&dialog_main_window, HWND_DESKTOP, MainWindowProc); //利用对话框模板建立普通的主窗口
				//建立一个在桌面上的，以MainWindowProc为消息处理函数的对话框主窗口
        if (hMainWnd == HWND_INVALID)    //如果窗口无效
        {
        	printf("CreateMainWindowIndirect return HWND_INVALID\n");
        	return -1;
        }
				
        while (GetMessage (&Msg, hMainWnd)) {     //调用从应用程序的消息队列中取出一个消息
                TranslateMessage (&Msg);    //把击键消息转换为 MSG_CHAR 消息 , 然后直接发送到窗口过程函数
                DispatchMessage (&Msg);  //最终把消息发往消息的目标窗口的窗口过程
        }

        MainWindowThreadCleanup (hMainWnd); //清除主窗口所使用的消息队列等系统资源并最终由 MiniGUIMain 返回

#ifndef _MGRM_THREADS         //如果是线程模式
        TermVectorialFonts();  //使用矢量字体结束后调用TermVectorialFonts
#endif

        return 0;
}

#ifndef _MGRM_PROCESSES   //如果使用进程模式
#include <minigui/dti.c>    //编译头文件<minigui/dti.c>
#endif
