#pragma once

#define GSP_REBASE_REG(r) ((r)-0x1EB00000)

typedef struct
{
	u32 active_framebuf;//"0=first, 1=second"
	u32 *framebuf0_vaddr;//"Framebuffer virtual address, for the main screen this is the 3D left framebuffer"
	u32 *framebuf1_vaddr;//"For the main screen: 3D right framebuffer address"
	u32 framebuf_widthbytesize;//"Value for 0x1EF00X90, controls framebuffer width"
	u32 format;//"Framebuffer format, this u16 is written to the low u16 for LCD register 0x1EF00X70."
	u32 framebuf_dispselect;//"Value for 0x1EF00X78, controls which framebuffer is displayed"
	u32 unk;//"?"
} GSP_FramebufferInfo;

typedef enum
{
	GSP_RGBA8_OES=0, //pixel_size = 4-bytes
	GSP_BGR8_OES=1, //pixel_size = 3-bytes
	GSP_RGB565_OES=2, //pixel_size = 2-bytes
	GSP_RGB5_A1_OES=3, //pixel_size = 2-bytes
	GSP_RGBA4_OES=4 //pixel_size = 2-bytes
}GSP_FramebufferFormats;

typedef struct//See this for GSP_CaptureInfoEntry and GSP_CaptureInfo: http://3dbrew.org/wiki/GSPGPU:ImportDisplayCaptureInfo
{
	u32 *framebuf0_vaddr;
	u32 *framebuf1_vaddr;
	u32 format;
	u32 framebuf_widthbytesize;
} GSP_CaptureInfoEntry;

typedef struct
{
	GSP_CaptureInfoEntry screencapture[2];
} GSP_CaptureInfo;

typedef enum
{
	GSPEVENT_PSC0 = 0,	// memory fill completed
	GSPEVENT_PSC1,
	GSPEVENT_VBlank0,
	GSPEVENT_VBlank1,
	GSPEVENT_PPF,		// display transfer finished
	GSPEVENT_P3D,		// command list processing finished
	GSPEVENT_DMA,

	GSPEVENT_MAX, // used to know how many events there are
} GSP_Event;

typedef enum
{
	GSPLCD_TOP    = BIT(0),
	GSPLCD_BOTTOM = BIT(1),
	GSPLCD_BOTH   = GSPLCD_TOP | GSPLCD_BOTTOM,
}GSPLCD_Screens;

Result gspInit(void);
void gspExit(void);

Result gspLcdInit(void);
void gspLcdExit(void);

Result gspInitEventHandler(Handle gspEvent, vu8* gspSharedMem, u8 gspThreadId);
void gspExitEventHandler(void);
void gspWaitForEvent(GSP_Event id, bool nextEvent);
#define gspWaitForPSC0() gspWaitForEvent(GSPEVENT_PSC0, false)
#define gspWaitForPSC1() gspWaitForEvent(GSPEVENT_PSC1, false)
#define gspWaitForVBlank() gspWaitForVBlank0()
#define gspWaitForVBlank0() gspWaitForEvent(GSPEVENT_VBlank0, true)
#define gspWaitForVBlank1() gspWaitForEvent(GSPEVENT_VBlank1, true)
#define gspWaitForPPF() gspWaitForEvent(GSPEVENT_PPF, false)
#define gspWaitForP3D() gspWaitForEvent(GSPEVENT_P3D, false)
#define gspWaitForDMA() gspWaitForEvent(GSPEVENT_DMA, false)

Result GSPGPU_AcquireRight(u8 flags);
Result GSPGPU_ReleaseRight(void);
Result GSPGPU_ImportDisplayCaptureInfo(GSP_CaptureInfo *captureinfo);
Result GSPGPU_SaveVramSysArea(void);
Result GSPGPU_RestoreVramSysArea(void);
Result GSPGPU_SetLcdForceBlack(u8 flags);
Result GSPGPU_SetBufferSwap(u32 screenid, GSP_FramebufferInfo *framebufinfo);
Result GSPGPU_FlushDataCache(const void* adr, u32 size);
Result GSPGPU_InvalidateDataCache(const void* adr, u32 size);
Result GSPGPU_WriteHWRegs(u32 regAddr, u32* data, u8 size);
Result GSPGPU_WriteHWRegsWithMask(u32 regAddr, u32* data, u8 datasize, u32* maskdata, u8 masksize);
Result GSPGPU_ReadHWRegs(u32 regAddr, u32* data, u8 size);
Result GSPGPU_RegisterInterruptRelayQueue(Handle eventHandle, u32 flags, Handle* outMemHandle, u8* threadID);
Result GSPGPU_UnregisterInterruptRelayQueue(void);
Result GSPGPU_TriggerCmdReqQueue(void);
Result GSPGPU_SubmitGxCommand(u32* sharedGspCmdBuf, u32 gxCommand[0x8]);

Result GSPLCD_PowerOffBacklight(GSPLCD_Screens screen);
Result GSPLCD_PowerOnBacklight(GSPLCD_Screens screen);
