## EasyAACEncoder ##

**EasyAACEncoder** is an audio transcoding library for converting audio to AAC, organized and developed by the EasyDarwin open-source streaming media service team. It currently supports transcoding from audio formats such as G711a, G711u, G726, and PCM, and supports Linux (32-bit and 64-bit) and ARM platforms.

## Usage Example ##

- **testEasyAACEncoder**: Transcodes G711A/G711U/G726 to AAC through the EasyAACEncoder API.

	Build on Linux:
		
		chmod +x ./Buildit
		./Buildit


## Notes ##
Audio formats currently supported by EasyAACEncoder:

	/* Audio Codec */
	enum Law
	{
		Law_ULaw	=	0, 		/**< U law */
		Law_ALaw	=	1, 		/**< A law */
		Law_PCM16	=	2, 		/**< 16 bit uniform PCM values. Raw PCM data */  
		Law_G726	=	3		/**< G726 */
	};
	
	/* Rate Bits */
	enum Rate
	{
		Rate16kBits=2,	/**< 16k bits per second (2 bits per ADPCM sample) */
		Rate24kBits=3,	/**< 24k bits per second (3 bits per ADPCM sample) */
		Rate32kBits=4,	/**< 32k bits per second (4 bits per ADPCM sample) */
		Rate40kBits=5	/**< 40k bits per second (5 bits per ADPCM sample) */
	};


## Acknowledgements ##

EasyAACEncoder builds on work from the following projects and specifications:

- [EasyDarwin](http://www.easydarwin.org), which organized and developed the original EasyAACEncoder library and API.
- [FAAC](https://www.audiocoding.com/faac.html), the Freeware Advanced Audio Coder used for AAC encoding. The bundled FAAC sources are under `src/libfaac` and `src/include`.
- [Kiss FFT](https://github.com/mborgerding/kissfft), used by the bundled FAAC implementation. The bundled Kiss FFT sources and license are under `src/libfaac/kiss_fft`.
- ITU-T G.711 and G.726 audio codec specifications, which define the telephony audio formats decoded by this library before AAC encoding.

Please review the license notices in the bundled source files before redistributing modified builds.


## More Information ##

Email: [support@easydarwin.org](mailto:support@easydarwin.org) 

Website: [www.EasyDarwin.org](http://www.easydarwin.org)

Copyright &copy; EasyDarwin.org 2012-2019
Copyright &copy; 2026 Yehor Smoliakov
