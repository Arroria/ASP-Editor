#include "stdafx.h"
#include "LineRenderer.h"


LineRenderer::LineRenderer()
{
	m_lineVertex[0] = LineVertex(D3DXVECTOR3(0, 0, 0), D3DXCOLOR(1, 1, 1, 1));
	m_lineVertex[1] = LineVertex(D3DXVECTOR3(1, 0, 0), D3DXCOLOR(1, 1, 1, 1));
}
LineRenderer::~LineRenderer()
{
}



HRESULT LineRenderer::Render(LPDIRECT3DDEVICE9 device, const D3DXCOLOR& color)
{
	m_lineVertex[0].c = color;
	m_lineVertex[1].c = color;

	device->SetFVF(LineVertex::FVF);
	return device->DrawPrimitiveUP(D3DPT_LINELIST, 1, &m_lineVertex[0], sizeof(LineVertex));
}
