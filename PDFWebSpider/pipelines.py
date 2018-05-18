# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: http://doc.scrapy.org/en/latest/topics/item-pipeline.html
import os
from sqlalchemy.orm import sessionmaker
from models import DBPdf, DBHtml, db_connect, create_pdf_table, create_html_table
from pdfdownloader import PdfDownloader
  
class PdfwebspiderPipeline(object):
    def __init__(self):
        engine = db_connect()
        create_pdf_table(engine)
        create_html_table(engine)
        self.Session = sessionmaker(bind=engine)
  
    def process_item(self, item, spider):
        dl = PdfDownloader()
        ret = dl.download_form(item['link'])

        item['status'] = ret
            
        session = self.Session()
        hn = DBPdf(**item)  
        session.add(hn)
        session.commit()
        return item

##class PdfwebspiderPipeline(object):
##    def process_item(self, item, spider):
##        return item
