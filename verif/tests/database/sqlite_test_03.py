# -*- coding: utf-8 -*-

import sqlite3 as sqlite
from sqliteUtils import sqlite_macros
from misc import import_Ansys_beam_intforces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

dbName= "/tmp/pilares.db"
import os
os.system("rm -f " + dbName)
sqlite_macros.SQLTcreaDBase(dbName)

conn= sqlite.connect(dbName)
cur= conn.cursor()
cur.execute("create table test3 ( num integer, name string );")
cur.execute("create table test4 ( A integer, B double );")
sqlite_macros.SQLTborraTabla(dbName,"test3")
sqlite_macros.SQLTborraTabla(dbName,"test4")
e1= sqlite_macros.SQLTexisteTabla(dbName,"test3")
e2= sqlite_macros.SQLTexisteTabla(dbName,"test4")



from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (not e1) & (not e2):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

os.system("rm -f /tmp/pilares.db")
