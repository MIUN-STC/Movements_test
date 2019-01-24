#pragma once

#include "debug.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

char const * str_from_cvtype (int depth)
{
	switch ( depth )
	{
		case CV_8U:  return "8U";
		case CV_8UC3:  return "CV_8UC3";
		case CV_8UC4:  return "CV_8UC4";
		case CV_8S:  return "8S";
		case CV_16U: return "16U";
		case CV_16S: return "16S";
		case CV_32S: return "32S";
		case CV_32F: return "32F";
		case CV_64F: return "64F";
		default:     return "User";
	}
	return "User";
}






int read_value (FILE * file, float value [1])
{
	ASSERT (file);
	int r = fscanf (file, "%e", value);
	//if (r < 0) {TRACE_F ("File %s ended", "");}
	return r;
}


void read_values (FILE * file, size_t dim, size_t * n, float values [])
{
	ASSERT (dim == 1);
	size_t i;
	for (i = 0; i < (*n); ++i)
	{
		int r = read_value (file, values + i);
		//vf32_print (stdout, values + i, dim, "%f ");
		if (r < 0) {break;}
	}
	(*n) = i;
}


struct Control
{
	int x;
};


void gltex_setup_from_cvmat (GLuint tex, cv::Mat & m)
{
	TRACE_F ("tex  %10i", tex);
	TRACE_F ("cols %10i", m.cols);
	TRACE_F ("rows %10i", m.rows);
	TRACE_F ("type %10s", str_from_cvtype (m.type ()));
	GLenum const target = GL_TEXTURE_2D;
	GLint const level = 0;
	GLint const internalformat = GL_RGB;
	GLsizei const width = m.cols;
	GLsizei const height = m.rows;
	GLint const border = 0;
	GLenum const format = GL_RGB;
	GLenum const type = GL_UNSIGNED_BYTE;
	GLvoid const * pixels = m.ptr ();
	glBindTexture (target, tex);
	glTexParameteri (target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D (target, level, internalformat, width, height, border, format, type, pixels);
	glGenerateMipmap (target);
}


void gltex_update_from_cvmat (GLuint tex, cv::Mat & m)
{
	GLenum const target = GL_TEXTURE_2D;
	GLint const level = 0;
	GLint const xoffset = 0;
	GLint const yoffset = 0;
	GLsizei const width = m.cols;
	GLsizei const height = m.rows;
	GLenum const format = GL_RGB;
	GLenum const type = GL_UNSIGNED_BYTE;
	GLvoid const * pixels = m.ptr ();
	glBindTexture (target, tex);
	glTexSubImage2D (target, level, xoffset, yoffset, width, height, format, type, pixels);
}



