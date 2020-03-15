# -*- coding: utf-8 -*-

import sqlite3 as sqlite

def extractElemFromTable(nmbDBase, nmbTablaEsf, nmbTablaElem):
  '''
  Crea una tabla con los números de elemento contenidos en la tabla de esfuerzos
     nmbDBase: Name of the database to store the tables.
     nmbTablaEsf: Name of the table that store the internal forces.
     nmbTableElem: Name of the table that store the element indexes.
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
   nmbDBase: Name of the database to store the tables.
   nmbTablaEsf: Name of the table that store the internal forces.
   nmbTableSecc: Name of the table that store the section indexes.
  '''
  SQLTcreaTabla(nmbDBase,nmbTablaSecc,"(ELEM integer)")
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  cur = con.cursor()
  cur.execute("insert into "+nmbTablaSecc+ " select distinct idSecc from " + nmbTablaEsf)



