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
#include "pdfasync.h"



long url_get_lenght(std::string url_str) {
	CURLcode res;
	double  content_length = -1;	
	CURL *curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		//curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_URL, url_str.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		//curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			curl_easy_cleanup(curl);
			return -2;
		}
		CURLcode a = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
		//CURLcode a = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
		curl_easy_cleanup(curl);
	}
	return content_length;
}

size_t url_write_data(void *ptr, size_t size, size_t nmemb, void* info) {
	DOWNLOAD_FILE_INFO* fileinfo = (DOWNLOAD_FILE_INFO*)info;
	DOWNLOAD_BUF_INFO* downbufinfo = (DOWNLOAD_BUF_INFO*)fileinfo->bufinfo;
	
	if (fileinfo->block_read) {
		if (nmemb > fileinfo->bufsize) {
			downbufinfo->buf = (char*)realloc(downbufinfo->buf, nmemb);
			fileinfo->bufsize = nmemb;
		}
		memcpy((char *)downbufinfo->buf, ptr, nmemb);
	} else {
		memcpy((char *)downbufinfo->buf + downbufinfo->len, ptr, nmemb);
		downbufinfo->len += nmemb;
	}
	
	return nmemb;
}




bool url_read(void* param, long beg, long end) {
	DOWNLOAD_FILE_INFO* file_info = (DOWNLOAD_FILE_INFO*)param;
	CURLcode res;
	CURL *curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_URL, file_info->url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

		if (end != -1) {
			std::stringstream ss;
			ss << beg << "-" << end;
			std::string range_str = ss.str();
			std::cout << range_str.c_str() << std::endl;
			curl_easy_setopt(curl, CURLOPT_RANGE, range_str.c_str());
		}
		file_info->pdf_offset = beg;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file_info);
		res = curl_easy_perform(curl);

		//std::cout << res;
		/* always cleanup */
		curl_easy_cleanup(curl);
		//fclose(file_info->fp);
	}
	return true;
}

bool url_store(DOWNLOAD_FILE_INFO* info) {
	FILE* fp = fopen(info->storepath.c_str(), "wb");
	if (fp == NULL) return false;

	fwrite(info->bufinfo->buf, sizeof(char), info->bufinfo->len, fp);
	fclose(fp);

	return true;
}

CTest_InternetFile::CTest_InternetFile(std::string path) {
	//m_pFileRead = FX_CreateFileRead(path.c_str());
	//if(m_pFileRead)
	//	m_pFileAvail = new CTest_FileAvailAsync(m_pFileRead->GetSize());
}


CTest_InternetFile::~CTest_InternetFile() {
	//Release();
}

void CTest_InternetFile::Release() {
	//if (m_pFileAvail)
	//	delete m_pFileAvail;
	//m_pFileRead->Release(); 
}

FX_FILESIZE CTest_InternetFile::GetSize() {
	return 0; 
}

FX_BOOL CTest_InternetFile::IsEOF() {
	return false; 
}

FX_FILESIZE CTest_InternetFile::GetPosition() {
	return 0; 
}

FX_BOOL CTest_InternetFile::ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) {
	return true;
}

FX_BOOL CTest_InternetFile::IsDataAvail(FX_FILESIZE offset, FX_DWORD size) {
	return true;
}

void CTest_InternetFile::Download() {

}

FX_FILESIZE CTest_InternetFile::getDownloadedSize() const {
	return 0;
}

FX_BOOL CTest_InternetFile::IsNeedDownload() const {
	return false;
}

void CTest_InternetFile::AddSegment(FX_FILESIZE offset, FX_DWORD size) {

}


CTest_FileAvailAsync::CTest_FileAvailAsync(FX_FILESIZE size) {

}

CTest_FileAvailAsync::~CTest_FileAvailAsync() {

}

FX_BOOL CTest_FileAvailAsync::IsDataAvail(FX_FILESIZE offset, FX_DWORD size) {
	return true;
}

void CTest_FileAvailAsync::AddDownloadedData(FX_FILESIZE offset, FX_FILESIZE size) {
	
}

void CTest_FileAvailAsync::AddWholeFile() {

}

void FileDownload(DOWNLOAD_FILE_INFO* download_info) {
	if (download_info->bufinfo->buf != NULL) {
		delete[] download_info->bufinfo->buf;
		download_info->bufinfo->buf = NULL;
	}

	download_info->bufinfo->buf = new char[download_info->content_len+1];

	download_info->block_read = false;
	url_read(download_info, 0, download_info->content_len);
	url_store(download_info);
	delete[] download_info->bufinfo->buf;
	download_info->bufinfo->buf = NULL;
}

int DownloadPdf(DOWNLOAD_FILE_INFO* download_info) {
	if (download_info->content_len <= 0) {
		return -1;
	}

	FXMEM_GetDefaultMgr();

	CCodec_ModuleMgr *pCodecModule = CCodec_ModuleMgr::Create();
	CFX_GEModule::Create();
	CFX_GEModule::Get()->SetCodecModule(pCodecModule);
	CPDF_ModuleMgr::Create();
	CPDF_ModuleMgr::Get()->SetCodecModule(pCodecModule);

	CTest_FileRead* pFileRead = new CTest_FileRead;
	pFileRead->SetDownloader(download_info);

	CTest_FileAvail* pFileAvail = new CTest_FileAvail(download_info);
	CPDF_DataAvail* pDataAvail = new CPDF_DataAvail(pFileAvail, pFileRead, FALSE);


	CTest_DownloadHints* pDownloadHints = new CTest_DownloadHints(pFileAvail);

	bool bRet = pDataAvail->IsDocAvail(pDownloadHints);
	if (!bRet) {
		return -1;
	}

	FileDownload(download_info);

	delete pDataAvail;
	delete pFileAvail;
	delete pFileRead;

	CFX_GEModule *pGEModule = CFX_GEModule::Get();
  if (pGEModule) {
		CFX_FontCache *pFontCache = pGEModule->GetFontCache();
		if (pFontCache) pFontCache->FreeCache();
  }

	CPDF_ModuleMgr::Destroy();
	CFX_GEModule::Destroy();
	pCodecModule->Destroy();

	return 0;
}