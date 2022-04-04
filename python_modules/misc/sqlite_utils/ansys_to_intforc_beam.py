# -*- coding: utf-8 -*-
from misc.sqlite_utils import sqlite_macros

def ansysToEsfBarra(nmbArch,nmbDBase,nmbTabla):
  '''Importa el listado de esfuerzos en barras de ansys que se pasa como parámetro
   nmbArchivo: Name of the file that contains the listing.
   nmbDBase: Name of the database that will store the table.
   nmbTabla: Name of the taable that will store the internal forces.
   '''

  sqlite_macros.SQLTcreaTabla(nmbDBase,nmbTabla,"(ACCION string,ELEM integer,AXIL double,Q_1 double,Q_2 double,M_1 double,M_2 double,TORSOR double)")

  fieldwidths = (0,8,9,20,21,32,33,44,45,56,57,68,69,80)
  fmtstring = ''.join('%ds' % f for f in fieldwidths)
  fieldstruct = struct.Struct(fmtstring)
  parse = fieldstruct.unpack_from

  con= sqlite.connect(nmbDBase)
  cur= con.cursor()

  listado.open(nmbArch,"r")
  for line in listado:
    str1= line
    str1SB= qBlancos(str1)
    if(strlen(str1)>0):
      if(regExpMatch(".*Accion.*",str1)):
        nmbAccion= copiaDesde(" ",copiaHasta(":",str1SB))
      else:
        if(regExpMatch("[0-9]+.*",str1SB)):
          lst=csv2list(fixedWidth2Csv(str1))
          cur.execute_sql("insert into "+nmbTabla+" values('"+nmbAccion+"',"+lst.sqlValues+")")
  listado.close()

def ansysExtractElem(nmbDBase,nmbTablaEsf,nmbTablaElem):
  '''Crea una tabla con los números de elemento contenidos en el listado
   nmbDBase: Name of the database that will store the tables.
   nmbTablaEsf: Name of the table that tore the internal forces.
   nmbTableElem: Name of the table that will store the element indexes.'''
  sqlite_macros.SQLTcreaTabla(nmbDBase,nmbTablaElem,"(ELEM integer)")

  con= sqlite.connect(nmbDBase)
  cur= con.cursor()
  cur.execute("insert into "+nmbTablaElem+ " select distinct ELEM from " + nmbTablaEsf )

def ansysCreaElem(nmbDBase,nmbTablaElem, sectionName):
    '''Crea los elementos a partir de la tabla creada anteriormente

     :param nmbDBase: Name of the database that will store the tables.
     :param nmbTableElem: Name of the table that stores the element indexes.
     :param sectionName: Name of the section assigned to each element.'''
    con= sqlite.connect(nmbDBase)
    cur= con.cursor()

    i= 1
    cur.execute("select * from " + nmbTablaElem)
    while True:
        row= cur.fetchone()
        if row is None:
            break
        else:
            nuevoZeroLengthSeccFibras(sectionName,i,row("ELEM"))
            i+= 2

