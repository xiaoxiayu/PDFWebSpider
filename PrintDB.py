# -*- coding: utf-8 -*-

import sqlite3


cx = sqlite3.connect("database.db3") 
cu = cx.cursor()

cu.execute("select * from pdf")
res = cu.fetchall()
for l in res:
    print(l)
