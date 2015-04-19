#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <comutil.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include "iflash.h"
#include <string>
#include <list>
using namespace std;
#include <amvideo.h>
#include "obj.h"

class CSwfPainter
{
private:
	string m_filename;

	ULONG_PTR m_gdiplusToken;
	Graphics *m_g;
	Bitmap *m_bmp;
	VIDEOINFO m_vi;

	CComPtr<IShockwaveFlash> m_flash;
	CComPtr<IViewObjectEx> m_viewobject;
	
	RECTL m_rectl;
	HDC m_hdccomp;
	HBITMAP m_bitmap;
	CAxWindow *m_axwnd;

	list<CSwfObject *>m_objects;
	void ClearObjects();
	void ReleaseInterfaces();
	void ReleaseGDI();
public:
	CSwfPainter();
	~CSwfPainter();
	HRESULT Init();
	void Reset();
	void SetVideoFormat(VIDEOINFO *pvi);
	void Add(string filename, int x, int y, int width, int height, double tstart, double tend, unsigned char alpha, unsigned int keycolor);
	void OnFrame(unsigned char *p, int size, REFERENCE_TIME time);
};

