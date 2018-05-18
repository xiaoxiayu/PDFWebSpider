## Simple User Guide

 1. The tool was developed with python 2.7. So please install python 2.7 first (latest version 2.7.11). 

 2. Then should install Scrapy, a necessary module of python. 
    * a. Run CMD
	* b. Run "pip install scrapy,pybloom,sqlalchemy" in the command window
>* Note: You may encounter several errors when install scrapy.
	* Error1:  error: Microsoft Visual C++ 9.0 is required (Unable to find vcvarsall.bat). Get it from http://aka.ms/vcpython27
	>>`How to Fix:` Download and install vc++ 9.0.
	* Error2: lxml.etree.c
    src\lxml\includes\etree_defs.h(14) : fatal error C1083: Cannot open include
file: 'libxml/xmlversion.h': No such file or directory
	>> `How to fix: `Try command "easy_install lxml" first. If sucessfully install, then try "pip install scrapy" again. If failed to install lxml, it's better to download lxml here http://www.lfd.uci.edu/~gohlke/pythonlibs/#lxml and install it(make sure you download the correct version, that means consistent with the python version you installed), then try "pip install scrapy" again.

 3. After successfully install the above libraries, go to directory "PDFWebSpider" in the command window, and run command "scrapy crawl bing", crawler will start.
Note: You may encounter an error here.
	* Error: No module named win32api
    > `How to fix: `Try "pip install pywin32". If this does not work, try to download pywin32 here http://www.lfd.uci.edu/~gohlke/pythonlibs/#pywin32 and install it. Then run "scrapy crawl bing" again.


 4. PDF Form are saved in the current working directory by default. You can change this path by modifying "DOWNLOAD_DIR" in "pdfdownloader.py". For example, you want to save the forms in a folder named "download", change "DOWNLOAD_DIR = os.getcwd()" to "DOWNLOAD_DIR = os.getcwd() + "\\download"".

 5. The tool was configured to search "pdf form" with bing and then try to download them. Will check if the file was a pdf form really with FXCORE before download. Checked both AcroForm and XFA.

