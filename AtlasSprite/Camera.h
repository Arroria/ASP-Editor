#pragma once
#include "Singleton.h"

class Camera :
	public Singleton<Camera>
{
private:
	float m_screenRatio;
	float m_viewScale;
	D3DXVECTOR2 m_focus;

public:
	void SetFocus(const D3DXVECTOR2& focus) { m_focus = focus; }
	void SetViewScale(float viewScale) { m_viewScale = viewScale; }

public:
	void Update();
	void ApplyTransform();

public:
	Camera();
	~Camera();
};

