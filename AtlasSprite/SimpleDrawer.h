#pragma once
#include "Singleton.h"
class SimpleDrawer :
	public Singleton<SimpleDrawer>
{
private:
	struct PNT_Vertex
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 n;
		D3DXVECTOR2 t;

		static inline HRESULT SetFVF(LPDIRECT3DDEVICE9 device) { return device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1); }

		PNT_Vertex() {}
		PNT_Vertex(const D3DXVECTOR3& _p, const D3DXVECTOR3& _n, const D3DXVECTOR2& _t)
			: p(_p), n(_n), t(_t) {}
	};
	struct PC_Vertex
	{
		D3DXVECTOR3 p;
		D3DCOLOR c;

		static inline HRESULT SetFVF(LPDIRECT3DDEVICE9 device) { return device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE); }

		PC_Vertex() {}
		PC_Vertex(const D3DXVECTOR3& _p, const D3DCOLOR& _c)
			: p(_p), c(_c) {}
	};
	
private:
	PNT_Vertex m_texPlane[4];
	PC_Vertex m_colorPlane[4];
	PC_Vertex m_frame[5];
	PC_Vertex m_line[2];

public:
	HRESULT DrawTexPlane(LPDIRECT3DDEVICE9 device);
	HRESULT DrawColorPlane(LPDIRECT3DDEVICE9 device, const D3DCOLOR& color);
	HRESULT DrawFrame(LPDIRECT3DDEVICE9 device, const D3DCOLOR& color);
	HRESULT DrawLine(LPDIRECT3DDEVICE9 device, const D3DCOLOR& color);

public:
	SimpleDrawer();
	~SimpleDrawer();
};

