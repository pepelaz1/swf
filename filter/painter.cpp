#include <windows.h>
#include "painter.h"

CComModule _Module;

CSwfPainter::CSwfPainter(void) :
	m_flash(NULL),
	m_viewobject(NULL),
	m_hdccomp(NULL),
	m_bitmap(NULL),
	m_axwnd(NULL),
	m_g(NULL),
	m_bmp(NULL)
{
}
	
CSwfPainter::~CSwfPainter(void)
{
	Reset();
}

void  CSwfPainter::SetVideoFormat(VIDEOINFO *pvi)
{
	CopyMemory(&m_vi, pvi, sizeof(VIDEOINFO));
	m_bmp = new Bitmap(m_vi.bmiHeader.biWidth, m_vi.bmiHeader.biHeight, PixelFormat32bppRGB);
	m_g  = new Graphics(m_bmp);

	list<CSwfObject *>::iterator it = m_objects.begin();
	for(;it != m_objects.end(); ++it)
		(*it)->SetGraphics(m_g);
}

HRESULT CSwfPainter::Init()
{
	CoInitialize(NULL);
	GdiplusStartupInput gdiplusStartupInput; 
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	return S_OK;
}

void CSwfPainter::Reset()
{
	ClearObjects();
	ReleaseInterfaces();
	ReleaseGDI();

	delete m_g;
	delete m_bmp;
	
	if (m_axwnd)
	{
		m_axwnd->DestroyWindow();
		delete m_axwnd;
	}

	GdiplusShutdown(m_gdiplusToken);
	CoUninitialize();
}

void CSwfPainter::ClearObjects()
{
	list<CSwfObject *>::iterator it = m_objects.begin();
	for(;it != m_objects.end(); ++it)
		delete (*it);
	m_objects.clear();
}

void CSwfPainter::ReleaseInterfaces()
{
	if (m_flash)
		m_flash.Release();

	if (m_viewobject)
		m_viewobject.Release();
}

void CSwfPainter::ReleaseGDI()
{
	if (m_bitmap)
		DeleteObject(m_bitmap);
	
	if (m_hdccomp)
		DeleteDC(m_hdccomp);
}

void CSwfPainter::Add(string filename, int x, int y, int width, int height, double tstart, double tend, unsigned char alpha, unsigned int keycolor)
{
	CSwfObject *o = new CSwfObject();
	o->Init(filename, x, y, width, height, tstart, tend, alpha, keycolor);
	m_objects.push_back(o);
}

void CSwfPainter::OnFrame(unsigned char *p, int size, REFERENCE_TIME time)
{
	// Copy bitmap bits into output buffer
	BitmapData bd;
	Rect r(0,0,m_vi.bmiHeader.biWidth, abs(m_vi.bmiHeader.biHeight));
	m_bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppRGB, &bd);           
	BYTE *src = p + (m_vi.bmiHeader.biHeight-1)*bd.Stride;
	BYTE *dst = (BYTE *)bd.Scan0;
	for ( int i = 0; i < abs(m_vi.bmiHeader.biHeight); i++)
	{
		CopyMemory(dst, src, bd.Stride);
		src -= bd.Stride;
		dst += bd.Stride;   
	}
	m_bmp->UnlockBits(&bd);

	list<CSwfObject *>::iterator it = m_objects.begin();
	for(;it != m_objects.end(); ++it)
		(*it)->Draw(time);
	
	// Copy bitmap bits into output buffer
	BitmapData bd1;
	Rect r1(0,0,m_vi.bmiHeader.biWidth, abs(m_vi.bmiHeader.biHeight));
	m_bmp->LockBits(&r, ImageLockModeRead, PixelFormat32bppRGB, &bd1);           
	BYTE *out = p + (m_vi.bmiHeader.biHeight-1)*bd1.Stride;
	BYTE *in = (BYTE *)bd1.Scan0;
	for ( int i = 0; i < abs(m_vi.bmiHeader.biHeight); i++)
	{
		CopyMemory(out, in, bd1.Stride);
		in += bd1.Stride;
		out -= bd1.Stride;   
	}
	m_bmp->UnlockBits(&bd1);
}