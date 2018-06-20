#include "stdafx.h"
#include "Camera.h"



Camera::Camera()
	: m_screenRatio(1280.f / 960.f)
	, m_viewScale(1)
	, m_focus(0, 0)
{
}
Camera::~Camera()
{
}



void Camera::Update()
{
	int mouseWheel = g_inputDevice.MouseWheel() / 120;
	while (mouseWheel)
	{
		if (mouseWheel < 0)
		{
			m_viewScale *= 1.1f;
			mouseWheel++;
		}
		else
		{
			m_viewScale *= 0.9f;
			mouseWheel --;
		}
	}



	POINT mouseDelta = g_inputDevice.MouseDelta();
	if (g_inputDevice.IsKeyPressed(VK_MBUTTON))
	{
		m_focus.x -= m_viewScale * 0.001f * mouseDelta.x;
		m_focus.y += m_viewScale * 0.001f * mouseDelta.y;
	}
}

void Camera::ApplyTransform()
{
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(m_focus.x, m_focus.y, -1), &D3DXVECTOR3(m_focus.x, m_focus.y, 1), &D3DXVECTOR3(0, 1, 0));
	DEVICE->SetTransform(D3DTS_VIEW, &view);

	auto GetProj = [](float width, float height)->D3DXMATRIX
	{
		D3DXMATRIX proj;
		ZeroMemory(&proj, sizeof(D3DXMATRIX));	proj._44 = 1;
		proj._11 = 2.f / width;
		proj._22 = 2.f / height;
		proj._33 = 1.f / 100;
		return proj;
	};
	DEVICE->SetTransform(D3DTS_PROJECTION, &GetProj(m_screenRatio * m_viewScale, m_viewScale));
}