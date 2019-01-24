#pragma once


#include "gevapi.h"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> //sleep

#define CAM_ASSERT_F ASSERT_F
#define CAM_TRACE_F TRACE_F
#define CAM_LIST_MAX 100
#define CAM_BUFFER_MAX 1000




struct Gev_Cam
{
	Gev_Camera_Handle handle;
	uint32_t buffer_count;
	uint8_t * buffer_data [CAM_BUFFER_MAX];
};


__attribute__ ((unused))
static enum Gev_Pixel_Format const available_format_values [] = 
{
	fmtMono8,
	fmtMono8Signed,
	fmtMono10,
	fmtMono10Packed,
	fmtMono12,
	fmtMono12Packed,
	fmtMono14,
	fmtMono16,
	fMtBayerGR8,
	fMtBayerRG8,
	fMtBayerGB8,
	fMtBayerBG8,
	fMtBayerGR10,
	fMtBayerRG10,
	fMtBayerGB10,
	fMtBayerBG10,
	fMtBayerGR12,
	fMtBayerRG12,
	fMtBayerGB12,
	fMtBayerBG12,
	fmtRGB8Packed,
	fmtBGR8Packed,
	fmtRGBA8Packed,
	fmtBGRA8Packed,
	fmtRGB10Packed,
	fmtBGR10Packed,
	fmtRGB12Packed,
	fmtBGR12Packed,
	fmtRGB10V1Packed,
	fmtRGB10V2Packed,
	fmtYUV411packed,
	fmtYUV422packed,
	fmtYUV444packed,
	fmt_PFNC_YUV422_8,
	fmtRGB8Planar,
	fmtRGB10Planar,
	fmtRGB12Planar,
	fmtRGB16Planar,
	fmt_PFNC_BiColorBGRG8,
	fmt_PFNC_BiColorBGRG10,
	fmt_PFNC_BiColorBGRG10p,
	fmt_PFNC_BiColorBGRG12,
	fmt_PFNC_BiColorBGRG12p,
	fmt_PFNC_BiColorRGBG8,
	fmt_PFNC_BiColorRGBG10,
	fmt_PFNC_BiColorRGBG10p,
	fmt_PFNC_BiColorRGBG12,
	fmt_PFNC_BiColorRGBG12p
};


__attribute__ ((unused))
static char const * available_format_names [] = 
{
	"fmtMono8",
	"fmtMono8Signed",
	"fmtMono10",
	"fmtMono10Packed",
	"fmtMono12",
	"fmtMono12Packed",
	"fmtMono14",
	"fmtMono16",
	"fMtBayerGR8",
	"fMtBayerRG8",
	"fMtBayerGB8",
	"fMtBayerBG8",
	"fMtBayerGR10",
	"fMtBayerRG10",
	"fMtBayerGB10",
	"fMtBayerBG10",
	"fMtBayerGR12",
	"fMtBayerRG12",
	"fMtBayerGB12",
	"fMtBayerBG12",
	"fmtRGB8Packed",
	"fmtBGR8Packed",
	"fmtRGBA8Packed",
	"fmtBGRA8Packed",
	"fmtRGB10Packed",
	"fmtBGR10Packed",
	"fmtRGB12Packed",
	"fmtBGR12Packed",
	"fmtRGB10V1Packed",
	"fmtRGB10V2Packed",
	"fmtYUV411packed",
	"fmtYUV422packed",
	"fmtYUV444packed",
	"fmt_PFNC_YUV422_8",
	"fmtRGB8Planar",
	"fmtRGB10Planar",
	"fmtRGB12Planar",
	"fmtRGB16Planar",
	"fmt_PFNC_BiColorBGRG8",
	"fmt_PFNC_BiColorBGRG10",
	"fmt_PFNC_BiColorBGRG10p",
	"fmt_PFNC_BiColorBGRG12",
	"fmt_PFNC_BiColorBGRG12p",
	"fmt_PFNC_BiColorRGBG8",
	"fmt_PFNC_BiColorRGBG10",
	"fmt_PFNC_BiColorRGBG10p",
	"fmt_PFNC_BiColorRGBG12",
	"fmt_PFNC_BiColorRGBG12p"
};


char const * get_error_name (Gev_Status Status)
{
	switch (Status)
	{
		case GEVLIB_SUCCESS                    : return "GEVLIB_SUCCESS";
		case GEVLIB_ERROR_GENERIC              : return "GEVLIB_ERROR_GENERIC";
		case GEVLIB_ERROR_NULL_PTR             : return "GEVLIB_ERROR_NULL_PTR";
		case GEVLIB_ERROR_ARG_INVALID          : return "GEVLIB_ERROR_ARG_INVALID";
		case GEVLIB_ERROR_INVALID_HANDLE       : return "GEVLIB_ERROR_INVALID_HANDLE";
		case GEVLIB_ERROR_NOT_SUPPORTED        : return "GEVLIB_ERROR_NOT_SUPPORTED";
		case GEVLIB_ERROR_TIME_OUT             : return "GEVLIB_ERROR_TIME_OUT";
		case GEVLIB_ERROR_NOT_IMPLEMENTED      : return "GEVLIB_ERROR_NOT_IMPLEMENTED";
		case GEVLIB_ERROR_NO_CAMERA            : return "GEVLIB_ERROR_NO_CAMERA";
		case GEVLIB_ERROR_INVALID_PIXEL_FORMAT : return "GEVLIB_ERROR_INVALID_PIXEL_FORMAT";
		case GEVLIB_ERROR_PARAMETER_INVALID    : return "GEVLIB_ERROR_PARAMETER_INVALID";
		case GEVLIB_ERROR_SOFTWARE             : return "GEVLIB_ERROR_SOFTWARE";
		case GEVLIB_ERROR_API_NOT_INITIALIZED  : return "GEVLIB_ERROR_API_NOT_INITIALIZED";
		case GEVLIB_ERROR_DEVICE_NOT_FOUND     : return "GEVLIB_ERROR_DEVICE_NOT_FOUND";
		case GEVLIB_ERROR_ACCESS_DENIED        : return "GEVLIB_ERROR_ACCESS_DENIED";
		case GEVLIB_ERROR_NOT_AVAILABLE        : return "GEVLIB_ERROR_NOT_AVAILABLE";
		case GEVLIB_ERROR_NO_SPACE             : return "GEVLIB_ERROR_NO_SPACE";
		
		// Resource errors.
		case GEVLIB_ERROR_SYSTEM_RESOURCE: return "GEVLIB_ERROR_SYSTEM_RESOURCE";
		case GEVLIB_ERROR_INSUFFICIENT_MEMORY: return "GEVLIB_ERROR_INSUFFICIENT_MEMORY";
		case GEVLIB_ERROR_INSUFFICIENT_BANDWIDTH: return "GEVLIB_ERROR_INSUFFICIENT_BANDWIDTH";
		case GEVLIB_ERROR_RESOURCE_NOT_ALLOCATED: return "GEVLIB_ERROR_RESOURCE_NOT_ALLOCATED";
		case GEVLIB_ERROR_RESOURCE_IN_USE: return "GEVLIB_ERROR_RESOURCE_IN_USE";
		case GEVLIB_ERROR_RESOURCE_NOT_ENABLED: return "GEVLIB_ERROR_RESOURCE_NOT_ENABLED";
		case GEVLIB_ERROR_RESOURCE_NOT_INITIALIZED: return "GEVLIB_ERROR_RESOURCE_NOT_INITIALIZED";
		case GEVLIB_ERROR_RESOURCE_CORRUPTED: return "GEVLIB_ERROR_RESOURCE_CORRUPTED";
		case GEVLIB_ERROR_RESOURCE_MISSING: return "GEVLIB_ERROR_RESOURCE_MISSING";
		case GEVLIB_ERROR_RESOURCE_LACK: return "GEVLIB_ERROR_RESOURCE_LACK";
		case GEVLIB_ERROR_RESOURCE_ACCESS: return "GEVLIB_ERROR_RESOURCE_ACCESS";
		case GEVLIB_ERROR_RESOURCE_INVALID: return "GEVLIB_ERROR_RESOURCE_INVALID";
		case GEVLIB_ERROR_RESOURCE_LOCK: return "GEVLIB_ERROR_RESOURCE_LOCK";
		case GEVLIB_ERROR_INSUFFICIENT_PRIVILEGE: return "GEVLIB_ERROR_INSUFFICIENT_PRIVILEGE";
		case GEVLIB_ERROR_RESOURCE_WRITE_PROTECTED: return "GEVLIB_ERROR_RESOURCE_WRITE_PROTECTED";
		case GEVLIB_ERROR_RESOURCE_INCOHERENCY: return "GEVLIB_ERROR_RESOURCE_INCOHERENCY";

		// Data errors
		case GEVLIB_ERROR_DATA_NO_MESSAGES: return "GEVLIB_ERROR_DATA_NO_MESSAGES";
		case GEVLIB_ERROR_DATA_OVERFLOW: return "GEVLIB_ERROR_DATA_OVERFLOW";
		case GEVLIB_ERROR_DATA_CHECKSUM: return "GEVLIB_ERROR_DATA_CHECKSUM";
		case GEVLIB_ERROR_DATA_NOT_AVAILABLE: return "GEVLIB_ERROR_DATA_NOT_AVAILABLE";
		case GEVLIB_ERROR_DATA_OVERRUN: return "GEVLIB_ERROR_DATA_OVERRUN";
		case GEVLIB_ERROR_DATA_XFER_ABORT: return "GEVLIB_ERROR_DATA_XFER_ABORT";
		case GEVLIB_ERROR_DATA_INVALID_HEADER: return "GEVLIB_ERROR_DATA_INVALID_HEADER";
		case GEVLIB_ERROR_DATA_ALIGNMENT: return "GEVLIB_ERROR_DATA_ALIGNMENT";

		// Ethernet errors
		case GEVLIB_ERROR_CONNECTION_DROPPED: return "GEVLIB_ERROR_CONNECTION_DROPPED";
		case GEVLIB_ERROR_ANSWER_TIMEOUT: return "GEVLIB_ERROR_ANSWER_TIMEOUT";
		case GEVLIB_ERROR_SOCKET_INVALID: return "GEVLIB_ERROR_SOCKET_INVALID";
		case GEVLIB_ERROR_PORT_NOT_AVAILABLE: return "GEVLIB_ERROR_PORT_NOT_AVAILABLE";
		case GEVLIB_ERROR_INVALID_IP: return "GEVLIB_ERROR_INVALID_IP";
		case GEVLIB_ERROR_INVALID_CAMERA_OPERATION: return "GEVLIB_ERROR_INVALID_CAMERA_OPERATION";
		case GEVLIB_ERROR_INVALID_PACKET: return "GEVLIB_ERROR_INVALID_PACKET";
		case GEVLIB_ERROR_INVALID_CONNECTION_ATTEMPT: return "GEVLIB_ERROR_INVALID_CONNECTION_ATTEMPT";
		case GEVLIB_ERROR_PROTOCOL: return "GEVLIB_ERROR_PROTOCOL";
		case GEVLIB_ERROR_WINDOWS_SOCKET_INIT: return "GEVLIB_ERROR_WINDOWS_SOCKET_INIT";
		case GEVLIB_ERROR_WINDOWS_SOCKET_CLOSE: return "GEVLIB_ERROR_WINDOWS_SOCKET_CLOSE";
		case GEVLIB_ERROR_SOCKET_CREATE: return "GEVLIB_ERROR_SOCKET_CREATE";
		case GEVLIB_ERROR_SOCKET_RELEASE: return "GEVLIB_ERROR_SOCKET_RELEASE";
		case GEVLIB_ERROR_SOCKET_DATA_SEND: return "GEVLIB_ERROR_SOCKET_DATA_SEND";
		case GEVLIB_ERROR_SOCKET_DATA_READ: return "GEVLIB_ERROR_SOCKET_DATA_READ";
		case GEVLIB_ERROR_SOCKET_WAIT_ACKNOWLEDGE: return "GEVLIB_ERROR_SOCKET_WAIT_ACKNOWLEDGE";
		case GEVLIB_ERROR_INVALID_INTERNAL_COMMAND: return "GEVLIB_ERROR_INVALID_INTERNAL_COMMAND";
		case GEVLIB_ERROR_INVALID_ACKNOWLEDGE: return "GEVLIB_ERROR_INVALID_ACKNOWLEDGE";
		case GEVLIB_ERROR_PREVIOUS_ACKNOWLEDGE: return "GEVLIB_ERROR_PREVIOUS_ACKNOWLEDGE";
		case GEVLIB_ERROR_INVALID_MESSAGE: return "GEVLIB_ERROR_INVALID_MESSAGE";
		case GEVLIB_ERROR_GIGE_ERROR: return "GEVLIB_ERROR_GIGE_ERROR";
		
		default: return "Unknown";
	}
}


char const * get_error_desc (Gev_Status Status)
{
	switch (Status)
	{
		case GEVLIB_SUCCESS                       : return "This mean you did something right.";
		case GEVLIB_ERROR_GENERIC                 : return "Generic Error. A catch: -all for unexpected behaviour.";
		case GEVLIB_ERROR_NULL_PTR                : return "NULL pointer passed to function or the result of a cast operation";
		case GEVLIB_ERROR_ARG_INVALID			  : return "Passed argument to a function is not valid";
		case GEVLIB_ERROR_INVALID_HANDLE	      : return "Invalid Handle";
		case GEVLIB_ERROR_NOT_SUPPORTED           : return "This version of hardware/fpga does not support this feature";
		case GEVLIB_ERROR_TIME_OUT                : return "Timed out waiting for a resource";
		case GEVLIB_ERROR_NOT_IMPLEMENTED         : return "Function / feature is not implemented.";
		case GEVLIB_ERROR_NO_CAMERA			      : return "The action can't be execute because the camera is not connected.";
		case GEVLIB_ERROR_INVALID_PIXEL_FORMAT    : return "Pixel Format is invalid (not supported or not recognized)";
		case GEVLIB_ERROR_PARAMETER_INVALID       : return "Passed Parameter (could be inside a data structure) is invalid/out of range.";
		case GEVLIB_ERROR_SOFTWARE                : return "software error, unexpected result";
		case GEVLIB_ERROR_API_NOT_INITIALIZED     : return "API has not been initialized";
		case GEVLIB_ERROR_DEVICE_NOT_FOUND	      : return "Device/camera specified was not found.";
		case GEVLIB_ERROR_ACCESS_DENIED		      : return "API will not access the device/camera/feature in the specified manner.";
		case GEVLIB_ERROR_NOT_AVAILABLE		      : return "Feature / function is not available for access (but is implemented).";
		case GEVLIB_ERROR_NO_SPACE    		      : return "The data being written to a feature is too large for the feature to store.";
		default: return "Unknown";
	}
}


char const * cam_get_img_error_desc (int status)
{
	switch (status)
	{
		case GEV_FRAME_STATUS_RECVD     : return "Frame is complete.";
		case GEV_FRAME_STATUS_PENDING   : return "Frame is not ready.";
		case GEV_FRAME_STATUS_TIMEOUT   : return "Frame was not ready before timeout condition met";
		case GEV_FRAME_STATUS_OVERFLOW  : return "Frame was not complete before the max number of frames to buffer queue was full";
		case GEV_FRAME_STATUS_BANDWIDTH : return "Frame had too many resend operations due to insufficient bandwidth.";
		case GEV_FRAME_STATUS_LOST      : return "Frame had resend operations that failed.";
		case GEV_FRAME_STATUS_RELEASED  : return "(Internal) Frame has been released for re-use.";
		default: return "Unknown";
	}
}


char const * cam_get_img_error_name (int status)
{
	switch (status)
	{
		case GEV_FRAME_STATUS_RECVD     : return "GEV_FRAME_STATUS_RECVD";
		case GEV_FRAME_STATUS_PENDING   : return "GEV_FRAME_STATUS_PENDING";
		case GEV_FRAME_STATUS_TIMEOUT   : return "GEV_FRAME_STATUS_TIMEOUT";
		case GEV_FRAME_STATUS_OVERFLOW  : return "GEV_FRAME_STATUS_OVERFLOW";
		case GEV_FRAME_STATUS_BANDWIDTH : return "GEV_FRAME_STATUS_BANDWIDTH";
		case GEV_FRAME_STATUS_LOST      : return "GEV_FRAME_STATUS_LOST";
		case GEV_FRAME_STATUS_RELEASED  : return "GEV_FRAME_STATUS_RELEASED";
		default: return "Unknown";
	}
}


int cam_get_str (struct Gev_Cam * cam, char const * name, int len, char * value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	Gev_Status status;
	int type;
	status = GevGetFeatureValueAsString (cam->handle, name, &type, len, value);
	CAM_TRACE_F ("%s %40s %10s : %s (%i)", "GevGetFeatureValueAsString", name, value, get_error_name (status), status);
	return status;
}


int cam_get_u32 (struct Gev_Cam * cam, char const * name, uint32_t * value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	int type = -1;
	Gev_Status status;
	status = GevGetFeatureValue (cam->handle, name, &type, sizeof (uint32_t), value);
	CAM_TRACE_F ("%s %40s %10lu %i : %s (%i)", "GevGetFeatureValue", name, (long unsigned)(*value), type, get_error_name (status), status);
	if (status != GEVLIB_SUCCESS) {goto error;}
	CAM_ASSERT_F (type == 2 || type == 9, "%s", "");
error:
	return status;
}


int cam_get_float (struct Gev_Cam * cam, char const * name, float * value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	int type;
	Gev_Status status;
	status = GevGetFeatureValue (cam->handle, name, &type, sizeof (float), value);
	CAM_TRACE_F ("%s %40s %8f %i : %s (%i)", "GevGetFeatureValue", name, (float)(*value), type, get_error_name (status), status);
	if (status != GEVLIB_SUCCESS) {goto error;}
	CAM_ASSERT_F (type == 5, "%s", "");
error:
	return status;
}


int cam_set_str (struct Gev_Cam * cam, char const * name, char const * value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	Gev_Status status;
	status = GevSetFeatureValueAsString (cam->handle, name, value);
	CAM_TRACE_F ("%s %40s %10s : %s (%i)", "GevSetFeatureValueAsString", name, value, get_error_name (status), status);
	return status;
}


int cam_set_u32 (struct Gev_Cam * cam, char const * name, uint32_t value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	Gev_Status status;
	status = GevSetFeatureValue (cam->handle, name, sizeof (value), &value);
	CAM_TRACE_F ("%s %40s %lu : %s (%i)", "GevSetFeatureValue", name, (long unsigned) value, get_error_name (status), status);
	return status;
}


int cam_set_float (struct Gev_Cam * cam, char const * name, float value)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (name != NULL, "%s", "");
	Gev_Status status;
	status = GevSetFeatureValue (cam->handle, name, sizeof (float), &value);
	CAM_TRACE_F ("%s %40s %8f : %s (%i)", "GevGetFeatureValue", name, value, get_error_name (status), status);
	return status;
}


int cam_parse_line (struct Gev_Cam * cam, char * str)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (str != NULL, "%s", "");
	char * name;
	char * value;
	char * rest = str;
	if (*rest == '#') {return GEVLIB_SUCCESS;}
	name = strtok_r (rest, " \n", &rest);
	if (name == NULL) {return GEVLIB_SUCCESS;}
	//printf ("set %s", name);
	value = strtok_r (rest, " \n", &rest);
	if (value == NULL) {return GEVLIB_SUCCESS;}
	//printf (" %s", value);
	
	Gev_Status status;
	status = cam_set_str (cam, name, value);
	if (status != GEVLIB_SUCCESS) {goto error;}
	return GEVLIB_SUCCESS;
	
error:

	return status;
}


int cam_load (struct Gev_Cam * cam, char * filename)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (filename != NULL, "%s", "");
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Loading feature values from %s" ANSIC_RESET, filename);
	FILE * file;
	file = fopen (filename, "r");
	CAM_ASSERT_F (file != NULL, "%s", "");
	char line [256];
	while (1)
	{
		char * res;
		res = fgets (line, sizeof (line), file);
		if (res == NULL) {break;}
		cam_parse_line (cam, line);
		//printf ("\n");
	}
	fclose (file);
	return GEVLIB_SUCCESS;
}


int cam_start (struct Gev_Cam * cam)
{
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Starting camera%s" ANSIC_RESET, "");
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (cam->buffer_count > 0, "Buffer count need to be positive%s", "");
	Gev_Status status;
	status = GevStartImageTransfer (cam->handle, -1);
	CAM_TRACE_F ("%s : %s (%i)", "GevStartImageTransfer", get_error_name (status), status);
	return status;
}


int cam_stop (struct Gev_Cam * cam)
{
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Stopping camera%s" ANSIC_RESET, "");
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	Gev_Status status;
	status = GevStopImageTransfer (cam->handle);
	CAM_TRACE_F ("%s : %s (%i)", "GevStopImageTransfer", get_error_name (status), status);
	return status;
}


int cam_close (struct Gev_Cam * cam)
{
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Destroying buffers%s" ANSIC_RESET, "");
	CAM_ASSERT_F (cam != NULL, "%s", "");
	Gev_Status status;
	for (size_t I = 0; I < cam->buffer_count; I = I + 1)
	{
		if (cam->buffer_data [I] == NULL) {continue;}
		CAM_TRACE_F ("free %p", cam->buffer_data [I]);
		free (cam->buffer_data [I]);
		cam->buffer_data [I] = NULL;
	}
	cam->buffer_count = 0;
	cam_stop (cam);
	CAM_TRACE_F ("@ Trying to avoid segmentation fault. sleeping 4 sec...%s", "");
	sleep (4);
	CAM_TRACE_F ("@ Trying to avoid segmentation fault. sleeping 4 sec...%s", "");
	//if (cam->handle == NULL) {return GEVLIB_SUCCESS;}
	status = GevCloseCamera (&(cam->handle));
	CAM_TRACE_F ("%s : %s (%i)", "GevCloseCamera", get_error_name (status), status);
	cam->handle = NULL;
	return status;
}


int cam_open (struct Gev_Cam * cam)
{
	cam->handle = NULL;
	cam->buffer_count = 0;
	//cam->buffer_data = NULL;
	int count;
	struct Gev_Camera list [CAM_LIST_MAX];
	Gev_Status status = GEVLIB_ERROR_GENERIC;
	
	
	//Find cameras on network
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Find cameras on network%s" ANSIC_RESET, "");
	status = GevGetCameraList (list, 1, &count);
	if (status != GEVLIB_SUCCESS) {goto error;}
	CAM_TRACE_F ("Camera count: %i", (int) count);
	for (int i = 0; i < count; i = i + 1)
	{
		struct Gev_Camera * c = list + i;
		CAM_TRACE_F ("%i: %s, %s, %s, %s, %s", (int)i, c->manufacturer, c->model, c->serial, c->version, c->username);
	}
	if (status != GEVLIB_SUCCESS) {return status;}
	if (count <= 0) {return GEVLIB_ERROR_GENERIC;}
	
	//Open the first camera
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Open the first camera%s" ANSIC_RESET, "");
	CAM_ASSERT_F (count > 0, "%s", "");
	CAM_ASSERT_F (cam->handle == NULL, "%s", "");
	status = GevOpenCamera (list + 0, GevExclusiveMode, &(cam->handle));
	CAM_TRACE_F ("%s %s : %s (%i)", "GevOpenCamera", list [0].serial, get_error_name (status), status);
	if (status != GEVLIB_SUCCESS) {goto error;}
	CAM_ASSERT_F (cam->handle != NULL, "GevOpenCamera%s", "");
	errno = 0; //Reset errno because errno can be set inside GevOpenCamera.
	
	//This enables to set and get features values
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Enabling ability to set and get features values%s" ANSIC_RESET, "");
	status = GevInitGenICamXMLFeatures (cam->handle, 0);
	CAM_TRACE_F ("%s : %s (%i)", "GevInitGenICamXMLFeatures", get_error_name (status), status);
	if (status != GEVLIB_SUCCESS) {goto error;}
	
	//Check if we have important features available
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Check if we have important features available%s" ANSIC_RESET, "");
	char buf [128] = {0};
	status = cam_get_str (cam, "Width", sizeof (buf), buf);
	if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "Height", sizeof (buf), buf);
	if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "transferTurboMode", sizeof (buf), buf);
	//if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "transferTurboCurrentlyAbailable", sizeof (buf), buf);
	//if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "AcquisitionFrameRate", sizeof (buf), buf);
	//if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "TriggerMode", sizeof (buf), buf);
	if (status != GEVLIB_SUCCESS) {goto error;}
	status = cam_get_str (cam, "ExposureTime", sizeof (buf), buf);
	if (status != GEVLIB_SUCCESS) {goto error;}
	
	return GEVLIB_SUCCESS;
	
error:
	cam_close (cam);
	return status;
}


int cam_allocate_frame_buffer (struct Gev_Cam * cam, size_t count)
{
	CAM_TRACE_F (ANSIC (ANSIC_BOLD, ANSIC_CYAN, ANSIC_DEFAULT) "Begin allocating frame buffers%s" ANSIC_RESET, "");
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (cam->buffer_count == 0, "%s", "");
	//CAM_ASSERT_F (cam->buffer_data == NULL, "%s", "");
	cam->buffer_count = count;
	
	//Retreive size data for allocation of image buffers.
	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t depth;
	int r;
	r = cam_get_u32 (cam, "Width", &width);
	if (r != GEVLIB_SUCCESS) {goto error;}
	r = cam_get_u32 (cam, "Height", &height);
	if (r != GEVLIB_SUCCESS) {goto error;}
	r = cam_get_u32 (cam, "PixelFormat", &format);
	if (r != GEVLIB_SUCCESS) {goto error;}
	depth = GevGetPixelSizeInBytes (format);
	
	CAM_ASSERT_F (cam->buffer_data != NULL, "%s", "");
	CAM_TRACE_F ("Allocating %i buffers", (int)cam->buffer_count);
	for (size_t I = 0; I < cam->buffer_count; I = I + 1)
	{
		uint32_t const size = width * height * depth;
		cam->buffer_data [I] = (uint8_t *) malloc (size);
		CAM_TRACE_F ("malloc %lu : %p", (long unsigned)size, cam->buffer_data [I]);
		memset (cam->buffer_data [I], 0, size);
	}
	
	//Initialize streaming transfer to the list of buffers indicated.
	//The transfer is set up with the (Asynchronous cycling mode).
	Gev_Status status;
	status = GevInitImageTransfer (cam->handle, GEV_SynchronousNextEmpty, cam->buffer_count, cam->buffer_data);
	CAM_TRACE_F ("%s : %s (%i)", "GevInitImageTransfer SynchronousNextEmpty", get_error_name (status), status);
	if (status != GEVLIB_SUCCESS) {goto error;}
	
	
	return GEVLIB_SUCCESS;
	
error:

	cam_close (cam);
	return status;
}


int cam_read_block (struct Gev_Cam * cam, struct Gev_Image ** image, uint32_t timeout)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	Gev_Status status;
	status = GevWaitForNextImage (cam->handle, image, timeout);
	if (status == GEVLIB_SUCCESS) {return status;}
	if (status == GEVLIB_ERROR_TIME_OUT) {return status;}
	CAM_TRACE_F ("%s : %s (%i)", "GevWaitForNextImage", get_error_name (status), status);
	return status;
}


int cam_read (struct Gev_Cam * cam, struct Gev_Image ** image)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	Gev_Status status;
	status = GevGetImage (cam->handle, image);
	if (status == GEVLIB_SUCCESS) {return status;}
	if (status == GEVLIB_ERROR_TIME_OUT) {return status;}
	CAM_TRACE_F ("%s : %s (%i)", "GevGetImage", get_error_name (status), status);
	return status;
}


int cam_release (struct Gev_Cam * cam, struct Gev_Image * image)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (image != NULL, "%s", "");
	Gev_Status status;
	status = GevReleaseImage (cam->handle, image);
	if (status == GEVLIB_SUCCESS) {return status;}
	CAM_TRACE_F ("%s : %s (%i)", "GevReleaseImage", get_error_name (status), status);
	return status;
}


void cam_print_image (struct Gev_Cam * cam, struct Gev_Image * image)
{
	CAM_ASSERT_F (cam != NULL, "%s", "");
	CAM_ASSERT_F (cam->handle != NULL, "%s", "");
	CAM_ASSERT_F (image != NULL, "%s", "");
	fprintf 
	(
		stderr, 
		//"%08i %30s (%04i) %100s\n", 
		"%08i %30s (%04i)\n", 
		image->id, 
		cam_get_img_error_name (image->status), 
		image->status//, 
		//cam_get_img_error_desc (image->status)
	);
}


