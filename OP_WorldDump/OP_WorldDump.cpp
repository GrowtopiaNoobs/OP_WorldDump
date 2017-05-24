// OP_WorldDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GT_Bot.h"

char abcd[100] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }; //36 at all, but fail at numbers so only 26
BYTE* img = new BYTE[3*1000*600];
void SaveBitmapToFile(BYTE* pBitmapBits, LONG lWidth, LONG lHeight, WORD wBitsPerPixel, LPCTSTR lpszFileName)
{
	RGBQUAD palette[256];
	for (int i = 0; i < 256; ++i)
	{
		palette[i].rgbBlue = (byte)i;
		palette[i].rgbGreen = (byte)i;
		palette[i].rgbRed = (byte)i;
	}

	BITMAPINFOHEADER bmpInfoHeader = { 0 };
	// Set the size
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	// Bit count
	bmpInfoHeader.biBitCount = wBitsPerPixel;
	// Use all colors
	bmpInfoHeader.biClrImportant = 0;
	// Use as many colors according to bits per pixel
	bmpInfoHeader.biClrUsed = 0;
	// Store as un Compressed
	bmpInfoHeader.biCompression = BI_RGB;
	// Set the height in pixels
	bmpInfoHeader.biHeight = lHeight;
	// Width of the Image in pixels
	bmpInfoHeader.biWidth = lWidth;
	// Default number of planes
	bmpInfoHeader.biPlanes = 1;
	// Calculate the image size in bytes
	bmpInfoHeader.biSizeImage = lWidth* lHeight * (wBitsPerPixel / 8);

	BITMAPFILEHEADER bfh = { 0 };
	// This value should be values of BM letters i.e 0x4D42
	// 0x4D = M 0×42 = B storing in reverse order to match with endian

	bfh.bfType = 'B' + ('M' << 8);
	// <<8 used to shift ‘M’ to end

	// Offset to the RGBQUAD
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + sizeof(RGBQUAD) * 256;
	// Total size of image including size of headers
	bfh.bfSize = bfh.bfOffBits + bmpInfoHeader.biSizeImage;
	// Create the file in disk to write
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hFile) // return if error opening file
	{
		return;
	}

	DWORD dwWritten = 0;
	// Write the File header
	WriteFile(hFile, &bfh, sizeof(bfh), &dwWritten, NULL);
	// Write the bitmap info header
	WriteFile(hFile, &bmpInfoHeader, sizeof(bmpInfoHeader), &dwWritten, NULL);
	// Write the palette
	WriteFile(hFile, &palette[0], sizeof(RGBQUAD) * 256, &dwWritten, NULL);
	// Write the RGB Data
	if (lWidth % 4 == 0)
	{
		WriteFile(hFile, pBitmapBits, bmpInfoHeader.biSizeImage, &dwWritten, NULL);
	}
	else
	{
		char* empty = new char[4 - lWidth % 4];
		for (int i = 0; i < lHeight; ++i)
		{
			WriteFile(hFile, &pBitmapBits[i * lWidth], lWidth, &dwWritten, NULL);
			WriteFile(hFile, empty, 4 - lWidth % 4, &dwWritten, NULL);
		}
	}
	// Close the file handle
	CloseHandle(hFile);
}

string worldName;
string dTime;

inline bool exists_test0(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleTitleA("GT Bot by Growtopia Noobs");
	Trainer trainer(FindWindowA(NULL, "Growtopia"));
	GTPlayer player;
	player.setProcessHandle(trainer.getProcessHandle());
	int playerBaseOffsets[] = { 0x560, 0x1c8, 0x708, 0x708, 0x8 };
	player.setPlayerBase(trainer.findPointer(5, 0x1403C12F8, playerBaseOffsets));
	int worldBaseOffsets[] = { 0x560, 0x158, 0x48, 0x30, 0x0 };
	player.setWorldBase(trainer.findPointer(5, 0x1403C12F8, worldBaseOffsets));
	int blocksOffsets[] = { 0x120, 0x0, 0x0, 0x0 };
	player.setBlocksBase(trainer.findPointer(4, 0x1403C1FB8, blocksOffsets));
	if (trainer.getProcessHWND() != NULL)
	{

	}
	else {
		cout << "Initializon of trainer failed!" << endl;
		_getch();
		return 0;
	}
	int a = 0, b = 0, c = 0;
	int attemptToDump = 0;
	bool dumpDone;
	char fileName[100];
	string fileName2b;
	string fileName3;
	int worldNameLen = 1;
	fileName[0] = 'i';
	fileName[1] = 'd';
	fileName[2] = 's';
	fileName[3] = '\\';
	while (true)
	{
		dTime = "";
		worldName = "";
		fileName2b = "ids_bg\\";
		fileName3 = "img\\";
		dTime = "time\\";
		fileName2b += abcd[a];
		fileName3 += abcd[a];
		worldName += abcd[a];
		dTime += abcd[a];
		if (worldNameLen > 1)
		{
			fileName2b += abcd[b];
			fileName3 += abcd[b];
			worldName += abcd[b];
			dTime += abcd[b];
		}
		if (worldNameLen > 2)
		{
			fileName2b += abcd[c];
			fileName3 += abcd[c];
			worldName += abcd[c];
			dTime += abcd[c];
		}
		fileName2b += ".txt";
		fileName3 += ".bmp";
		dTime += ".txt";
		if (exists_test0(fileName2b))
		{
			a++;
			if (a == 26)
			{
				if (worldNameLen == 1)
				{
					worldNameLen = 2;
				}
				else {
					b = b + 1;
				}
				a = 0;
			}
			if (b == 26)
			{
				if (worldNameLen == 2)
				{
					worldNameLen = 3;
				}
				else {
					c = c + 1;
				}
				b = 0;
			}
			if (c == 26)
			{
				cout << "World dump is done!" << endl;
				_getch();
				return 0;
			}
			continue;
		}
		//here we enter world
		trainer.mouseClick(518, 175, 50);
		for (int i = 0; i < 10; i++)
		{
			trainer.sendKeyStroke(VK_BACK, 50);
		}
		trainer.sendKeyStroke(VkKeyScanEx(abcd[a], GetKeyboardLayout(0)), 50);
		if (worldNameLen>1)
		trainer.sendKeyStroke(VkKeyScanEx(abcd[b], GetKeyboardLayout(0)), 50);
		if (worldNameLen>2)
		trainer.sendKeyStroke(VkKeyScanEx(abcd[c], GetKeyboardLayout(0)), 50);
		fileName[4] = abcd[a];
		fileName[5] = abcd[b];
		fileName[6] = abcd[c];
		fileName[4 + worldNameLen] = '.';
		fileName[5 + worldNameLen] = 't';
		fileName[6 + worldNameLen] = 'x';
		fileName[7 + worldNameLen] = 't';
		fileName[8 + worldNameLen] = NULL;
		trainer.sendKeyStroke(VK_RETURN, 50);
		sleep(2000);
		//here we start dumping world
		dumpDone = false;
		ofstream blocksIds;
		blocksIds.open(fileName, ios::out);
		ofstream backgroundIds;
		backgroundIds.open(fileName2b.c_str(), ios::out);
		attemptToDump = 0;
		while (attemptToDump < 200 && !dumpDone)
		{
			player.setPlayerBase(trainer.findPointer(5, 0x1403C12F8, playerBaseOffsets));
			player.setWorldBase(trainer.findPointer(5, 0x1403C12F8, worldBaseOffsets));
			sleep(500);
			if (player.getBlockIdInWorld(99, 59) == 8)
			{
				for (int i = 0; i < 600; i++){
					for (int j = 0; j < 1000; j++){
						if (player.getBlockIdInWorld(j/10, i/10)!=0)
						{
							img[0 + j * 3 + (599 - i) * 3 * 1000] = 0;
							img[1 + j * 3 + (599 - i) * 3 * 1000] = 0;
							img[2 + j * 3 + (599 - i) * 3 * 1000] = 0;
						} else if (player.getBackgroundIdInWorld(j/10, i/10)!=0)
						{
							img[0 + j * 3 + (599 - i) * 3 * 1000] = 128;
							img[1 + j * 3 + (599 - i) * 3 * 1000] = 128;
							img[2 + j * 3 + (599 - i) * 3 * 1000] = 128;
						} else {
							img[0 + j * 3 + (599 - i) * 3 * 1000] = 255;
							img[1 + j * 3 + (599 - i) * 3 * 1000] = 255;
							img[2 + j * 3 + (599 - i) * 3 * 1000] = 255;
						}
					}
				}
				for (int i = 0; i < 60; i++){
					for (int j = 0; j < 100; j++){
						blocksIds << player.getBlockIdInWorld(j, i) << ";";
						backgroundIds << player.getBackgroundIdInWorld(j, i) << ";";
					}
				}
				blocksIds << endl;
				backgroundIds << endl;
				dumpDone = true;
			}
			attemptToDump++;
		}
		SaveBitmapToFile(img, 1000, 600, 24, (LPCTSTR)fileName3.c_str());
		blocksIds.close();
		backgroundIds.close();
		if (dumpDone)
		{
			ofstream wList;
			wList.open("list.txt", ios::app);
			wList << worldName << endl;
			wList.close();
			ofstream fTime;
			fTime.open(dTime.c_str(), ios::out);
			fTime << time(0);
			fTime.close();
			cout << "World dumped! (" << fileName << ")" << endl;
		}
		else {
			cout << "World dump failed!!!" << endl;
			ofstream failedW;
			failedW.open("failedWorlds.txt", ios::app);
			failedW << fileName << endl;
			failedW.close();
		}
		//here we left world
		trainer.mouseClick(985, 50, 100);
		sleep(500);
		trainer.mouseClick(520, 136, 100);
		sleep(500);

		//here we increase counter
		a++;
		if (a == 26)
		{
			if (worldNameLen == 1)
			{
				worldNameLen = 2;
			} else {
				b = b + 1;
			}
			a = 0;
		}
		if (b == 26)
		{
			if (worldNameLen == 2)
			{
				worldNameLen = 3;
			} else {
				c = c + 1;
			}
			b = 0;
		}
		if (c == 26)
		{
			cout << "World dump is done!" << endl;
			_getch();
		}
	}
	return 0;
}

