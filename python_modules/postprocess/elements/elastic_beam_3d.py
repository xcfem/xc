# -*- coding: utf-8 -*-
def controlTNHA():
  ''' Callback control for normal stresses in reinforced
      concrete sections.'''
  return """nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
Ntmp= self.getN1
MyTmp= self.getMy1
MzTmp= self.getMz1
scc= self.getSection()
FCtmp= scc.getCapacityFactor(nmbDiagInt,Ntmp,MyTmp,MzTmp)
if(FCtmp >= FCCP): # Caso pésimo
  FCCP= FCtmp 
  HIPCP= nmbComb
  NCP= Ntmp
  MyCP= MyTmp
  MzCP= MzTmp
Ntmp= self.getN2
MyTmp= self.getMy2
MzTmp= self.getMz2
FCtmp= scc.getCapacityFactor(nmbDiagInt,Ntmp,MyTmp,MzTmp)
if(FCtmp >= FCCP):
  FCCP= FCtmp
  HIPCP= nmbComb
  NCP= Ntmp
  MyCP= MyTmp
  MzCP= MzTmp"""

