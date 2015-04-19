#include "stdafx.h"
#pragma warning( disable : 4995)

CPlayer::CPlayer() :
	m_owner(NULL)
{
	ZeroMemory(m_file, MAX_PATH);
}

CPlayer::~CPlayer()
{
}

void  CPlayer::SetFile(char *file)
{
	lstrcpy(m_file, file);
}

HRESULT CPlayer::Init(HWND owner)
{
	HRESULT hr = S_OK;
	Reset();
	m_owner = owner;
		
	hr = m_gb.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_AsyncReader, L"File source", m_src);
	if (FAILED(hr))
		return hr;

	CComPtr<IFileSourceFilter>fsf;
	hr = m_src.QueryInterface(&fsf);
	if (FAILED(hr))
		return hr;

	USES_CONVERSION;
	LPCOLESTR lpo = A2COLE(m_file);

	hr = fsf->Load(lpo, NULL);
	if (FAILED(hr))
		return hr;

	fsf.Release();

	hr = AddFilter(CLSID_SwfFilter, L"Swf filter", m_swf);
	if (FAILED(hr))
		return hr;

	// Add swfs
	CComPtr<ISwfFilter>iswf;
	hr = m_swf->QueryInterface(IID_ISwfFilter, (void **)&iswf);
	if (FAILED(hr))
		return hr;

	char file1[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,file1);
	lstrcat(file1,"\\test.swf");
	
	iswf->AddSwf(file1,50,50,320,240,0.5,70.0, 255, RGB(255,255,255));
	//iswf->AddSwf(file1,200,200,500,300,0.5,70.0, 255);
	//iswf->AddSwf(file1,300,200,300,200,7.0,9.0, 200);
	
	iswf.Release();
	//

	hr = AddFilter(CLSID_Colour, L"Color Space Converter", m_clr);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_EnhancedVideoRenderer, L"Enhanced Video Renderer", m_evr); 
	if (FAILED(hr))
		return hr;


	hr = ConnectFilters(m_src, m_swf);
	if (FAILED(hr))
		return hr;

	hr = ConnectFilters(m_swf, m_clr);
	if (FAILED(hr))
		return hr;

	hr = ConnectFilters(m_clr, m_evr);
	if (FAILED(hr))
		return hr;
	
	//DWORD reg;
	//AddToRot(m_gb,&reg);

	hr = m_gb.QueryInterface(&m_mc);
	if (FAILED(hr))
		return hr;

		
	hr = m_evr.QueryInterface(&m_gs);
	if (FAILED(hr))
		return hr;

	hr = m_gs->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl
		, (void **)&m_vdc);
	if (FAILED(hr))
		return hr;


	TuneVideoWindow();
	return hr;
}

void CPlayer::TuneVideoWindow()
{
	m_vdc->SetVideoWindow(m_owner);
	UpdateVWPos();
}

void CPlayer::UpdateVWPos()
{
	if (m_vdc)
	{	
		RECT r;
		GetClientRect(m_owner, &r);
		m_vdc->SetVideoPosition(NULL,&r);
	}
}

void CPlayer::RepaintVW()
{
	if (m_vdc)
	{
		m_vdc->RepaintVideo();
	}
}

void CPlayer::Reset()
{
	if (m_mc)
		m_mc.Release();

	if (m_gs)
		m_gs.Release();

	if (m_vdc)
		m_vdc.Release();

	if (m_src)
		m_src.Release();

	if (m_swf)
		m_swf.Release();

	if (m_clr)
		m_clr.Release();

	if (m_evr)
		m_evr.Release();

	if (m_gb)
		m_gb.Release();
}

HRESULT CPlayer::Play()
{
	HRESULT hr = S_OK;
	hr = m_mc->Run();
	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT CPlayer::Pause()
{
	HRESULT hr = S_OK;
	hr = m_mc->Pause();
	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT CPlayer::Stop()
{
	HRESULT hr = S_OK;
	hr = m_mc->Stop();
	m_mc->StopWhenReady();
	if (FAILED(hr))
		return hr;
	return hr;
}


HRESULT CPlayer::AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter> &filter)
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter> flt;
	hr = flt.CoCreateInstance(guid);
	if (FAILED(hr))
		return hr;

	filter = flt;

	hr = m_gb->AddFilter(filter, name);
	if (FAILED(hr))
		return hr;	
	
	return hr;
}

HRESULT CPlayer::ConnectFilters(CComPtr<IBaseFilter>&sflt, CComPtr<IBaseFilter>&dflt)
{
	HRESULT hr = S_OK;
	CComPtr<IEnumPins>senm;
	sflt->EnumPins(&senm);

	CComPtr<IEnumPins>denm;
	dflt->EnumPins(&denm);
	
	CComPtr<IPin>opin;
	ULONG ftch1 = 0;
	while(senm->Next(1,&opin,&ftch1) == S_OK)
	{
		PIN_DIRECTION spd;
		opin->QueryDirection(&spd);
		if (spd == PINDIR_OUTPUT)
		{
			CComPtr<IPin>ipin;
			ULONG ftch2 = 0;
			while(denm->Next(1,&ipin,&ftch2) == S_OK)
			{
				PIN_DIRECTION dpd;
				ipin->QueryDirection(&dpd);
				if (dpd == PINDIR_INPUT)
				{
					hr = m_gb->Connect(opin,ipin);
					if (SUCCEEDED(hr))
						return hr;
				}
				ipin.Release();
			}
		}
		opin.Release();
	}
	return hr;
}

HRESULT CPlayer::AddToRot(IUnknown *graph, DWORD *reg) 
{
    CComPtr<IMoniker> moniker = NULL;
    CComPtr<IRunningObjectTable>rot = NULL;

    if (FAILED(GetRunningObjectTable(0, &rot))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
    StringCchPrintfW(wsz, STRING_LENGTH, L"FilterGraph %08x pid %08x", (DWORD_PTR)graph, GetCurrentProcessId());
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &moniker);
    if (SUCCEEDED(hr)) 
    {
        hr = rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, 
							graph, 
							moniker, 
							reg);
    }
    return hr;
}  

