# -*- coding: utf-8 -*-
import struct
import sqlite3 as sqlite
from sqliteUtils import macros_sqlite
import re #regular expresions
import tempfile

def splitActionFiles(nmbArch):
  actionFiles= {}
  listado= open(nmbArch,"r")
  outputFile= None
  for line in listado:
    str1= line
    str1SB= str1.strip()
    if(len(str1)>0):
      if(re.match(".*Accion.*",str1)):
        nmbAccion= re.search(' (.+?):',str1SB).group(1)
        if(outputFile):
          outputFile.close()
        outputFile= tempfile.NamedTemporaryFile(delete=False)
        actionFiles[nmbAccion]= outputFile.name 
      elif(re.match("[0-9]+.*",str1SB)):
        outputFile.write(line)
  outputFile.close()
  listado.close()
  return actionFiles

def ansysToEsfBarra(nmbArch, nmbDBase, nmbTabla):
  '''
  Importa el listado de esfuerzos en barras de ansys que se pasa como parámetro
     nmbArchivo: Nombre del archivo que contiene el listado.
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTabla: Nombre de la tabla que contendrá los esfuerzos.
  '''
  macros_sqlite.SQLTcreaTabla(nmbDBase,nmbTabla,"(ACCION string,ELEM integer,AXIL double,Q_1 double,Q_2 double,M_1 double,M_2 double,TORSOR double)")
  actionFiles= splitActionFiles(nmbArch)
  
  fieldWidths = [9,12,12,12,12,12,12]
  con= sqlite.connect(nmbDBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  for action in actionFiles.keys():
    nmbAccion= action
    fName= actionFiles[action]
    inputFile= open(fName,"r")
    for line in inputFile:
      idx = 0
      lst = []
      for i in fieldWidths:
        upper = idx+i
        lst.append(line[idx:upper].strip())
        idx = upper
      sqlValues= ",".join(lst)
      sqlStr= "insert into "+nmbTabla+" values('"+nmbAccion+"',"+sqlValues+");"
      cur.execute(sqlStr)
    con.commit()
