#pragma once


struct AtlasPiece;

class Atlas
{
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DTEXTURE9 m_tex;
	D3DXIMAGE_INFO m_texInfo;
	D3DXVECTOR3 m_raycastPlane[4];


	AtlasPiece* m_atlasPiece;
	std::vector<AtlasPiece*> m_atlasPieceList;

	IME_Manager* m_imeManager;
	bool m_isASPSaving;
	bool m_seleteX;
	POINT m_gridDist;

private:
	void ChangeTexture(const std::wstring& path);
	bool GetRaycastUV(D3DXVECTOR2& uv);

public:
	void Update();
	void Render();
	void MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	Atlas(LPDIRECT3DDEVICE9 device);
	~Atlas();
};

struct AtlasPiece
{
	long minU;
	long minV;
	long maxU;
	long maxV;
	std::filesystem::path key;
	AtlasPiece(long _minU, long _minV, long _maxU, long _maxV) 
	: minU(_minU), maxV(_minV), maxU(_maxU), minV(_maxV) {}
};
