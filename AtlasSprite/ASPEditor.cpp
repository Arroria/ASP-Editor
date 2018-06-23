#include "stdafx.h"
#include "ASPEditor.h"

#include <Ime.h>

constexpr static bool IsSucceeded = true;
constexpr static bool IsFailed = false;
constexpr static const wchar_t* defaultResultPath = L"./_Result.asp";



enum class ASPEditor::IMEUsage
{
	_NULL,

	GridSizeX,
	GridSizeY,

	ASPMinU,
	ASPMinV,
	ASPMaxU,
	ASPMaxV,

	RegistASP,
};



ASPEditor::ASPEditor(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_refTex(nullptr)
	, m_gridInterval{ 0, 0 }
	, m_mousePoint{ -1, -1 }

	, m_asp(nullptr)

	, m_imeDevice(nullptr)
	, m_imeUsage(IMEUsage::_NULL)
{
	m_device->AddRef();
}
ASPEditor::~ASPEditor()
{
	m_device->Release();
}



void ASPEditor::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_imeDevice)
		m_imeDevice->MsgProc(hWnd, uMsg, wParam, lParam);


	auto ApplyPath = [this](const wchar_t* path)
	{
		if (RegistTexture(path))
		{
			SetDefaultCamera();
			CreateRaycastPlane();
		}
	};

	if (uMsg == WM_DROPFILES)
	{
		HDROP hDrop = (HDROP)wParam;
		size_t fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, NULL);
		
		for (size_t fileIndex = 0; fileIndex < fileCount; fileIndex++)
		{
			size_t pathSize = DragQueryFileW(hDrop, fileIndex, nullptr, NULL);
			wchar_t pathBuffer[1024] = { NULL };

			if (pathSize < 1024)
			{
				DragQueryFileW(hDrop, fileIndex, pathBuffer, pathSize + 1);

				ApplyPath(pathBuffer);
			}
			else //파일경로가 예상보다 큼;;
			{
				//귀찮으니까 벡터쓰자
				std::vector<wchar_t> pathBigBuffer(pathSize + 1, NULL);
				DragQueryFileW(hDrop, fileIndex, &pathBigBuffer[0], pathSize + 1);

				ApplyPath(&pathBigBuffer[0]);
			}
		}
		DragFinish(hDrop);
	}
}

void ASPEditor::Update()
{
	if (m_refTex)
	{
		if (UpdateMouseUV())
			;
	}

	//TEMP RANGE
	{
		if (g_inputDevice.IsKeyDown(VK_F9))		cout << "Grid Interval\n- " << m_gridInterval.x << "\n- " << m_gridInterval.y << endl;
		if (g_inputDevice.IsKeyDown(VK_F10))	cout << "Mouse Point\n- " << m_mousePoint.x << "\n- " << m_mousePoint.y << endl;
		if (g_inputDevice.IsKeyDown(VK_F11))
		{
			if (g_inputDevice.IsKeyNone(VK_CONTROL))
				m_asp ?
					cout << "ASP area\n- " << m_asp->minU << "\n- " << m_asp->minV << "\n- " << m_asp->maxU << "\n- " << m_asp->maxV << endl :
					cout << "ASP Not Found" << endl;
			else
			{
				cout << "ASP List\n{" << endl;
				for (auto& asp : m_aspList)
					m_asp ?
					wcout << "ASP : " << asp->name << "\n- " << asp->minU << "\n- " << asp->minV << "\n- " << asp->maxU << "\n- " << asp->maxV << endl :
					wcout << "ASP Not Found" << endl;
				cout << "}" << endl;
			}
		}
	}

	if (m_mousePoint.x != -1)
	{
		auto NewASP = [this]()
		{
			if (!m_asp)
			{
				m_asp = new ASP;
				m_asp->minU = m_asp->maxU = m_mousePoint.x;
				m_asp->minV = m_asp->maxV = m_mousePoint.y;
			}
		};
		if (g_inputDevice.IsKeyPressed(VK_LBUTTON))
		{
			NewASP();
			m_asp->minU = m_mousePoint.x;	if (m_asp->minU > m_asp->maxU) m_asp->minU = m_asp->maxU;
			m_asp->minV = m_mousePoint.y;	if (m_asp->minV > m_asp->maxV) m_asp->minV = m_asp->maxV;
		}
		if (g_inputDevice.IsKeyPressed(VK_RBUTTON))
		{
			NewASP();
			m_asp->maxU = m_mousePoint.x;	if (m_asp->maxU < m_asp->minU) m_asp->maxU = m_asp->minU;
			m_asp->maxV = m_mousePoint.y;	if (m_asp->maxV < m_asp->minV) m_asp->maxV = m_asp->minV;
		}
	}



	if (!m_imeDevice)
	{
		auto IMEDeviceCreate = [this](IMEUsage imeUsage)
		{
			m_imeUsage = imeUsage;
			m_imeDevice = new IMEDevice;
		};

		if (g_inputDevice.IsKeyDown(VK_F5))	IMEDeviceCreate(IMEUsage::GridSizeX);
		if (g_inputDevice.IsKeyDown(VK_F6))	IMEDeviceCreate(IMEUsage::GridSizeY);

		if (g_inputDevice.IsKeyDown(VK_F1))	IMEDeviceCreate(IMEUsage::ASPMinU);
		if (g_inputDevice.IsKeyDown(VK_F2))	IMEDeviceCreate(IMEUsage::ASPMinV);
		if (g_inputDevice.IsKeyDown(VK_F3))	IMEDeviceCreate(IMEUsage::ASPMaxU);
		if (g_inputDevice.IsKeyDown(VK_F4))	IMEDeviceCreate(IMEUsage::ASPMaxV);

		if (g_inputDevice.IsKeyDown(VK_F8))	IMEDeviceCreate(IMEUsage::RegistASP);
	}
	else
	{
		if (g_inputDevice.IsKeyDown(VK_RETURN))
		{
			switch (m_imeUsage)
			{
			case IMEUsage::GridSizeX:	m_gridInterval.x = _wtoi(m_imeDevice->GetString().data());	break;
			case IMEUsage::GridSizeY:	m_gridInterval.y = _wtoi(m_imeDevice->GetString().data());	break;

			case IMEUsage::ASPMinU:		if (m_asp)	m_asp->minU = _wtoi(m_imeDevice->GetString().data());	break;
			case IMEUsage::ASPMinV:		if (m_asp)	m_asp->minV = _wtoi(m_imeDevice->GetString().data());	break;
			case IMEUsage::ASPMaxU:		if (m_asp)	m_asp->maxU = _wtoi(m_imeDevice->GetString().data());	break;
			case IMEUsage::ASPMaxV:		if (m_asp)	m_asp->maxV = _wtoi(m_imeDevice->GetString().data());	break;

			case IMEUsage::RegistASP:
				if (m_asp)
				{
					m_asp->name = m_imeDevice->GetString();
					m_aspList.push_back(m_asp);
					m_asp = nullptr;
				}
				break;
			}
			SAFE_DELETE(m_imeDevice);
		}
	}
}

void ASPEditor::Render()
{
	if (!m_refTex)
		return;



	//텍스쳐 렌더
	{
		const auto& texWidth = m_refTex->info.Width;
		const auto& texHeight = m_refTex->info.Height;

		D3DXMATRIX w;
		D3DXMatrixScaling(&w, texWidth, texHeight, 1);
		m_device->SetTransform(D3DTS_WORLD, &w);

		//텍스쳐
		m_device->SetTexture(0, m_refTex->texture);
		SingletonInstance(SimpleDrawer)->DrawTexPlane(m_device);

		//텍스쳐 테두리
		m_device->SetTexture(0, nullptr);
		SingletonInstance(SimpleDrawer)->DrawFrame(m_device, D3DXCOLOR(1, 0, 1, 1));

		//그리드
		if (m_gridInterval.x > 0)
		{
			D3DXMATRIX sm;
			D3DXMatrixScaling(&sm, 1, m_refTex->info.Height, 1);

			for (size_t i = m_gridInterval.x; i < m_refTex->info.Height; i += m_gridInterval.x)
			{
				D3DXMATRIX tm;
				D3DXMatrixTranslation(&tm,
					(int)texWidth * -0.5f + i,
					(int)texHeight * -0.5f,
					0);
				m_device->SetTransform(D3DTS_WORLD, &(sm * tm));
				SingletonInstance(SimpleDrawer)->DrawLineY(m_device, D3DXCOLOR(1, 1, 1, 0.5f));
			}
		}
		if (m_gridInterval.y > 0)
		{
			D3DXMATRIX sm;
			D3DXMatrixScaling(&sm, m_refTex->info.Width, 1, 1);
			
			for (size_t i = m_gridInterval.y; i < m_refTex->info.Width; i += m_gridInterval.y)
			{
				D3DXMATRIX tm;
				D3DXMatrixTranslation(&tm,
					(int)texWidth * -0.5f,
					(int)texHeight * 0.5f - i,
					0);
				m_device->SetTransform(D3DTS_WORLD, &(sm * tm));
				SingletonInstance(SimpleDrawer)->DrawLineX(m_device, D3DXCOLOR(1, 1, 1, 0.5f));
			}
		}

		//ASP Area
		if (m_asp)
		{
			D3DXMATRIX pivot, sm, tm;
			D3DXMatrixTranslation(&pivot, 0.5f, -0.5f, 0);
			D3DXMatrixScaling(&sm, m_asp->maxU - m_asp->minU, m_asp->maxV - m_asp->minV, 1);
			D3DXMatrixTranslation(&tm, (int)texWidth * -0.5f + m_asp->minU, (int)texHeight * 0.5f - m_asp->minV, 0);
			m_device->SetTransform(D3DTS_WORLD, &(pivot * sm * tm));
			SingletonInstance(SimpleDrawer)->DrawFrame(m_device, D3DXCOLOR(1, 0, 1, 1));
		}

		//크로스헤드
		if (m_mousePoint.x != -1)
		{
			D3DXMATRIX pivotX, pivotY, sm, tm;
			D3DXMatrixTranslation(&pivotX, -0.5f, 0, 0);
			D3DXMatrixTranslation(&pivotY, 0, -0.5f, 0);
			D3DXMatrixScaling(&sm, 3, 3, 3);
			D3DXMatrixTranslation(&tm, (int)texWidth * -0.5f + m_mousePoint.x, (int)texHeight * 0.5f - m_mousePoint.y, 0);
			m_device->SetTransform(D3DTS_WORLD, &(pivotX * sm * tm));	SingletonInstance(SimpleDrawer)->DrawLineX(m_device, D3DXCOLOR(1, 0, 0, 1));
			m_device->SetTransform(D3DTS_WORLD, &(pivotY * sm * tm));	SingletonInstance(SimpleDrawer)->DrawLineY(m_device, D3DXCOLOR(1, 0, 0, 1));
		}
	}
}



bool ASPEditor::UpdateMouseUV()
{
	D3DXVECTOR3 rayPos, rayDir;
	{
		D3DVIEWPORT9 viewPort;
		D3DXMATRIX invViewM, projM;
		POINT mousePos = g_inputDevice.MousePos();
		m_device->GetViewport(&viewPort);
		m_device->GetTransform(D3DTS_VIEW, &invViewM);
		m_device->GetTransform(D3DTS_PROJECTION, &projM);
		D3DXMatrixInverse(&invViewM, 0, &invViewM);

		rayDir.x = (2 * mousePos.x / (float)viewPort.Width - 1) / projM._11;
		rayDir.y = (-2 * mousePos.y / (float)viewPort.Height + 1) / projM._22;
		rayDir.z = 1;
		D3DXVec3TransformNormal(&rayDir, &rayDir, &invViewM);
		D3DXVec3Normalize(&rayDir, &rayDir);

		rayPos.x = invViewM._41;
		rayPos.y = invViewM._42;
		rayPos.z = invViewM._43;
	}

	float u, v, dist;
	if (D3DXIntersectTri(&m_rayCastPlane[0], &m_rayCastPlane[1], &m_rayCastPlane[2], &rayPos, &rayDir, &u, &v, &dist))
		; //Nothing need
	else if (D3DXIntersectTri(&m_rayCastPlane[3], &m_rayCastPlane[2], &m_rayCastPlane[1], &rayPos, &rayDir, &u, &v, &dist))
	{
		u = 1 - u;
		v = 1 - v;
	}
	else
	{
		m_mousePoint.x = m_mousePoint.y = -1;
		return IsFailed;
	}

	const auto& texWidth = m_refTex->info.Width;
	const auto& texHeight = m_refTex->info.Height;
	
	m_mousePoint.x = u * texWidth + 0.5f;
	m_mousePoint.y = v * texHeight + 0.5f;
	return IsSucceeded;
}



bool ASPEditor::RegistTexture(const std::filesystem::path & path)
{
	ASPE_RefTex* refTex(nullptr);
	if (!(refTex = new ASPE_RefTex))
		return IsFailed;

	if (FAILED(D3DXCreateTextureFromFileExW(m_device, path.wstring().data(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, &refTex->info, nullptr, &refTex->texture)))
	{
		delete refTex;
		return IsFailed;
	}

	refTex->path = path;


	m_refTex = refTex;
	return IsSucceeded;
}

void ASPEditor::SetDefaultCamera()
{
	//텍스쳐 크기에 화면을 맞춰줌
	D3DVIEWPORT9 viewport;
	m_device->GetViewport(&viewport);
	D3DXIMAGE_INFO& info = m_refTex->info;


	D3DXMATRIX viewM;
	ZeroMemory(&viewM, sizeof(D3DXMATRIX));


	SingletonInstance(Camera)->SetViewScale(
		info.Width / (float)viewport.Width > info.Height / (float)viewport.Height ?
		info.Width : info.Height
	);
	SingletonInstance(Camera)->SetFocus(D3DXVECTOR2(0, 0));
}

void ASPEditor::CreateRaycastPlane()
{
	m_rayCastPlane[0] = D3DXVECTOR3((int)m_refTex->info.Width * -0.5f, (int)m_refTex->info.Height * +0.5f, 0);
	m_rayCastPlane[1] = D3DXVECTOR3((int)m_refTex->info.Width * +0.5f, (int)m_refTex->info.Height * +0.5f, 0);
	m_rayCastPlane[2] = D3DXVECTOR3((int)m_refTex->info.Width * -0.5f, (int)m_refTex->info.Height * -0.5f, 0);
	m_rayCastPlane[3] = D3DXVECTOR3((int)m_refTex->info.Width * +0.5f, (int)m_refTex->info.Height * -0.5f, 0);
}
