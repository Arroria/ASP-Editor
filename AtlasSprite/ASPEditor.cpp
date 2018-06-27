#include "stdafx.h"
#include "ASPEditor.h"


//OPENFILENAME, GetOpenFileName 사용
#include <commdlg.h>



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

	CreateASPFile, // 더이상 사용되지 않음
};



ASPEditor::ASPEditor(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_uiGrid(new ASPE_UI_GridInfo(device))
	, m_uiASP(new ASPE_UI_ASPInfo(device))
	, m_uiASPList(new ASPE_UI_ASPListInfo(device))

	
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

				ChangeRefTex(pathBuffer);
			}
			else //파일경로가 예상보다 큼;;
			{
				//귀찮으니까 벡터쓰자
				std::vector<wchar_t> pathBigBuffer(pathSize + 1, NULL);
				DragQueryFileW(hDrop, fileIndex, &pathBigBuffer[0], pathSize + 1);

				ChangeRefTex(&pathBigBuffer[0]);
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

	if (m_mousePoint.x != -1)
	{
		auto NewASP = [this]()
		{
			if (!m_asp)
			{
				m_asp = new ASPE_ASP;
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

		if (g_inputDevice.IsKeyDown(VK_F7))	IMEDeviceCreate(IMEUsage::RegistASP);

		wchar_t path[MAX_PATH] = { NULL };
		if (g_inputDevice.IsKeyDown(VK_F9))
		{
			if (OpenFileReferenceWindow(path, false))
				ChangeRefTex(path);
		}
		if (g_inputDevice.IsKeyDown(VK_F10))
		{
			if (OpenFileReferenceWindow(path, true))
			{
				std::wfstream aspFile;
				std::filesystem::path aspPath(path);
				aspPath.replace_extension(L".asp");
				aspFile.imbue(std::locale("kor"));
				aspFile.open(aspPath, std::ios::out | std::ios::trunc);

				for (auto& aspIter : m_aspList)
				{
					auto& asp = *aspIter;
					aspFile << asp.name << L" " <<
						asp.minU << L" " <<
						asp.minV << L" " <<
						asp.maxU << L" " <<
						asp.maxV << endl;
				}

				aspFile.close();
			}
		}
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
			
			case IMEUsage::CreateASPFile:
				break;
				//더이상 미사용됨
			{
				std::wfstream aspFile;
				std::filesystem::path path(m_imeDevice->GetString());
				path.replace_extension(L".asp");
				aspFile.imbue(std::locale("kor"));
				aspFile.open(path, std::ios::out | std::ios::trunc);

				for (auto& aspIter : m_aspList)
				{
					auto& asp = *aspIter;
					aspFile << asp.name << L" " <<
						asp.minU << L" " <<
						asp.minV << L" " <<
						asp.maxU << L" " <<
						asp.maxV << endl;
				}

				aspFile.close();
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
			SingletonInstance(SimpleDrawer)->DrawFrame(m_device, D3DXCOLOR(0, 1, 0, 1));
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


	m_uiGrid->Render(m_gridInterval);
	if (m_asp)
		m_uiASP->Render(*m_asp);
	m_uiASPList->Render(m_aspList);


	D3DXMATRIX iden;
	D3DXMatrixIdentity(&iden);
	g_sprtie->Begin(D3DXSPRITE_ALPHABLEND);
	g_sprtie->SetTransform(&iden);
	D3DXMATRIX tm = iden;
	D3DXMatrixTranslation(&tm, 30, 30, 0);	g_sprtie->SetTransform(&tm);	g_sprtie->Draw(m_uiGrid->GetTexture(), nullptr, nullptr, nullptr, D3DXCOLOR(1, 1, 1, 1));
	D3DXMatrixTranslation(&tm, 30, 230, 0);	g_sprtie->SetTransform(&tm);	g_sprtie->Draw(m_uiASP->GetTexture(), nullptr, nullptr, nullptr, D3DXCOLOR(1, 1, 1, 1));
	D3DXMatrixTranslation(&tm, 1280 - 230, 30, 0);	g_sprtie->SetTransform(&tm);	g_sprtie->Draw(m_uiASPList->GetTexture(), nullptr, nullptr, nullptr, D3DXCOLOR(1, 1, 1, 1));
	
	g_sprtie->End();
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



void ASPEditor::ChangeRefTex(const wchar_t * path)
{
	if (RegistTexture(path))
	{
		SetDefaultCamera();
		CreateRaycastPlane();
	}
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

bool ASPEditor::OpenFileReferenceWindow(wchar_t* path, bool forSave)
{
	HWND hWnd = g_processManager->GetWndInfo()->hWnd;

	OPENFILENAME OFN;
	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = L"All\0*.*\0Image\0*.png;*.jpg;*.bmp\0ASP File\0*.asp";
	OFN.lpstrFile = path;
	OFN.nMaxFile = MAX_PATH;
	OFN.lpstrInitialDir = L".\\";
	
	if (forSave ? GetSaveFileNameW(&OFN) : GetOpenFileNameW(&OFN))
		return IsSucceeded;
	return IsFailed;
}




ASPE_UI_GridInfo::ASPE_UI_GridInfo(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_renderTarget(nullptr)
{
	constexpr size_t UI_SIZE_X = 200;
	constexpr size_t UI_SIZE_Y = 100;
	D3DXCreateTexture(m_device, UI_SIZE_X, UI_SIZE_Y, NULL, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderTarget);

	D3DXCreateFontW(m_device, 20, 0, FW_DONTCARE, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, nullptr, &m_font);
}
ASPE_UI_GridInfo::~ASPE_UI_GridInfo()
{
	m_renderTarget->Release();
}



void ASPE_UI_GridInfo::Render(const POINT & gridInterval)
{
	LPDIRECT3DSURFACE9 mySurface;
	m_renderTarget->GetSurfaceLevel(0, &mySurface);
	LPDIRECT3DSURFACE9 mainRenderTarget;
	m_device->GetRenderTarget(0, &mainRenderTarget);
	m_device->SetRenderTarget(0, mySurface);
	m_device->Clear(NULL, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 0, NULL);
	g_sprtie->SetTransform(&g_identityMatrix);
	{
		g_sprtie->Begin(D3DXSPRITE_ALPHABLEND);

		RECT rc = { NULL };

		m_font->DrawTextW(g_sprtie, L"Grid Interval"										, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - X : " + std::to_wstring(gridInterval.x)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - Y : " + std::to_wstring(gridInterval.y)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;



		g_sprtie->End();
	}
	m_device->SetRenderTarget(0, mainRenderTarget);
	mainRenderTarget->Release();
	mySurface->Release();
}





ASPE_UI_ASPInfo::ASPE_UI_ASPInfo(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_renderTarget(nullptr)
{
	constexpr size_t UI_SIZE_X = 200;
	constexpr size_t UI_SIZE_Y = 200;
	D3DXCreateTexture(m_device, UI_SIZE_X, UI_SIZE_Y, NULL, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderTarget);

	D3DXCreateFontW(m_device, 20, 0, FW_DONTCARE, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, nullptr, &m_font);
}
ASPE_UI_ASPInfo::~ASPE_UI_ASPInfo()
{
}



void ASPE_UI_ASPInfo::Render(const ASPE_ASP& asp)
{
	LPDIRECT3DSURFACE9 mySurface;
	m_renderTarget->GetSurfaceLevel(0, &mySurface);
	LPDIRECT3DSURFACE9 mainRenderTarget;
	m_device->GetRenderTarget(0, &mainRenderTarget);
	m_device->SetRenderTarget(0, mySurface);
	m_device->Clear(NULL, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 0, NULL);
	g_sprtie->SetTransform(&g_identityMatrix);
	{
		g_sprtie->Begin(D3DXSPRITE_ALPHABLEND);

		RECT rc = { NULL };
		
		m_font->DrawTextW(g_sprtie, L"ASP Information"									, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - min U : " + std::to_wstring(asp.minU)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - min V : " + std::to_wstring(asp.minV)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - max U : " + std::to_wstring(asp.maxU)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		m_font->DrawTextW(g_sprtie, (L" - max V : " + std::to_wstring(asp.maxV)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;



		g_sprtie->End();
	}
	m_device->SetRenderTarget(0, mainRenderTarget);
	mainRenderTarget->Release();
	mySurface->Release();
}





ASPE_UI_ASPListInfo::ASPE_UI_ASPListInfo(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_renderTarget(nullptr)
{
	constexpr size_t UI_SIZE_X = 200;
	constexpr size_t UI_SIZE_Y = 960;
	D3DXCreateTexture(m_device, UI_SIZE_X, UI_SIZE_Y, NULL, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderTarget);

	D3DXCreateFontW(m_device, 20, 0, FW_DONTCARE, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, nullptr, &m_font);
}
ASPE_UI_ASPListInfo::~ASPE_UI_ASPListInfo()
{
}



void ASPE_UI_ASPListInfo::Render(const std::list<ASPE_ASP*>& aspList)
{
	LPDIRECT3DSURFACE9 mySurface;
	m_renderTarget->GetSurfaceLevel(0, &mySurface);
	LPDIRECT3DSURFACE9 mainRenderTarget;
	m_device->GetRenderTarget(0, &mainRenderTarget);
	m_device->SetRenderTarget(0, mySurface);
	m_device->Clear(NULL, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 0, NULL);
	g_sprtie->SetTransform(&g_identityMatrix);
	{
		g_sprtie->Begin(D3DXSPRITE_ALPHABLEND);

		RECT rc = { NULL };
		
		for (auto& pAsp : aspList)
		{
			ASPE_ASP& asp = *pAsp;
			m_font->DrawTextW(g_sprtie, (L"ASP : " + asp.name).data()						, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
			m_font->DrawTextW(g_sprtie, (L" - min U : " + std::to_wstring(asp.minU)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
			m_font->DrawTextW(g_sprtie, (L" - min V : " + std::to_wstring(asp.minV)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
			m_font->DrawTextW(g_sprtie, (L" - max U : " + std::to_wstring(asp.maxU)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
			m_font->DrawTextW(g_sprtie, (L" - max V : " + std::to_wstring(asp.maxV)).data()	, -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));	rc.top = rc.bottom += 30;
		}



		g_sprtie->End();
	}
	m_device->SetRenderTarget(0, mainRenderTarget);
	mainRenderTarget->Release();
	mySurface->Release();
}