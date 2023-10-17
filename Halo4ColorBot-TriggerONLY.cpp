#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

using std::string;
using std::cout;
using std::cin;
using std::endl;

bool isr3d(BYTE red, BYTE green, BYTE blue) {
    return red > 200 && green < 100 && blue < 100;
}

typedef int(*pDD_btn)(int btn);
pDD_btn      DD_btn;          //Mouse button

int main()
{

    bool alwayson = false;


    string on;
    cout << "M4 (Y/N): ";
    cin >> on;
    if (on == "N") { alwayson = true; }

    cout << "\nBing Bong";

    // Set the coordinates of the center box
    int centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
    int centerY = GetSystemMetrics(SM_CYSCREEN) / 2;
    int boxSize = 20; // Adjust the size of the box as needed
    int requiredRedPixels = 5; // Adjust the threshold as needed

    //Load Chinese Mouse Driver
    HMODULE hDll = LoadLibraryW(L"3jkbnhhjkb.dll");    //App x64
    if (hDll == nullptr)
    {
        return -1;
    }
    DD_btn = (pDD_btn)GetProcAddress(hDll, "DD_btn");
    int st = DD_btn(0);
    if (st != 1)
    {
        //DD Initialize Error
        return st;
    }

    while (true) {
        if (GetKeyState(VK_XBUTTON1) & 0x8000 || alwayson == true) {
            // Capture the screen
            HDC hdcScreen = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdcScreen);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, boxSize, boxSize);
            SelectObject(hdcMem, hBitmap);
            BitBlt(hdcMem, 0, 0, boxSize, boxSize, hdcScreen, centerX - boxSize / 2, centerY - boxSize / 2, SRCCOPY);

            // Check pixels in the captured box
            int redPixelCount = 0;
            for (int x = 0; x < boxSize; ++x) {
                for (int y = 0; y < boxSize; ++y) {
                    COLORREF pixelColor = GetPixel(hdcMem, x, y);
                    BYTE red = GetRValue(pixelColor);
                    BYTE green = GetGValue(pixelColor);
                    BYTE blue = GetBValue(pixelColor);

                    if (isr3d(red, green, blue)) {
                        redPixelCount++;
                        if (redPixelCount >= requiredRedPixels) {
                            break;
                        }
                    }
                }
                if (redPixelCount >= requiredRedPixels) {
                    break;
                }
            }

            // If the required number of red pixels is detected, simulate a left mouse button click
            if (redPixelCount >= requiredRedPixels) {

                DD_btn(1);
                cout << ".";

                std::mt19937_64 eng{ std::random_device{}() };
                std::uniform_int_distribution<> dist{ 30, 90 };
                std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });

                DD_btn(2);

                std::mt19937_64 eng1{ std::random_device{}() };
                std::uniform_int_distribution<> dist1{ 200, 800 };
                std::this_thread::sleep_for(std::chrono::milliseconds{ dist1(eng1) });
            }

            DeleteObject(hBitmap);
            DeleteDC(hdcMem);
            ReleaseDC(NULL, hdcScreen);

            std::mt19937_64 eng1{ std::random_device{}() };  // or seed however you want
            std::uniform_int_distribution<> dist1{ 20, 30 };
            std::this_thread::sleep_for(std::chrono::milliseconds{ dist1(eng1) });
        }
    }
}
