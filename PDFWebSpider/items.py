# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/en/latest/topics/items.html

import scrapy


class HtmlItem(scrapy.Item):
    link = scrapy.Field()

class PdfItem(scrapy.Item):
    status = scrapy.Field()
    link = scrapy.Field()

class PdfwebspiderItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    pass
