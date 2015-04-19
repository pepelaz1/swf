#pragma once

#include <evr.h>
#include "../filter/guids.h"
#include "../filter/interface.h"

using namespace std;
class CPlayer
{
private:
	HWND m_owner;
	char m_file[MAX_PATH];


	CComPtr<IGraphBuilder>m_gb;
	CComPtr<IBaseFilter>m_swf;
	CComPtr<ISwfFilter>m_iswf;
	CComPtr<IBaseFilter>m_src;
	CComPtr<IBaseFilter>m_clr;
	CComPtr<IBaseFilter>m_evr;
	CComPtr<IMFGetService> m_gs;
	CComPtr<IMFVideoDisplayControl> m_vdc;

	CComPtr<IMediaControl>m_mc;
	CComPtr<IVideoWindow>m_vw;
	
	
	HRESULT AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter>&filter);
	HRESULT ConnectFilters(CComPtr<IBaseFilter>&sflt, CComPtr<IBaseFilter>&dflt);
	HRESULT AddToRot(IUnknown *graph, DWORD *reg);
	void TuneVideoWindow();
public:
	CPlayer();
	~CPlayer();

	HRESULT Init(HWND owner);
	void Reset();

	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	void UpdateVWPos();
	void RepaintVW();
	void SetFile(char *file);
};

