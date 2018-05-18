# -*- coding: utf-8 -*-
import scrapy
import os, sys
from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.engine.url import URL
from sqlalchemy.ext.declarative import declarative_base
from PDFWebSpider.items import PdfItem, HtmlItem
import md5
from sqlalchemy.orm import sessionmaker
sys.path.append('PDFWebSpider')
from models import DBPdf, DBHtml, db_connect
from pybloom import ScalableBloomFilter

def GetStringMD5(nstr):  
    m = md5.new()
    m.update(nstr)  
    return m.hexdigest()

class BingSpider(scrapy.Spider):
    name = "bing"
##    allowed_domains = ["bing.com"]
    start_urls = (
        'https://cn.bing.com/search?q=pdf+english+text&qs=n&form=QBRE&sp=-1&pq=pdf+english+text&sc=8-11&sk=&cvid=B223BC1080F14B7E8D689BA6E7747DC1',
    )

    def __init__(self):
        engine = db_connect()
        self.Session = sessionmaker(bind=engine)
        self.fiter = ScalableBloomFilter(mode=ScalableBloomFilter.SMALL_SET_GROWTH)

    def parse(self, response):
        for sel in response.xpath('//@href'):
            href_data = sel.extract()
            ext_name = os.path.splitext(href_data)[1]
            if ext_name == '' \
               or ext_name == '.html' \
               or ext_name == '.htm':
                abs_href_data = href_data
                if abs_href_data == '#':
                    continue
                abs_href_data = response.urljoin(href_data)
                if abs_href_data[0:4] != 'http':
                    continue

                href_md5 = GetStringMD5(abs_href_data.encode('utf8'))
                if href_md5 in self.fiter:
                    continue
                else:
                    self.fiter.add(href_md5)

##                session = self.Session()
##                dburl = session.query(DBHtml).filter_by(link=abs_href_data).first()
##                if dburl != None:
##                    continue

##                print('================')
##                print(abs_href_data)
##                print(href_md5)
##                print('================')

                html_item = HtmlItem()
                html_item['link'] = abs_href_data
                hn = DBHtml(**html_item)
                session = self.Session()
                session.add(hn)
                session.commit()

                yield scrapy.Request(abs_href_data, callback=self.parse)

            if ext_name == '.pdf':
                abs_href_data = response.urljoin(href_data)
                href_md5 = GetStringMD5(abs_href_data.encode('utf8'))
                if href_md5 in self.fiter:
                    continue
                else:
                    self.fiter.add(href_md5)

                session = self.Session()
                dburl = session.query(DBPdf).filter_by(link=abs_href_data).first()
                if dburl != None:
                    continue

                pdf_item = PdfItem()
                pdf_item['link'] =  abs_href_data
                pdf_item['status'] = 0
                yield pdf_item
