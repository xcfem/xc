# -*- coding: utf-8 -*-
#Check normal stresses limit state.
import geom
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv

class BiaxialBendingNormalStressController(lsc.LimitStateControllerBase):
  '''Object that controls normal stresses limit state.'''

  def __init__(self,limitStateLabel):
    super(BiaxialBendingNormalStressController,self).__init__(limitStateLabel)

  def initControlVars(self,elements):
    '''Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars())

  def check(self,elements, nmbComb):
    '''Launch checking.
    
      :param elements: elements to check
    '''
    #print "Postprocessing combination: ",nmbComb
    for e in elements:
      e.getResistingForce()
      TagTmp= e.tag
      scc= e.getSection()
      idSection= e.getProp("idSection")
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      posEsf= geom.Pos3d(Ntmp,MyTmp,MzTmp)
      diagInt= e.getProp("diagInt")
      CFtmp= diagInt.getCapacityFactor(posEsf)
      if(CFtmp>e.getProp(self.limitStateLabel).CF):
        e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp,MzTmp)) # Worst case.

class UniaxialBendingNormalStressController(lsc.LimitStateControllerBase):
  '''Object that controls normal stresses limit state (uniaxial bending).'''

  def __init__(self,limitStateLabel):
    super(UniaxialBendingNormalStressController,self).__init__(limitStateLabel)

  def initControlVars(self,elements):
    '''Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars())

  def check(self,elements, nmbComb):
    '''
    Parameters:
      elements:    elements to check
    '''
    #print "Postprocessing combination: ",nmbComb
    for e in elements:
      e.getResistingForce()
      TagTmp= e.tag
      scc= e.getSection()
      idSection= e.getProp("idSection")
      Ntmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      posEsf= geom.Pos2d(Ntmp,MyTmp)
      diagInt= e.getProp("diagInt")
      CFtmp= diagInt.getCapacityFactor(posEsf)
      if(CFtmp>e.getProp(self.limitStateLabel).CF):
        e.setProp(self.limitStateLabel,cv.BiaxialBendingControlVars(idSection,nmbComb,CFtmp,Ntmp,MyTmp)) # Worst case.
