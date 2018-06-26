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

struct ASP;
struct ASPE_RefTex;

class ASPEUI_GridInfo;
class ASPEUI_ASPInfo;
class ASPEUI_ASPListInfo;


class ASPEditor
{
	enum class IMEUsage;

private:
	const LPDIRECT3DDEVICE9 m_device;
	D3DXVECTOR3 m_rayCastPlane[4];

	ASPEUI_GridInfo* m_uiGrid;
	ASPEUI_ASPInfo* m_uiASP;
	ASPEUI_ASPListInfo* m_uiASPList;
	

	ASPE_RefTex* m_refTex;
	POINT m_gridInterval;
	POINT m_mousePoint;

	ASP* m_asp;
	std::list<ASP*> m_aspList;

	IMEDevice* m_imeDevice;
	IMEUsage m_imeUsage;

private:
	bool UpdateMouseUV();

	bool RegistTexture(const std::filesystem::path& path);
	void SetDefaultCamera();
	void CreateRaycastPlane();

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

struct ASP
{
	size_t minU;
	size_t minV;
	size_t maxU;
	size_t maxV;
	std::wstring name;
	ASP() : minU(NULL), minV(NULL), maxU(NULL), maxV(NULL) {}
};


class ASPEUI_GridInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const POINT& gridInterval);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPEUI_GridInfo(LPDIRECT3DDEVICE9 device);
	~ASPEUI_GridInfo();
};

class ASPEUI_ASPInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const ASP& asp);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPEUI_ASPInfo(LPDIRECT3DDEVICE9 device);
	~ASPEUI_ASPInfo();
};

class ASPEUI_ASPListInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const std::list<ASP*>& aspList);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPEUI_ASPListInfo(LPDIRECT3DDEVICE9 device);
	~ASPEUI_ASPListInfo();
};

