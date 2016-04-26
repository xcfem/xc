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

def xLaminaCalculaComb(preprocessor,elements, analysis, controller):
  '''
  Lanza el análisis (lineal) y la comprobación en las combinaciones que se pasan como parámetros
  Parameters:
    elements: elements to check
    controller: object that controls limit state in elements.
  '''
  combs= preprocessor.getLoadLoader.getLoadPatterns #Here each load pattern represents a combination.
  for key in combs.getKeys():
    comb= combs[key]
    #print "Resolviendo para acción: ",key
    resuelve_combinacion.resuelveComb(preprocessor,key,analysis,1)
    controller.check(elements,key)

def xLaminaCalculaCombEstatLin(preprocessor,elements, analysis, controller):
  '''
  Lanza el análisis (lineal) y la comprobación en las combinaciones que se pasan como parámetros
  Parameters:
    elements: elements to check
    controller: object that controls limit state in elements.
  '''
  combs= preprocessor.getLoadLoader.getLoadPatterns #Here each load pattern represents a combination.
  for key in combs.getKeys():
    comb= combs[key]
    #print "Resolviendo para acción: ",key
    resuelve_combinacion.resuelveComb(preprocessor,key,analysis,1)
    controller.check(elements,key)


def xLaminaCalculaCombEstatNoLin(elements,nmbArchDefHipELU,controller):
  '''
  Lanza el análisis (no lineal) y la comprobación en las combinaciones que se pasan como parámetros
  Parameters:
    nmbArchDefHipELU: Archivo en el que se definen las combinaciones a calcular.
    controller: object that controls limit state in elements.
  '''
  # Definimos el procedimiento de solución.
  print "XXX Definir el procedimiento de solution (simple_newton_raphson_band_genr)"
  listaCombinaciones= getListaCombinaciones(nmbArchDefHipELU)
  for comb in listaCombinaciones:
    print "Resolviendo para acción: ",comb
    resuelveCombEstatNoLin(comb)
    controller.check(elements,comb,nmbDiagIntSec1,nmbDiagIntSec2)
  os.sys("rm -f "+"/tmp/acciones.xci")
  os.sys("rm -f "+"/tmp/cargas.xci")
