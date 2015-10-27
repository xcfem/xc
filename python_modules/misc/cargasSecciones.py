# -*- coding: utf-8 -*-

import sqlite3 as sqlite

# Introduce las cargas en cada una de las secciones
def creaCargasSecciones(preprocessor,nmbDatabase, nmbTablaEsfuerzos, nmbQueryEsfuerzos, nmbTablaSecc):
  resetAccionesConstantTS("constant_ts","ts",1000) # Factor 1000 (acciones en kN y kN m)
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  casos.currentTimeSeries= "ts"

  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  idSecc= 0.0
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaSecc)
  for row in cur:
    idSecc= row['ELEM']
    creaCargasFromTable(nmbQueryEsfuerzos,nmbTablaEsfuerzos,idSecc)

