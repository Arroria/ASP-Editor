#pragma once
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <windowsx.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <array>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <locale>
#include <bitset>
using std::cout;
using std::wcout;
using std::endl;



#include <shellapi.h>
#include <chrono>
#include <random>
extern std::random_device g_randomDevice;



#include "ProcessManager.h"
extern ProcessManager* g_processManager;
#define DEVICE (g_processManager->GetDevice())
D3DXMATRIX MatrixPerspectiveBySprite(float width, float height);



//InputDevice
#include "./lib/InputDevice.h"
#ifndef _DEBUG
#pragma comment (lib, "./lib/InputDevice.lib")
#else
#pragma comment (lib, "./lib/InputDeviceD.lib")
#endif
extern InputDevice g_inputDevice;



//Singleton Header
#include "SimpleDrawer.h"

#include "LineRenderer.h"
#include "FrameRenderer.h"
#include "PlaneRenderer.h"
#include "Camera.h"


#include "./../../IME/IME/IME_Manager.h"



extern LPD3DXSPRITE g_sprtie;
extern D3DXMATRIX g_identityMatrix;
