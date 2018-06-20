#include "stdafx.h"
#include "FrameRenderer.h"



FrameRenderer::FrameRenderer()
{
	m_frameVertex[0] = FrameVertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXCOLOR(1, 1, 1, 1));
	m_frameVertex[1] = FrameVertex(D3DXVECTOR3(+0.5f, +0.5f, 0), D3DXCOLOR(1, 1, 1, 1));
	m_frameVertex[2] = FrameVertex(D3DXVECTOR3(+0.5f, -0.5f, 0), D3DXCOLOR(1, 1, 1, 1));
	m_frameVertex[3] = FrameVertex(D3DXVECTOR3(-0.5f, -0.5f, 0), D3DXCOLOR(1, 1, 1, 1));
	m_frameVertex[4] = m_frameVertex[0];
}
FrameRenderer::~FrameRenderer()
{
}



HRESULT FrameRenderer::Render(LPDIRECT3DDEVICE9 device, const D3DXCOLOR& color)
{
	for (size_t i = 0; i < 5; i++)
		m_frameVertex[i].c = color;

	device->SetFVF(FrameVertex::FVF);
	return device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &m_frameVertex[0], sizeof(FrameVertex));
}
