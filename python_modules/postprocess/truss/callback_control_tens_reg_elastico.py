# -*- coding: utf-8 -*-
def controTNHA():
  return """nmbComb= recorder.getCurrentCombinationName
  self.calcResistingForce()

  Sg= self.getStress

  if(Sg>SgMax):
    SgMax= Sg 
    FCTN= abs(SgMax)/fyd
    if(FCTN>FCTNCP): 
      FCTNCP= FCTN}
      NCP=getN  
      HIPCPTN= getCurrentCombinationName

  if(Sg<SgMin):
    SgMin= Sg 
    FCTN= abs(SgMin)/fyd
    if(FCTN>FCTNCP):
      FCTNCP= FCTN
      NCP=getN  
      HIPCPTN= getCurrentCombinationName"""
