#ifndef __INTERFACE__
#define __INTERFACE__

#ifdef __cplusplus
extern "C" {
#endif

	// {BE3F9943-0109-45EC-8CBF-971493FF38A7}
	DEFINE_GUID(IID_ISwfFilter, 
	0xbe3f9943, 0x109, 0x45ec, 0x8c, 0xbf, 0x97, 0x14, 0x93, 0xff, 0x38, 0xa7);


    DECLARE_INTERFACE_(ISwfFilter, IUnknown)
    {
		  STDMETHOD(AddSwf) (THIS_
                    char *filename,
					int x,
					int y,
					int width,
					int height,
					double tstart,
					double tend,
					unsigned char alpha,
					unsigned int keycolor
                 ) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __INTERFACE__

