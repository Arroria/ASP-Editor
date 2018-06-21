#pragma once
#include <vector>
#include <filesystem>

#include "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDevice.h"
#ifndef _DEBUG
#pragma comment (lib, "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDevice.lib")
#else
#pragma comment (lib, "C:/.Arroria/Arroria SDK/InputDevice/lib/InputDeviceD.lib")
#endif

struct ASPE_RefTex;
class ASPEditor
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	ASPE_RefTex* m_refTex;

private:
	bool RegistTexture(const std::filesystem::path& path);
	void SetDefaultCamera();

public:
	void MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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