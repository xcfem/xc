# -*- coding: utf-8 -*-
from __future__ import print_function

from __future__ import division
''' Classes and functions for limit state checking according to Eurocode 5. '''

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

from misc_utils import log_messages as lmsg
from materials.sections import section_properties
from postprocess import def_vars_control
from postprocess import callback_controls
from postprocess import prop_statistics as ps


class EC5TimberRectCrossSection(section_properties.RectangularSection):
  """Timber section with Eurocode 5 verification routines."""
  def __init__(self,name,b,h,E,nu,fyd,taud):
    super(EC3TimberRectCrossSection,self).__init__(name,b,h,E,nu)
    self.fyd= fyd
    self.taud= taud

  def setupULSControlVars2d(self,elems):
    '''For each element creates the variables
       needed to check ultimate limit state criterion to satisfy.'''
    def_vars_control.defVarsControlTensRegElastico2d(elems)
    W= self.Wzel()
    for e in elems:
      e.setProp("fyd",self.fyd)
      e.setProp("fydV",self.taud)
      e.setProp("Wel",W)

  def setupULSControlVars3d(self,elems):
    '''For each element creates the variables
       needed to check ultimate limit state criterion to satisfy.'''
    def_vars_control.defVarsControlTensRegElastico3d(elems)
    Wz= self.Wzel()
    Wy= self.Wyel()
    for e in elems:
      e.setProp("fyd",self.fyd)
      e.setProp("fydV",self.taud)
      e.setProp("Wyel",Wy)
      e.setProp("AreaQy",0.9*self.A())
      e.setProp("Wzel",Wz)
      e.setProp("AreaQz",self.A()-e.getProp("AreaQy"))

  def installElementElasticStressesControlRecorder(self,recorderName, elemSet):
    preprocessor= elemSet.owner.getPreprocessor
    nodes= preprocessor.getNodeHandler
    domain= preprocessor.getDomain
    recorder= domain.newRecorder(recorderName,None)
    recorder.setElements(elemSet.getTags())
    if(nodes.numDOFs==3):
      self.setupULSControlVars2d(elemSet)
      recorder.callbackRecord= callback_controls.controTensRecElastico2d()
    else:
      self.setupULSControlVars3d(elemSet)
      recorder.callbackRecord= callback_controls.controTensRecElastico3d()

    recorder.callbackRestart= "print(\"Restart method called.\")"
    return recorder

def printResultsELU(elems,crossSection):
  '''print(ULS results.)'''
  fmt= "{:6.1f}"
  fmt2= "{:5.2f}"
  e= ps.getItemWithMaxProp(elems,"getProp",'FCTNCP')
  N= fmt.format(e.getProp("NCP")/1e3)
  My= fmt.format(e.getProp("MyCP")/1e3)
  Mz= fmt.format(e.getProp("MzCP")/1e3)
  sgMax= fmt.format(e.getProp("SgMax")/1e6)
  sgMin= fmt.format(e.getProp("SgMin")/1e6)
  sgAdm= fmt.format(crossSection.fyd/1e6)
  fctnCP= fmt2.format(e.getProp("FCTNCP"))
  print("tag= ", e.tag, " N= ", N , "kN  My= ", My , "kN.m  Mz= ", Mz , "kN-m   SgMax= ", sgMax , "MPa  SgMin= ", sgMin, "MPa  sgAdm= ", sgAdm , "MPa  FCTNCP= ", fctnCP , "HIPCPTN= ",e.getProp("HIPCPTN"))
  e=  ps.getItemWithMaxProp(elems,"getProp",'FCVCP')
  Vy= fmt.format(e.getProp("VyCP")/1e3)
  Vz= fmt.format(e.getProp("VzCP")/1e3)
  tauMax=  fmt.format(e.getProp("TauMax")/1e6)
  tauAdm= fmt.format(crossSection.taud/1e6)
  fcvCP= fmt2.format(e.getProp("FCVCP"))
  print("tag= ", e.tag, " Vy= ", Vy , "kN  Vz= ", Vz , "kN  TauMax= ", tauMax , "MPa tauAdm= ", tauAdm, "MPa FCVCP= ", fcvCP , " HIPCPV= ", e.getProp("HIPCPV"))
