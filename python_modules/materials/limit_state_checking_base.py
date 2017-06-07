# -*- coding: utf-8 -*-
''' Base classes for limit state control.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
# from materials.fiber_section import createFiberSets
# from materials.fiber_section import fiberUtils
# import math

class LimitStateControllerBase(object):
  '''
  Basic parameters for limit state control (normal stresses, shear, crack,...).'''
  def __init__(self,limitStateLabel):
    self.limitStateLabel= limitStateLabel #Property name in the check results file (something like 'ULS_shear' or 'SLS_crack_freq' or ...) 

  def check(self,elements,nmbComb):
    '''Crack control.'''
    lmsg.error('limit state check not implemented.')

class CrackControlBaseParameters(LimitStateControllerBase):
  '''
  Basic parameters for crack control.'''
  def __init__(self,limitStateLabel):
    super(CrackControlBaseParameters,self).__init__(limitStateLabel)
    self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
    self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
    self.tensionedRebarsFiberSetName= "reinforcementTraccion" #Name of the tensiones rebar fibers set.
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

