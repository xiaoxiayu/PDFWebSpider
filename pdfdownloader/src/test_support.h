// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_TEST_SUPPORT_H_
#define TESTING_TEST_SUPPORT_H_

#include <stdlib.h>
#include <memory>
#include <string>
#include <sstream>
#include <curl/curl.h>

//#include "public/fpdf_save.h"
//#include "public/fpdfview.h"

#ifdef PDF_ENABLE_V8
#include "v8/include/v8.h"
#endif  // PDF_ENABLE_V8


#if defined(_MSC_VER)
#include <Windows.h>
typedef CRITICAL_SECTION	FXQA_LOCK;
#elif defined(__GNUC__)
#include <pthread.h>
typedef pthread_mutex_t		FXQA_LOCK;
#else
typedef void*				FXQA_LOCK;
#endif


enum OutputFormat {
  OUTPUT_NONE,
  OUTPUT_PPM,
  OUTPUT_PNG,
#ifdef _WIN32
  OUTPUT_BMP,
  OUTPUT_EMF,
#endif
};

struct Options {
  Options() : show_config(false), output_format(OUTPUT_NONE) {}

  bool show_config;
  OutputFormat output_format;
  std::string scale_factor_as_string;
  std::string save_dir;
  std::string exe_path;
  std::string bin_directory;
  std::string font_directory;
};


typedef struct _DOWNLOAD_BUFFER_INFO {
	void* buf;
	long len;
}DOWNLOAD_BUF_INFO;
typedef struct _DOWNLOAD_FILE_INFO {
	std::string url;
	long content_len;
	long pdf_offset;
	bool block_read;
	long bufsize;

	FILE* fp;
	std::string storepath;

	DOWNLOAD_BUF_INFO* bufinfo;
} DOWNLOAD_FILE_INFO;


class CLock
{
private:
	FXQA_LOCK m_section;
public:
	CLock(void) {
#if defined(_MSC_VER)
		InitializeCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutexattr_t tma;
		pthread_mutexattr_init(&tma);
		pthread_mutexattr_settype(&tma, PTHREAD_MUTEX_RECURSIVE);
		int nErr = pthread_mutex_init(&m_section, &tma);
		pthread_mutexattr_destroy(&tma);
#endif
	}
	~CLock(void) {
#if defined(_MSC_VER)
		DeleteCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_destroy(&m_section);
#endif
	}
	void lock() {
#if defined(_MSC_VER)
		EnterCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_lock(&m_section);
#else
		//do nothing
#endif	
	}
	void unlock() {
#if defined(_MSC_VER)
		LeaveCriticalSection(&m_section);
#elif defined(__GNUC__)
		pthread_mutex_unlock(&m_section);
#else
		//do nothing
#endif
	}
};

class CQALocker {
private:
	CLock * m_pLock;
public:

	CQALocker(CLock * pLock) {
		m_pLock = pLock;
		pLock->lock();
	}
	~CQALocker() {
		m_pLock->unlock();
	}
};

bool url_read(void* param, long beg, long end);
bool url_store(DOWNLOAD_FILE_INFO* info);
long url_get_lenght(std::string url_str);

namespace pdfium {

#define STR_IN_TEST_CASE(input_literal, ...)                         \
  {                                                                  \
    (const unsigned char*) input_literal, sizeof(input_literal) - 1, \
        __VA_ARGS__                                                  \
  }

#define STR_IN_OUT_CASE(input_literal, expected_literal, ...)                 \
  {                                                                           \
    (const unsigned char*) input_literal, sizeof(input_literal) - 1,          \
        (const unsigned char*)expected_literal, sizeof(expected_literal) - 1, \
        __VA_ARGS__                                                           \
  }

struct StrFuncTestData {
  const unsigned char* input;
  unsigned int input_size;
  const unsigned char* expected;
  unsigned int expected_size;
};

struct DecodeTestData {
  const unsigned char* input;
  unsigned int input_size;
  const unsigned char* expected;
  unsigned int expected_size;
  // The size of input string being processed.
  unsigned int processed_size;
};

struct NullTermWstrFuncTestData {
  const wchar_t* input;
  const wchar_t* expected;
};

// Used with std::unique_ptr to free() objects that can't be deleted.
struct FreeDeleter {
  inline void operator()(void* ptr) const { free(ptr); }
};

}  // namespace pdfium

// Reads the entire contents of a file into a newly alloc'd buffer.
std::unique_ptr<char, pdfium::FreeDeleter> GetFileContents(const char* filename,
                                                           size_t* retlen);

// Converts a FPDF_WIDESTRING to a std::wstring.
// Deals with differences between UTF16LE and wchar_t.
//std::wstring GetPlatformWString(const FPDF_WIDESTRING wstr);

// Returns a newly allocated FPDF_WIDESTRING.
// Deals with differences between UTF16LE and wchar_t.
std::unique_ptr<unsigned short, pdfium::FreeDeleter> GetFPDFWideString(
    const std::wstring& wstr);

#ifdef PDF_ENABLE_V8
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
bool InitializeV8ForPDFium(const std::string& exe_path,
                           const std::string& bin_dir,
                           v8::StartupData* natives_blob,
                           v8::StartupData* snapshot_blob,
                           v8::Platform** platform);
#else   // V8_USE_EXTERNAL_STARTUP_DATA
bool InitializeV8ForPDFium(v8::Platform** platform);
#endif  // V8_USE_EXTERNAL_STARTUP_DATA
#endif  // PDF_ENABLE_V8

class TestLoader {
 public:
  TestLoader(const char* pBuf, size_t len);
  static int GetBlock(void* param,
                      unsigned long pos,
                      unsigned char* pBuf,
                      unsigned long size);

  DOWNLOAD_FILE_INFO* download_info;

 private:
  const char* const m_pBuf;
  const size_t m_Len;
};
//
//class TestSaver : public FPDF_FILEWRITE {
// public:
//  TestSaver();
//
//  void ClearString();
//  const std::string& GetString() const { return m_String; }
//
// private:
//  static int WriteBlockCallback(FPDF_FILEWRITE* pFileWrite,
//                                const void* data,
//                                unsigned long size);
//
//  std::string m_String;
//};

#endif  // TESTING_TEST_SUPPORT_H_
