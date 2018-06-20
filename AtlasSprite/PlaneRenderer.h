#pragma once
#include "Singleton.h"
class PlaneRenderer :
	public Singleton<PlaneRenderer>
{
private:
	struct PlaneVertex
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 n;
		D3DXVECTOR2 t;
		PlaneVertex() { ZeroMemory(this, sizeof(PlaneVertex)); }
		PlaneVertex(const D3DXVECTOR3& _p, const D3DXVECTOR3& _n, const D3DXVECTOR2& _t) : p(_p), n(_n), t(_t) {}

		enum { FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };
	};
	PlaneVertex m_plane[4];

public:
	HRESULT Render(LPDIRECT3DDEVICE9 device);

public:
	PlaneRenderer();
	~PlaneRenderer();
};

