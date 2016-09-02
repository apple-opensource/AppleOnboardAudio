/*
 * Copyright (c) 1998-2001 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 *
 * Interface definition for the TAS3004 audio Controller
 *
 * HISTORY
 *
 */

#ifndef _APPLETAS3004AUDIO_H
#define _APPLETAS3004AUDIO_H

#include "TAS_hw.h"
#include "AppleDBDMAAudio.h"
#include "AppleOnboardAudio.h"
#include "AudioHardwareObjectInterface.h"

class IORegistryEntry;

class AppleTAS3004Audio : public AudioHardwareObjectInterface
{
    OSDeclareDefaultStructors(AppleTAS3004Audio);

private:
	Boolean					mDisableLoadingEQFromFile;

	SInt32					minVolume;
	SInt32					maxVolume;
	Boolean					gVolMuteActive;
	Boolean					headphonesActive;
	Boolean					lineOutActive;
	Boolean					headphonesConnected;
	Boolean					lineOutConnected;								
	Boolean					dallasSpeakersConnected;
	DRCInfo					drc;										// dynamic range compression info
	UInt32					layoutID;									// The ID of the machine we're running on
	UInt32					familyID;									// The ID of the speakers that are plugged in (required for rom verification)
	UInt32					speakerID;									// The ID of the speakers that are plugged in
	UInt32					detectCollection;
	TAS3004_ShadowReg		shadowTAS3004Regs;							// write through shadow registers for TAS3004
	Boolean					semaphores;
	UInt32					deviceID;
	AppleOnboardAudio *		mAudioDeviceProvider;

	static const UInt8		kDEQAddress;								// Address for i2c TAS3004

public:
    // Classical Unix driver functions
    virtual bool		init (OSDictionary *properties);
    virtual void		free ();
	virtual bool		start (IOService * provider);
	virtual bool 		willTerminate (IOService * provider, IOOptionBits options);
	virtual bool 		requestTerminate (IOService * provider, IOOptionBits options);

    // Initializatioin
    virtual bool		preDMAEngineInit () ;
	virtual void		postDMAEngineInit ();
	virtual void		initPlugin (PlatformInterface* inPlatformObject);

    // IO activation functions
    virtual UInt32		getActiveOutput (void);
    virtual IOReturn	setActiveOutput (UInt32 outputPort);
    virtual UInt32		getActiveInput (void);
    virtual IOReturn	setActiveInput (UInt32 input);
	virtual IOReturn 	prepareForOutputChange (void);
   
    // control function
    virtual bool		getMute (void);
    virtual IOReturn	setMute (bool mutestate);
	virtual	UInt32		getMaximumdBVolume (void);
	virtual	UInt32		getMinimumdBVolume (void);
	virtual	UInt32		getMaximumVolume (void);
	virtual	UInt32		getMinimumVolume (void);
	virtual	UInt32		getMaximumdBGain (void);
	virtual	UInt32		getMinimumdBGain (void);
	virtual	UInt32		getMaximumGain (void);
	virtual	UInt32		getMinimumGain (void);

    virtual bool		setVolume (UInt32 leftVolume, UInt32 rightVolume);

    virtual IOReturn	setPlayThrough (bool playthroughstate);

	virtual	void		setEQ ( UInt32 inEQIndex );
	virtual	void		disableEQ (void);

	virtual	void		notifyHardwareEvent ( UInt32 statusSelector, UInt32 newValue ) { return; }
	virtual	void		recoverFromFatalError ( FatalRecoverySelector selector );

	virtual	UInt32		getCurrentSampleFrame (void);
	virtual void		setCurrentSampleFrame (UInt32 value);

	virtual IOReturn	performDeviceWake ();
	virtual IOReturn	performDeviceSleep ();

	virtual IOReturn	setSampleRate ( UInt32 sampleRate );
	virtual IOReturn	setSampleDepth ( UInt32 sampleDepth );

	virtual UInt32		getClockLock ( void ) { return 0; }
	virtual IOReturn	breakClockSelect ( UInt32 clockSource );
	virtual IOReturn	makeClockSelect ( UInt32 clockSource );

private:
	// activation functions
	IOReturn			SetVolumeCoefficients (UInt32 left, UInt32 right);
	IOReturn			SetAmplifierMuteState (UInt32 ampID, Boolean muteState);
    IOReturn			SetMixerState ( UInt32 mixerState );								
	IOReturn			InitEQSerialMode (UInt32 mode, Boolean restoreOnNormal);
	IOReturn 			GetShadowRegisterInfo( UInt8 regAddr, UInt8 ** shadowPtr, UInt8* registerSize );
	IOReturn			CODEC_Initialize ();
	void				CODEC_LogRegisters ();
    void				CODEC_Reset ( void );
	IOReturn			CODEC_ReadRegister (UInt8 regAddr, UInt8* registerData);
	IOReturn			CODEC_WriteRegister (UInt8 regAddr, UInt8* registerData, UInt8 mode);
	void				SetBiquadInfoToUnityAllPass (void);
	void				SetUnityGainAllPass (void);
	IOReturn			SndHWSetDRC( DRCInfoPtr theDRCSettings );
	IOReturn			GetCustomEQCoefficients ( UInt32 inEQIndex, EQPrefsElementPtr * eqPrefs );
	IOReturn			SndHWSetOutputBiquad( UInt32 streamID, UInt32 biquadRefNum, FourDotTwenty *biquadCoefficients );
	IOReturn			SndHWSetOutputBiquadGroup( UInt32 biquadFilterCount, FourDotTwenty *biquadCoefficients );
	IOReturn			SetOutputBiquadCoefficients (UInt32 streamID, UInt32 biquadRefNum, UInt8 *biquadCoefficients);
	void				SelectOutputAndLoadEQ ( void );
	IOReturn			SetAnalogPowerDownMode( UInt8 mode );
	IOReturn			ToggleAnalogPowerDownWake( void );
	IORegistryEntry *	FindEntryByProperty (const IORegistryEntry * start, const char * key, const char * value);
	IORegistryEntry *	FindEntryByNameAndProperty (const IORegistryEntry * start, const char * name, const char * key, UInt32 value);
	Boolean				HasInput (void);

	//	The normal volume range is from 0.0 dB to -70 dB.  A setting of -70.5 dB results in a muted state.
	//	A value of 0 represents -70.5 dB.  Volume increases 0.5 dB per step.  A value of 141 represents
	//	-70.5 dB + ( 0.5 dB X 141 ) = - 70.0 dB + 70.5 = 0.0 dB.  The absolute maximum available volume
	//	is +18.0 dB.  A value of 177 represents -70.5 dB + ( 0.5 X 177 ) = -70.5 dB + 88.5 dB.
	enum  {
		kMaximumVolume = 141,
		kMinimumVolume = 0,
		kInitialVolume = 101
	};
	
	enum {
		kInternalSpeakerActive	= 1,
		kHeadphonesActive		= 2,
		kExternalSpeakersActive	= 4
	};

	static const EQPrefsPtr 	kEQPrefsPtr;

#if 0
			// User Client calls
	virtual UInt8		readGPIO (UInt32 selector);
	virtual void		writeGPIO (UInt32 selector, UInt8 data);
	virtual Boolean		getGPIOActiveState (UInt32 gpioSelector);
	virtual void		setGPIOActiveState ( UInt32 selector, UInt8 gpioActiveState );
	virtual Boolean		checkGpioAvailable ( UInt32 selector );
	virtual IOReturn	readHWReg32 ( UInt32 selector, UInt32 * registerData );
	virtual IOReturn	writeHWReg32 ( UInt32 selector, UInt32 registerData );
	virtual IOReturn	readCodecReg ( UInt32 selector, void * registerData,  UInt32 * registerDataSize );
	virtual IOReturn	writeCodecReg ( UInt32 selector, void * registerData );
	virtual IOReturn	readSpkrID ( UInt32 selector, UInt32 * speakerIDPtr );
	virtual IOReturn	getCodecRegSize ( UInt32 selector, UInt32 * codecRegSizePtr );
	virtual	IOReturn	getVolumePRAM ( UInt32 * pramDataPtr );
	virtual IOReturn	getDmaState ( UInt32 * dmaStatePtr );
	virtual IOReturn	getStreamFormat ( IOAudioStreamFormat * streamFormatPtr );
	virtual IOReturn	readPowerState ( UInt32 selector, IOAudioDevicePowerState * powerState );
	virtual IOReturn	setPowerState ( UInt32 selector, IOAudioDevicePowerState powerState );
	virtual IOReturn	setBiquadCoefficients ( UInt32 selector, void * biquadCoefficients, UInt32 coefficientSize );
	virtual IOReturn	getBiquadInformation ( UInt32 scalarArg1, void * outStructPtr, IOByteCount * outStructSizePtr );
	virtual IOReturn	getProcessingParameters ( UInt32 scalarArg1, void * outStructPtr, IOByteCount * outStructSizePtr );
	virtual IOReturn	setProcessingParameters ( UInt32 scalarArg1, void * inStructPtr, UInt32 inStructSize );
	virtual	IOReturn	invokeInternalFunction ( UInt32 functionSelector, void * inData );
#endif
};

#endif /* _APPLETAS3004AUDIO_H */
