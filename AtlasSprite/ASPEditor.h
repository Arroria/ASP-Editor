#pragma once
#include <vector>
#include <filesystem>

//InputDevice
#include "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDevice.h"
#ifndef _DEBUG
#pragma comment (lib, "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDevice.lib")
#else
#pragma comment (lib, "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDeviceD.lib")
#endif

//IMEDevice
#include "C:/.Arroria/Arroria SDK/IMEDevice/lib/IMEDevice.h"
#ifndef _DEBUG
#pragma comment (lib, "C:/.Arroria/Arroria SDK/IMEDevice/lib/IMEDevice.lib")
#else
#pragma comment (lib, "C:/.Arroria/Arroria SDK/IMEDevice/lib/IMEDeviceD.lib")
#endif

struct ASPE_RefTex;
class ASPEditor
{
	enum class IMEUsage
	{
		_NULL,
		GridSizeX,
		GridSizeY,
	};

private:
	const LPDIRECT3DDEVICE9 m_device;

	ASPE_RefTex* m_refTex;
	POINT m_gridInterval;


	IMEDevice* m_imeDevice;
	IMEUsage m_imeUsage;

private:
	bool RegistTexture(const std::filesystem::path& path);
	void SetDefaultCamera();

public:
	void MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Update();
	void Render();

public:
	ASPEditor(LPDIRECT3DDEVICE9 device);
	~ASPEditor();
};


struct ASPE_RefTex
{
	LPDIRECT3DTEXTURE9 texture;
	D3DXIMAGE_INFO info;
	std::filesystem::path path;
	ASPE_RefTex() : texture(nullptr), path() { ZeroMemory(&info, sizeof(D3DXIMAGE_INFO)); }
};