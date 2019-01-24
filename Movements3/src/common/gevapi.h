/****************************************************************************** 
Copyright (c) 2008-2015, Teledyne DALSA Inc.
All rights reserved.

File: gevapi.h
	Public API for GigEVision C library.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:
	-Redistributions of source code must retain the above copyright 
	notice, this list of conditions and the following disclaimer. 
	-Redistributions in binary form must reproduce the above 
	copyright notice, this list of conditions and the following 
	disclaimer in the documentation and/or other materials provided 
	with the distribution. 
	-Neither the name of Teledyne DALSA nor the names of its contributors 
	may be used to endorse or promote products derived 
	from this software without specific prior written permission. 

===============================================================================
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#pragma once

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef int16_t Gev_Status;


#define GEVLIB_SUCCESS 0
#define GEVLIB_ERROR_GENERIC                 -1    // Generic Error. A catch-all for unexpected behaviour.
#define GEVLIB_ERROR_NULL_PTR                -2    // NULL pointer passed to function or the result of a cast operation
#define GEVLIB_ERROR_ARG_INVALID			   	-3		// Passed argument to a function is not valid            												   
#define GEVLIB_ERROR_INVALID_HANDLE	         -4		// Invalid Handle
#define GEVLIB_ERROR_NOT_SUPPORTED           -5    // This version of hardware/fpga does not support this feature
#define GEVLIB_ERROR_TIME_OUT                -6    // Timed out waiting for a resource
#define GEVLIB_ERROR_NOT_IMPLEMENTED         -10   // Function / feature is not implemented.
#define GEVLIB_ERROR_NO_CAMERA			      -11   // The action can't be execute because the camera is not connected.
#define GEVLIB_ERROR_INVALID_PIXEL_FORMAT    -12   // Pixel Format is invalid (not supported or not recognized)
#define GEVLIB_ERROR_PARAMETER_INVALID       -13   // Passed Parameter (could be inside a data structure) is invalid/out of range.
#define GEVLIB_ERROR_SOFTWARE                -14   // software error, unexpected result
#define GEVLIB_ERROR_API_NOT_INITIALIZED     -15   // API has not been initialized
#define GEVLIB_ERROR_DEVICE_NOT_FOUND	      -16   // Device/camera specified was not found.
#define GEVLIB_ERROR_ACCESS_DENIED		      -17   // API will not access the device/camera/feature in the specified manner.
#define GEVLIB_ERROR_NOT_AVAILABLE		      -18   // Feature / function is not available for access (but is implemented).
#define GEVLIB_ERROR_NO_SPACE    		      -19   // The data being written to a feature is too large for the feature to store.

// Resource errors.
#define GEVLIB_ERROR_SYSTEM_RESOURCE          -2001 // Error creating a system resource
#define GEVLIB_ERROR_INSUFFICIENT_MEMORY	    -2002 // error allocating memory
#define GEVLIB_ERROR_INSUFFICIENT_BANDWIDTH   -2003 // Not enough bandwidth to perform operation and/or acquisition
#define GEVLIB_ERROR_RESOURCE_NOT_ALLOCATED   -2004 // Resource is not currently allocated
#define GEVLIB_ERROR_RESOURCE_IN_USE          -2005 // resource is currently being used.
#define GEVLIB_ERROR_RESOURCE_NOT_ENABLED     -2006 // The resource(feature) is not enabled
#define GEVLIB_ERROR_RESOURCE_NOT_INITIALIZED -2007 // Resource has not been intialized.
#define GEVLIB_ERROR_RESOURCE_CORRUPTED       -2008 // Resource has been corrupted
#define GEVLIB_ERROR_RESOURCE_MISSING         -2009 // A resource (ie.DLL) needed could not located
#define GEVLIB_ERROR_RESOURCE_LACK            -2010 // Lack of resource to perform a request.
#define GEVLIB_ERROR_RESOURCE_ACCESS          -2011 // Unable to correctly access the resource.
#define GEVLIB_ERROR_RESOURCE_INVALID         -2012 // A specified resource does not exist.
#define GEVLIB_ERROR_RESOURCE_LOCK            -2013 // resource is currently lock
#define GEVLIB_ERROR_INSUFFICIENT_PRIVILEGE   -2014 // Need administrator privilege.
#define GEVLIB_ERROR_RESOURCE_WRITE_PROTECTED -2015 // No data can be written to the resource
#define GEVLIB_ERROR_RESOURCE_INCOHERENCY     -2016 // The required resources are not valid together

// Data errors
#define GEVLIB_ERROR_DATA_NO_MESSAGES	      -5001	// no more messages (in fifo, queue, input stream etc)
#define GEVLIB_ERROR_DATA_OVERFLOW           -5002 // data could not be added to fifo, queue, stream etc.
#define GEVLIB_ERROR_DATA_CHECKSUM           -5003 // checksum validation fail
#define GEVLIB_ERROR_DATA_NOT_AVAILABLE		-5004 // data requested isn't available yet
#define GEVLIB_ERROR_DATA_OVERRUN				-5005 // data requested has been overrun by newer data
#define GEVLIB_ERROR_DATA_XFER_ABORT			-5006 // transfer of requested data did not finish
#define GEVLIB_ERROR_DATA_INVALID_HEADER     -5007 // data header is invalid.
#define GEVLIB_ERROR_DATA_ALIGNMENT          -5008 // data is not correctly align.

// Ethernet errors
#define GEVLIB_ERROR_CONNECTION_DROPPED          -11000
#define GEVLIB_ERROR_ANSWER_TIMEOUT              -11001
#define GEVLIB_ERROR_SOCKET_INVALID              -11002
#define GEVLIB_ERROR_PORT_NOT_AVAILABLE          -11003
#define GEVLIB_ERROR_INVALID_IP                  -11004
#define GEVLIB_ERROR_INVALID_CAMERA_OPERATION    -11005
#define GEVLIB_ERROR_INVALID_PACKET              -11006
#define GEVLIB_ERROR_INVALID_CONNECTION_ATTEMPT  -11007
#define GEVLIB_ERROR_PROTOCOL                    -11008
#define GEVLIB_ERROR_WINDOWS_SOCKET_INIT         -11009
#define GEVLIB_ERROR_WINDOWS_SOCKET_CLOSE        -11010
#define GEVLIB_ERROR_SOCKET_CREATE               -11011
#define GEVLIB_ERROR_SOCKET_RELEASE              -11012
#define GEVLIB_ERROR_SOCKET_DATA_SEND            -11013
#define GEVLIB_ERROR_SOCKET_DATA_READ            -11014
#define GEVLIB_ERROR_SOCKET_WAIT_ACKNOWLEDGE     -11015
#define GEVLIB_ERROR_INVALID_INTERNAL_COMMAND    -11016
#define GEVLIB_ERROR_INVALID_ACKNOWLEDGE         -11017
#define GEVLIB_ERROR_PREVIOUS_ACKNOWLEDGE        -11018
#define GEVLIB_ERROR_INVALID_MESSAGE             -11019
#define GEVLIB_ERROR_GIGE_ERROR                  -11020


//===================================================
// Device Level Status Codes (From low-level library)

#define GEV_STATUS_SUCCESS					0x0000		//!< Requested operation was completed successfully.
#define GEV_STATUS_NOT_IMPLEMENTED		0x8001		//!< The request isn't supported by the device.
#define GEV_STATUS_INVALID_PARAMETER	0x8002		//!< At least one parameter provided in the command is invalid (or out of range) for the device
#define GEV_STATUS_INVALID_ADDRESS		0x8003		//!< An attempt was made to access a non existent address space location.
#define GEV_STATUS_WRITE_PROTECT			0x8004		//!< The addressed register must not be written.
#define GEV_STATUS_BAD_ALIGNMENT			0x8005		//!< A badly aligned address offset or data size was specified.
#define GEV_STATUS_ACCESS_DENIED			0x8006		//!< An attempt was made to access an address location which is currently/momentary not accessible.
#define GEV_STATUS_BUSY						0x8007		//!< A required resource to service the request isn't currently available. The request may be retried.
#define GEV_STATUS_LOCAL_PROBLEM			0x8008		//!< A internal problem in the device implementation occurred while processing the request. 
#define GEV_STATUS_MSG_MISMATCH			0x8009		//!< Message mismatch (request and acknowledge don't match)
#define GEV_STATUS_INVALID_PROTOCOL		0x800A		//!< This version of the GVCP protocol is not supported
#define GEV_STATUS_NO_MSG					0x800B		//!< No message received, timeout.
#define GEV_STATUS_PACKET_UNAVAILABLE	0x800C		//!< The request packet is not available anymore.
#define GEV_STATUS_DATA_OVERRUN			0x800D		//!< Internal memory of device overrun (typically for image acquisition)
#define GEV_STATUS_INVALID_HEADER		0x800E		//!< The message header is not valid. Some of its fields do not match the specificiation.

#define GEV_STATUS_ERROR					0x8FFF		//!< Generic error. 

//=======================================
// Public Pixel Format Value Definitions
//=======================================

enum Gev_Pixel_Format
{
	fmtMono8         = 0x01080001,	/* 8 Bit Monochrome Unsigned    */
	fmtMono8Signed   = 0x01080002,	/* 8 Bit Monochrome Signed      */
	fmtMono10        = 0x01100003,	/* 10 Bit Monochrome Unsigned   */
	fmtMono10Packed  = 0x010C0004,	/* 10 Bit Monochrome Packed     */
	fmtMono12        = 0x01100005,	/* 12 Bit Monochrome Unsigned   */
	fmtMono12Packed  = 0x010C0006,	/* 12 Bit Monochrome Packed      */
	fmtMono14        = 0x01100025,	/* 14 Bit Monochrome Unsigned   */
	fmtMono16        = 0x01100007,	/* 16 Bit Monochrome Unsigned   */
	fMtBayerGR8      = 0x01080008,	/* 8-bit Bayer                  */
	fMtBayerRG8      = 0x01080009,	/* 8-bit Bayer                  */
	fMtBayerGB8      = 0x0108000A,	/* 8-bit Bayer                  */
	fMtBayerBG8      = 0x0108000B,	/* 8-bit Bayer                  */
	fMtBayerGR10     = 0x0110000C,	/* 10-bit Bayer                 */
	fMtBayerRG10     = 0x0110000D,	/* 10-bit Bayer                 */
	fMtBayerGB10     = 0x0110000E,	/* 10-bit Bayer                 */
	fMtBayerBG10     = 0x0110000F,	/* 10-bit Bayer                 */
	fMtBayerGR12     = 0x01100010,	/* 12-bit Bayer                 */
	fMtBayerRG12     = 0x01100011,	/* 12-bit Bayer                 */
	fMtBayerGB12     = 0x01100012,	/* 12-bit Bayer                 */
	fMtBayerBG12     = 0x01100013,	/* 12-bit Bayer                 */
	fmtRGB8Packed    = 0x02180014,	/* 8 Bit RGB Unsigned in 24bits */
	fmtBGR8Packed    = 0x02180015,	/* 8 Bit BGR Unsigned in 24bits */
	fmtRGBA8Packed   = 0x02200016,	/* 8 Bit RGB Unsigned           */
	fmtBGRA8Packed   = 0x02200017,	/* 8 Bit BGR Unsigned           */
	fmtRGB10Packed   = 0x02300018,	/* 10 Bit RGB Unsigned          */
	fmtBGR10Packed   = 0x02300019,	/* 10 Bit BGR Unsigned          */
	fmtRGB12Packed   = 0x0230001A,	/* 12 Bit RGB Unsigned          */
	fmtBGR12Packed   = 0x0230001B,	/* 12 Bit BGR Unsigned          */
	fmtRGB10V1Packed = 0x0220001C,	/* 10 Bit RGB custom V1 (32bits)*/
	fmtRGB10V2Packed = 0x0220001D,	/* 10 Bit RGB custom V2 (32bits)*/
	fmtYUV411packed  = 0x020C001E,	/* YUV411 (composite color) */
	fmtYUV422packed  = 0x0210001F,	/* YUV422 (composite color) */
	fmtYUV444packed  = 0x02180020,	/* YUV444 (composite color) */
	fmt_PFNC_YUV422_8 = 0x02100032,	/* YUV 4:2:2 8-bit */
	fmtRGB8Planar     = 0x02180021,	/* RGB8 Planar buffers      */
	fmtRGB10Planar    = 0x02300022,	/* RGB10 Planar buffers     */
	fmtRGB12Planar    = 0x02300023,	/* RGB12 Planar buffers     */
	fmtRGB16Planar    = 0x02300024,	/* RGB16 Planar buffers     */
	fmt_PFNC_BiColorBGRG8   = 0x021000A6, /* Bi-color Blue/Green - Red/Green 8-bit */
	fmt_PFNC_BiColorBGRG10  = 0x022000A9, /* Bi-color Blue/Green - Red/Green 10-bit unpacked */
	fmt_PFNC_BiColorBGRG10p = 0x021400AA, /* Bi-color Blue/Green - Red/Green 10-bit packed */
	fmt_PFNC_BiColorBGRG12  = 0x022000AD, /* Bi-color Blue/Green - Red/Green 12-bit unpacked */
	fmt_PFNC_BiColorBGRG12p = 0x021800AE, /* Bi-color Blue/Green - Red/Green 12-bit packed */
	fmt_PFNC_BiColorRGBG8   = 0x021000A5, /* Bi-color Red/Green - Blue/Green 8-bit */
	fmt_PFNC_BiColorRGBG10  = 0x022000A7, /* Bi-color Red/Green - Blue/Green 10-bit unpacked */
	fmt_PFNC_BiColorRGBG10p = 0x021400A8, /* Bi-color Red/Green - Blue/Green 10-bit packed */
	fmt_PFNC_BiColorRGBG12  = 0x022000AB, /* Bi-color Red/Green - Blue/Green 12-bit unpacked */
	fmt_PFNC_BiColorRGBG12p = 0x021800AC /* Bi-color Red/Green - Blue/Green 12-bit packed */
};

#define GEV_PIXFORMAT_ISMONO		0x01000000
#define GEV_PIXFORMAT_ISCOLOR		0x02000000
#define GEV_PIXFORMAT_ISCUSTOM	0x80000000

#define GEV_PIXEL_FORMAT_MONO				0x0001		//!< Monochrome - each pixel aligned on a byte boundary.
#define GEV_PIXEL_FORMAT_MONO_PACKED	0x0002		//!< Monochrome - pixels packed end to end in memory.	
#define GEV_PIXEL_FORMAT_RGB				0x0004		//!< RGB - each pixel color aligned on a byte boundary.
#define GEV_PIXEL_FORMAT_RGB_PACKED		0x0008		//!< RGB - pixel colors packed end to end in memory.
#define GEV_PIXEL_FORMAT_BAYER			0x0010		//!< RGB - Bayer filter output.	
#define GEV_PIXEL_FORMAT_YUV				0x0020		//!< RGB - Packed YUV.	
#define GEV_PIXEL_FORMAT_RGB_PLANAR		0x0040		//!< RGB - Planar (each color plane in its own memory region).

#define GEV_PIXEL_ORDER_NONE		0x0000				//!< A "don't care" or "not applicable" value.
#define GEV_PIXEL_ORDER_RGB		0x0001				//!< Pixels are RGB (or RG for Bayer).
#define GEV_PIXEL_ORDER_BGR		0x0002				//!< Pixels are BGR (or BG for Bayer).
#define GEV_PIXEL_ORDER_GRB		0x0004				//!< Pixels are GR for Bayer.
#define GEV_PIXEL_ORDER_GBR		0x0008				//!< Pixels are GB for Bayer.
#define GEV_PIXEL_ORDER_RGB10V1	0xF000				//!< Custom format #1 for 10-bit RGB.
#define GEV_PIXEL_ORDER_RGB10V2	0xE000				//!< Custom format #2 for 10-bit RGB.

int GevIsPixelTypeMono (uint32_t pixelType);
int GevIsPixelTypeRGB (uint32_t pixelType);
int GevIsPixelTypeCustom (uint32_t pixelType);
int GevIsPixelTypePacked (uint32_t pixelType);
uint32_t GevGetPixelSizeInBytes (uint32_t pixelType);
uint32_t GevGetPixelDepthInBits (uint32_t pixelType);
uint32_t GevGetRGBPixelOrder (uint32_t pixelType);
Gev_Status GevTranslateRawPixelFormat (uint32_t rawFormat, uint32_t * translatedFormat, uint32_t * bitDepth, uint32_t * order);
const char *GevGetFormatString (uint32_t format);
//=======================================
// Public Access Mode Value Definitions
//=======================================
enum Gev_Access_Mode
{
	GevMonitorMode = 0,
	GevControlMode = 2,
	GevExclusiveMode = 4
};


//====================================================================
// Public Data Structures
//====================================================================

struct Gev_Config_Option
{
	uint32_t version;
	uint32_t logLevel;
	uint32_t numRetries;
	uint32_t command_timeout_ms;
	uint32_t discovery_timeout_ms;
	uint32_t enumeration_port;
	uint32_t gvcp_port_range_start;
	uint32_t gvcp_port_range_end;
};

struct Gev_Camera_Option
{
	uint32_t numRetries;
	uint32_t command_timeout_ms;
	uint32_t heartbeat_timeout_ms;
	uint32_t streamPktSize;				// GVSP max packet size  (less than or equal to MTU size).
	uint32_t streamPktDelay;				// Delay between packets (microseconds) - to tune packet pacing out of NIC.
	uint32_t streamNumFramesBuffered;	// # of frames to buffer (min 2)
	uint32_t streamMemoryLimitMax;		// Maximum amount of memory to use (puts an upper limit on the # of frames to buffer).
	uint32_t streamMaxPacketResends;	// Maximum number of packet resends to allow for a frame (defaults to 100).
	uint32_t streamFrame_timeout_ms;	// Frame timeout (msec) after leader received.
	int32_t  streamThreadAffinity;		// CPU affinity for streaming thread (marshall/unpack/write to user buffer) - default handling is "-1" 
	int32_t  serverThreadAffinity;		// CPU affinity for packet server thread (recv/dispatch) - default handling is "-1"
	uint32_t msgChannel_timeout_ms;
};

struct Gev_Network
{
	int fIPv6;				// GEV is only IPv4 for now.
	uint32_t ipAddr;
	uint32_t ipAddrLow;
	uint32_t ipAddrHigh;
	uint32_t ifIndex;		// Index of network interface (set by system - required for packet interface access).
};

#define GEV_MAX_STRING_LENGTH	64

struct Gev_Camera
{
	int fIPv6;				// GEV is only IPv4 for now.
	uint32_t ipAddr;
	uint32_t ipAddrLow;
	uint32_t ipAddrHigh;
	uint32_t macLow;
	uint32_t macHigh;
	struct Gev_Network host;
	uint32_t mode;
	uint32_t capabilities;
	char manufacturer[GEV_MAX_STRING_LENGTH + 1];
	char model[GEV_MAX_STRING_LENGTH + 1];
	char serial[GEV_MAX_STRING_LENGTH + 1];
	char version[GEV_MAX_STRING_LENGTH + 1];
	char username[GEV_MAX_STRING_LENGTH + 1];
};

typedef void * Gev_Camera_Handle;

// Buffer object structure - returned 
struct Gev_Image
{
	uint32_t state;         // Full/empty state for image buffer
	uint32_t status;        // Frame Status (success, error types) (see below - GEV_FRAME_STATUS_*)
	uint32_t timestamp_hi;
	uint32_t timestamp_lo;
	uint32_t recv_size;     // Received size for buffer (allows variable sized data).
	uint32_t id;            // Block id for image (starts at 1, wraps to 1 at 65535).
	uint32_t h;             // Received heigth (pixels) for this buffer
	uint32_t w;             // Received width (pixels) for ROI in this buffer
	uint32_t x_offset;      // Received x offset for origin of ROI in this buffer
	uint32_t y_offset;      // Received y offset for origin of ROI in this buffer
	uint32_t x_padding;     // Received x padding bytes (invalid data padding end of each line [horizontal invalid])
	uint32_t y_padding;     // Received y padding bytes (invalid data padding end of image [vertical invalid])
	uint32_t d;             // Received depth (bytes per pixel) for this buffer
	uint32_t format;        // Received format for image.
	uint8_t * address;
};

#define GEV_FRAME_STATUS_RECVD     0 // Frame is complete.
#define GEV_FRAME_STATUS_PENDING   1 // Frame is not ready.
#define GEV_FRAME_STATUS_TIMEOUT   2 // Frame was not ready before timeout condition met.
#define GEV_FRAME_STATUS_OVERFLOW  3 // Frame was not complete before the max number of frames to buffer queue was full.
#define GEV_FRAME_STATUS_BANDWIDTH 4 // Frame had too many resend operations due to insufficient bandwidth.
#define GEV_FRAME_STATUS_LOST      5 // Frame had resend operations that failed.
#define GEV_FRAME_STATUS_RELEASED -1 // (Internal) Frame has been released for re-use.



// Buffer cycling control definition
enum GevBufferCyclingMode
{
	GEV_Asynchronous = 0, 
	GEV_SynchronousNextEmpty = 1 
};


uint32_t GevFormatCameraInfo (struct Gev_Camera *pCamera, char *pBuf, uint32_t size);



// Feature types returned (in C API).
#define GENAPI_UNUSED_TYPE    1
#define GENAPI_VALUE_TYPE     0
#define GENAPI_BASE_TYPE      1	
#define GENAPI_INTEGER_TYPE	  2
#define GENAPI_BOOLEAN_TYPE	  3
#define GENAPI_COMMAND_TYPE	  4
#define GENAPI_FLOAT_TYPE     5
#define GENAPI_STRING_TYPE    6
#define GENAPI_REGISTER_TYPE  7
#define GENAPI_CATEGORY_TYPE  8
#define GENAPI_ENUM_TYPE      9
#define GENAPI_ENUMENTRY_TYPE 10





//====================================================================
// Public API
//====================================================================
// API Initialization
Gev_Status GevApiInitialize(void);
Gev_Status GevApiUninitialize(void);

//====================================================================
// API Configuratoin options
Gev_Status GevGetLibraryConfigOptions (struct Gev_Config_Option *options);
Gev_Status GevSetLibraryConfigOptions (struct Gev_Config_Option *options);

//=================================================================================================
// Camera automatic discovery
int GevDeviceCount(void);	// Get the number of Gev devices seen by the system.
Gev_Status GevGetCameraList (struct Gev_Camera *cameras, int maxCameras, int *numCameras); // Automatically detect and list cameras.

Gev_Status GevForceCameraIPAddress (uint32_t macHi, uint32_t macLo, uint32_t IPAddress, uint32_t subnetmask);
Gev_Status GevEnumerateNetworkInterfaces(struct Gev_Network *pIPAddr, uint32_t maxInterfaces, uint32_t * pNumInterfaces );

//=================================================================================================
// Utility function (external) for discovering camera devices.  
Gev_Status GevEnumerateGevDevices(struct Gev_Network *pIPAddr, uint32_t discoveryTimeout, struct Gev_Camera *pDevice, uint32_t maxDevices, uint32_t * pNumDevices );

// Camera Manual discovery/setup 
Gev_Status GevSetCameraList (struct Gev_Camera *cameras, int numCameras); // Manually set camera list from data structure.

//=================================================================================================
// Gige Vision Camera Access
Gev_Status GevOpenCamera (struct Gev_Camera *device, enum Gev_Access_Mode mode, Gev_Camera_Handle *handle);
Gev_Status GevOpenCameraByAddress (unsigned long ip_address, enum Gev_Access_Mode mode, Gev_Camera_Handle *handle);
Gev_Status GevOpenCameraByName (char *name, enum Gev_Access_Mode mode, Gev_Camera_Handle *handle);
Gev_Status GevOpenCameraBySN (char *sn, enum Gev_Access_Mode mode, Gev_Camera_Handle *handle);

Gev_Status GevCloseCamera(Gev_Camera_Handle *handle);

struct Gev_Camera * GevGetCameraInfo (Gev_Camera_Handle handle);

Gev_Status GevGetCameraInterfaceOptions (Gev_Camera_Handle handle, struct Gev_Camera_Option *options);
Gev_Status GevSetCameraInterfaceOptions (Gev_Camera_Handle handle, struct Gev_Camera_Option *options);

//=================================================================================================
// Manual GigeVision access to GenICam XML File
Gev_Status Gev_RetrieveXMLData (Gev_Camera_Handle handle, int size, char *xml_data, int *num_read, int *data_is_compressed );
Gev_Status Gev_RetrieveXMLFile (Gev_Camera_Handle handle, char *file_name, int size, int force_download );

//=================================================================================================
// GenICam XML Feature Node Map manual registration/access functions (for use in C++ code).
Gev_Status GevConnectFeatures (Gev_Camera_Handle handle,  void *featureNodeMap);
void * GevGetFeatureNodeMap (Gev_Camera_Handle handle);

//=================================================================================================
// GenICam XML Feature access functions (C language compatible).
Gev_Status GevGetGenICamXML_FileName (Gev_Camera_Handle handle, int size, char *xmlFileName);
Gev_Status GevInitGenICamXMLFeatures (Gev_Camera_Handle handle, int updateXMLFile);
Gev_Status GevInitGenICamXMLFeatures_FromFile (Gev_Camera_Handle handle, char *xmlFileName);
Gev_Status GevInitGenICamXMLFeatures_FromData (Gev_Camera_Handle handle, int size, void *pXmlData);

Gev_Status GevGetFeatureValue (Gev_Camera_Handle handle, const char *feature_name, int *feature_type, int value_size, void *value);
Gev_Status GevSetFeatureValue (Gev_Camera_Handle handle, const char *feature_name, int value_size, void *value);
Gev_Status GevGetFeatureValueAsString (Gev_Camera_Handle handle, const char *feature_name, int *feature_type, int value_string_size, char *value_string);
Gev_Status GevSetFeatureValueAsString (Gev_Camera_Handle handle, const char *feature_name, const char *value_string);

//=================================================================================================
// Camera image acquisition
Gev_Status GevGetImageParameters(Gev_Camera_Handle handle,uint32_t * width, uint32_t * height, uint32_t * x_offset, uint32_t * y_offset, uint32_t * format);
Gev_Status GevSetImageParameters(Gev_Camera_Handle handle,uint32_t width, uint32_t height, uint32_t x_offset, uint32_t y_offset, uint32_t format);

Gev_Status GevInitImageTransfer (Gev_Camera_Handle handle, enum GevBufferCyclingMode mode, uint32_t numBuffers, uint8_t **bufAddress);
Gev_Status GevInitializeImageTransfer (Gev_Camera_Handle handle, uint32_t numBuffers, uint8_t **bufAddress);
Gev_Status GevFreeImageTransfer (Gev_Camera_Handle handle);
Gev_Status GevStartImageTransfer (Gev_Camera_Handle handle, uint32_t numFrames);
Gev_Status GevStopImageTransfer (Gev_Camera_Handle handle);
Gev_Status GevAbortImageTransfer (Gev_Camera_Handle handle);

Gev_Status GevQueryImageTransferStatus (Gev_Camera_Handle handle, uint32_t * pTotalBuffers, uint32_t * pNumUsed, uint32_t * pNumFree, uint32_t * pNumTrashed, enum GevBufferCyclingMode *pMode);
int GetPixelSizeInBytes (uint32_t pixelType);

// +Coming soon
Gev_Status GevResetImageTransfer (Gev_Camera_Handle handle );
// -Coming soon

Gev_Status GevGetNextImage (Gev_Camera_Handle handle, struct Gev_Image **image_object_ptr, struct timeval *pTimeout);
Gev_Status GevGetImageBuffer (Gev_Camera_Handle handle, void **image_buffer);
Gev_Status GevGetImage (Gev_Camera_Handle handle, struct Gev_Image **image_object);
Gev_Status GevWaitForNextImageBuffer (Gev_Camera_Handle handle, void **image_buffer, uint32_t timeout);
Gev_Status GevWaitForNextImage (Gev_Camera_Handle handle, struct Gev_Image **image_object, uint32_t timeout);

Gev_Status GevReleaseImage (Gev_Camera_Handle handle, struct Gev_Image *image_object_ptr);
Gev_Status GevReleaseImageBuffer (Gev_Camera_Handle handle, void *image_buffer_ptr);



#ifdef __cplusplus
}
#endif


