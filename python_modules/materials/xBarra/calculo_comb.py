# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from solution import predefined_solutions
from solution import resuelve_combinacion

def getListaCombinaciones(nmbArchDefHipELU):
  '''
  Define las combinaciones descritas en el archivo que se pasa como parámetro
   y coloca sus nombres en la lista que devuelve.
   nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular. 
  '''
  lstCombRetval= []
  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  execfile("/tmp/acciones.xci")
  execfile("/tmp/cargas.xci")
  execfile(nmbArchDefHipELU)

  lstCombRetval= cargas.listaNombresCombinaciones()
  os.sys("rm -f "+"/tmp/acciones.xci")
  os.sys("rm -f "+"/tmp/cargas.xci")
  return lstCombRetval

def xBarraCalculaCombEstatLin(preprocessor, analysis, diagIntSec,procesResultVerif):
  '''
  Lanza el análisis (lineal) y la comprobación en las combinaciones que se pasan como parámetros
  nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular.
  '''
  casos= preprocessor.getLoadLoader.getLoadPatterns #Una combinación en cada caso de carga. 
  for key in casos.getKeys():
    comb= casos[key]
    #print "Resolviendo para acción: ",key
    resuelve_combinacion.resuelveComb(preprocessor,key,analysis,1)
    procesResultVerif(preprocessor,key,diagIntSec)


def xBarraCalculaCombEstatNoLin(nmbArchDefHipELU):
  '''
  Lanza el análisis (no lineal) y la comprobación en las combinaciones que se pasan como parámetros
  nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular.
  '''
  # Definimos el procedimiento de solución.
  print "XXX Definir el procedimiento de solution (simple_newton_raphson_band_genr)"
  listaCombinaciones= getListaCombinaciones(nmbArchDefHipELU)
  for comb in listaCombinaciones:
    print "Resolviendo para acción: ",comb
    resuelveCombEstatLin(comb)
    procesResultVerif(comb,nmbDiagIntSec)
  os.sys("rm -f "+"/tmp/acciones.xci")
  os.sys("rm -f "+"/tmp/cargas.xci")
