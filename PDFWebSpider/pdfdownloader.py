# -*- coding: utf-8 -*-
import os
from urllib import unquote
import urllib2
import urlparse
import httplib


# The root directory of the saved file.
DOWNLOAD_DIR = os.getcwd()

class PdfDownloader():
    def __init__(self):
        pass

    def get_content_lenght(self, url_str):
        u = urllib2.urlopen(url_str)
        scheme, netloc, path, query, fragment = urlparse.urlsplit(url_str)
        meta = u.info()
        meta_func = meta.getheaders if hasattr(meta, 'getheaders') else meta.get_all
        meta_length = meta_func('Content-Length')
        file_size = int(meta_length[0])
        return file_size
        

    def create_save_path(self, url_str):
        # Change this.
        decode_url = unquote(url_str)
        return os.path.join(DOWNLOAD_DIR, os.path.basename(decode_url))
        
  
    def download_form(self, url_str):
        print('Download Parse Start: %s' % url_str)
        try:
            print('Content-Lenght: %s' % self.get_content_lenght(url_str))
        except:
            print('Content-Lenght: Get Failed. Continue.')
        save_path = self.create_save_path(url_str)
        buf = os.popen('pdfdownloader.exe --download "%s" --save "%s"' \
                       % (url_str, save_path)).read()
        if buf.find('PDF Download Failed') != -1:
            print('Download Skip.')
            return 0
        elif buf.find('PDF Parse ERROR') != -1:
            print('PDF Parse ERROR.')
            return -1
        elif buf.find('PDF Parse CRASH') != -1:
            print('PDF Crash.')
            return -2
        elif buf.find('URL Could not connect') != -1:
            print('Connect failed.')
            return -3
        else:
            print('Download OK: %s' % (save_path))
            return 1
        
