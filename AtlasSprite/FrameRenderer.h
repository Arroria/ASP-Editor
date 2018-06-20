#pragma once
#include "Singleton.h"
class FrameRenderer :
	public Singleton<FrameRenderer>
{
private:
	struct FrameVertex
	{
		D3DXVECTOR3 p;
		D3DCOLOR c;
		FrameVertex() { ZeroMemory(this, sizeof(FrameVertex)); }
		FrameVertex(const D3DXVECTOR3& _p, const D3DCOLOR& _c) : p(_p), c(_c) {}

		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
	};
	FrameVertex m_frameVertex[5];

public:
	HRESULT Render(LPDIRECT3DDEVICE9 device, const D3DXCOLOR& color);

public:
	FrameRenderer();
	~FrameRenderer();
};

