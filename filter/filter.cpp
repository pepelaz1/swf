#include <windows.h>
#include <streams.h>
#include <initguid.h>
#include "guids.h"
#include "filter.h"
#include "resource.h"
#include "interface.h"


// Setup information

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudpPins[] =
{
    { L"Input",             // Pins string name
      FALSE,                // Is it rendered
      FALSE,                // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
      &sudPinTypes          // Pin information
    },
    { L"Output",            // Pins string name
      FALSE,                // Is it rendered
      TRUE,                 // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
      &sudPinTypes          // Pin information
    }
};

const AMOVIESETUP_FILTER sudSwf =
{
    &CLSID_SwfFilter,         // Filter CLSID
    L"Swf filter",       // String name
    MERIT_DO_NOT_USE,       // Filter merit
    2,                      // Number of pins
    sudpPins                // Pin information
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance

CFactoryTemplate g_Templates[] = {
    { L"Swf filter"
    , &CLSID_SwfFilter
    , CSwfFilter::CreateInstance
    , NULL
    , &sudSwf }
 };
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

//
// DllRegisterServer
//
// Handles sample registry and unregistry
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}


//
// Constructor
//
CSwfFilter::CSwfFilter(TCHAR *tszName,LPUNKNOWN punk,
                   HRESULT *phr) : CTransformFilter(tszName, punk, CLSID_SwfFilter)
{
	HRESULT hr = S_OK;
	hr = m_painter.Init();
	if (FAILED(hr))
		*phr = hr;
		return;	 
} 


CUnknown *CSwfFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    ASSERT(phr);
    
    CSwfFilter *pNewObject = new CSwfFilter(NAME("Swf filter"), punk, phr);

    if (pNewObject == NULL) {
        if (phr)
            *phr = E_OUTOFMEMORY;
    }
    return pNewObject;

} 



STDMETHODIMP CSwfFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_ISwfFilter) {
        return GetInterface((ISwfFilter *) this, ppv);
    }  else {
        return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
    }
} 

HRESULT  CSwfFilter::CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin)
{
	if (direction == PINDIR_OUTPUT)
	{
		AM_MEDIA_TYPE amt;
		HRESULT hr = GetPin(1)->ConnectionMediaType(&amt);
		VIDEOINFO *pvi = (VIDEOINFO *)amt.pbFormat;
		m_painter.SetVideoFormat(pvi);

	}
	return CTransformFilter::CompleteConnect(direction, pReceivePin);
}

HRESULT CSwfFilter::StartStreaming()
{
	HRESULT hr = CTransformFilter::StartStreaming();
	if(FAILED(hr))
		return hr;

	//return m_painter.Play();
	return S_OK;
}

STDMETHODIMP CSwfFilter::AddSwf(char *filename, int x, int y, int width, int height, double tstart, double tend,unsigned char alpha, unsigned int keycolor)
{
	m_painter.Add(filename,x,y,width,height, tstart, tend, alpha, keycolor); 
	return S_OK;
}


HRESULT CSwfFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
    CheckPointer(pIn,E_POINTER);   
    CheckPointer(pOut,E_POINTER);   

    // Copy the properties across

    HRESULT hr = Copy(pIn, pOut);
    if (FAILED(hr)) {
        return hr;
    }

    // Check to see if it is time to do the sample

    CRefTime tStart, tStop ;
    hr = pIn->GetTime((REFERENCE_TIME *) &tStart, (REFERENCE_TIME *) &tStop);

    //if (tStart >= m_effectStartTime) 
    //{
     //   if (tStop <= (m_effectStartTime + m_effectTime)) 
      //  {
            return Transform(pOut);
       // }
    //}

    //return NOERROR;

} // Transform


//
// Copy
//
// Make destination an identical copy of source
//
HRESULT CSwfFilter::Copy(IMediaSample *pSource, IMediaSample *pDest) const
{
    CheckPointer(pSource,E_POINTER);   
    CheckPointer(pDest,E_POINTER);   

    // Copy the sample data

    BYTE *pSourceBuffer, *pDestBuffer;
    long lSourceSize = pSource->GetActualDataLength();

#ifdef DEBUG
    long lDestSize = pDest->GetSize();
    ASSERT(lDestSize >= lSourceSize);
#endif

    pSource->GetPointer(&pSourceBuffer);
    pDest->GetPointer(&pDestBuffer);

    CopyMemory( (PVOID) pDestBuffer,(PVOID) pSourceBuffer,lSourceSize);

    // Copy the sample times

    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pSource->GetTime(&TimeStart, &TimeEnd)) {
        pDest->SetTime(&TimeStart, &TimeEnd);
    }

    LONGLONG MediaStart, MediaEnd;
    if (pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
        pDest->SetMediaTime(&MediaStart,&MediaEnd);
    }

    // Copy the Sync point property

    HRESULT hr = pSource->IsSyncPoint();
    if (hr == S_OK) {
        pDest->SetSyncPoint(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetSyncPoint(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the media type

    AM_MEDIA_TYPE *pMediaType;
    pSource->GetMediaType(&pMediaType);
    pDest->SetMediaType(pMediaType);
    DeleteMediaType(pMediaType);

    // Copy the preroll property

    hr = pSource->IsPreroll();
    if (hr == S_OK) {
        pDest->SetPreroll(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetPreroll(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property

    hr = pSource->IsDiscontinuity();
    if (hr == S_OK) {
    pDest->SetDiscontinuity(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetDiscontinuity(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the actual data length

    long lDataLength = pSource->GetActualDataLength();
    pDest->SetActualDataLength(lDataLength);

    return NOERROR;

} // Copy


//
// Transform (in place)
//
// 'In place' apply the image effect to this sample
//
HRESULT CSwfFilter::Transform(IMediaSample *pMediaSample)
{
    BYTE *pData;                
    long lDataLen;             

    pMediaSample->GetPointer(&pData);
    lDataLen = pMediaSample->GetSize();
	
	REFERENCE_TIME tStart, tStop ;
    pMediaSample->GetTime(&tStart,&tStop);

	m_painter.OnFrame(pData, lDataLen, tStart);
    return NOERROR;

} // Transform (in place)


// Check the input type is OK - return an error otherwise

HRESULT CSwfFilter::CheckInputType(const CMediaType *mtIn)
{
    CheckPointer(mtIn,E_POINTER);

    // check this is a VIDEOINFOHEADER type

    if (*mtIn->FormatType() != FORMAT_VideoInfo) {
        return E_INVALIDARG;
    }

    // Can we transform this type

    if (CanTransformRgb32(mtIn)) {
        return NOERROR;
    }
    return E_FAIL;
}


//
// Checktransform
//
// Check a transform can be done between these formats
//
HRESULT CSwfFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer(mtIn,E_POINTER);
    CheckPointer(mtOut,E_POINTER);

    if (CanTransformRgb32(mtIn)) 
    {
        if (*mtIn == *mtOut) 
        {
            return NOERROR;
        }
    }

    return E_FAIL;

} // CheckTransform


//
// DecideBufferSize
//
// Tell the output pin's allocator what size buffers we
// require. Can only do this when the input is connected
//
HRESULT CSwfFilter::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
    // Is the input pin connected

    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    CheckPointer(pAlloc,E_POINTER);
    CheckPointer(pProperties,E_POINTER);
    HRESULT hr = NOERROR;

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT( Actual.cBuffers == 1 );

    if (pProperties->cBuffers > Actual.cBuffers ||
            pProperties->cbBuffer > Actual.cbBuffer) {
                return E_FAIL;
    }
    return NOERROR;

} // DecideBufferSize


//
// GetMediaType
//
// I support one type, namely the type of the input pin
// This type is only available if my input is connected
//
HRESULT CSwfFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    // Is the input pin connected

    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    // This should never happen

    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    // Do we have more items to offer

    if (iPosition > 0) {
        return VFW_S_NO_MORE_ITEMS;
    }

    CheckPointer(pMediaType,E_POINTER);
    *pMediaType = m_pInput->CurrentMediaType();

    return NOERROR;

} // GetMediaType


//
// CanTransformRgb32
//
// Check if this is a RGB32 true colour format
//
BOOL CSwfFilter::CanTransformRgb32(const CMediaType *pMediaType) const
{
    CheckPointer(pMediaType,FALSE);

    if (IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video)) 
    {
        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB32)) 
        {
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pMediaType->Format();
            return (pvi->bmiHeader.biBitCount == 32);
        }
    }

    return FALSE;

} 

