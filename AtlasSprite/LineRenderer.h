#pragma once
#include "Singleton.h"
class LineRenderer :
	public Singleton<LineRenderer>
{
private:
	struct LineVertex
	{
		D3DXVECTOR3 p;
		D3DCOLOR c;
		LineVertex() { ZeroMemory(this, sizeof(LineVertex)); }
		LineVertex(const D3DXVECTOR3& _p, const D3DCOLOR& _c) : p(_p), c(_c) {}

		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	};
	LineVertex m_lineVertex[2];

public:
	HRESULT Render(LPDIRECT3DDEVICE9 device, const D3DXCOLOR& color);

public:
	LineRenderer();
	~LineRenderer();
};

