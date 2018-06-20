#include "stdafx.h"
#include "PlaneRenderer.h"


PlaneRenderer::PlaneRenderer()
{
	m_plane[0] = PlaneVertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0, 0));
	m_plane[1] = PlaneVertex(D3DXVECTOR3(+0.5f, +0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(1, 0));
	m_plane[2] = PlaneVertex(D3DXVECTOR3(-0.5f, -0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0, 1));
	m_plane[3] = PlaneVertex(D3DXVECTOR3(+0.5f, -0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(1, 1));
}
PlaneRenderer::~PlaneRenderer()
{
}



HRESULT PlaneRenderer::Render(LPDIRECT3DDEVICE9 device)
{
	device->SetFVF(PlaneVertex::FVF);
	return device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_plane[0], sizeof(PlaneVertex));
}
