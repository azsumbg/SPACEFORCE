#include "framework.h"
#include "SPACEFORCE.h"
#include "resource.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "airfhelper.h"
#include"D2BMPLOADER.h"
#include "ErrH.h"
#include "fcheck.h"
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"d2d1.lib")
#pragma comment (lib,"dwrite.lib")
#pragma comment (lib,"airfhelper.lib")
#pragma comment (lib,"d2bmploader.lib")
#pragma comment (lib,"errh.lib")
#pragma comment (lib,"fcheck.lib")

#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"
#define help_file L".\\res\\data\\help.dat"
#define sound_file L".\\res\\snd\\main.wav"

#define bWinClassName L"SpaceForce1"
#define mNew 1001
#define mSpeed 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define record 2001
#define first_record 2002
#define no_record 2003

//////////////////////////////////////////

WNDCLASS bWin;
HINSTANCE bIns = nullptr;
HWND bHwnd = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
MSG bMsg;
BOOL bRet = 0;
UINT bTimer = 0;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint;

float cl_width = 0;
float cl_height = 0;
D2D1_RECT_F b1Rect = { 0, 0, 200.0f,50.0f };
D2D1_RECT_F b2Rect = { 300.0f,0,500.0f,50.0f };
D2D1_RECT_F b3Rect = { 600.0f,0,800.0f,50.0f };
POINT cur_pos = { 0,0 };

bool pause = false; 
bool sound = true;
bool in_client = true;
bool show_help = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool name_set = false;

int lifes = 100;
int seconds = 0;
int minutes = 0;
int score = 0;
float game_speed = 1.0f;
wchar_t current_player[16] = L"A PILOT";

dirs objects_dir = dirs::stop;

///////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmTextFormat = nullptr;
IDWriteTextFormat* bigTextFormat = nullptr;

ID2D1RadialGradientBrush* ButBckgBrush = nullptr;
ID2D1SolidColorBrush* ButBrush = nullptr;
ID2D1SolidColorBrush* ButHgltBrush = nullptr;
ID2D1SolidColorBrush* ButInactiveBrush = nullptr;

ID2D1SolidColorBrush* FieldBrush = nullptr;
ID2D1SolidColorBrush* StarBrush = nullptr;
ID2D1SolidColorBrush* TextBrush = nullptr;

ID2D1Bitmap* bmpBigAsteroid = nullptr;
ID2D1Bitmap* bmpMidAsteroid = nullptr;
ID2D1Bitmap* bmpSmallAsteroid = nullptr;
ID2D1Bitmap* bmpMeteorL = nullptr;
ID2D1Bitmap* bmpMeteorR = nullptr;
ID2D1Bitmap* bmpLaserH = nullptr;
ID2D1Bitmap* bmpLaserV = nullptr;
ID2D1Bitmap* bmpStrongLaserH = nullptr;
ID2D1Bitmap* bmpStrongLaserV = nullptr;
ID2D1Bitmap* bmpNebula1 = nullptr;
ID2D1Bitmap* bmpNebula2 = nullptr;
ID2D1Bitmap* bmpSpare = nullptr;
ID2D1Bitmap* bmpStation = nullptr;

ID2D1Bitmap* bmpExplosion[24] = { nullptr };
ID2D1Bitmap* bmpShipL[2] = { nullptr };
ID2D1Bitmap* bmpShipR[2] = { nullptr };
ID2D1Bitmap* bmpShipU[2] = { nullptr };
ID2D1Bitmap* bmpShipD[2] = { nullptr };

///////////////////////////////////////////////

std::vector<obj_ptr> vStars;

///////////////////////////////////////////////
template<typename COM> void GarbageCollector(COM** object)
{
    if ((*object))
    {
        (*object)->Release();
        object = nullptr;
    }
}
void SafeRelease()
{
    if (iFactory)GarbageCollector(&iFactory);
    if (iWriteFactory)GarbageCollector(&iWriteFactory);
    if (Draw)GarbageCollector(&Draw);

    if (nrmTextFormat)GarbageCollector(&nrmTextFormat);
    if (bigTextFormat)GarbageCollector(&bigTextFormat);

    if (ButBckgBrush)GarbageCollector(&ButBckgBrush);
    if (ButBrush)GarbageCollector(&ButBrush);
    if (ButHgltBrush)GarbageCollector(&ButHgltBrush);
    if (ButInactiveBrush)GarbageCollector(&ButInactiveBrush);

    if (FieldBrush)GarbageCollector(&FieldBrush);
    if (StarBrush)GarbageCollector(&StarBrush);
    if (TextBrush)GarbageCollector(&TextBrush);

    if (bmpBigAsteroid)GarbageCollector(&bmpBigAsteroid);
    if (bmpMidAsteroid)GarbageCollector(&bmpMidAsteroid);
    if (bmpSmallAsteroid)GarbageCollector(&bmpSmallAsteroid);

    if (bmpMeteorL)GarbageCollector(&bmpMeteorL);
    if (bmpMeteorR)GarbageCollector(&bmpMeteorR);

    if (bmpLaserH)GarbageCollector(&bmpLaserH);
    if (bmpLaserV)GarbageCollector(&bmpLaserV);
    if (bmpStrongLaserH)GarbageCollector(&bmpStrongLaserH);
    if (bmpStrongLaserV)GarbageCollector(&bmpStrongLaserV);

    if (bmpNebula1)GarbageCollector(&bmpNebula1);
    if (bmpNebula2)GarbageCollector(&bmpNebula2);
    if (bmpSpare)GarbageCollector(&bmpSpare);
    if (bmpStation)GarbageCollector(&bmpStation);

    for (int i = 0; i < 24; i++)
        if (bmpExplosion[i])GarbageCollector(&bmpExplosion[i]);

    for (int i = 0; i < 2; i++)
        if (bmpShipL[i])GarbageCollector(&bmpShipL[i]);

    for (int i = 0; i < 2; i++)
        if (bmpShipR[i])GarbageCollector(&bmpShipR[i]);

    for (int i = 0; i < 2; i++)
        if (bmpShipU[i])GarbageCollector(&bmpShipU[i]);

    for (int i = 0; i < 2; i++)
        if (bmpShipD[i])GarbageCollector(&bmpShipD[i]);

}
void ErrExit(int _which_error, LPCWSTR err_log = L"\0")
{
    if (_which_error == eD2D)
    {
        std::wofstream log(L".\\res\\data\\error.log", std::ios::app);
        log << err_log << L", " << std::chrono::system_clock::now() << std::endl;
        log.close();
    }
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(_which_error), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    std::remove(tmp_file);
    SafeRelease();
    exit(1);
}
void Init2D()
{
    HRESULT hr = S_OK;
    D2D1_GRADIENT_STOP gStops[2] = { 0 };
    ID2D1GradientStopCollection* gStopCollection = nullptr;
    
    gStops[0].position = 0;
    gStops[0].color = D2D1::ColorF(D2D1::ColorF::Aqua);
    gStops[1].position = 1.0f;
    gStops[1].color = D2D1::ColorF(D2D1::ColorF::LightBlue);
    
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 FACTORY !");
    
    if (iFactory && bHwnd)
        hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT)(cl_width), (UINT)(cl_height))), &Draw);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Render Target !");

    if (Draw)
        hr = Draw->CreateGradientStopCollection(gStops, 2, &gStopCollection);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 GradientStop Collection !");
    
    if (Draw)
        Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(cl_width / 2, 25.0f),
            D2D1::Point2F(0, 0), cl_width / 2, 25.0f), gStopCollection, &ButBckgBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 RadialGradientBrush - buttons background !");

    GarbageCollector(&gStopCollection);

    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::IndianRed), &ButBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Button Brush !");

    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &ButHgltBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Button HGLT Brush !");

    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &ButInactiveBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Button Inactive Brush !");
    
    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &FieldBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Field Brush !");

    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::AntiqueWhite), &StarBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Star Brush !");

    if (Draw)
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Azure), &TextBrush);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Text Brush !");
    
    // WRITE FACTORY *******************************

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 Write Factory !");

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"", &nrmTextFormat);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 nrmTextFormat !");

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 64.0f, L"", &bigTextFormat);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 bigTextFormat !");

    //BITMAPS **********************************

    bmpBigAsteroid = Load(L".\\res\\img\\bigasteroid.png", Draw);
    if (!bmpBigAsteroid)ErrExit(eD2D, L"Error loading BigAsteroid !");

    bmpMidAsteroid = Load(L".\\res\\img\\midasteroid.png", Draw);
    if (!bmpMidAsteroid)ErrExit(eD2D, L"Error loading MidAsteroid !");

    bmpSmallAsteroid = Load(L".\\res\\img\\smallasteroid.png", Draw);
    if (!bmpSmallAsteroid)ErrExit(eD2D, L"Error loading SmallAsteroid !");

    bmpMeteorL = Load(L".\\res\\img\\meteorl.png", Draw);
    if (!bmpMeteorL)ErrExit(eD2D, L"Error loading MeteorL !");

    bmpMeteorR = Load(L".\\res\\img\\meteorr.png", Draw);
    if (!bmpMeteorR)ErrExit(eD2D, L"Error loading MeteorR !");

    bmpLaserH = Load(L".\\res\\img\\hlaser.png", Draw);
    if (!bmpLaserH)ErrExit(eD2D, L"Error loading LaserH !");

    bmpLaserV = Load(L".\\res\\img\\vlaser.png", Draw);
    if (!bmpLaserV)ErrExit(eD2D, L"Error loading LaserV !");

    bmpStrongLaserH = Load(L".\\res\\img\\hstrong_laser.png", Draw);
    if (!bmpStrongLaserH)ErrExit(eD2D, L"Error loading StrongLaserH !");

    bmpStrongLaserV = Load(L".\\res\\img\\vstrong_laser.png", Draw);
    if (!bmpStrongLaserV)ErrExit(eD2D, L"Error loading StrongLaserV !");

    bmpStation = Load(L".\\res\\img\\station.png", Draw);
    if (!bmpStation)ErrExit(eD2D, L"Error loading Station !");

    bmpSpare = Load(L".\\res\\img\\spare.png", Draw);
    if (!bmpSpare)ErrExit(eD2D, L"Error loading Spare !");

    bmpNebula1 = Load(L".\\res\\img\\nebula1.png", Draw);
    if (!bmpNebula1)ErrExit(eD2D, L"Error loading Nebula1 !");

    bmpNebula2 = Load(L".\\res\\img\\nebula2.png", Draw);
    if (!bmpNebula2)ErrExit(eD2D, L"Error loading Nebula2 !");

    for (int i = 0; i < 24; i++)
    {
        wchar_t path[75] = L".\\res\\img\\explosion\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(path, add);
        wcscat_s(path, L".png");

        bmpExplosion[i] = Load(path, Draw);
        if (!bmpExplosion[i])ErrExit(eD2D, L"Error loading Explosion !");
    }

    for (int i = 0; i < 2; ++i)
    {
        wchar_t path[75] = L".\\res\\img\\ship\\down\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(path, add);
        wcscat_s(path, L".png");

        bmpShipD[i] = Load(path, Draw);
        if (!bmpShipD[i])ErrExit(eD2D, L"Error loading Ship - down aspect !");

    }

    for (int i = 0; i < 2; ++i)
    {
        wchar_t path[75] = L".\\res\\img\\ship\\up\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(path, add);
        wcscat_s(path, L".png");

        bmpShipU[i] = Load(path, Draw);
        if (!bmpShipU[i])ErrExit(eD2D, L"Error loading Ship - up aspect !");

    }

    for (int i = 0; i < 2; ++i)
    {
        wchar_t path[75] = L".\\res\\img\\ship\\left\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(path, add);
        wcscat_s(path, L".png");

        bmpShipL[i] = Load(path, Draw);
        if (!bmpShipL[i])ErrExit(eD2D, L"Error loading Ship - left aspect !");

    }

    for (int i = 0; i < 2; ++i)
    {
        wchar_t path[75] = L".\\res\\img\\ship\\right\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(path, add);
        wcscat_s(path, L".png");
        bmpShipR[i] = Load(path, Draw);
        if (!bmpShipR[i])ErrExit(eD2D, L"Error loading Ship - right aspect !");
    }

    // FIRST SCREEN *****************************

    wchar_t back_text[30] = L"ЗВЕЗДЕН ПАТРУЛ !\n\ndev. Daniel";
    wchar_t front_text[30] = L"\0";
    
    if (Draw && bigTextFormat)
    {
        for (int i = 0; i < 30; i++)
        {
            mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
            front_text[i] = back_text[i];
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::Black));
            Draw->DrawText(front_text, i, bigTextFormat, D2D1::RectF(100.0f, 200.0f, cl_width, cl_height), TextBrush);
            Draw->EndDraw();
            Sleep(40);
        }
        Sleep(2500);
    }
}
void InitGame()
{
    wcscpy_s(current_player, L"A PILOT");
    name_set = false;
    game_speed = 1.0f;

    score = 0;
    lifes = 100;
    seconds = 0;
    minutes = 0;

    vStars.clear();


    for (float x_counter = frame_min_width; x_counter <= frame_max_width; x_counter += 30.0f + rand() % 50)
    {
        for (float y_counter = frame_min_height; y_counter <= 650.0f; y_counter += 30.0f + rand() % 50)
        {
            if (rand() % 2 == 0)
                vStars.push_back(iObjectFactory(types::small_star, x_counter, y_counter));
            else
                vStars.push_back(iObjectFactory(types::big_star, x_counter, y_counter));

        }
    }
}

void GameOver()
{
    pause = true;
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

////////////////////////////////////////////////

INT_PTR CALLBACK bDlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM(mainIcon)));
        return true;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
        {
            int size = GetDlgItemText(hwnd, IDC_NAME, current_player, 15);
            if (size < 1)
            {
                if (sound)MessageBeep(MB_ICONASTERISK);
                MessageBox(bHwnd, L"Името си ли забрави ?", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONQUESTION);
                wcscpy_s(current_player, L"A PILOT");
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }

        }
        break;

    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK bWinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        {
            RECT clr = { 0 };
            GetClientRect(hwnd, &clr);
            cl_width = (float)(clr.right);
            cl_height = (float)(clr.bottom);
            SetTimer(hwnd, bTimer, 1000, NULL);
            srand((unsigned int)(time(0)));

            bBar = CreateMenu();
            bMain = CreateMenu();
            bStore = CreateMenu();

            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

            AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
            AppendMenu(bMain, MF_STRING, mSpeed, L"Следващо ниво");
            AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bMain, MF_STRING, mExit, L"Изход");

            AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
            AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
            AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

            SetMenu(hwnd, bBar);
            InitGame();
        }
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONQUESTION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !", L"Наистина ли излизаш ?", 
            MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_TIMER:
        if (pause)break;
        seconds++;
        if (seconds > 59)
        {
            seconds = 0;
            minutes++;
        }
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(80, 80, 80)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                    }
                }
                else
                {
                    if (b1Hglt && sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                }

                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                    }
                }
                else
                {
                    if (b2Hglt && sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b2Hglt = false;
                }

                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                    }
                }
                else
                {
                    if (b3Hglt && sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b3Hglt = false;
                }

                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                }
                if (b2Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b2Hglt = false;
                }
                if (b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b3Hglt = false;
                }
            }
            
            SetCursor(mainCursor);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
            }
            if (b2Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b2Hglt = false;
            }
            if (b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b3Hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case mNew:
                pause = true;
                if (sound)MessageBeep(MB_ICONQUESTION);
                if (MessageBox(hwnd, L"Ако рестартираш, ще загубиш тази игра !", L"Наистина ли рестартираш ?",
                    MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
                {
                    pause = false;
                    break;
                }
                InitGame();
                pause = false;
                break;

            case mSpeed:
                pause = true;
                if (sound)MessageBeep(MB_ICONQUESTION);
                if (MessageBox(hwnd, L"Ако продължиш, ще стане по-трудно !", L"Трудност !",
                    MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
                {
                    pause = false;
                    break;
                }
                game_speed++;
                pause = false;
                break;

            case mExit:
                SendMessage(hwnd, WM_CLOSE, NULL, NULL);
                break;




            }
            break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    PlaySound(NULL, NULL, NULL);

    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    
    std::wofstream tmp(Ltmp_file);
    tmp << L"Игрътъ ръботи бря !";
    tmp.close();

    if (GetSystemMetrics(SM_CXSCREEN) + 100 < scr_width || GetSystemMetrics(SM_CYSCREEN) + 50 < scr_height)ErrExit(eScreen);

    mainIcon = (HICON)LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFile(L".\\res\\bcursor.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCursor || !outCursor)ErrExit(eCursor);

    ZeroMemory(&bWin, sizeof(WNDCLASS));

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &bWinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(80, 80, 80));
    bWin.hIcon = mainIcon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"Звезден патрул !", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 50, (int)(scr_width), 
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);
        Init2D();
    }

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            if (Draw && bigTextFormat && TextBrush)
            {
                Draw->BeginDraw();
                Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));
                Draw->DrawText(L"ПАУЗА", 6, bigTextFormat, D2D1::RectF(350.0f, 300.0f, cl_width, cl_height), TextBrush);
                Draw->EndDraw();
                continue;
            }
        }







        // DRAW THINGS **************************

        if (Draw && ButBckgBrush && ButBrush && ButHgltBrush && ButInactiveBrush)
        {
            Draw->BeginDraw();
            Draw->FillRectangle(D2D1::RectF(0, 0, cl_width, 50.0f), ButBckgBrush);
            Draw->FillRectangle(D2D1::RectF(0, 50.0f, cl_width, cl_height), FieldBrush);
            if (name_set)
                Draw->DrawText(L"Име на пилот", 13, nrmTextFormat, D2D1::RectF(b1Rect.left + 50.0f, 0, b1Rect.right, 50.0f), 
                    ButInactiveBrush);
            else
            {
                if(b1Hglt)
                    Draw->DrawText(L"Име на пилот", 13, nrmTextFormat, D2D1::RectF(b1Rect.left + 50.0f, 0, b1Rect.right, 50.0f),
                        ButHgltBrush);
                else
                    Draw->DrawText(L"Име на пилот", 13, nrmTextFormat, D2D1::RectF(b1Rect.left + 50.0f, 0, b1Rect.right, 50.0f),
                        ButBrush);
            }
            if (b2Hglt)
                Draw->DrawText(L"Звуци ON / OFF", 15, nrmTextFormat, D2D1::RectF(b2Rect.left + 50.0f, 0, b2Rect.right, 50.0f),
                    ButHgltBrush);
            else
                Draw->DrawText(L"Звуци ON / OFF", 15, nrmTextFormat, D2D1::RectF(b2Rect.left + 50.0f, 0, b2Rect.right, 50.0f),
                    ButBrush);
            if (b3Hglt)
                Draw->DrawText(L"Помощ за играта", 16, nrmTextFormat, D2D1::RectF(b3Rect.left + 30.0f, 0, b3Rect.right, 50.0f),
                    ButHgltBrush);
            else
                Draw->DrawText(L"Помощ за играта", 16, nrmTextFormat, D2D1::RectF(b3Rect.left + 30.0f, 0, b3Rect.right, 50.0f),
                    ButBrush);
        }

        if (!vStars.empty())
        {
            for (int i = 0; i < vStars.size(); i++)
                if ((vStars[i]->ex >= 0 || vStars[i]->x <= cl_width) && vStars[i]->ey > 50.0f)
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->ex - 25.0f, vStars[i]->ey - 25.0f), 
                        (vStars[i]->ex - vStars[i]->x) / 4, 
                        (vStars[i]->ey - vStars[i]->y) / 4), StarBrush);
        }


        Draw->EndDraw();
    }


    std::remove(tmp_file);
    SafeRelease();
    return (int) bMsg.wParam;
}