# -*- coding: utf-8 -*-
#Basic parameters for crack control.

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.fiber_section import createFiberSets
from materials.fiber_section import fiberUtils
from materials import LimitStateControllerBase as lsc
import math

class CrackControlBaseParameters(lsc.LimitStateControllerBase):
  '''
  Basic parameters for crack control.'''
  def __init__(self,limitStateLabel):
    super(CrackControlBaseParameters,self).__init__(limitStateLabel)
    self.setNameFibrasHormigon= "hormigon" #Nombre del conjunto de fibras de hormigón.
    self.setNameFibrasArmadura= "reinforcement" #Nombre del conjunto de fibras de reinforcement.
    self.setNameFibrasArmaduraTraccion= "reinforcementTraccion" #Nombre del conjunto de fibras de reinforcement sometida a tracción.
    self.claseEsfuerzo= "" #Clase de esfuerzo al que está sometida la sección.
    self.rcSets= None
    self.numBarrasTracc= 0.0 #Número de barras sometidas a tracción.
    self.rebarsSpacingTracc= 0.0 #Separación entre barras traccionadas.
    self.areaRebarTracc= 0.0 #Area total de las barras traccionadas.
    self.tensMediaBarrasTracc= 0.0 #Tensión media en las barras traccionadas.
    self.yCDGBarrasTracc= 0.0 #Coordendada y del centro de gravedad de las barras traccionadas.
    self.zCDGBarrasTracc= 0.0 #Coordendada z del centro de gravedad de las barras traccionadas.
    self.coverCDGBarrasTracc= 0.0 #Recubrimiento del CDG de las barras traccionadas.

  def printParams(self):
    # Imprime los parámetros de fisuración de la sección.
    print "Clase esfuerzo: ",self.claseEsfuerzo,"\n"
    print "Num. reinforcement a tracción: ",self.numBarrasTracc,"\n"
    print "Separación entre reinforcement traccionadas; s= ",self.rebarsSpacingTracc," m\n"
    print "Area de las reinforcement traccionadas; As= ",self.areaRebarTracc*1e4," cm2\n"
    print "Centro de gravedad de las reinforcement traccionadas; CDG= (",self.yCDGBarrasTracc,",",self.zCDGBarrasTracc,") m\n"
    print "Tensión media en barras traccionadas= ",self.tensMediaBarrasTracc/1e6," MPa\n"

