#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <comutil.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include <string>
using namespace std;
#include "iflash.h"
#include <amvideo.h>

class CSwfObject
{
private:
	string m_filename;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	double m_tstart;
	double m_tend;
	unsigned char m_alpha;
	unsigned int m_key_color;

	Graphics *m_g;
	
	RECTL m_rectl;
	HDC m_hdccomp;
	HBITMAP m_bitmap;
	CAxWindow *m_axwnd;

	CComPtr<IShockwaveFlash> m_flash;
	CComPtr<IViewObjectEx> m_viewobject;

	void ReleaseInterfaces();
	void ReleaseGDI();
public:
	CSwfObject();
	~CSwfObject();
	void SetGraphics(Graphics *p);
	HRESULT Init(string filename, int x, int y, int width, int height, double tstart,	double tend,unsigned char alpha, unsigned int keycolor);
	void Reset();
	void Draw(REFERENCE_TIME time);
};

