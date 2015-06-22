# -*- coding: utf-8 -*-
'''
Funciones para comprobación de una sección a fisuración según el
artículo 49.2.4 de la EHE-08.
'''
from materials.fiber_section import creaSetsFibras
from materials.fiber_section import fiberUtils
import math

class CrackControlBaseParameters(object):
  '''
  Define las propiedades del registro que contiene los parámetros de cálculo
     de la fisuración.'''
  nmbSetFibrasHormigon= "hormigon" #Nombre del conjunto de fibras de hormigón.
  nmbSetFibrasArmadura= "armadura" #Nombre del conjunto de fibras de armadura.
  nmbSetFibrasArmaduraTraccion= "armaduraTraccion" #Nombre del conjunto de fibras de armadura sometida a tracción.
  claseEsfuerzo= "" #Clase de esfuerzo al que está sometida la sección.
  rcSets= None
  numBarrasTracc= 0.0 #Número de barras sometidas a tracción.
  sepBarrasTracc= 0.0 #Separación entre barras traccionadas.
  areaBarrasTracc= 0.0 #Area total de las barras traccionadas.
  tensMediaBarrasTracc= 0.0 #Tensión media en las barras traccionadas.
  yCDGBarrasTracc= 0.0 #Coordendada y del centro de gravedad de las barras traccionadas.
  zCDGBarrasTracc= 0.0 #Coordendada z del centro de gravedad de las barras traccionadas.
  recubCDGBarrasTracc= 0.0 #Recubrimiento del CDG de las barras traccionadas.

  def printParams(self):
    # Imprime los parámetros de fisuración de la sección.
    print "Clase esfuerzo: ",self.claseEsfuerzo,"\n"
    print "Num. armaduras a tracción: ",self.numBarrasTracc,"\n"
    print "Separación entre armaduras traccionadas; s= ",self.sepBarrasTracc," m\n"
    print "Area de las armaduras traccionadas; As= ",self.areaBarrasTracc*1e4," cm2\n"
    print "Centro de gravedad de las armaduras traccionadas; CDG= (",self.yCDGBarrasTracc,",",self.zCDGBarrasTracc,") m\n"
    print "Tensión media en barras traccionadas= ",self.tensMediaBarrasTracc/1e6," MPa\n"

