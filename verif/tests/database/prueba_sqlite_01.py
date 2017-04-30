# -*- coding: utf-8 -*-

import sqlite3 as sqlite
from sqliteUtils import macros_sqlite
from misc import ansysToEsfBarra

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
fNameIn= pth+"/../aux/pilares.lst"
dbName= "/tmp/pilares.db"
tbName= "esfPilares"
os.system("rm -f " + dbName)
macros_sqlite.SQLTcreaDBase(dbName)
ansysToEsfBarra.ansysToEsfBarra(fNameIn,dbName,tbName)

i= 0
e1= 0
conn= sqlite.connect(dbName)
conn.row_factory= sqlite.Row
otra= conn.cursor()
cur= conn.cursor()
cur.execute("select distinct ACCION from " + tbName)
for r in cur:
  i+=1

e1= macros_sqlite.SQLTexisteTabla(dbName,tbName)


ratio1= abs(i-6)/6

# print "ratio1= ",ratio1

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & e1:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

os.system("rm -f /tmp/pilares.db")
