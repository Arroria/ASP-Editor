#pragma once
#include <vector>
#include <filesystem>

//IMEDevice
#include <Ime.h>
#include "./lib/IMEDevice.h"
#ifndef _DEBUG
#pragma comment (lib, "./lib/IMEDevice.lib")
#else
#pragma comment (lib, "./lib/IMEDeviceD.lib")
#endif



struct ASPE_ASP;
struct ASPE_RefTex;

class ASPE_UI_GridInfo;
class ASPE_UI_ASPInfo;
class ASPE_UI_ASPListInfo;


class ASPEditor
{
	enum class IMEUsage;

private:
	const LPDIRECT3DDEVICE9 m_device;
	D3DXVECTOR3 m_rayCastPlane[4];

	ASPE_UI_GridInfo* m_uiGrid;
	ASPE_UI_ASPInfo* m_uiASP;
	ASPE_UI_ASPListInfo* m_uiASPList;
	

	ASPE_RefTex* m_refTex;
	POINT m_gridInterval;
	POINT m_mousePoint;

	ASPE_ASP* m_asp;
	std::list<ASPE_ASP*> m_aspList;

	IMEDevice* m_imeDevice;
	IMEUsage m_imeUsage;

private:
	bool UpdateMouseUV();

	void ChangeRefTex(const wchar_t* path);
	bool RegistTexture(const std::filesystem::path& path);
	void SetDefaultCamera();
	void CreateRaycastPlane();

	bool OpenFileReferenceWindow(wchar_t* path, bool forSave);

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

struct ASPE_ASP
{
	size_t minU;
	size_t minV;
	size_t maxU;
	size_t maxV;
	std::wstring name;
	ASPE_ASP() : minU(NULL), minV(NULL), maxU(NULL), maxV(NULL) {}
};


class ASPE_UI_GridInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const POINT& gridInterval);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPE_UI_GridInfo(LPDIRECT3DDEVICE9 device);
	~ASPE_UI_GridInfo();
};

class ASPE_UI_ASPInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const ASPE_ASP& asp);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPE_UI_ASPInfo(LPDIRECT3DDEVICE9 device);
	~ASPE_UI_ASPInfo();
};

class ASPE_UI_ASPListInfo
{
private:
	const LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_renderTarget;
	LPD3DXFONT m_font;

public:
	void Render(const std::list<ASPE_ASP*>& aspList);
	LPDIRECT3DTEXTURE9 GetTexture() { return m_renderTarget; }

public:
	ASPE_UI_ASPListInfo(LPDIRECT3DDEVICE9 device);
	~ASPE_UI_ASPListInfo();
};

