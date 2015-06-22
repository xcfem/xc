# -*- coding: utf-8 -*-
def controlTNHA():
  return """nmbComb= recorder.getNombreCombActual
self.getResistingForce()
Ntmp= self.getN1
MyTmp= self.getMy1
MzTmp= self.getMz1
scc= self.getSection()
FCtmp= scc.getFactorCapacidad(nmbDiagInt,Ntmp,MyTmp,MzTmp)
if(FCtmp >= FCCP): # Caso pésimo
  FCCP= FCtmp 
  HIPCP= nmbComb
  NCP= Ntmp
  MyCP= MyTmp
  MzCP= MzTmp
Ntmp= self.getN2
MyTmp= self.getMy2
MzTmp= self.getMz2
FCtmp= scc.getFactorCapacidad(nmbDiagInt,Ntmp,MyTmp,MzTmp)
if(FCtmp >= FCCP):
  FCCP= FCtmp
  HIPCP= nmbComb
  NCP= Ntmp
  MyCP= MyTmp
  MzCP= MzTmp"""

