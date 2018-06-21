#include "stdafx.h"
#include "ASPEditor.h"

#include <Ime.h>

constexpr static bool IsSucceeded = true;
constexpr static bool IsFailed = false;
constexpr static const wchar_t* defaultResultPath = L"./_Result.asp";


ASPEditor::ASPEditor(LPDIRECT3DDEVICE9 device)
	: m_device(device)
	
	, m_refTex(nullptr)
{
	m_device->AddRef();
}
ASPEditor::~ASPEditor()
{
	m_device->Release();
}



void ASPEditor::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
