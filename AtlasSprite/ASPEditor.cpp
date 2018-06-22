#include "stdafx.h"
#include "ASPEditor.h"

#include <Ime.h>

constexpr static bool IsSucceeded = true;
constexpr static bool IsFailed = false;
constexpr static const wchar_t* defaultResultPath = L"./_Result.asp";


ASPEditor::ASPEditor(LPDIRECT3DDEVICE9 device)
	: m_device(device)

	, m_refTex(nullptr)
	, m_gridInterval{ 0, 0 }

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
			SetDefaultCamera();
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
	if (!m_imeDevice)
	{
		auto IMEDeviceCreate = [this](IMEUsage imeUsage)
		{
			m_imeUsage = imeUsage;
			m_imeDevice = new IMEDevice;
		};

		if (g_inputDevice.IsKeyDown(VK_NUMPAD1))	IMEDeviceCreate(IMEUsage::GridSizeX);
		if (g_inputDevice.IsKeyDown(VK_NUMPAD2))	IMEDeviceCreate(IMEUsage::GridSizeY);
	}
	else
	{
		if (g_inputDevice.IsKeyDown(VK_RETURN))
		{
			switch (m_imeUsage)
			{
			case IMEUsage::GridSizeX:	m_gridInterval.x = _wtoi(m_imeDevice->GetString().data());	break;
			case IMEUsage::GridSizeY:	m_gridInterval.y = _wtoi(m_imeDevice->GetString().data());	break;
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
		D3DXMATRIX w;
		D3DXMatrixScaling(&w, m_refTex->info.Width, m_refTex->info.Height, 1);
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
					(int)m_refTex->info.Width * -0.5f + i,
					(int)m_refTex->info.Height * -0.5f,
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
					(int)m_refTex->info.Width * -0.5f,
					(int)m_refTex->info.Height * 0.5f - i,
					0);
				m_device->SetTransform(D3DTS_WORLD, &(sm * tm));
				SingletonInstance(SimpleDrawer)->DrawLineX(m_device, D3DXCOLOR(1, 1, 1, 0.5f));
			}
		}
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
