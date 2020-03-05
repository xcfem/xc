# -*- coding: utf-8 -*-
import struct
import sqlite3 as sqlite

def ansysToEsfShell(nmbArch, nmbDBase, nmbTabla):
  '''
  Importa el listado de esfuerzos en elementos shell de ansys que se pasa como parámetro
     nmbArchivo: Nombre del archivo que contiene el listado.
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTabla: Nombre de la tabla que contendrá los esfuerzos.
  '''
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  cur.execute("PRAGMA journal_mode = OFF") # Desactivamos journal
  borraTabla(nmbTabla) # Borra la tabla de esfuerzos si existe.

  SQLTcreaTabla(nmbDBase,nmbTabla,"(ACCION string,ELEM integer,AXIL_X double,AXIL_Y double,RASANTE double,MOM_X double,MOM_Y double,TORSOR double,Q_X double,Q_Y double)")

  fieldwidths = (0,8,9,20,21,32,33,44,45,56,57,68,69,80,81,92,93,106)
  fmtstring = ''.join('%ds' % f for f in fieldwidths)
  fieldstruct = struct.Struct(fmtstring)
  parse = fieldstruct.unpack_from

  listado= open(nmbArch,"r")
  for line in listado:
    str1= line
    str1SB= str1.strip()
    if(len(str1)>0):
      if(re.match(".*Accion.*",str1)):
        nmbAccion= copiaDesde(" ",copiaHasta(":",str1SB))
      elif(re.match("[0-9]+.*",str1SB)):
        lst= parse(str1)
        cur.execute("insert into "+nmbTabla+" values('"+nmbAccion+"',"+sqlValues(lst)+")")
  listado.close()
