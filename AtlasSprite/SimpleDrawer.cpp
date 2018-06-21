#include "stdafx.h"
#include "SimpleDrawer.h"


SimpleDrawer::SimpleDrawer()
{
	m_texPlane[0] = PNT_Vertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0, 0));
	m_texPlane[1] = PNT_Vertex(D3DXVECTOR3(+0.5f, +0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(1, 0));
	m_texPlane[2] = PNT_Vertex(D3DXVECTOR3(-0.5f, -0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(0, 1));
	m_texPlane[3] = PNT_Vertex(D3DXVECTOR3(+0.5f, -0.5f, 0), D3DXVECTOR3(0, 0, -1), D3DXVECTOR2(1, 1));

	
	m_colorPlane[0] = PC_Vertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_colorPlane[1] = PC_Vertex(D3DXVECTOR3(+0.5f, +0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_colorPlane[2] = PC_Vertex(D3DXVECTOR3(-0.5f, -0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_colorPlane[3] = PC_Vertex(D3DXVECTOR3(+0.5f, -0.5f, 0), D3DXCOLOR(0, 0, 0, 1));

	m_frame[0] = PC_Vertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_frame[1] = PC_Vertex(D3DXVECTOR3(+0.5f, +0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_frame[2] = PC_Vertex(D3DXVECTOR3(+0.5f, -0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_frame[3] = PC_Vertex(D3DXVECTOR3(-0.5f, -0.5f, 0), D3DXCOLOR(0, 0, 0, 1));
	m_frame[4] = PC_Vertex(D3DXVECTOR3(-0.5f, +0.5f, 0), D3DXCOLOR(0, 0, 0, 1));

	m_line[0] = PC_Vertex(D3DXVECTOR3(0, 0, 0), D3DXCOLOR(0, 0, 0, 1));
	m_line[1] = PC_Vertex(D3DXVECTOR3(0, 0, 0), D3DXCOLOR(0, 0, 0, 1));
}
SimpleDrawer::~SimpleDrawer()
{
}


HRESULT SimpleDrawer::DrawTexPlane(LPDIRECT3DDEVICE9 device)
{
	PNT_Vertex::SetFVF(device);
	return device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_texPlane[0], sizeof(PNT_Vertex));
}

HRESULT SimpleDrawer::DrawColorPlane(LPDIRECT3DDEVICE9 device, const D3DCOLOR & color)
{
	for (size_t i = 0; i < 4; i++)
		m_colorPlane->c = color;

	PC_Vertex::SetFVF(device);
	return device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &m_colorPlane[0], sizeof(PC_Vertex));
}

HRESULT SimpleDrawer::DrawFrame(LPDIRECT3DDEVICE9 device, const D3DCOLOR & color)
{
	for (size_t i = 0; i < 5; i++)
		m_frame->c = color;

	PC_Vertex::SetFVF(device);
	return device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &m_frame[0], sizeof(PC_Vertex));
}

HRESULT SimpleDrawer::DrawLine(LPDIRECT3DDEVICE9 device, const D3DCOLOR & color)
{
	for (size_t i = 0; i < 2; i++)
		m_line->c = color;

	PC_Vertex::SetFVF(device);
	return device->DrawPrimitiveUP(D3DPT_LINELIST, 1, &m_line[0], sizeof(PC_Vertex));
}
