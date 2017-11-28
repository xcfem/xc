# -*- coding: utf-8 -*-
''' Base classes for limit state control.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
# from materials.sections.fiber_section import createFiberSets
from materials.sections.fiber_section import fiberUtils
import math

class LimitStateControllerBase(object):
  '''
  Basic parameters for limit state control (normal stresses, shear, crack,...).'''
  tensionedRebarsFiberSetName= "tensionedReinforcement" #Name of the tensiones rebar fibers set.

  def __init__(self,limitStateLabel, fakeSection= True):
    '''
    :param limitStateLabel: property name in the check results file 
                            (something like 'ULS_shear' or 'SLS_crack_freq' 
                            or ...).
    :param fakeSection:    true if a fiber section model of the section is not 
                           needed to perform control.
    '''
    self.limitStateLabel= limitStateLabel
    self.fakeSection= fakeSection

  def check(self,elements,nmbComb):
    '''Crack control.'''
    lmsg.error('limit state check not implemented.')

class TensionedRebarsBasicProperties(object):
  '''Basic properties of tensioned rebars (used in shear checking).

  '''
  
  def __init__(self):
    self.number= 0 #Number of tensioned bars.
    self.area= 0.0 #Tensioned rebars area.

    
class TensionedRebarsProperties(TensionedRebarsBasicProperties):
  '''Properties of tensioned rebars that are useful for crack control 
   analysis.
  '''
  
  def __init__(self):
    super(TensionedRebarsProperties,self).__init__()
    self.averageStress= 0.0 #Tensioned rebars average stress.
    self.iAreaMaxima= None #Tensioned rebars maximum area.
    self.maxDiameter= 0.0 #Tensioned rebars maximum diameter.
    self.E= 0.0 #Tensioned rebars young modulus.
    self.spacing= None #Tensioned rebars spacing.
    self.yCentroid= None
    self.zCentroid= None
    self.cover= None #Cover of tensioned rebars.
    self.effectiveArea= None #Area of concrete of the cover zone, (see figure 49.2.4.b of EHE-08)
                             # in which the tension bars effectively influence the crack opening.
  def setup(self,tensionedReinforcement):
    '''Get the parameter values from the fiber set.

     :param tensionedReinforcement: fibers that represent tensioned rebars.
    '''
    self.spacing= tensionedReinforcement.getAverageDistanceBetweenFibers()
    self.area= tensionedReinforcement.getArea(1)
    self.yCentroid= tensionedReinforcement.getCdgY()
    self.zCentroid= tensionedReinforcement.getCdgZ()
    self.averageStress= tensionedReinforcement.getStressMed()
    self.iAreaMaxima= fiberUtils.getIMaxPropFiber(tensionedReinforcement,"getArea")
    self.maxDiameter= 2*math.sqrt(tensionedReinforcement[self.iAreaMaxima].getArea()/math.pi) 
    self.E= tensionedReinforcement[0].getMaterial().getInitialTangent()

  def printParams(self):
    print "Number of tensioned bars: ",self.number,"\n"
    print "Spacement of tensioned bars; s= ",self.spacing," m\n"
    print "Area of tensioned bars; As= ",self.area*1e4," cm2\n"
    print "Centroid of tensioned bars; COG= (",self.yCentroid,",",self.zCentroid,") m\n"
    print "Tensioned rebars average stress: ",self.averageStress/1e6," MPa\n"
    print "Effective area; AcEf= ",self.effectiveArea*1e4," cm2\n"

class CrackControlBaseParameters(LimitStateControllerBase):
  '''
  Basic parameters for crack control.'''
  def __init__(self,limitStateLabel):
    super(CrackControlBaseParameters,self).__init__(limitStateLabel)
    self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
    self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
    self.tensionedRebars= TensionedRebarsProperties()
    self.claseEsfuerzo= "" #Clase de esfuerzo al que está sometida la sección.
    self.rcSets= None

  def printParams(self):
    # Prints the section crack control parameters.
    print "Clase esfuerzo: ",self.claseEsfuerzo,"\n"
    self.tensionedRebars.printParams()


