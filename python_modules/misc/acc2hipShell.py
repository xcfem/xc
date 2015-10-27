# -*- coding: utf-8 -*-

from materials import parametrosSeccionRectangular
import sqlite3 as sqlite
import os

def construyeModeloShell(preprocessor,nmbDB,pth, nmbTbEsf, nmbTbElem, offset):
  '''
   Genera las combinaciones para los elementos shell
  nmbDB: Nombre de la base de datos.
  pth: Ruta del archivo que contiene los valores de las acciones.
  estado: "elu" para estados límite últimos ó "els" para estados límite de servicio.'''
  ansysToEsfShell(pth,nmbDB,nmbTbEsf)
  extractElemFromTable(nmbDB,nmbTbEsf,nmbTbElem)
  seccPrueba= parametrosSeccionRectangular.RectangularSection()
  seccPrueba.nmb= "prueba" #Nombre para la sección.
  seccPrueba.b= .40 #Ancho de la sección expresado en m.
  seccPrueba.h= .60 #Canto de la sección expresado en m.
  seccPrueba.E= 2.1e6 #Módulo de Young del material en kp/cm2.
  seccPrueba.nu= 0.3 #Coeficiente de Poisson del material.
  defSeccAggregation3d(preprocessor,seccPrueba)
  simulaShellElemFromTable(nmbDB,nmbTbElem,seccPrueba.nmb,offset)


def acc2hipShellEstado(nmbDB, nmbTbComb, ruta, raiz, nmbTbEsf, nmbTbElem, offset, estado, fmt):
  '''
   Genera las combinaciones para los elementos shell
  nmbDB: Nombre de la base de datos.
  nmbTbComb: Nombre de la tabla que contiene las combinaciones en estado límite.
  pth: Ruta del archivo que contiene los valores de las acciones.
  estado: "elu" para estados límite últimos ó "els" para estados límite de servicio.
  '''
  nmbTbResultEsfComb= raiz+"_EsfComb_"+estado
  creaTablaResultEsfComb(nmbDB,nmbTbResultEsfComb,nmbTbElem)

  nmbQueryEsfuerzos= "recuperaLoadsElem"
  nmbQueryCombinaciones= "recuperaCombinaciones"
  nmbFileUpdate= ruta+".update"
  calculaCombinacionesShell(nmbDB,nmbTbEsf,nmbQueryEsfuerzos,nmbTbComb,nmbQueryCombinaciones,nmbFileUpdate,nmbTbElem,nmbTbResultEsfComb,offset)
  nmbArchHip= ruta+".hip_"+estado
  exportaCombxLamina(nmbDB,nmbTbElem,"querySecciones",nmbTbResultEsfComb,nmbArchHip,offset,estado,fmt)


def acc2hipShell(nmbDBComb, nmbTbCombELU, nmbTbCombELS,pth, fmt):
  '''
   Genera las combinaciones para los elementos shell
  nmbDBComb: Nombre de la base de datos que contiene las tablas de combonaciones.
  nmbTbCombELU: Nombre de la tabla que contiene las combinaciones en estado límite último.
  nmbTbCombELS: Nombre de la tabla que contiene las combinaciones en estado límite de servicio.
  pth: Ruta del archivo que contiene los valores de las acciones.
  estado: "elu" para estados límite últimos ó "els" para estados límite de servicio.
  '''
  raiz= pth.getStem
  ruta= pth.copiaHasta(".",getString)
  nmbTbEsf= raiz+"_esf"
  nmbTbElem= raiz+"_elem"
  offset= 10000
  nmbDB= ruta+".db"
  os.system("cp " + nmbDBComb + " " + nmbDB)
  #con= sqlite.connect(nmbDB)

  construyeModeloShell(nmbDB,pth,nmbTbEsf,nmbTbElem,offset)
  acc2hipShellEstado(nmbDB,nmbTbCombELU,ruta,raiz,nmbTbEsf,nmbTbElem,offset,"elu",fmt)

  acc2hipShellEstado(nmbDB,nmbTbCombELS,pth,raiz,nmbTbEsf,nmbTbElem,offset,"els",fmt)
  nmbArchHipELU= ruta+".hip_elu"
  nmbArchHipELS= ruta+".hip_els"
  nmbArchHip= ruta+".hip"
  os.system("cat " + nmbArchHipELU + " " + nmbArchHipELS + " > " + nmbArchHip)
  os.system("rm -f"+ nmbArchHipELU)
  os.system("rm -f"+ nmbArchHipELS)
  preprocessor.clearAll # Borra el modelo de elementos finitos.
  # \borra_archivo{nmbDB}

