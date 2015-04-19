#include "interface.h"
#include "painter.h"

class CSwfFilter : public CTransformFilter,
		 public ISwfFilter
{

public:

    DECLARE_IUNKNOWN;
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    // Reveals ISwfFilter and ISpecifyPropertyPages
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    // Overrriden from CTransformFilter base class
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);
	HRESULT StartStreaming();


	// ISwfFilter implementation
	STDMETHODIMP AddSwf(char *filename, int x, int y, int width, int height, double tstart,	double tend,unsigned char alpha, unsigned int keycolor);
private:

    // Constructor
    CSwfFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);

    // Look after doing the special effect
    BOOL CanTransformRgb32(const CMediaType *pMediaType) const;
    HRESULT Copy(IMediaSample *pSource, IMediaSample *pDest) const;
    HRESULT Transform(IMediaSample *pMediaSample);

    CCritSec    m_SwfLock;          // Private play critical section
	CSwfPainter m_painter;
}; 
