#include "stdafx.h"
#include "Atlas.h"


Atlas::Atlas(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_tex(nullptr)


	, m_atlasPiece(nullptr)

	, m_imeManager(nullptr)
	, m_isASPSaving(false)
	, m_seleteX(false)
	, m_gridDist{ 0, 0 }
{
	ZeroMemory(&m_raycastPlane, sizeof(D3DXVECTOR3) * 4);
}
Atlas::~Atlas()
{
}



void Atlas::Update()
{
	if (!m_tex)
		return;

	if (!m_imeManager)
	{
		m_seleteX = false;
		m_seleteX = g_inputDevice.IsKeyDown('A');
		if (m_seleteX || g_inputDevice.IsKeyDown('S'))
			m_imeManager = new IME_Manager;
	}



	if (m_imeManager)
	{
		if (m_isASPSaving)
		{
			if (g_inputDevice.IsKeyDown(VK_RETURN))
			{
				if (m_atlasPiece)
				{
					m_atlasPiece->key = m_imeManager->GetString();
					m_atlasPieceList.push_back(m_atlasPiece);
					m_atlasPiece = nullptr;
				}
				SAFE_DELETE(m_imeManager);
			}
		}
		else
		{
			if (g_inputDevice.IsKeyDown(VK_RETURN))
			{
				(m_seleteX ? m_gridDist.x : m_gridDist.y) = _wtoi(m_imeManager->GetString().data());
				SAFE_DELETE(m_imeManager);
			}
		}
	}
	else
	{
		D3DXVECTOR2 uv;
		if (GetRaycastUV(uv))
		{
			POINT pixel;
			pixel.x = uv.x * m_texInfo.Width + 0.5f;
			pixel.y = uv.y * m_texInfo.Height + 0.5f;
			cout << pixel.x << " " << pixel.y << endl;


			bool lKeyPressed = g_inputDevice.IsKeyPressed(VK_LBUTTON);
			bool rKeyPressed = g_inputDevice.IsKeyPressed(VK_RBUTTON);
			auto NewAtlasPiece = [&, this]()
			{
				if (!m_atlasPiece)
					m_atlasPiece = new AtlasPiece(pixel.x, pixel.y, pixel.x, pixel.y);
			};

			if (lKeyPressed)
			{
				NewAtlasPiece();
				m_atlasPiece->minU = pixel.x;
				m_atlasPiece->minV = pixel.y;
				if (m_atlasPiece->maxU < m_atlasPiece->minU)	m_atlasPiece->minU = m_atlasPiece->maxU;
				if (m_atlasPiece->maxV < m_atlasPiece->minV)	m_atlasPiece->minV = m_atlasPiece->maxV;
			}
			if (rKeyPressed)
			{
				NewAtlasPiece();
				m_atlasPiece->maxU = pixel.x;
				m_atlasPiece->maxV = pixel.y;
				if (m_atlasPiece->maxU < m_atlasPiece->minU)	m_atlasPiece->maxU = m_atlasPiece->minU;
				if (m_atlasPiece->maxV < m_atlasPiece->minV)	m_atlasPiece->maxV = m_atlasPiece->minV;
			}

			if (g_inputDevice.IsKeyDown(VK_BACK))
			{
				SAFE_DELETE(m_atlasPiece);
			}
			else if (g_inputDevice.IsKeyDown(VK_RETURN))
			{
				m_isASPSaving = true;
				m_imeManager = new IME_Manager;
			}
			else if (g_inputDevice.IsKeyDown(VK_F1))
			{
				std::fstream file;
				file.imbue(std::locale("kor"));

				file.open("./_Result.asp", std::ios::out | std::ios::trunc);
				for (auto& asp : m_atlasPieceList)
				{
					file << '\"' << asp->key.string() << "\" " <<
						asp->minU << ' ' <<
						asp->maxU << ' ' <<
						asp->minV << ' ' <<
						asp->maxV << ' ' <<
						endl;
				}
				file.close();
			}
		}
	}

	cout << m_gridDist.x << "GRID" << m_gridDist.y << endl;
}

void Atlas::Render()
{
	m_device->SetRenderState(D3DRS_ZENABLE, false);
	m_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	
	
	
	//Set World Matrix
	if (m_tex)
	{
		D3DXMATRIX sm;
		D3DXMatrixScaling(&sm, m_texInfo.Width, m_texInfo.Height, 1);
		m_device->SetTransform(D3DTS_WORLD, &sm);
	}
	else
	{
		D3DXMATRIX iden;
		D3DXMatrixIdentity(&iden);
		m_device->SetTransform(D3DTS_WORLD, &iden);
	}

	//Tex
	m_device->SetTexture(0, m_tex);
	SingletonInstance(PlaneRenderer)->Render(m_device);

	//Frame
	m_device->SetTexture(0, nullptr);
	SingletonInstance(FrameRenderer)->Render(m_device, D3DXCOLOR(1, 0, 1, 1));

	//AtlasPiece
	if (m_atlasPiece)
	{
		POINT size;
		size.x = m_atlasPiece->maxU - m_atlasPiece->minU;
		size.y = m_atlasPiece->maxV - m_atlasPiece->minV;

		D3DXMATRIX pvt, sm, tm;
		D3DXMatrixTranslation(&pvt, 0.5f, 0.5f, 0);
		D3DXMatrixScaling(&sm, m_atlasPiece->maxU - m_atlasPiece->minU, m_atlasPiece->maxV - m_atlasPiece->minV, 1);
		D3DXMatrixTranslation(&tm, m_atlasPiece->minU - m_texInfo.Width * 0.5f, -m_atlasPiece->maxV + m_texInfo.Height * 0.5f, 0);
		m_device->SetTransform(D3DTS_WORLD, &(pvt * sm * tm));
		SingletonInstance(FrameRenderer)->Render(m_device, D3DXCOLOR(0, 1, 0, 1));


		//XY
		{
			std::wstring str = 
				std::wstring(L"선택 영역\n") +
				L" - minU : " + std::to_wstring(m_atlasPiece->minU) +	L"\n" +
				L" - minV : " + std::to_wstring(m_atlasPiece->minV) +	L"\n" +
				L" - maxU : " + std::to_wstring(m_atlasPiece->maxU) +	L"\n" +
				L" - maxV : " + std::to_wstring(m_atlasPiece->maxV);


			LPD3DXSPRITE sp;
			D3DXCreateSprite(DEVICE, &sp);
			sp->Begin(D3DXSPRITE_ALPHABLEND);

			LPD3DXFONT font;
			D3DXCreateFontW(DEVICE, 20, 0, 0, 0, 0, 1, 0, 0, 0, L"", &font);
			RECT rc;
			SetRect(&rc, 25, 100, 25, 100);
			font->DrawTextW(sp, str.data() , -1, &rc, DT_NOCLIP, D3DXCOLOR(0, 0, 0, 1));
			font->Release();

			sp->End();
			sp->Release();
		}
	}


	//x, y
	{
		LPD3DXSPRITE sp;
		D3DXCreateSprite(DEVICE, &sp);
		sp->Begin(D3DXSPRITE_ALPHABLEND);

		LPD3DXFONT font;
		D3DXCreateFontW(DEVICE, 20, 0, 0, 0, 0, 1, 0, 0, 0, L"", &font);
		RECT rc;
		SetRect(&rc, 25, 25, 25, 25);
		std::wstring _0 = L"그리드 간격"; if (m_imeManager) _0 += std::wstring(L" (") + (m_seleteX ? L"X" : L"Y") + L" 수정중)";
		std::wstring _1 = L" - X : " + (m_imeManager && m_seleteX ? m_imeManager->GetString() : std::to_wstring(m_gridDist.x));
		std::wstring _2 = L" - Y : " + (m_imeManager && !m_seleteX ? m_imeManager->GetString() : std::to_wstring(m_gridDist.y));
		font->DrawTextW(sp, _0.data(), -1, &rc, DT_NOCLIP, D3DXCOLOR(0, 0, 0, 1));	rc.top = rc.bottom += 25;
		font->DrawTextW(sp, _1.data(), -1, &rc, DT_NOCLIP, D3DXCOLOR(0, 0, 0, 1));	rc.top = rc.bottom += 25;
		font->DrawTextW(sp, _2.data(), -1, &rc, DT_NOCLIP, D3DXCOLOR(0, 0, 0, 1));
		font->Release();

		sp->End();
		sp->Release();
	}

	
	//Grid
	{
		int x = 0;
		int y = 0;
		while (m_gridDist.x)
		{
			x += m_gridDist.x;
			if (x < m_texInfo.Width)
			{
				D3DXMATRIX rm, sm, tm;
				D3DXMatrixRotationZ(&rm, -0.5f * D3DX_PI);
				D3DXMatrixScaling(&sm, 1, m_texInfo.Height, 1);
				D3DXMatrixTranslation(&tm, (int)m_texInfo.Width * -0.5f + x, (int)m_texInfo.Height * 0.5f, 0);
				m_device->SetTransform(D3DTS_WORLD, &(rm * sm * tm));
				SingletonInstance(LineRenderer)->Render(m_device, D3DXCOLOR(1, 1, 1, 0.5f));
			}
			else
				break;
		}
		while (m_gridDist.y)
		{
			y += m_gridDist.y;
			if (y < m_texInfo.Height)
			{
				D3DXMATRIX sm, tm;
				D3DXMatrixScaling(&sm, m_texInfo.Width, 1, 1);
				D3DXMatrixTranslation(&tm, (int)m_texInfo.Width * -0.5f, (int)m_texInfo.Height * 0.5f - y, 0);
				m_device->SetTransform(D3DTS_WORLD, &(sm * tm));
				SingletonInstance(LineRenderer)->Render(m_device, D3DXCOLOR(1, 1, 1, 0.5f));
			}
			else
				break;
		}
	}


	m_device->SetRenderState(D3DRS_ZENABLE, true);
	m_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void Atlas::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_imeManager)
		m_imeManager->MsgProc(hWnd, uMsg, wParam, lParam);
		
	switch (uMsg)
	{
	case WM_DROPFILES:
	{
		HDROP dropFile = (HDROP)wParam;

		size_t fileCount = DragQueryFileW(dropFile, 0xFFFFFFFF, nullptr, NULL);
		if (fileCount > 1)	wcout << L"여러개의 파일 호출은 구현되지 않았습니다." << endl;
		for (size_t fileIndex = 0; fileIndex < fileCount; fileIndex++)
		{
			size_t filePathSize = DragQueryFileW(dropFile, fileIndex, nullptr, NULL);
			wchar_t filePath[256] = { NULL };
			DragQueryFileW(dropFile, fileIndex, filePath, filePathSize + 1);
			if (fileIndex)
			{
				wcout << L"사용되지 않은 파일 : " << filePath << endl;
				continue;
			}

			wcout << L"TEST : " << filePath << endl;
			ChangeTexture(filePath);
		}
		DragFinish(dropFile);
	}
	break;
	}
}



void Atlas::ChangeTexture(const std::wstring & path)
{
	//기존 텍스쳐 제거
	if (m_tex)
		SAFE_RELEASE(m_tex);
	
	//텍스쳐 변경
	D3DXCreateTextureFromFileExW(m_device, path.data(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, NULL, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, &m_texInfo, nullptr, &m_tex);


	if (m_tex)
	{
		//텍스쳐 크기에 맞는 레이캐스트 플랜 생성
		float x = m_texInfo.Width	* 0.5f;
		float y = m_texInfo.Height	* 0.5f;
		m_raycastPlane[0] = D3DXVECTOR3(-x, +y, 0);
		m_raycastPlane[1] = D3DXVECTOR3(+x, +y, 0);
		m_raycastPlane[2] = D3DXVECTOR3(-x, -y, 0);
		m_raycastPlane[3] = D3DXVECTOR3(+x, -y, 0);

		//텍스쳐 크기에 화면을 맞춰줌
		D3DVIEWPORT9 viewport;
		m_device->GetViewport(&viewport);

		SingletonInstance(Camera)->SetViewScale(
			m_texInfo.Width / (float)viewport.Width > m_texInfo.Height / (float)viewport.Height ?
			m_texInfo.Width : m_texInfo.Height
		);
		SingletonInstance(Camera)->SetFocus(D3DXVECTOR2(0, 0));
	}

	//AtlasPiece 제거
	SAFE_DELETE(m_atlasPiece);
}

bool Atlas::GetRaycastUV(D3DXVECTOR2& uv)
{
	D3DXVECTOR3 rayPos, rayDir;
	//Get RayData
	{
		POINT mousePos = g_inputDevice.MousePos();
		D3DXMATRIX vm, pm, vInvM;
		D3DVIEWPORT9 viewPort;

		m_device->GetTransform(D3DTS_VIEW, &vm);
		m_device->GetTransform(D3DTS_PROJECTION, &pm);
		D3DXMatrixInverse(&vInvM, 0, &vm);
		m_device->GetViewport(&viewPort);

		rayDir.x = ((mousePos.x * 2.f) / viewPort.Width - 1) / pm._11;
		rayDir.y = ((mousePos.y * -2.f) / viewPort.Height + 1) / pm._22;
		rayDir.z = 1;
		D3DXVec3TransformNormal(&rayDir, &rayDir, &vInvM);
		D3DXVec3Normalize(&rayDir, &rayDir);

		rayPos.x = vInvM._41;
		rayPos.y = vInvM._42;
		rayPos.z = vInvM._43;
	}

	float u = NULL, v = NULL, dist = NULL;
	if (D3DXIntersectTri(&m_raycastPlane[0], &m_raycastPlane[1], &m_raycastPlane[2], &rayPos, &rayDir, &u, &v, &dist))
	{
		uv.x = u;
		uv.y = v;
		return true;
	}
	if (D3DXIntersectTri(&m_raycastPlane[3], &m_raycastPlane[2], &m_raycastPlane[1], &rayPos, &rayDir, &u, &v, &dist))
	{
		uv.x = 1 - u;
		uv.y = 1 - v;
		return true;
	}
	return false;
}
