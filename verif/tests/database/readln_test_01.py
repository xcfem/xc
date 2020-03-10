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

idElem= 0.0
nmbAccion= ""
sumN= 0.0
n= 0.0
sumM1= 0.0
m1= 0.0
sumM2= 0.0
m2= 0.0
sumT= 0.0
t= 0.0

conn= sqlite.connect(dbName)
conn.row_factory= sqlite.Row
otra= conn.cursor()
cur= conn.cursor()
cur.execute("select * from " + tbName + " where elem=3470")
for r in cur:
  idElem= r['ELEM']
  nmbAccion= r['ACCION']
  n= r['AXIL']
  m1= r['M_1']
  m2= r['M_2']
  t= r['TORSOR']
  sumN+= n
  sumM1+= m1
  sumM2+= m2
  sumT+= t

ratio1= abs(sumN+3951.69)/3951.69
ratio2= abs(sumM1-0.0242008)/0.0242008
ratio3= abs(sumM2-32.43870)/32.43870
ratio4= abs(sumT-0.0000115157)/0.0000115157

''' 
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
   '''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -f /tmp/pilares.db")

