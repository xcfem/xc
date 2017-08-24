# -*- coding: utf-8 -*-
import sqlite3 as sqlite

# Tensiones normales
def creaTablaResultTN(nmbDBase, nmbTabla, nmbTablaElem):
  con= sqlite.connect(nmbDBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  cur.execute("CREATE TABLE " + nmbTabla + " (idSecc BIGINT NOT NULL PRIMARY KEY, idElem INT, idComb INT, axil DOUBLE, m1 DOUBLE, m2 DOUBLE,tipoSol INT,epsCMin DOUBLE,epsCMax DOUBLE,epsSMax DOUBLE,aprovTN DOUBLE)") # Creamos la tabla
  cur.execute("insert into "+nmbTabla+ " (idSecc) select ELEM from " + nmbTablaElem)
  con.close()

def writeUpdateResultTN(fName, nmbTabl, idSecc, idElem, idComb, esfN, esfMy, esfMz, tipoSolic, epsCMin, epsCMax, epsSMax, aprovSeccTN):
    fName.write("\execute_sql{"+char(34))
    fName.write("UPDATE " + nmbTabl + " SET idElem = "+ sqlValue(int(idElem)) + ",idComb = " + sqlValue(int(idComb)) + ",axil = " + sqlValue(esfN) + ",m1 = " + sqlValue(esfMy) + ",m2 = " + sqlValue(esfMz) + ",tipoSol = " + sqlValue(int(tipoSolic)) + ",epsCMin = " + sqlValue(epsCMin) + ",epsCMax = " + sqlValue(epsCMax) + ",epsSMax = " + sqlValue(epsSMax) + ", aprovTN= " + sqlValue(aprovSeccTN) + " WHERE idSecc = " + sqlValue(int(idSecc)))
    fName.write(char(34)+"}\n")

def creaTablaResultV(nmbDBase, nmbTabla):
  con= sqlite.connect(nmbDBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  cur.execute("CREATE TABLE " + nmbTabla + " (idSecc BIGINT NOT NULL PRIMARY KEY, idElem INT, idComb INT, axil DOUBLE, vrd DOUBLE, vu1 DOUBLE, vu2 DOUBLE, vu DOUBLE, aprovV DOUBLE)") # Creamos la tabla
  cur.execute("insert into "+nmbTabla+ " (idSecc) select ELEM from " + nmbTablaElem)
  con.close()

def writeUpdateResultV(fName, nmbTabl, idSecc, idElem, idComb, esfN, PropVrDato, Vu1, Vu2, Vu, aprovSeccV):
  fName.write("\execute_sql{"+char(34))
  fName.write("UPDATE " + nmbTabl + " SET idElem = "+ sqlValue(int(idElem)) + ",idComb = " + sqlValue(int(idComb)) + ",axil = " + sqlValue(esfN) + ",vrd = " + sqlValue(PropVrDato) + ",vu1 = " + sqlValue(Vu1) + ",vu2 = " + sqlValue(Vu2) + ",vu = " + sqlValue(Vu) + ", aprovV= " + sqlValue(aprovSeccV) + " WHERE idSecc = " + sqlValue(int(idSecc)))
  fName.write(char(34)+"}\n")

# Esfuerzos combinados
def creaTablaResultEsfComb(nmbDBase, nmbTabla, nmbTablaElem):
  con= sqlite.connect(nmbDBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  cur.execute("DROP TABLE " + nmbTabla + "IF EXISTS")
  cur.execute("CREATE TABLE " + nmbTabla + " (idSecc BIGINT NOT NULL, idElem INT, idComb INT, n DOUBLE, vy DOUBLE, vz DOUBLE, mx DOUBLE, my DOUBLE, mz DOUBLE)")# Creamos la tabla
