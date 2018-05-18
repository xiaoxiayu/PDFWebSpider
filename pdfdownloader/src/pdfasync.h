#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <utility>
#include <vector>
#include <curl/curl.h>
#include "test_support.h"

#include "fpdfapi/fpdfapi.h"
#include "fxcodec/fx_codec.h"
#include "fxge/fx_ge.h"
#include "fxge/fx_ge_win32.h"
#include "fpdfapi/fpdfapi.h"
#include "fpdfdoc/fpdf_doc.h"
#include "fxge/fx_dib.h"
#include "fxcrt/fx_basic.h"
//
//#include "fpdf_dataavail.h"
//#include "fpdfview.h"

//class FXQA_FILEAVAIL :public FX_FILEAVAIL {
//public:
//	long avabuf_len;
//};



class CTest_FileAvailAsync : public IFX_FileAvail, public CFX_Object
{
public:
	CTest_FileAvailAsync(FX_FILESIZE size);
	~CTest_FileAvailAsync();
	virtual FX_BOOL			IsDataAvail(FX_FILESIZE offset, FX_DWORD size);

	void					AddDownloadedData(FX_FILESIZE offset, FX_FILESIZE size);
	void					AddWholeFile();
protected:
	CTest_FileAvailAsync();

	FX_FILESIZE	m_dwSize;
	FX_LPBYTE	m_pFlags;
	FX_FILESIZE	m_nDownloadBytes;

	friend class CTest_InternetFile;
};


class CTest_InternetFile : public IFX_FileRead, public IFX_FileAvail, public CFX_Object
{
public:
	CTest_InternetFile(IFX_FileRead* pFile);
	CTest_InternetFile(std::string path);
	~CTest_InternetFile();
public:
	virtual void			Release();
	virtual FX_FILESIZE		GetSize();
	virtual FX_BOOL			IsEOF();
	virtual FX_FILESIZE		GetPosition();
	virtual FX_BOOL			ReadBlock(void* buffer, FX_FILESIZE offset, size_t size);

	void					Download();
public:
	virtual FX_BOOL			IsDataAvail(FX_FILESIZE offset, FX_DWORD size);
public:
	FX_FILESIZE				getDownloadedSize() const;
	FX_BOOL					IsNeedDownload() const;
protected:
	CTest_InternetFile();
	CTest_InternetFile(const CTest_InternetFile& that);
	CTest_InternetFile& operator=(const CTest_InternetFile& that);

	void					AddSegment(FX_FILESIZE offset, FX_DWORD size);
	struct FileSegment_
	{
		FX_FILESIZE offset;
		FX_FILESIZE size;
	};
	std::vector<FileSegment_>	m_segmentsToDownload;

	IFX_FileRead*			m_pFileRead;
	CTest_FileAvailAsync*		m_pFileAvail;
};


class CTest_FileAvail : public IFX_FileAvail, public CFX_Object
{
	DOWNLOAD_FILE_INFO* m_download_info;
public:
	CTest_FileAvail(DOWNLOAD_FILE_INFO* download_info/*FX_FILESIZE size*/)
	{
		m_download_info = download_info;
	}

	~CTest_FileAvail()
	{
		
	}

	FX_FILESIZE	m_dwSize;
	FX_LPBYTE	m_pFlags;
	FX_FILESIZE	m_nDownloadBytes;

	virtual FX_BOOL			IsDataAvail(FX_FILESIZE offset, FX_DWORD size)
	{
		return TRUE;
	}

	void Download(FX_FILESIZE offset, FX_FILESIZE size)
	{

	}
};

class CTest_FileRead : public IFX_FileRead, public CFX_Object
{
public:
	CFX_ByteString			m_FileContent;
	CTest_FileAvail*		m_pFileAvail;
	DOWNLOAD_FILE_INFO* m_download_info;
	void SetDownloader(DOWNLOAD_FILE_INFO* download_info) { m_download_info = download_info;};

	virtual FX_FILESIZE		GetSize() 
	{ 
		
		return m_download_info->content_len;
		//return (FX_FILESIZE)m_FileContent.GetLength();
	}
	virtual FX_BOOL		ReadBlock(void* buffer, FX_FILESIZE offset, size_t size)
	{
		m_download_info->bufinfo->buf = new char[size + 1];
		m_download_info->block_read = true;
		m_download_info->bufsize = size;
		url_read(m_download_info, offset, offset + size);
		

		//memcpy(buffer, (char*)m_download_info->bufinfo->buf+offset, size);
		memcpy(buffer, (char*)m_download_info->bufinfo->buf, size);

		delete[] m_download_info->bufinfo->buf;
		m_download_info->bufinfo->buf = NULL;
		m_download_info->block_read = false;

		return TRUE;
	}

	virtual void		Release() {
	return;
	}
};

class CTest_DownloadHints : public IFX_DownloadHints, public CFX_Object
{
public:
	CTest_DownloadHints(CTest_FileAvail* pFileAvail)
	{
		m_pFileAvail = pFileAvail;
	}
	CTest_FileAvail*	m_pFileAvail; 
	void			AddSegment(FX_FILESIZE offset, FX_DWORD size)
	{
		m_pFileAvail->Download(offset, size);
	}
};

int DownloadPdf(DOWNLOAD_FILE_INFO* download_info);