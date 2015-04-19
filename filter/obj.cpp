#include <windows.h>
#include "obj.h"

const char *g_flash_clsid_c="{D27CDB6E-AE6D-11CF-96B8-444553540000}";

CSwfObject::CSwfObject() :
	m_g(NULL),
	m_filename(""),
	m_hdccomp(NULL),
	m_bitmap(NULL),
	m_flash(NULL),
	m_viewobject(NULL),
	m_axwnd(NULL),
	m_x(0),
	m_y(0),
	m_width(0),
	m_height(0),
	m_tstart(0),
	m_tend(0),
	m_alpha(255),
	m_key_color(RGB(0xab, 0xab, 0xab))

{
}
	
CSwfObject::~CSwfObject()
{
	Reset();
}

HRESULT CSwfObject::Init(string filename, int x, int y,  int width, int height, double tstart,	double tend,unsigned char alpha, unsigned int keycolor)
{
	HRESULT hr = S_OK;
	m_filename = filename;
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_tstart = tstart;
	m_tend = tend;
	m_alpha = alpha;
	m_key_color = keycolor;		

	ReleaseInterfaces();
	ReleaseGDI();

	AtlAxWinInit();

	if (m_axwnd == NULL)
		m_axwnd = new CAxWindow();
	if (m_axwnd->IsWindow())
		m_axwnd->DestroyWindow();

	RECT rc = {0, 0, m_width + 7, m_height + 28};
	AdjustWindowRect(&rc, 0, FALSE);
	HWND hwnd = m_axwnd->Create(NULL, &rc, g_flash_clsid_c, 0);
	RECT client;
	GetWindowRect(hwnd,&client);

	hr = m_axwnd->QueryControl(&m_flash);
	if (FAILED(hr))
		return hr;

	_bstr_t bstrf(m_filename.c_str());
	hr = m_flash->LoadMovie(0,bstrf);
	if (FAILED(hr))
		return hr;

	hr = m_flash.QueryInterface(&m_viewobject);
	if (FAILED(hr))
		return hr;

	m_rectl.left = 0;
	m_rectl.right = m_width;
	m_rectl.top = 0;
	m_rectl.bottom = m_height;

	ReleaseGDI();

	HDC desktop_dc = GetDC(GetDesktopWindow());
	m_hdccomp = CreateCompatibleDC(desktop_dc);	

	BITMAPINFO binfo;
	memset(&binfo,0, sizeof(binfo));
	binfo.bmiHeader.biSize = sizeof(binfo);
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biBitCount = 32;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = m_rectl.right;
	binfo.bmiHeader.biWidth = m_rectl.bottom;
	
	//m_bitmap = CreateDIBSection(m_hdccomp, &binfo, DIB_RGB_COLORS, &frame_buffer, NULL, 0);
	m_bitmap = CreateCompatibleBitmap(desktop_dc,m_rectl.right,m_rectl.bottom);
	SelectObject(m_hdccomp, m_bitmap);
	
	// vietdoor's code start here
	SetMapMode(m_hdccomp, MM_TEXT);	
	return S_OK;
}

void CSwfObject::SetGraphics(Graphics *g)
{
	m_g = g;
}

void CSwfObject::Reset()
{
	ReleaseInterfaces();
	ReleaseGDI();

	if (m_axwnd)
	{
		m_axwnd->DestroyWindow();
		delete m_axwnd;
	}	
}

void CSwfObject::ReleaseInterfaces()
{
	if (m_flash)
		m_flash.Release();

	if (m_viewobject)
		m_viewobject.Release();
}

void CSwfObject::ReleaseGDI()
{
	if (m_bitmap)
		DeleteObject(m_bitmap);
	
	if (m_hdccomp)
		DeleteDC(m_hdccomp);
}

#define UNITS 10000000 

void CSwfObject::Draw(REFERENCE_TIME time)
{
	if (!m_flash)
		return;

	REFERENCE_TIME tstart = m_tstart * 10000000;
	REFERENCE_TIME tend = m_tend * 10000000;

	if ( time >= tend )
	{
		VARIANT_BOOL b;
		m_flash->IsPlaying(&b);
		if (b)
			m_flash->Stop();
		return;
	}

	if ( time >= tstart )
	{

		VARIANT_BOOL b;
		m_flash->IsPlaying(&b);
		if (!b)
			if (FAILED(m_flash->Play()))
				return;

		// Hack
		long num = 0;
		long total = 0;
		m_flash->get_TotalFrames(&total);
		m_flash->get_FrameNum(&num);
		
		if ((num+1) >= total)
			num = 0;
		m_flash->put_FrameNum(num+1);
		//

		m_viewobject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, m_hdccomp, &m_rectl, &m_rectl, NULL, NULL);

		Rect r2;
		r2.X = m_x;
		r2.Y = m_y;
		r2.Width = m_rectl.right ;
		r2.Height = m_rectl.bottom;

		Bitmap *bmpflash = Bitmap::FromHBITMAP(m_bitmap,NULL);
		Color c = m_key_color;
		//bmpflash->GetPixel(0,0,&c);
		ImageAttributes ia;
		
		float f = (float)m_alpha / 255.0;
		ColorMatrix maxtix = { 
            f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		ia.SetColorMatrix(&maxtix, ColorMatrixFlagsDefault, ColorAdjustTypeDefault);
		ia.SetColorKey(c, c, ColorAdjustTypeDefault);


		Status st = m_g->DrawImage(bmpflash,r2,0,0,m_rectl.right,m_rectl.bottom, Unit::UnitPixel, &ia);
		delete bmpflash;
	}
}

