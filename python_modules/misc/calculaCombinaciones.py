# -*- coding: utf-8 -*-

import math
import sqlite3 as sqlite

def getListaCombinaciones(nmbDatabase, nmbQueryCombinaciones, nmbListaIdComb, nmbListaDescompComb):
  # Obtiene las combinaciones
  con= sqlite.connect(nmbDataBase)
  con.row_factory = sqlite.Row
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaComb)
  for row in cur:
    nmbListaIdComb.inserta(row['idComb'])
    nmbListaDescompComb.inserta(row['descomp'])


def writeInsertResultEsfComb(fName, nmbTabl, idSecc, idElem, idComb, n, vy, vz, mx, my, mz):
  fName.write("\execute_sql{"+char(34))
  fName.write("INSERT INTO " + nmbTabl + " (idSecc, idElem, idComb, n, vy, vz, mx, my, mz) VALUES ("+ sqlValue(int(idSecc)) + "," + sqlValue(int(idElem)) + "," + sqlValue(int(idComb)) + "," + sqlValue(n) + "," + sqlValue(vy) + "," + sqlValue(vz) + "," + sqlValue(mx) + "," + sqlValue(my) + "," + sqlValue(mz)+")")
  fName.write(char(34)+"}\n")


def calculaCombSeccionesBarra(preprocessor,nmbDatabase, nmbQuerySecciones, idComb, nmbTablaSecc, fOut, nmbTablaResultEsfComb):
# Obtiene los resultados de las secciones para una combinación
  idScc= 0.0
  idElem= 0.0
  idNodo= 0.0
  nodos= preprocessor.getNodeLoader()
  elements= preprocessor.getElementLoader()

  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  cur= con.cursor()
  cur.execute("select * from " + nmbTablaSecc)
  for row in cur:
    idScc= row['ELEM']
    idElem= math.floor(idScc/10)
    idNodo= 10*idScc+1
    if((analOk==0.0)):
      nodos.calculateNodalReactions(True)
      logging.info("Calculando combinación ",idComb," para sección: ",int(idScc),"\n")
      nod= nodos.getNode(idNodo)
      reac= nod.getReac()
      Qy= reac[1]
      Qz= reac[2]  # Obtención de los cortantes.
      elem= elements.getElement(idElem)
      scc= elem.getSections().getScc(idScc)
      writeInsertResultEsfComb(fOut,nmbTablaResultEsfComb,idScc,idElem,idComb,N,Qy,Qz,Mx,My,Mz)
    else:
      logging.error("Fallo en sección: ",ind(idScc), ", elemento: ",int(idElem)," combinación: ",idComb,"\n")
      exit()
      

def calculaCombinacionesBarra(nmbDatabase, nmbTablaEsfuerzos, nmbQueryEsfuerzos, nmbTablaComb, nmbQueryCombinaciones, nmbFileUpdate, nmbTablaSecc, nmbTablaResultEsfComb, nmbProcedureCreaCargas):
  '''

    nmbDatabase: Nombre de la base de datos.
    nmbTablaEsfuerzos: Nombre de la tabla de esfuerzos.
    nmbQueryEsfuerzos: Consulta para los esfuerzos de una sección.
    nmbTablaComb: Nombre de la tabla con las combinaciones.
    nmbQueryCombinaciones: Consulta para las combinaciones.
    nmbFileUpdate: Archivo en el que se guardan las sentencias de actualización.
    nmbTablaSecc: Tabla con los identificadores de las secciones.
    nmbTablaResultEsfComb: Tabla en la que se guardarán las combinaciones que resultan para cada sección.
  '''
  creaCargasSecciones(nmbDatabase,nmbTablaEsfuerzos,nmbQueryEsfuerzos,nmbTablaSecc)
  getListaCombinaciones(nmbDatabase,nmbQueryCombinaciones,"listaIdComb","listaDescompComb")
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  querySecciones= con.cursor()
  queryResultPrevio= con.cursor()
  queryResultUpdate= con.cursor()

  cargas= preprocessor.getLoadLoader
  combs= cargas.getLoadCombinations
  idComb= 0.0
  descomp= ""
  nComb= listaIdComb.size
  i= 0.0
  analisis= predefined_solutions.simple_static_linear(prueba)
  for i in range(0,nComb+1):
    idComb= listaIdComb[i]
    logging.info("idComb= ",idComb,"\n")
    descomp= listaDescompComb[i]
    logging.info("descomp= ",descomp,"\n")
    # Lanzamos el calculo de la combinación.
    comb= combs.newLoadCombination(idComb,descomp)
    cargas.addToDomain(idComb)
    analOk= analisis.analyze(1)
    sqlOut= open(nmbFileUpdate,"r")
    calculaCombSeccionesBarra(nmbDatabase,"querySecciones",idComb,nmbTablaSecc,"sqlOut",nmbTablaResultEsfComb)
    sqlOut.close()
    queryResultUpdate.execfile(nmbFileUpdate) # Actualizamos datos secciones
    os.sys("rm "+nmbFileUpdate)
    cargas.removeFromDomain(idComb)
    cargas.clearCombinaciones()
    dom.revertToStart()  
 

def exportaCombELUxBarra(nmbDatabase, nmbTablaSecc, nmbQuerySecc, nmbTablaResultEsfComb, nmbArchHip):
  fOut= open(nmbArchHip,"r")

  fOut.write("\\combinaciones_elu\n")
  fOut.write("   {\n")

  idScc= 0.0
  idComb= 0.0
  n= 0.0
  mx= 0.0
  my= 0.0
  t= 0.0
  vx= 0.0
  vy= 0.0
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  queryCombinaciones= con.cursor()

  nmbQuerySec.execute("select * from " + nmbTablaSecc)
  for rowQS in nmbQuerySec:
    idScc= rpwQS['ELEM']
    fOut.write("     \\comb_elemento\n")
    fOut.write("        {\n")
    fOut.write("           \\elemento{",sqlValue(int(idScc)),"}\n")
    queryCombinaciones.execute("select * from " + nmbTablaResultEsfComb + " where idSecc= " + sqlValue(int(idScc)))
    for rowC in queryCombinaciones:
      idComb= rowC['dComb']
      logging.info("idScc= ",int(idScc)," idComb= ",idComb,"\n")
      fOut.write("           \\combinacion{ \\nmb_hipot{",sqlValue(int(idComb)),"}")
      fOut.write(" \\esf_norm{\\N{",sqlValue(rowC['n']),"} \\Mx{",sqlValue(rowC['my']),"} \\My{",sqlValue(rowC['mz']),"}} \\esf_tang{ \\T{",sqlValue(rowC['mx']),"} \\Vx{",sqlValue(rowC['vz']),"} \\Vy{",sqlValue(rowC['vy']),"}}")
      fOut.write("}\n")
    fOut.write("        }\n")
  fOut.write("  }\n")
  fOut.close()


# Obtiene los resultados de las secciones para una combinación
def calculaCombSeccionesShell(nmbDatabase, nmbQuerySecciones, idComb, nmbTablaSecc, fOut, nmbTablaResultEsfComb, offset):

  nmbQuerySecciones.execute("select * from " + nmbTablaSecc)
  for row in nmbQuerySec:
    idElem= row['ELEM']
    idSecc1= idElem*10
    idSecc2= offset+idSecc1

    if((analOk==0.0)):
      elem= elementos.getElement(idSecc2)
      elem.calcResistingForce()
      scc= elem.getSection()
      scc.writeInsertResultEsfComb(fOut,nmbTablaResultEsfComb,idSecc1,idElem,idComb,N,Vy,Vz,Mx,My,Mz)
      elem= elementos.getElement(idSecc2)
      elem.calcResistingForce()
      scc= elem.getSection()
      scc.writeInsertResultEsfComb(fOut,nmbTablaResultEsfComb,idSecc2,idElem,idComb,N,Vy,Vz,Mx,My,Mz)
    else:
      logging.error("Fallo en elemento: ",int(idElem)," combinación: ",idComb,"\n")
      exit()
 
def calculaCombinacionesShell(nmbDatabase, nmbTablaEsfuerzos, nmbQueryEsfuerzos, nmbTablaComb, nmbQueryCombinaciones, nmbFileUpdate, nmbTablaSecc, nmbTablaResultEsfComb, offset):
  simulaCargasSeccionesShell(nmbDatabase,nmbTablaEsfuerzos,nmbQueryEsfuerzos,nmbTablaSecc,offset)

  getListaCombinaciones(nmbDatabase,nmbQueryCombinaciones,"listaIdComb","listaDescompComb")

  querySecciones= con.cursor()
  queryResultPrevio= con.cursor()
  queryResultUpdate= con.cursor()
 
  idCombShell= 0.0
  descomp= ""
  nComb= listaIdComb.size
  i= 0.0
  analisis= predefined_solutions.simple_static_linear(prueba)
  for i in range(0,nComb+1):
    idCombShell= listaIdComb[i]
    logging.info("idCombShell= ",idCombShell,"\n")
    descomp= listaDescompComb[i]
    logging.info("descomp= ",descomp,"\n")
    # Lanzamos el calculo de la combinación.
    comb= combs.newLoadCombination(idCombShell,descomp)
    cargas.addToDomain(idCombShell)
    analOk= analisis.analyze(1)
    sqlOut= open(nmbFileUpdate,"r")
    calculaCombSeccionesShell(nmbDatabase,"querySecciones",idCombShell,nmbTablaSecc,"sqlOut",nmbTablaResultEsfComb,offset)
    sqlOut.close()
    queryResultUpdate.execfile(nmbFileUpdate) # Actualizamos datos secciones
    os.sys("rm "+nmbFileUpdate)
    cargas.removeFromDomain(idCombShell)
    cargas.revertToStart()

def creaTablaCombxLamina(nmbDBase, nmbTabla):
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row
  borraTabla(nmbTabla)
  cur= con.cursor()
  cur.execute("CREATE TABLE " + nmbTabla + " (idElem INT, idComb INT, T1 DOUBLE, T2 DOUBLE, T12 DOUBLE,M1 DOUBLE,M2 DOUBLE,M12 DOUBLE,V1 DOUBLE,V2 DOUBLE);") # Creamos la tabla)
  print "existe tabla: ",nmbTabla," ",existeTabla(nmbTabla),"\n"
 

def writeUpdateCombLaminaX(fName, nmbTabl, idElem, idComb, T1, T12, M1, M12, V1):
  fName.write("\execute_sql{"+char(34))
  fName.write("insert into " + nmbTabl + " (idElem,idComb,T1,T12,M1,M12,V1) values(" + sqlValue(int(idElem)) + "," + sqlValue(int(idComb)) + "," + sqlValue(T1) + "," + sqlValue(T12) + "," + sqlValue(int(M1)) + "," + sqlValue(M12) + "," + sqlValue(V1) + ");")
  fName.write(char(34)+"}\n")
 
def writeUpdateCombLaminaY(fName, nmbTabl, idElem, idComb, T2, M2, V2):
  fName.write("\execute_sql{"+char(34))
  fName.write("UPDATE " + nmbTabl + " SET T2 = " + sqlValue(T2) + ",M2 = " + sqlValue(M2) + ", V2= " + sqlValue(V2) + " WHERE idElem = " + sqlValue(int(idElem))+ " AND idComb = ", + sqlValue(int(idComb)))
  fName.write(char(34)+"}\n")


def exportaCombxLamina(nmbDatabase, nmbTablaSecc, nmbQuerySecc, nmbTablaResultEsfComb, nmbArchHip, offset, estado, fmt):
  idElem= 0
  idComb= 0

  sqlOut= open("/tmp/combXLamina.update","r")
  con= sqlite.connect(nmbDataBase)
  con.row_factory= sqlite.Row()

  queryCombinaciones= con.cursor()
  nmbQuerySecc.execute("select * from " + nmbTablaSecc)
  for row in nmbQuerySecc:
    row= nmbQuerySecc.fetchrow()
    idElem= row["ELEM"]
    logging.info("Cargando en tabla resultados del elemento: ",int(idElem),"\n")
    idSecc1= idElem*10
    idSecc2= offset+idSecc1
    queryCombinaciones.execute("select * from " + nmbTablaResultEsfComb + " where idSecc= " + sqlValue(int(idSecc1)))
    for rowC in queryCombinaciones:
      idComb= rowC['idComb']
      writeUpdateCombLaminaX("sqlOut","combXLamina",idElem,idComb,rowC['n'],rowC['vz'],rowC['my'],rowC['mx'],rowC['vy'])

    queryCombinaciones.execute("select * from " + nmbTablaResultEsfComb + " where idSecc= " + sqlValue(int(idSecc2)))
    for rowC in queryCombinaciones:
      idComb= rowC['idComb']
      writeUpdateCombLaminaY("sqlOut","combXLamina",idElem,idComb,rowC['n'],rowC['my'],rowC['vy'])

    sqlOut.close()

    fOut= open(nmbArchHip,"r")

    fOut.write("\\combinaciones_"+estado+"\n")
    fOut.write("   {\n")
    queryResultUpdate= con.cursor()
    queryResultUpdate.execute("create table if not exists combXLamina ( idElem integer, idComb integer, T1 DOUBLE, T2 DOUBLE, T12 DOUBLE,M1 DOUBLE,M2 DOUBLE,M12 DOUBLE,V1 DOUBLE,V2 DOUBLE )") # Creamos la tabla)
    queryResultUpdate.execute("DROP INDEX if exists idx_combXLamina;") # Borramos el índice)
    queryResultUpdate.execute("delete from combXLamina") # Borramos la tabla)
    queryResultUpdate.execute("CREATE INDEX idx_combXLamina ON combXLamina(idElem,idComb);")
    queryResultUpdate.execute("VACUUM;") # Compactamos base datos)
    queryResultUpdate.execute("BEGIN;")
    execfile("/tmp/combXLamina.update") # Actualizamos datos secciones
    queryResultUpdate.execute("END;")

    # \erase{"queryResultUpdate"}
    queryResultUpdate= con.cursor()
    nmbQuerySecc.execute("select * from " + nmbTablaSecc)
    for rowC in nmbQuerySecc:
      idElem= rowC["ELEM"]
      logging.info("Exportando combinaciones elemento: ",idElem,"\n")
      idSecc1= idElem*10
      idSecc2= offset+idSecc1
      fOut.write("     \\comb_elemento\n")
      fOut.write("        {\n")
      fOut.write("           \\elemento{",sqlValue(int(idElem)),"}\n")
      queryResultUpdate.execute("select * from combXLamina where idElem = " + idElem)
      for row in cur:
        fOut.write("           \\combinacion{ \\nmb_hipot{","H-"+toupper(estado)+"-"+int(row['idComb']),"}")
        fOut.write(" \\esf_membrana{\\T1{",format(row['T1'],fmt),"} \\T2{",format(row['T2'],fmt),"} \\T12{",format(row['T12'],fmt),"} } \\esf_placa{ \\M1{",format(row['M1'],fmt),"} \\M2{",format(row['M2'],fmt),"} \\M12{",format(row['M12'],fmt),"}} \\V1{",format(row['V1'],fmt),"} \\V2{",format(row['V2'],fmt),"}")
        fOut.write("}\n")
    fOut.write("        }\n")
  fOut.write("  }\n")
  fOut.close()
  os.sys("rm -f "+"/tmp/combXLamina.update")

