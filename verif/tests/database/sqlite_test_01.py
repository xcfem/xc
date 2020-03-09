# -*- coding: utf-8 -*-

import sqlite3 as sqlite
from sqliteUtils import sqlite_macros
from misc import import_Ansys_beam_intforces

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
sqlite_macros.SQLTcreaDBase(dbName)
import_Ansys_beam_intforces.ansysToEsfBarra(fNameIn,dbName,tbName)

i= 0
e1= 0
conn= sqlite.connect(dbName)
conn.row_factory= sqlite.Row
otra= conn.cursor()
cur= conn.cursor()
cur.execute("select distinct ACCION from " + tbName)
for r in cur:
  i+=1

e1= sqlite_macros.SQLTexisteTabla(dbName,tbName)


ratio1= abs(i-6)/6

# print "ratio1= ",ratio1

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & e1:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -f /tmp/pilares.db")
