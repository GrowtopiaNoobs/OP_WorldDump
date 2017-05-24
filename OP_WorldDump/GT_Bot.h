#include <iostream>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>

using namespace std;

void sleep(int x)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

void messagePump()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

class Trainer
{
	private:
		HANDLE hProcess = NULL;
		DWORD ProcessId = NULL;
		HWND hWnd = NULL;
		unsigned long threadID;
		bool secefullKeyboardHook = false;
		typedef void(*SetHook)(bool);
		typedef void(*UninstallHook)();
		typedef void(*CurrentVkey)(int);
		bool secefullMouseHook = false;
		typedef void(*SetPtM)(LPARAM);
		typedef void(*SetmouseDataM)(DWORD);
	public:
		void sendKeyStroke(WPARAM key, int len);
		void writeLetter(char key, int len);
		bool writeProcessMemory(_In_  LPVOID  lpBaseAddress, _In_  LPCVOID lpBuffer, _In_  SIZE_T  nSize);
		bool readProcessMemory(_In_  LPCVOID lpBaseAddress, _Out_ LPVOID  lpBuffer, _In_  SIZE_T  nSize);
		HANDLE getProcessHandle();
		DWORD getProcessId();
		HWND getProcessHWND();
		void installKeyboardHook(); // if you call this function, then please include hookKbd32.dll to your exe folder
		void installMouseHook(); // if you call this function, then please include hookMouse32.dll to your exe folder
		SetHook setKeyboardHookState;
		UninstallHook uninstallKeyboardHook;
		CurrentVkey setCurrentHookVkey;
		bool isSecefullKeyboardHook();
		SetHook setMouseHookState;
		UninstallHook uninstallMouseHook;
		SetPtM setMousePointer;
		SetmouseDataM setMouseData;
		bool isSecefullMouseHook();
		void mouseClick(int x, int y, int len);
		DWORD getPixelColor(int x, int y);
		long long int findPointer(int offset, long long int baseaddr, int offsets[]);
		int getWindowXSize();
		int getWindowYSize();
		Trainer(HWND appHWND);
};

Trainer::Trainer(HWND appHWND)
{
	hWnd = appHWND;
	GetWindowThreadProcessId(hWnd, &ProcessId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessId);
	threadID = GetWindowThreadProcessId(hWnd, &ProcessId);
}

void Trainer::sendKeyStroke(WPARAM key, int len)
{
	/*if (secefullKeyboardHook)
	{
		setCurrentHookVkey(key);
	}*/
	PostMessage(hWnd, WM_KEYDOWN, key, 0);
	sleep(len);
	PostMessage(hWnd, WM_KEYUP, key, 0);
}

void Trainer::writeLetter(char key, int len)
{
	if (key != ' ')
	{
		if (isupper(key))
		{
			sendKeyStroke(VkKeyScanEx(tolower(key), GetKeyboardLayout(0)), len);
		}
		else {
			sendKeyStroke(VkKeyScanEx(key, GetKeyboardLayout(0)), len);
		}
	}
	else {
		sendKeyStroke(0x20, len);
	}
}

bool Trainer::writeProcessMemory(_In_  LPVOID  lpBaseAddress, _In_  LPCVOID lpBuffer, _In_  SIZE_T  nSize)
{
	SIZE_T lpNumberOfBytesWritten = 0;
	WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, &lpNumberOfBytesWritten);
	if (lpNumberOfBytesWritten != nSize)
	{
		return false;
	}
	else {
		return true;
	}
}

bool Trainer::readProcessMemory(_In_  LPCVOID lpBaseAddress, _Out_ LPVOID  lpBuffer, _In_  SIZE_T  nSize)
{
	SIZE_T lpNumberOfBytesRead=0;
	ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, &lpNumberOfBytesRead);
	if (lpNumberOfBytesRead != nSize)
	{
		return false;
	} else {
		return true;
	}
}

HANDLE Trainer::getProcessHandle()
{
	return hProcess;
}

DWORD Trainer::getProcessId()
{
	return ProcessId;
}

HWND Trainer::getProcessHWND()
{
	return hWnd;
}

void Trainer::installKeyboardHook()
{
	HINSTANCE hinst;
	hinst = LoadLibrary(_T("hookKbd32.dll"));

	if (hinst) {
		typedef void(*Install)(unsigned long);

		Install install = (Install)GetProcAddress(hinst, "install");
		uninstallKeyboardHook = (UninstallHook)GetProcAddress(hinst, "uninstallHook");
		setKeyboardHookState = (SetHook)GetProcAddress(hinst, "setHook");
		setCurrentHookVkey = (CurrentVkey)GetProcAddress(hinst, "currentVkey");

		install(threadID);
		secefullKeyboardHook = true;
	}
	else {
		secefullKeyboardHook = false;
	}
}

bool Trainer::isSecefullKeyboardHook()
{
	return secefullKeyboardHook;
}

void Trainer::installMouseHook()
{
	HINSTANCE hinst;
	hinst = LoadLibrary(_T("hookMouse32.dll"));
	if (hinst) {
		typedef void(*Install)(unsigned long);

		Install install = (Install)GetProcAddress(hinst, "install");
		uninstallMouseHook = (UninstallHook)GetProcAddress(hinst, "uninstallHook");
		setMouseHookState = (SetHook)GetProcAddress(hinst, "setHook");
		setMousePointer = (SetPtM)GetProcAddress(hinst, "setPtM");
		setMouseData = (SetmouseDataM)GetProcAddress(hinst, "setmouseDataM");
		install(threadID);
		secefullMouseHook = true;
	}
	else {
		secefullMouseHook = false;
	}
}

bool Trainer::isSecefullMouseHook()
{
	return secefullMouseHook;
}

void Trainer::mouseClick(int x, int y, int len)
{
	if (secefullMouseHook)
	{
		setMousePointer(MAKELPARAM(x, y));
		setMouseData(NULL);
	}
	SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
	if (secefullMouseHook)
	{
		setMouseData(WM_XBUTTONDOWN);
	}
	SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));
	sleep(len);
	if (secefullMouseHook)
	{
		setMouseData(WM_XBUTTONUP);
	}
	SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
}

DWORD Trainer::getPixelColor(int x, int y)
{
	HDC DC = GetWindowDC(hWnd);
	DWORD Color = GetPixel(DC, x, y);
	return Color;
}

long long int Trainer::findPointer(int offset, long long int baseaddr, int offsets[])
{
	long long int Address = baseaddr;
	int total = offset;
	for (int i = 0; i < total; i++) //Loop trough the offsets
	{
		ReadProcessMemory(hProcess, (LPCVOID)Address, &Address, 8, NULL);
		Address += offsets[i];
	}
	return Address;
}

int Trainer::getWindowXSize()
{
	RECT rect;
	if (GetWindowRect(hWnd, &rect))
	{
		int width = rect.right - rect.left;
		return width;
	} else {
		return 0;
	}
}

int Trainer::getWindowYSize()
{
	RECT rect;
	if (GetWindowRect(hWnd, &rect))
	{
		int height = rect.bottom - rect.top;
		return height;
	} else {
		return 0;
	}
}

class GTPlayer
{
	public:
		// address functions
		void setPlayerBase(unsigned long long int base);
		void setWorldBase(unsigned long long int base);
		void setBlocksBase(unsigned long long int base);
		void setProcessHandle(HANDLE hProc);
		// player state
		float getXPos(void);
		float getYPos(void);
		string getName(void);
		string getNameNoColor(void);
		bool getIsTurnedLeft(void);
		bool getIsInAir(void);
		bool getIsOnGround(void);
		bool getIsSliding(void);
		char getSolidState(void);
		bool getIsInWater(void);
		// world state
		int getBlockIdInWorld(int x, int y);
		int getBackgroundIdInWorld(int x, int y);
		// blocks info
		string getBlockName(int id);

	private:
		unsigned long long int playerBase;
		unsigned long long int worldBase;
		unsigned long long int blocksBase;
		HANDLE hProcess;
		int XPosOffset = 0x0000;
		int YPosOffset = 0x0004;
		int nameOffset = 0x0028;
		int isTurnedLeftOffset = 0x0169;
		int isInAirOffset = 0x018C;
		int isOnGroundOffset = 0x019C;
		int isSlidingOffset = 0x019D;
		int solidStateOffset = 0x01F0;
		int isInWaterOffset = 0x0251;
};

void GTPlayer::setPlayerBase(unsigned long long int base)
{
	playerBase = base;
}

void GTPlayer::setWorldBase(unsigned long long int base)
{
	worldBase = base;
}

void GTPlayer::setBlocksBase(unsigned long long int base)
{
	blocksBase = base;
}

void GTPlayer::setProcessHandle(HANDLE hProc)
{
	hProcess = hProc;
}

float GTPlayer::getXPos()
{
	float XPosVal = 0;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + XPosOffset), &XPosVal, sizeof(float), NULL);
	return XPosVal;
}

float GTPlayer::getYPos()
{
	float YPosVal = 0;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + YPosOffset), &YPosVal, sizeof(float), NULL);
	return YPosVal;
}

string GTPlayer::getName()
{
	string returnName="";
	char nameBuf=1;
	int i=0;
	while (nameBuf!=0)
	{
		ReadProcessMemory(hProcess, (LPCVOID)(playerBase + nameOffset + i), &nameBuf, sizeof(char), 0);
		if (nameBuf)
		{
			returnName += nameBuf;
		}
		i++;
		if (i > 100)
		{
			return "";
		}
	}
	return returnName;
}

string GTPlayer::getNameNoColor()
{
	string coloredName = getName();
	if (coloredName.size() > 4)
	{
		string nameWithoutColor = "";
		nameWithoutColor = coloredName.substr(2, coloredName.size() - 4);
		if (coloredName.substr(0, 1) != coloredName.substr(coloredName.size() - 2, 1))
		{
			return "";
		}
		return nameWithoutColor;
	} else {
		return "";
	}
}

bool GTPlayer::getIsTurnedLeft()
{
	bool isTurnedLeftVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + isTurnedLeftOffset), &isTurnedLeftVal, sizeof(bool), 0);
	return isTurnedLeftVal;
}

bool GTPlayer::getIsInAir()
{
	bool isInAirVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + isInAirOffset), &isInAirVal, sizeof(bool), 0);
	return isInAirVal;
}

bool GTPlayer::getIsOnGround()
{
	bool isOnGroundVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + isOnGroundOffset), &isOnGroundVal, sizeof(bool), 0);
	return isOnGroundVal;
}

bool GTPlayer::getIsSliding()
{
	bool isSlidingVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + isSlidingOffset), &isSlidingVal, sizeof(bool), 0);
	return isSlidingVal;
}

char GTPlayer::getSolidState()
{
	bool solidStateVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + solidStateOffset), &solidStateVal, sizeof(char), 0);
	return solidStateVal;
}

bool GTPlayer::getIsInWater()
{
	bool isInWaterVal = false;
	ReadProcessMemory(hProcess, (LPCVOID)(playerBase + isInWaterOffset), &isInWaterVal, sizeof(bool), 0);
	return isInWaterVal;
}

int GTPlayer::getBlockIdInWorld(int x, int y)
{
	long long int BlockIdInWorldAddress = worldBase;
	int BlockIdInWorldVal = 0;
	BlockIdInWorldAddress = BlockIdInWorldAddress + (x * 0x58);
	BlockIdInWorldAddress = BlockIdInWorldAddress + (y * 100 * 0x58);
	ReadProcessMemory(hProcess, (LPCVOID)BlockIdInWorldAddress, &BlockIdInWorldVal, 2, 0);
	return BlockIdInWorldVal;
}

int GTPlayer::getBackgroundIdInWorld(int x, int y)
{
	long long int BackgroundIdInWorldAddress = worldBase + 2;
	int BackgroundIdInWorldVal = 0;
	BackgroundIdInWorldAddress = BackgroundIdInWorldAddress + (x * 0x58);
	BackgroundIdInWorldAddress = BackgroundIdInWorldAddress + (y * 100 * 0x58);
	if (!ReadProcessMemory(hProcess, (LPCVOID)BackgroundIdInWorldAddress, &BackgroundIdInWorldVal, 2, 0))
	{
		BackgroundIdInWorldVal = NULL;
	}
	return BackgroundIdInWorldVal;
}

string GTPlayer::getBlockName(int id)
{
	long long int blockNameAddress = blocksBase + (id * 0x130) + 0x58;
	string returnName = "";
	char blockNameBuf = 1;
	int i = 0;
	while (blockNameBuf != 0)
	{
		if (!ReadProcessMemory(hProcess, (LPCVOID)(blockNameAddress + i), &blockNameBuf, sizeof(char), 0))
		{
			blockNameBuf = NULL;
		}
		if (blockNameBuf)
		{
			returnName += blockNameBuf;
		}
		i++;
		if (i > 100)
		{
			return "";
		}
	}
	return returnName;
}