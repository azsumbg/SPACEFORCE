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
bool laser_upgraded = false;

int seconds = 0;
int minutes = 0;
int score = 0;
float game_speed = 1.0f;
wchar_t current_player[16] = L"A PILOT";
int spare_life = 15;

dirs objects_dir = dirs::stop;

///////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmTextFormat = nullptr;
IDWriteTextFormat* bigTextFormat = nullptr;

ID2D1RadialGradientBrush* ButBckgBrush = nullptr;
ID2D1LinearGradientBrush* SpareLifeBrush = nullptr;
ID2D1SolidColorBrush* ButBrush = nullptr;
ID2D1SolidColorBrush* ButHgltBrush = nullptr;
ID2D1SolidColorBrush* ButInactiveBrush = nullptr;

ID2D1SolidColorBrush* FieldBrush = nullptr;
ID2D1SolidColorBrush* StarBrush = nullptr;
ID2D1SolidColorBrush* BigStarBrush = nullptr;
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

D2D1_GRADIENT_STOP FieldStops[2] = { 0 };
ID2D1GradientStopCollection* FieldCollection = nullptr;
ID2D1RadialGradientBrush* ShipForceBrush = nullptr;

///////////////////////////////////////////////

prime_ptr neb1 = nullptr;
prime_ptr neb2 = nullptr;
obj_ptr Ship = nullptr;
obj_ptr Spare = nullptr;
obj_ptr Station = nullptr;

std::vector<obj_ptr> vStars;
std::vector<obj_ptr> vMeteors;
std::vector<obj_ptr> vLasers;


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
    if (BigStarBrush)GarbageCollector(&StarBrush);
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

    if (FieldCollection)GarbageCollector(&FieldCollection);
    if (ShipForceBrush)GarbageCollector(&ShipForceBrush);

    GarbageCollector(&SpareLifeBrush);
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

    gStops[0].position = 0;
    gStops[0].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);
    gStops[1].position = 1.0f;
    gStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkCyan);
    
    if (Draw)
        hr = Draw->CreateGradientStopCollection(gStops, 2, &gStopCollection);
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 GradientStop Collection ! ");

    if (Draw)
        hr = Draw->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(cl_width / 2 + 150.0f, 80.0f),
            D2D1::Point2F(cl_width / 2 + 250.0f, 100.0f)), gStopCollection, &SpareLifeBrush);
    
    if (hr != S_OK)ErrExit(eD2D, L"Error creating D2D1 LinearGradientBrush - life spare brush !");

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
        Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod), &BigStarBrush);
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
    laser_upgraded = false;

    score = 0;
    seconds = 0;
    minutes = 0;
    spare_life = 15;

    if (neb1)neb1->Release();
    if (neb2)neb2->Release();
    if (Ship)Ship->Release();
    if (Spare)Spare->Release();
    if (Station)Station->Release();

    Spare = nullptr;
    Station = nullptr;

    neb1 = iPrimeFactory(types::nebula1, (float)(rand() % 600), 80.0f);
    neb2 = iPrimeFactory(types::nebula2, (float)(rand() % 600), 450.0f);
    
    vStars.clear();
    vMeteors.clear();
    vLasers.clear();

    for (float x_counter = frame_min_width; x_counter < frame_max_width; x_counter += (30.0f + rand() % 60))
    {
        for (float y_counter = frame_min_height; y_counter < frame_max_height; y_counter += (30.0f + rand() % 70))
        {
            int acase = rand() % 4;
            
            if (acase == 0)
                vStars.push_back(iObjectFactory(types::small_star, x_counter, y_counter));
            else if (acase == 1)
                vStars.push_back(iObjectFactory(types::big_star, x_counter, y_counter));
            else continue;
        }
    }

    Ship = iObjectFactory(types::ship, cl_width / 2, cl_height - 100.0f);
    Ship->lifes = 120;

    for (int i = 0; i < 7; i++)
    {
        float tx = static_cast<float>(rand() % 800);
        float ty = static_cast<float>(rand() % 400 + 50);
        int ttype = rand() % 4;

        switch (ttype)
        {
        case 0:
            vMeteors.push_back(iObjectFactory(types::big_asteroid, tx, ty));
            break;

        case 1:
            vMeteors.push_back(iObjectFactory(types::mid_asteroid, tx, ty));
            break;

        case 2:
            vMeteors.push_back(iObjectFactory(types::small_asteroid, tx, ty));
            break;

        case 3:
            vMeteors.push_back(iObjectFactory(types::meteor, tx, ty));
            break;

        }

        if (tx > cl_width) vMeteors.back()->dir = dirs::right;
        else vMeteors.back()->dir = dirs::left;
        
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
        if (Spare || laser_upgraded)
        {
            spare_life--;
            if (spare_life <= 0)
            {
                laser_upgraded = false;
                Spare->Release();
                Spare = nullptr;
            }
        }
        if (minutes >= 1 && !Station)
        {
            Station = iObjectFactory(types::station, static_cast<float>(rand() % 600), 60.0f);
            Station->lifes = 250;
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

        case WM_LBUTTONDOWN:
            if (LOWORD(lParam) >= b1Rect.left && LOWORD(lParam) <= b1Rect.right
                && HIWORD(lParam) >= b1Rect.top && HIWORD(lParam) <= b1Rect.bottom)
            {
                if (name_set)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    break;
                }
                else
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                    if (DialogBoxW(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &bDlgProc) == IDOK)name_set = true;
                    break;
                }
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

        case WM_KEYDOWN:
            switch (LOWORD(wParam))
            {
            case VK_RIGHT:
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                if (Ship)Ship->dir = dirs::right;
                break;

            case VK_LEFT:
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                if (Ship)Ship->dir = dirs::left;
                break;

            case VK_UP:
                if (Ship)Ship->dir = dirs::up;
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                break;

            case VK_DOWN:
                if (Ship)Ship->dir = dirs::down;
                if (sound)mciSendString(L"play .\\res\\snd\\engine.wav", NULL, NULL, NULL);
                break;

            case VK_CONTROL:
                if (Ship)Ship->dir = dirs::stop;
                if (sound)mciSendString(L"play .\\res\\snd\\stop.wav", NULL, NULL, NULL);
                break;

            case VK_SHIFT:
                if (Ship)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\laser.wav", NULL, NULL, NULL);
                    switch (Ship->dir)
                    {
                    case dirs::up:
                        if (!laser_upgraded)vLasers.push_back(iObjectFactory(types::vlaser, Ship->x + 43.0f, Ship->y));
                        else vLasers.push_back(iObjectFactory(types::vstrong_laser, Ship->x + 43.0f, Ship->y));
                        if (!vLasers.empty())vLasers.back()->dir = dirs::up;
                        break;

                    case dirs::down:
                        if (!laser_upgraded)vLasers.push_back(iObjectFactory(types::vlaser, Ship->x + 43.0f, Ship->ey));
                        else vLasers.push_back(iObjectFactory(types::vstrong_laser, Ship->x + 43.0f, Ship->ey));
                        if (!vLasers.empty())vLasers.back()->dir = dirs::down;
                        break;

                    case dirs::left:
                        if (!laser_upgraded)vLasers.push_back(iObjectFactory(types::hlaser, Ship->x, Ship->y + 50.0f));
                        else vLasers.push_back(iObjectFactory(types::hstrong_laser, Ship->x, Ship->y + 50.0f));
                        if (!vLasers.empty())vLasers.back()->dir = dirs::left;
                        break;

                    case dirs::right:
                        if (!laser_upgraded)vLasers.push_back(iObjectFactory(types::hlaser, Ship->ex, Ship->y + 50.0f));
                        else vLasers.push_back(iObjectFactory(types::hstrong_laser, Ship->ex, Ship->y + 50.0f));
                        if (!vLasers.empty())vLasers.back()->dir = dirs::right;
                        break;
                    }
                }
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
        //////////////////////////////////////////

        if (Ship)
        {
            Ship->Move(game_speed);
            switch (Ship->dir)
            {
            case dirs::up:
                objects_dir = dirs::down;
                break;

            case dirs::down:
                objects_dir = dirs::up;
                break;

            case dirs::left:
                objects_dir = dirs::right;
                break;

            case dirs::right:
                objects_dir = dirs::left;
                break;

            case dirs::stop:
                objects_dir = dirs::stop;
                break;
            }
        }

        if (!vStars.empty())
        {
            for (int i = 0; i < vStars.size(); i++)
            {
                vStars[i]->dir = objects_dir;
                vStars[i]->Move(game_speed);
            }
        }

        if (!vMeteors.empty() && Ship)
        {
            for (int i = 0; i < vMeteors.size(); i++)
            {
                switch (Ship->dir)
                {
                case dirs::stop:
                    if (vMeteors[i]->x >= cl_width / 2)vMeteors[i]->dir = dirs::right;
                    else vMeteors[i]->dir = dirs::left;
                    break;

                case dirs::up:
                    if (vMeteors[i]->dir == dirs::up)vMeteors[i]->dir = dirs::down;
                    else if (vMeteors[i]->dir == dirs::down)vMeteors[i]->dir = dirs::up;
                    else if (vMeteors[i]->dir == dirs::left || vMeteors[i]->dir == dirs::up_left ||
                        vMeteors[i]->dir == dirs::down_left) vMeteors[i]->dir = dirs::down_left;
                    else if (vMeteors[i]->dir == dirs::right || vMeteors[i]->dir == dirs::up_right ||
                        vMeteors[i]->dir == dirs::down_right) vMeteors[i]->dir = dirs::down_right;
                    break;

                case dirs::down:
                    if (vMeteors[i]->dir == dirs::up)vMeteors[i]->dir = dirs::down;
                    else if (vMeteors[i]->dir == dirs::down)vMeteors[i]->dir = dirs::up;
                    else if (vMeteors[i]->dir == dirs::left || vMeteors[i]->dir == dirs::up_left ||
                        vMeteors[i]->dir == dirs::down_left) vMeteors[i]->dir = dirs::up_left;
                    else if (vMeteors[i]->dir == dirs::right || vMeteors[i]->dir == dirs::up_right ||
                        vMeteors[i]->dir == dirs::down_right) vMeteors[i]->dir = dirs::up_right;
                    break;

                case dirs::left:
                    if (vMeteors[i]->dir == dirs::left)vMeteors[i]->dir = dirs::right;
                    else if (vMeteors[i]->dir == dirs::up_left)vMeteors[i]->dir = dirs::down_right;
                    else if (vMeteors[i]->dir == dirs::down_left)vMeteors[i]->dir = dirs::up_right;
                    break;

                case dirs::right:
                    if (vMeteors[i]->dir == dirs::right)vMeteors[i]->dir = dirs::left;
                    else if (vMeteors[i]->dir == dirs::up_right)vMeteors[i]->dir = dirs::down_left;
                    else if (vMeteors[i]->dir == dirs::down_right)vMeteors[i]->dir = dirs::up_left;
                    break;

                }
                
                if (vMeteors[i]->GetType() != types::explosion)vMeteors[i]->Move(game_speed);
            }
        }

        if (Spare && Ship)
        {
            switch (Ship->dir)
            {
            case dirs::stop:
                if (Spare->x >= cl_width / 2)Spare->dir = dirs::right;
                else Spare->dir = dirs::left;
                break;

            case dirs::up:
                if (Spare->dir == dirs::up)Spare->dir = dirs::down;
                else if (Spare->dir == dirs::down)Spare->dir = dirs::up;
                else if (Spare->dir == dirs::left || Spare->dir == dirs::up_left ||
                    Spare->dir == dirs::down_left) Spare->dir = dirs::down_left;
                else if (Spare->dir == dirs::right || Spare->dir == dirs::up_right ||
                    Spare->dir == dirs::down_right) Spare->dir = dirs::down_right;
                break;

            case dirs::down:
                if (Spare->dir == dirs::up)Spare->dir = dirs::down;
                else if (Spare->dir == dirs::down)Spare->dir = dirs::up;
                else if (Spare->dir == dirs::left || Spare->dir == dirs::up_left ||
                    Spare->dir == dirs::down_left) Spare->dir = dirs::up_left;
                else if (Spare->dir == dirs::right || Spare->dir == dirs::up_right ||
                    Spare->dir == dirs::down_right) Spare->dir = dirs::up_right;
                break;

            case dirs::left:
                if (Spare->dir == dirs::left)Spare->dir = dirs::right;
                else if (Spare->dir == dirs::up_left)Spare->dir = dirs::down_right;
                else if (Spare->dir == dirs::down_left)Spare->dir = dirs::up_right;
                break;

            case dirs::right:
                if (Spare->dir == dirs::right)Spare->dir = dirs::left;
                else if (Spare->dir == dirs::up_right)Spare->dir = dirs::down_left;
                else if (Spare->dir == dirs::down_right)Spare->dir = dirs::up_left;
                break;

            }

            Spare->Move(game_speed);
          
        }

        if (Station && Ship)
        {
            switch (Station->dir)
            {
            case dirs::stop:
                if (Station->x >= cl_width / 2)Station->dir = dirs::right;
                else Station->dir = dirs::left;
                break;

            case dirs::up:
                if (Station->dir == dirs::up)Station->dir = dirs::down;
                else if (Station->dir == dirs::down)Station->dir = dirs::up;
                else if (Station->dir == dirs::left || Station->dir == dirs::up_left ||
                    Station->dir == dirs::down_left) Station->dir = dirs::down_left;
                else if (Station->dir == dirs::right || Station->dir == dirs::up_right ||
                    Station->dir == dirs::down_right) Station->dir = dirs::down_right;
                break;

            case dirs::down:
                if (Station->dir == dirs::up)Station->dir = dirs::down;
                else if (Station->dir == dirs::down)Station->dir = dirs::up;
                else if (Station->dir == dirs::left || Station->dir == dirs::up_left ||
                    Station->dir == dirs::down_left) Station->dir = dirs::up_left;
                else if (Station->dir == dirs::right || Station->dir == dirs::up_right ||
                    Station->dir == dirs::down_right) Station->dir = dirs::up_right;
                break;

            case dirs::left:
                if (Station->dir == dirs::left)Station->dir = dirs::right;
                else if (Station->dir == dirs::up_left)Station->dir = dirs::down_right;
                else if (Station->dir == dirs::down_left)Station->dir = dirs::up_right;
                break;

            case dirs::right:
                if (Station->dir == dirs::right)Station->dir = dirs::left;
                else if (Station->dir == dirs::up_right)Station->dir = dirs::down_left;
                else if (Station->dir == dirs::down_right)Station->dir = dirs::up_left;
                break;

            }

            Station->Move(game_speed);

        }

        if (!vLasers.empty())
        {
            for (std::vector<obj_ptr>::iterator las = vLasers.begin(); las < vLasers.end(); ++las)
            {
                bool out = false;
                (*las)->Move(game_speed);
                switch ((*las)->dir)
                {
                case dirs::up:
                    if ((*las)->y <= 50.0f)
                    {
                        (*las)->Release();
                        vLasers.erase(las);
                        out = true;
                        break;
                    }
                    break;

                case dirs::down:
                    if ((*las)->ey >= cl_height)
                    {
                        (*las)->Release();
                        vLasers.erase(las);
                        out = true;
                        break;
                    }
                    break;

                case dirs::left:
                    if ((*las)->x <= 0)
                    {
                        (*las)->Release();
                        vLasers.erase(las);
                        out = true;
                        break;
                    }
                    break;

                case dirs::right:
                    if ((*las)->ex >= cl_width)
                    {
                        (*las)->Release();
                        vLasers.erase(las);
                        out = true;
                        break;
                    }
                    break;
                }
                if (out)break;
            }
        }

        if (Ship && !vMeteors.empty())
        {
            for (std::vector<obj_ptr>::iterator met = vMeteors.begin(); met < vMeteors.end(); met++)
            {
                if ((*met)->GetType() == types::explosion)continue;

                if (!((*met)->x >= Ship->ex || (*met)->ex <= Ship->x || (*met)->y >= Ship->ey || (*met)->ey <= Ship->y))
                {
                    Ship->lifes -= 20;
                
                    if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);

                    switch (Ship->dir)
                    {
                    case dirs::up:
                        Ship->y += 100;
                        if (Ship->y >= cl_height)Ship->y = 50.0f;
                        Ship->SetEdges();
                        break;

                    case dirs::down:
                        Ship->y -= 100;
                        if (Ship->y <= 50.0f) Ship->y = cl_height - 100.0f;
                        Ship->SetEdges();
                        break;

                    case dirs::left:
                        Ship->x += 100;
                        if (Ship->x >= cl_width)Ship->x = cl_width - 100.0f;
                        Ship->SetEdges();
                        break;

                    case dirs::right:
                        Ship->x -= 100;
                        if (Ship->x < 0)Ship->x = 0.0f;
                        Ship->SetEdges();
                        break;
                    }
                    if (Ship->lifes <= 0)Ship->SetType(types::explosion);
                    break;
                }
            }
        }

        if (Station && !vMeteors.empty())
        {
            for (std::vector<obj_ptr>::iterator met = vMeteors.begin(); met < vMeteors.end(); met++)
            {
                if ((*met)->GetType() == types::explosion)continue;

                if (!((*met)->x >= Station->ex || (*met)->ex <= Station->x || (*met)->y >= Station->ey || (*met)->ey <= Station->y))
                {
                    Station->lifes -= 20;

                    if (sound)mciSendString(L"play .\\res\\snd\\damage.wav", NULL, NULL, NULL);

                    switch (Station->dir)
                    {
                    case dirs::up:
                        Station->y += 100;
                        if (Station->y >= cl_height)Station->y = 50.0f;
                        Station->SetEdges();
                        break;

                    case dirs::down:
                        Station->y -= 100;
                        if (Station->y <= 50.0f) Station->y = cl_height - 100.0f;
                        Station->SetEdges();
                        break;

                    case dirs::left:
                        Station->x += 100;
                        if (Station->x >= cl_width)Station->x = cl_width - 100.0f;
                        Station->SetEdges();
                        break;

                    case dirs::right:
                        Station->x -= 100;
                        if (Station->x < 0)Station->x = 0.0f;
                        Station->SetEdges();
                        break;
                    }
                    if (Station->lifes <= 0)Station->SetType(types::explosion);
                    break;
                }
            }
        }

        if (!vLasers.empty() && !vMeteors.empty())
        {
            for (std::vector<obj_ptr>::iterator meteor = vMeteors.begin(); meteor < vMeteors.end(); ++meteor)
            {
                for (std::vector<obj_ptr>::iterator laser = vLasers.begin(); laser < vLasers.end(); laser++)
                {
                    if (!((*meteor)->x >= (*laser)->ex || (*meteor)->ex <= (*laser)->x ||
                        (*meteor)->y >= (*laser)->ey || (*meteor)->ey <= (*laser)->y))
                    {
                        if ((*meteor)->GetType() != types::explosion)
                        {
                            score += 10;
                            (*laser)->Release();
                            vLasers.erase(laser);
                
                            if (!laser_upgraded)
                            {
                                switch ((*meteor)->GetType())
                                {
                                case types::big_asteroid:
                                    (*meteor)->SetType(types::mid_asteroid);
                                    break;

                                case types::mid_asteroid:
                                    (*meteor)->SetType(types::small_asteroid);
                                    break;

                                case types::small_asteroid:
                                    (*meteor)->SetType(types::meteor);
                                    break;

                                case types::meteor:
                                    if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                                    (*meteor)->y -= 100.0f;
                                    (*meteor)->SetType(types::explosion);
                                    score += 100;
                                    break;
                                }
                            }
                            else
                            {
                                if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                                (*meteor)->y -= 100.0f;
                                (*meteor)->SetType(types::explosion);
                                score += 100;
                            }

                            break;
                        }
                    }
                }
            }
        }

        if (vMeteors.size() < 3)
        {
            float tx = static_cast<float>(rand() % 800);
            float ty = static_cast<float>(rand() % 400 + 50);
            int ttype = rand() % 4;

            switch (ttype)
            {
            case 0:
                vMeteors.push_back(iObjectFactory(types::big_asteroid, tx, ty));
                break;

            case 1:
                vMeteors.push_back(iObjectFactory(types::mid_asteroid, tx, ty));
                break;

            case 2:
                vMeteors.push_back(iObjectFactory(types::small_asteroid, tx, ty));
                break;

            case 3:
                vMeteors.push_back(iObjectFactory(types::meteor, tx, ty));
                break;
            }
        }

        if (Ship && Spare)
        {
            if (!(Ship->x >= Spare->ex || Ship->ex <= Spare->x || Ship->y >= Spare->ey || Ship->ey <= Spare->y))
            {
                if (rand() % 2 == 0)
                {
                    if (Ship->lifes + 20 <= 100)Ship->lifes += 20;
                    else Ship->lifes = 100;
                    if (sound)mciSendString(L"play .\\res\\snd\\lifes.wav", NULL, NULL, NULL);
                }
                else
                {
                    spare_life = 15;
                    laser_upgraded = true;
                    if (sound)mciSendString(L"play .\\res\\snd\\upgrade.wav", NULL, NULL, NULL);
                }

                Spare->Release();
                Spare = nullptr;
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

        if (neb1)Draw->DrawBitmap(bmpNebula1, D2D1::RectF(neb1->x, neb1->y, neb1->ex, neb1->ey));
        if (neb2)Draw->DrawBitmap(bmpNebula2, D2D1::RectF(neb2->x, neb2->y, neb2->ex, neb2->ey));

        wchar_t status[150] = L"\0";
        wchar_t add[5] = L"\0";
        int st_size = 0;

        wcscpy_s(status, current_player);
        
        wcscat_s(status, L", ниво: ");
        swprintf(add, 2, L"%f", game_speed);
        wcscat_s(status, add);

        wcscat_s(status, L", резултат: ");
        swprintf(add, 5, L"%d", score);
        wcscat_s(status, add);

        wcscat_s(status, L", време: ");
        if (minutes < 10)wcscat_s(status, L"0");
        swprintf(add, 3, L"%d", minutes);
        wcscat_s(status, add);

        wcscat_s(status, L" : ");
        if (seconds < 10)wcscat_s(status, L"0");
        swprintf(add, 3, L"%d", seconds);
        wcscat_s(status, add);

        for (int i = 0; i < 150; ++i)
        {
            if (status[i] != '\0')st_size++;
            else break;
        }

        Draw->DrawText(status, st_size, nrmTextFormat, D2D1::RectF(10.0f, 60.0f, cl_width, 120.0f), TextBrush);

        if (!vStars.empty())
        {
            for (int i = 0; i < vStars.size(); i++)
                if (vStars[i]->x >= 0 && vStars[i]->x <= cl_width && vStars[i]->y > 75.0f && vStars[i]->y <= cl_height)
                {
                    if (vStars[i]->GetType() == types::small_star)
                        Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->ex - 25.0f, vStars[i]->ey - 25.0f),
                            vStars[i]->ex - vStars[i]->x,
                            vStars[i]->ey - vStars[i]->y), StarBrush);
                    else
                        Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(vStars[i]->ex - 25.0f, vStars[i]->ey - 25.0f),
                            vStars[i]->ex - vStars[i]->x,
                            vStars[i]->ey - vStars[i]->y), BigStarBrush);
                }
        }

        if (Ship)
        {
            if (Ship->GetType() != types::explosion)
            {
                if (Ship->lifes >= 80)
                {
                    FieldStops[0].position = 0;
                    FieldStops[0].color = D2D1::ColorF(D2D1::ColorF::Green);
                    FieldStops[1].position = 1.0f;
                    FieldStops[1].color = D2D1::ColorF(D2D1::ColorF::LightGreen);
                }
                else if (Ship->lifes>=40)
                {
                    FieldStops[0].position = 0;
                    FieldStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow);
                    FieldStops[1].position = 1.0f;
                    FieldStops[1].color = D2D1::ColorF(D2D1::ColorF::LightYellow);
                }
                else
                {
                    FieldStops[0].position = 0;
                    FieldStops[0].color = D2D1::ColorF(D2D1::ColorF::DarkRed);
                    FieldStops[1].position = 1.0f;
                    FieldStops[1].color = D2D1::ColorF(D2D1::ColorF::OrangeRed);
                }

                if (Draw)
                {
                    if (Draw->CreateGradientStopCollection(FieldStops, 2, &FieldCollection) != S_OK)ErrExit(eD2D,
                        L"Error in ShipForceGradientStopCollection ");
                    if (Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(Ship->x + 45.0f,
                        Ship->y + 45.0f), D2D1::Point2F(0, 0), 60.0f, 60.0f), FieldCollection, &ShipForceBrush) != S_OK)
                        ErrExit(eD2D, L"Error in ShipForceGradientBrush ");
   
                    Draw->FillEllipse(D2D1::Ellipse(D2D1::Point2F(Ship->x + 45.0f, Ship->y + 45.0f), 60.0f, 60.0f),
                        ShipForceBrush);

                    GarbageCollector(&FieldCollection);
                    GarbageCollector(&ShipForceBrush);
                }

            }
        }
       
        if (Ship)
        {
            if (Ship->GetType() == types::explosion)
            {
                int aframe = Ship->GetFrame();
                Draw->DrawBitmap(bmpExplosion[aframe], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
                if (aframe >= 23)GameOver();
            }
            else if (Ship->dir == dirs::up || Ship->dir == dirs::stop)
                Draw->DrawBitmap(bmpShipU[Ship->GetFrame()], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
            else if (Ship->dir == dirs::down)
                Draw->DrawBitmap(bmpShipD[Ship->GetFrame()], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
            else if (Ship->dir == dirs::up_right || Ship->dir == dirs::down_right || Ship->dir == dirs::right)
                Draw->DrawBitmap(bmpShipR[Ship->GetFrame()], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
            else if (Ship->dir == dirs::down_left || Ship->dir == dirs::up_left || Ship->dir == dirs::left)
                Draw->DrawBitmap(bmpShipL[Ship->GetFrame()], D2D1::RectF(Ship->x, Ship->y, Ship->ex, Ship->ey));
        }

        if (!vMeteors.empty())
        {
            for (int i = 0; i < vMeteors.size(); i++)
            {
                if (vMeteors[i]->ey <= 50 || vMeteors[i]->ey >= cl_height)continue;
                bool killed = false;

                switch (vMeteors[i]->GetType())
                {
                case types::big_asteroid:
                    Draw->DrawBitmap(bmpBigAsteroid, D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, vMeteors[i]->ex,
                        vMeteors[i]->ey));
                    break;

                case types::mid_asteroid:
                    Draw->DrawBitmap(bmpMidAsteroid, D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, vMeteors[i]->ex,
                        vMeteors[i]->ey));
                    break;

                case types::small_asteroid:
                    Draw->DrawBitmap(bmpBigAsteroid, D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, vMeteors[i]->ex,
                        vMeteors[i]->ey));
                    break;

                case types::meteor:
                    if (vMeteors[i]->x >= cl_width / 2)
                        Draw->DrawBitmap(bmpMeteorR, D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, vMeteors[i]->ex,
                            vMeteors[i]->ey));
                    else
                        Draw->DrawBitmap(bmpMeteorL, D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, vMeteors[i]->ex,
                            vMeteors[i]->ey));
                    break;

                case types::explosion:
                    {
                        int last_frame = vMeteors[i]->GetFrame();
                        Draw->DrawBitmap(bmpExplosion[last_frame], D2D1::RectF(vMeteors[i]->x, vMeteors[i]->y, 
                            vMeteors[i]->ex, vMeteors[i]->ey));
                        if (last_frame >= 23)
                        {
                            
                            if (!Spare && rand() % 10 == 6)
                            {
                                Spare = iObjectFactory(types::spare, vMeteors[i]->x, vMeteors[i]->y);
                                spare_life = 15;
                            }
                            vMeteors[i]->Release();
                            vMeteors.erase(vMeteors.begin() + i);
                            killed = true;
                            break;
                        }
                    }
                    break;

                }
                if (killed)break;
            }
        }

        if (!vLasers.empty())
        {
            for (int i = 0; i < vLasers.size(); ++i)
            {
                if (vLasers[i]->GetType() == types::hlaser)
                    Draw->DrawBitmap(bmpLaserH, D2D1::RectF(vLasers[i]->x, vLasers[i]->y, vLasers[i]->ex, vLasers[i]->ey));
                else if (vLasers[i]->GetType() == types::vlaser)
                    Draw->DrawBitmap(bmpLaserV, D2D1::RectF(vLasers[i]->x, vLasers[i]->y, vLasers[i]->ex, vLasers[i]->ey));
                else if (vLasers[i]->GetType() == types::hstrong_laser)
                    Draw->DrawBitmap(bmpStrongLaserH, D2D1::RectF(vLasers[i]->x, vLasers[i]->y, vLasers[i]->ex, vLasers[i]->ey));
                else if (vLasers[i]->GetType() == types::vstrong_laser)
                    Draw->DrawBitmap(bmpStrongLaserV, D2D1::RectF(vLasers[i]->x, vLasers[i]->y, vLasers[i]->ex, vLasers[i]->ey));
            }
        }

        if (Spare)
            Draw->DrawBitmap(bmpSpare, D2D1::RectF(Spare->x, Spare->y, Spare->ex, Spare->ey));

        if (laser_upgraded)
            Draw->DrawLine(D2D1::Point2F(cl_width / 2 + 150.0f, 80.0f), D2D1::Point2F((cl_width / 2 + 150.0f)
                + spare_life * 6, 80.0f), SpareLifeBrush, 20.0f);

        if (Station)
        {
            if (Station->GetType() != types::explosion)
                Draw->DrawBitmap(bmpStation, D2D1::RectF(Station->x, Station->y, Station->ex, Station->ey));
            else
            {
                int aframe = Station->GetFrame();
                Draw->DrawBitmap(bmpExplosion[aframe], D2D1::RectF(Station->x, Station->y, Station->ex, Station->ey));
                if (aframe >= 23)GameOver();
            }
        }

        ////////////////////////////////////////////////////



        Draw->EndDraw();
    }


    std::remove(tmp_file);
    SafeRelease();
    return (int) bMsg.wParam;
}