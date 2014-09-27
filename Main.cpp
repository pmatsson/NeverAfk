#define WINVER 0x0500
#include <windows.h>
#include <Tchar.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include "Dik.h" //http://www.gamespp.com/directx/directInputKeyboardScanCodes.html
using namespace std;

//ERROR cases
#define noKeyPress 0
#define noKeyRelease 1
#define windowNotFound 2
#define noForeground 3

//keyFlags
#define KeyDown 0x0000
#define KeyUp 0x0002
#define Scancode 0x0008

bool running;
int interval;
time_t startTime;
TCHAR windowtext[MAX_PATH];
HWND window;

void Error(int err)
{
    switch(err)
    {
        case noKeyPress:
            cout << "Failed to send key. Aborting!" << endl;
            break;
        case noKeyRelease:
            cout << "Failed to release key. Aborting!" << endl;
            break;
        case windowNotFound:
            cout << "Window not found!" << endl;
            break;
        case noForeground:
            cout << "Failed to put window to foreground. Aborting!" << endl;
            break;
    }
    running = 0;
}

void Wait(long seconds)
{
    //Loop here because we need to check for user input
	for(int i = 0; i<seconds; i++)
    {
        if(GetAsyncKeyState(VK_ESCAPE))
        {
            running = false;
            break;
        }
        else
            Sleep(1000);
    }
}

void GiveInfo(bool newKeystroke)
{
    static int NOT = 0;
    system("CLS");

    if(newKeystroke)
    {
        cout << "*****************************************" << endl
             << "Window: " << windowtext << endl
             << "Interval: " << interval << endl
             << "Simulated keystrokes: " << ++NOT << endl
             << "Amount of time since first keystroke: " << difftime(time(NULL), startTime) << endl
             << "*****************************************" << endl
             << "Press ESC to exit..." << endl;
    }

    else
    {
        cout << "Total amount of keystrokes this session: " << NOT << endl
             << "Approx amount of time since first keystroke: " << difftime(time(NULL), startTime)
             << " seconds" <<endl;
    }

}

//Basically as low we can get without writing a new
//input driver. For some games this might not work.
static bool SendKey(short keyCode, int keyFlag )
{
     INPUT* InputData = new INPUT[1];

     InputData[0].type = 1;
     InputData[0].ki.wScan = keyCode;
     InputData[0].ki.dwFlags = (int)keyFlag;
     InputData[0].ki.time = 0;
     InputData[0].ki.dwExtraInfo = 0;

     return SendInput(1, InputData, sizeof(INPUT));

}

static bool PressKey(short key)
{
    if(!SendKey(key, KeyDown | Scancode) ) Error(noKeyPress);
    Wait(2);    //To make sure the application pick up on the keypress
    if(!SendKey(key, KeyUp | Scancode) ) Error(noKeyRelease);

    return 1;

}

static bool FindProgram(const char* program)
{
    // Get first window on desktop
    HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    window = firstwindow;

    // We need to get the console title in case we
    // accidentally match the search word with it
    // instead of the intended target window.
    TCHAR consoletitle[MAX_PATH];
    GetConsoleTitle(consoletitle, MAX_PATH);

    while(1)
    {
        // Check window title for a match
        GetWindowText(window, windowtext, MAX_PATH);
        if (strstr(windowtext, program) != NULL && strcmp(windowtext, consoletitle) != 0)
                break;

        // Get next window
        window = FindWindowEx(NULL, window, NULL, NULL);
        if (window == NULL || window == firstwindow)
        {
            Error(windowNotFound);
            return 0;
        }
    }
    cout << "Window found : " << windowtext << endl;

    return 1;
}


int main()
{
    char prog[100];
    bool foreground = 1;

    do{
        cout <<  "What window should I attach to? If none enter 'manual': " ;
        cin.getline(prog, 100);

        if(strstr(prog, "manual"))
        {
            foreground = 0;
            strcpy(windowtext, "Manual");
            break;
        }

    }while(!FindProgram(prog) );


    cout << "Choose interval between key simulations (min 2 seconds): ";
    cin >> interval;

    if(interval<2)      //We are already waiting 2 sec between press and release
        interval = 2;

    running = 1;
    time(&startTime);
    while(running)
    {
        Wait(interval-2);
        if(foreground == 1)
            if(!SetForegroundWindow(window) )
                Error(noForeground);

        PressKey(DIK_SPACE);
        GiveInfo(true);
    }

    GiveInfo(false);
    system("pause");

    return 0;

}


