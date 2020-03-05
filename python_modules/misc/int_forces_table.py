# -*- coding: utf-8 -*-

import sqlite3 as sqlite

def extractElemFromTable(nmbDBase, nmbTablaEsf, nmbTablaElem):
  '''
  Crea una tabla con los números de elemento contenidos en la tabla de esfuerzos
     nmbDBase: Nombre de la base de datos donde se guardará la tabla.
     nmbTablaEsf: Nombre de la tabla que contiene los esfuerzos.
     nmbTableElem: Nombre de la tabla que contendrá los índices de elemento.
  '''
  borraTabla(nmbTablaElem) # Borra la tabla de elementos si existe.
  SQLTcreaTabla(nmbDBase,nmbTablaElem,"(ELEM integer)")
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  cur = con.cursor()
  cur.execute("insert into "+nmbTablaElem+ " select distinct ELEM from " + nmbTablaEsf)

def extractSeccFromTable(nmbDBase, nmbTablaEsf, nmbTablaSecc):
  '''
  Crea una tabla con los números de sección contenidos en la tabla de esfuerzos
   nmbDBase: Nombre de la base de datos donde se guardará la tabla.
   nmbTablaEsf: Nombre de la tabla que contiene los esfuerzos.
   nmbTableSecc: Nombre de la tabla que contendrá los índices de elemento.
  '''
  SQLTcreaTabla(nmbDBase,nmbTablaSecc,"(ELEM integer)")
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  cur = con.cursor()
  cur.execute("insert into "+nmbTablaSecc+ " select distinct idSecc from " + nmbTablaEsf)



