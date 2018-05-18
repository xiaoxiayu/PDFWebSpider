# -*- coding: utf-8 -*-

from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.engine.url import URL
  
import settings
  
DeclarativeBase = declarative_base()
  
def db_connect():
    return create_engine('sqlite:///database.db3')
  
def create_pdf_table(engine):
    DeclarativeBase.metadata.create_all(engine)

def create_html_table(engine):
    DeclarativeBase.metadata.create_all(engine)
  
class DBPdf(DeclarativeBase):
    __tablename__ = "pdf"
  
    status = Column(Integer)
    #title = Column('title', String(200))
    link = Column('link', String(200), primary_key=True)

class DBHtml(DeclarativeBase):
    __tablename__ = "html"
  
    id = Column(Integer, primary_key=True)
    #title = Column('title', String(200))
    link = Column('link', String(200))
