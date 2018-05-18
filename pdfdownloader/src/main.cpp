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

#include "pdfasync.h"



////////////////////////////////
////////////////////////////////


//long url_get_lenght(std::string url_str);

bool ParseCommandLine(const std::vector<std::string>& args,
	Options* options, std::list<std::string>* files,
	std::list<std::string>* saves) {
		if (args.empty()) {
			return false;
		}
		options->exe_path = args[0];
		size_t cur_idx = 1;
		bool bDownload = false;
		bool bSaveDir = false;
		for (; cur_idx < args.size(); ++cur_idx) {
			const std::string& cur_arg = args[cur_idx];
			if (cur_arg == "--show-config") {
				options->show_config = true;
			} else if (cur_arg == "--save") {
				bSaveDir = true;
				bDownload = false;
			} else if (cur_arg == "--download") {
				bDownload = true;
				bSaveDir = false;
			} 
			if (bDownload) {
				if (cur_arg.compare("--download") != 0 && 
					cur_arg.compare("--save") != 0)
					files->push_back(cur_arg);
			}
			if (bSaveDir) {
				if (cur_arg.compare("--download") != 0 &&
					cur_arg.compare("--save") != 0)
					saves->push_back(cur_arg);
			}
		}
		return true;
}

static const char usage_string[] =
	"Usage: pdfium_test [OPTION] [FILE]...\n"
	"  --show-config     - print build options and exit\n"
	"  --bin-dir=<path>  - override path to v8 external data\n"
	"  --font-dir=<path> - override path to external fonts\n"
	"  --scale=<number>  - scale output size by number (e.g. 0.5)\n"
#ifdef _WIN32
	"  --bmp - write page images <pdf-name>.<page-number>.bmp\n"
	"  --emf - write page meta files <pdf-name>.<page-number>.emf\n"
#endif  // _WIN32
	"  --png - write page images <pdf-name>.<page-number>.png\n"
	"  --ppm - write page images <pdf-name>.<page-number>.ppm\n";


static void ShowConfig() {
	std::string config;
	std::string maybe_comma;
#if PDF_ENABLE_V8
	config.append(maybe_comma);
	config.append("V8");
	maybe_comma = ",";
#endif  // PDF_ENABLE_V8
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
	config.append(maybe_comma);
	config.append("V8_EXTERNAL");
	maybe_comma = ",";
#endif  // V8_USE_EXTERNAL_STARTUP_DATA
#ifdef PDF_ENABLE_XFA
	config.append(maybe_comma);
	config.append("XFA");
	maybe_comma = ",";
#endif  // PDF_ENABLE_XFA
	printf("%s\n", config.c_str());
}


TestLoader::TestLoader(const char* pBuf, size_t len)
	: m_pBuf(pBuf), m_Len(len) {
}

// static
int TestLoader::GetBlock(void* param,
	unsigned long pos,
	unsigned char* pBuf,
	unsigned long size) {
		TestLoader* pLoader = static_cast<TestLoader*>(param);
		//if (pos + size < pos || pos + size > pLoader->m_Len)
		//  return 0;

		if ((pos + size) > pLoader->download_info->bufinfo->len) {
			url_read(pLoader->download_info, pLoader->download_info->bufinfo->len, (pos + size));
		}
		//while ((pos + size) > pLoader->m_download_len);

		//memcpy(pBuf, pLoader->m_pBuf + pos, size);

		memcpy(pBuf, (char*)pLoader->download_info->bufinfo->buf + pos, size);
		return 1;
}



int main(int argc, char** argv) {
	std::vector<std::string> args(argv, argv + argc);
	Options options;
	std::list<std::string> files;
	std::list<std::string> saves;
	if (!ParseCommandLine(args, &options, &files, &saves)) {
		fprintf(stderr, "%s", usage_string);
		return 1;
	}

	if (options.show_config) {
		ShowConfig();
		return 0;
	}

	if (files.empty()) {
		fprintf(stderr, "No input files.\n");
		return 1;
	}

	while (!files.empty()) {
		std::string filename = files.front();
		files.pop_front();

		DOWNLOAD_FILE_INFO* download_info = new DOWNLOAD_FILE_INFO;
		download_info->storepath = saves.front();
		saves.pop_front();
		download_info->url = filename;
		download_info->bufinfo = new DOWNLOAD_BUF_INFO;

		download_info->bufinfo->len = 0;

		download_info->content_len = url_get_lenght(download_info->url);
		if (download_info->content_len == -1) {
			//download_info->bufinfo->buf = new char[1024];
			//url_read(download_info, 0, -1);
		}
		else {
			//download_info->bufinfo->buf = new char[download_info->content_len];
			//url_read(download_info, 0, 1024);
		}

		int status = DownloadPdf(download_info);
		switch (status) {
		case 0:
			std::cout << "PDF Download OK.";
			break;
		case 1:
			std::cout << "PDF Download OK: Form.";
			break;
		case 2:
			std::cout << "PDF Download OK: XFA.";
			break;
		case -1:
			std::cout << "PDF Parse ERROR.";
			break;
		};

		delete download_info->bufinfo;
		download_info->bufinfo = NULL;
	}

	return 0;
}