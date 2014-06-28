#define WINVER 0x0500
#include <windows.h>
#include "Tchar.h"
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

#define noKeyPress 0
#define noKeyRelease 1
#define windowNotFound 2
#define noForeground 3

#define KeyDown 0x0000
#define KeyUp 0x0002
#define Scancode 0x0008

bool running;
HWND window;

void error(int err)
{
    switch(err)
    {
        case 0:
            cout << "Failed to send key. Aborting!" << endl;
            break;
        case 1:
            cout << "Failed to release key. Aborting!" << endl;
            break;
        case 2:
            cout << "Window not found!" << endl;
            break;
        case 3:
            cout << "Failed to put window to foreground. Aborting!" << endl;
            break;
    }
    running = 0;
}

void wait(long seconds)
{
	seconds = seconds * 1000;
	Sleep(seconds);
}


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
    if(!SendKey(key, 0 | Scancode) ) error(noKeyPress);
    wait(2);
    if(!SendKey(key, KeyUp | Scancode) ) error(noKeyRelease);

    return 1;


}

static bool FindProgram(const char* program)
{
    // Get first window on desktop
    HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    window = firstwindow;
    TCHAR windowtext[MAX_PATH];

    // We need to get the console title in case we
    // accidentally match the search word with it
    // instead of the intended target window.
    TCHAR consoletitle[MAX_PATH];
    GetConsoleTitle(consoletitle, MAX_PATH);

    while(1)
    {
        //fprintf(stderr, ".");
        // Check window title for a match
        GetWindowText(window, windowtext, MAX_PATH);
        if (strstr(windowtext, program) != NULL &&
            strcmp(windowtext, consoletitle) != 0) break;

        // Get next window
        window = FindWindowEx(NULL, window, NULL, NULL);
        if (window == NULL || window == firstwindow)
        {
            error(windowNotFound);
            return 0;
        }
    }
    cout << "Window found : " << windowtext << endl;
    return 1;
}


int main()
{
    int interval;

    char prog[100];
    bool foreground = 1;

    do{
        cout << "Name of program to attatch to (if none enter NULL): ";
        cin.getline(prog, 100);
        if(strstr(prog, "NULL")){foreground = 0; break;}
    }while(!FindProgram(prog) );

    cout << "Choose interval between keypress (seconds): ";
    cin >> interval;

    running = 1;
    while(running)
    {
        wait(interval);
        if(foreground == 1)
            if(!SetForegroundWindow(window) ) error(noForeground);
        PressKey(57);   //http://www.comptechdoc.org/os/linux/howlinuxworks/linux_hlkeycodes.html
    }

    system("pause");

    return 0;

}


