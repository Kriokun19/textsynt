#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <xstring>
#include <vector>
#include <fstream>
#include <commdlg.h>
//#include <iostream>
#include <string>
using namespace std;

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > String;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // прототип оконной функции
// LRESULT = long, вызывается, когда требуется обработать сообщение
const int n = 18;
const String KeyWord[n] = {_T("for"), _T("#include"), _T("else"), _T("if"), _T("int"), _T("#define"), _T("typedef"), 
   _T("#endif"), _T("long"), _T("char"), _T("const"), _T("return"), _T("while"), _T("static"), _T("unsigned"), _T("switch"), 
   _T("case"), _T("break")};
TCHAR WinName[] = _T("MyWindow"); // название класса окна
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPTSTR Cmd, int mode)
// дескриптор текущего приложения, =0, командная строка, режим отображения окна
{
	HWND hWnd; // дескриптор гл окна
	MSG msg; // сообщение от ОС
	WNDCLASS wc; // класс окна

	// Определение класса окна
	wc.hInstance = hInstance; // дескриптор приложения
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProc; // функция окна
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // стандартная иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // стандартныйы курсор
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0; //нет доп данных класса
	wc.cbWndExtra = 0; //нет доп данных окна
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1); //заполнение окна белым цветом
	
	if (!RegisterClass(&wc)) return 0; //регистрация класса окна
	//Создание окна
	hWnd = CreateWindow(WinName,
		_T("MyFirstWindow"),
		WS_OVERLAPPEDWINDOW, //стиль окна
		CW_USEDEFAULT, // x
		CW_USEDEFAULT, //y координаты
		CW_USEDEFAULT, //width
		CW_USEDEFAULT, //height
		HWND_DESKTOP, //родительское окно
		NULL, //меню
		hInstance, 
		NULL); //доп информация
	ShowWindow(hWnd, mode);

	HMENU hMenu=CreateMenu();
	HMENU hPopMenu=CreatePopupMenu();
	AppendMenu(hPopMenu, MF_STRING, 101, _T("&New file"));
	AppendMenu(hPopMenu, MF_STRING, 102, _T("&Open"));
	AppendMenu(hPopMenu, MF_STRING, 103, _T("&Save"));
	AppendMenu(hPopMenu, MF_STRING, 104, _T("&Exit"));
	AppendMenu(hMenu, MF_POPUP,(UINT) hPopMenu, _T("&File"));
	SetMenu(hWnd, hMenu);

	//цикл обработки сообщений
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg); //ф-ия трансляции кодов нажатой клавиши
		DispatchMessage(&msg); //посылает сообщение функции WndProc()
	}

	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	static OPENFILENAME file;
	wifstream in;
	wofstream out;
	PAINTSTRUCT ps; //инф-ция о перерисовке окна
	HDC hdc; //дескриптор контекста устройства вывода
	TEXTMETRIC tm;
	String symb;
	const String p;
	static vector <String> Text;
	Text.push_back(symb);
	TCHAR part;
	//static String text; //для хранения введенного текста

	static int cxChar, cyChar, cxClient, cyClient;
	static int nCharPerLine, nClientLines;
	static int xCaret = 0, yCaret = 0, count, str=0;
	static TCHAR name[256] = _T("");
	unsigned int curIndex;
	int nLines = 0, thisLine =0;
	int nTailChar;
	//Обработчик сообщений
	int x,y=0,i;
	const long MaxSize = Text[0].max_size();

	//раскраска
	int uk1=0, uk2=0;
	size_t position;
	struct pos{size_t posit;
	           int len;
	           };
	pos pos0;
	int prev=0;
	bool isPainted = true;

	switch(message){
		//case WM_RBUTTONDOWN:
		case WM_CREATE: //перед созданием окна получаем некоторые данные: УВ, шрифт и тд
			file.lStructSize = sizeof(OPENFILENAME);
			//file.hInstance = hInstance;
			file.lpstrFilter = _T("*.txt;*.cpp;*.h");
			file.lpstrFile = name;
			file.nMaxFile = 256;
			file.lpstrInitialDir = _T(".//");
			file.lpstrDefExt = _T("cpp");
			hdc = GetDC(hWnd);
			SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
			GetTextMetrics(hdc, &tm);
			cxChar =tm.tmAveCharWidth;
			cyChar =tm.tmHeight;
			ReleaseDC(hWnd, hdc);
			break;
		case WM_SIZE: //изменение размеров окна, рассчитать новое начало
			cxClient = LOWORD(lParam); //размеры окна
			cyClient = HIWORD(lParam);
			nCharPerLine = max(1, cxClient/cxChar);
			nClientLines = max(1, cyClient/cyChar);
			if (hWnd == GetFocus())
			   SetCaretPos(xCaret*cxChar, yCaret*cyChar);
			break;
		case WM_SETFOCUS:
		// окно получает фокус, создать и показать каретку
		    CreateCaret(hWnd, NULL, 0, cyChar);              
		    SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		    ShowCaret(hWnd);
		    break;
	    case WM_KILLFOCUS:
		// окно теряет фокус, каретку прячем
		    HideCaret(hWnd);
		    DestroyCaret();
		    break;
		case WM_COMMAND:
			switch(wParam){
		case 101: //New
			Text.clear();
			Text.push_back(p);
			SetCaretPos(0,0);
			xCaret=0; yCaret=0;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 102: //Open
			file.Flags = OFN_HIDEREADONLY;
			Text.clear();
			if(!GetOpenFileName(&file)) return 1;
			in.open(name);
			while(in.get(part)){
				if(part == '\r' || part == '\n'){
					x=Text[str].size();
					for (i = 0; i<nCharPerLine-x%nCharPerLine; ++i)
				             Text[str] += ' ';
				}
				else
			      Text[str] += part;
			}
			in.close();
			xCaret=Text[str].size()%nCharPerLine; 
			yCaret=Text[str].size()/nCharPerLine;
			SetCaretPos(xCaret, yCaret);
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case 103: //Save
			file.Flags = OFN_NOTESTFILECREATE;
			if(!GetSaveFileName(&file)) return 1;
			out.open(name);
			for(i=0;i<=str;++i)
				out << Text[i];
			out.close();
			break;
		case 104: //Exit
			 SendMessage(hWnd, WM_DESTROY, 1, 1);
			 break;
			}
		case WM_KEYDOWN: //нажатие клавиши
			nLines = Text[str].size() / nCharPerLine; //кол-во строк
			nTailChar = Text[str].size()%nCharPerLine; //посл строка
			switch(wParam){ //стрелки+delete
				case VK_LEFT:   xCaret = max(xCaret - 1, 0);
						break;
		        case VK_RIGHT:  xCaret = min(xCaret + 1, nCharPerLine - 1);
						if ((yCaret == nLines) && (xCaret > nTailChar))
							xCaret = nTailChar;
						break;
		        case VK_UP:     yCaret = max(yCaret - 1, 0);
						break;
		        case VK_DOWN:   yCaret = min(yCaret + 1, nLines);
						if ((yCaret == nLines) && (xCaret > nTailChar))
							xCaret = nTailChar;
						break;
				case VK_DELETE: //от del
					curIndex = yCaret*nCharPerLine+xCaret; //на каком символе каретка
					if (curIndex < Text[str].size()){
						Text[str].erase(curIndex, 1);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					break;
			}
			SetCaretPos(xCaret*cxChar, yCaret*cyChar);
			break;
		case WM_ERASEBKGND:
			{
				return 1;
			}
        case WM_CHAR: //сообщения от кнопок
			switch(wParam){
		case 0x08: //backspace
			if(xCaret>0){
				--xCaret;
			}
			else{
				if(yCaret > 0){
				xCaret = nCharPerLine;
				--yCaret;
				}
			}
			SendMessage(hWnd, WM_KEYDOWN, VK_DELETE, 1);
			break;
		case '\r': case '\n':
			for (i = 0; i<nCharPerLine-xCaret; ++i)
				Text[str] += ' ';
			xCaret = 0;
			++yCaret;
			SetCaretPos(xCaret*cxChar, yCaret*cyChar); //перемещаем каретку
			InvalidateRect(hWnd,NULL,FALSE);
			break;
		default:
			curIndex = yCaret*nCharPerLine+xCaret;
			if (curIndex == Text[str].size()){
				//тутподлинетроки
				Text[str] += (TCHAR)wParam;
				}
			else{
				symb = (TCHAR)wParam;			 //если не в конце строки
				Text[str].insert(curIndex, symb);
			}
			++xCaret;
			if (xCaret == nCharPerLine) {//каретка вправо
				xCaret = 0;
				++yCaret;
				if (yCaret == nClientLines)
					yCaret--;
			}
			SetCaretPos(xCaret*cxChar, yCaret*cyChar); //перемещаем каретку
			InvalidateRect(hWnd,NULL,FALSE);
		    break;
			}
		case WM_PAINT: //вывод символа
				hdc = BeginPaint(hWnd, &ps);
				RECT rect;
				GetClientRect(hWnd, &rect);
				/*
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmMem = CreateCompatibleBitmap(hdcMem, rect.right - rect.left, rect.bottom - rect.top);
				HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);*/

				SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
				FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

					nLines = Text[str].size() / nCharPerLine; //кол-во строк
					nTailChar = Text[str].size()%nCharPerLine; //посл строка
					for(int h=0; h<=str; ++h){
						for(y=0; y<nLines; ++y){
							if(isPainted == true){
							pos0.posit = Text[h].find(KeyWord[0], uk1);
							if(pos0.posit != string::npos){
									pos0.len = KeyWord[0].size();
									uk2 = pos0.posit+pos0.len+1;
								}
							}
							for(i=1;i<n && isPainted == true;++i){
								position = Text[h].find(KeyWord[i], uk1);
								if(position != string::npos && position < pos0.posit){
									pos0.posit = position;
									pos0.len = KeyWord[i].size();
									uk2 = pos0.posit+pos0.len+1;
								}
							}
							uk1 = uk2;
								if(pos0.posit/nCharPerLine>y || pos0.posit == string::npos){
									SetTextColor(hdc, RGB(0,0,0));
									TextOut(hdc, prev*cxChar, y*cyChar, Text[h].c_str()+y*nCharPerLine+prev, nCharPerLine-prev);
									prev=0;
									isPainted = false;
								}
								else{
									SetTextColor(hdc, RGB(0,0,0));
									TextOut(hdc, prev*cxChar, y*cyChar, Text[h].c_str()+y*nCharPerLine+prev,pos0.posit%nCharPerLine-prev);
									SetTextColor(hdc, RGB(0,0,210));
									TextOut(hdc, (pos0.posit%nCharPerLine)*cxChar, y*cyChar, Text[h].c_str()+pos0.posit, pos0.len);
									prev = (uk1-1)%nCharPerLine;
									--y;
									isPainted = true;
								}
							}
						}
					pos0.posit = 1;
					prev=0;
					uk1 = y*nCharPerLine;
					uk2 = uk1;
					while(pos0.posit != string::npos){
						pos0.posit = Text[str].find(KeyWord[0], uk1);
							if(pos0.posit != string::npos){
									pos0.len = KeyWord[0].size();
									uk2 = pos0.posit+pos0.len+1;
							}
					for(i=1;i<n;++i){
								position = Text[str].find(KeyWord[i], uk1);
								if(position != string::npos && position < pos0.posit){
									pos0.posit = position;
									pos0.len = KeyWord[i].size();
									uk2 = pos0.posit+pos0.len+1;
								}
							}
					uk1 = uk2;
								if(pos0.posit == string::npos){
									SetTextColor(hdc, RGB(0,0,0));
									TextOut(hdc, prev*cxChar, y*cyChar, Text[0].c_str()+y*nCharPerLine+prev, nTailChar-prev);
									prev=0;
								}
								else{
									SetTextColor(hdc, RGB(0,0,0));
									TextOut(hdc, prev*cxChar, y*cyChar, Text[0].c_str()+y*nCharPerLine+prev,pos0.posit%nCharPerLine-prev);
									SetTextColor(hdc, RGB(0,0,200));
									TextOut(hdc, (pos0.posit%nCharPerLine)*cxChar, y*cyChar, Text[0].c_str()+pos0.posit, pos0.len);
									prev = (uk1-1)%nCharPerLine;
								}
					}
							/*TextOut(hdc, 0, y*cyChar, Text[h].c_str()+y*nCharPerLine, nCharPerLine);
					    }
					}
					TextOut(hdc, 0, y*cyChar, Text[str].c_str()+y*nCharPerLine, nTailChar); //c_str указатель на содержимое строки*/
				/*BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, hbmOld);
				DeleteObject(hbmMem);
				DeleteDC(hdcMem);*/
				EndPaint(hWnd, &ps); 
				break;
		case WM_DESTROY: 
			PostQuitMessage(0);
		    break;
	//обработка сообщения по умолчанию
        default: 
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}