# -*- coding: utf-8 -*-
def controTNHA():
  return """nmbComb= recorder.getNombreCombActual
  self.calcResistingForce()

  Sg= self.getStress

  if(Sg>SgMax):
    SgMax= Sg 
    FCTN= abs(SgMax)/fyd
    if(FCTN>FCTNCP): 
      FCTNCP= FCTN}
      NCP=getAxil  
      HIPCPTN= getNombreCombActual

  if(Sg<SgMin):
    SgMin= Sg 
    FCTN= abs(SgMin)/fyd
    if(FCTN>FCTNCP):
      FCTNCP= FCTN
      NCP=getAxil  
      HIPCPTN= getNombreCombActual"""
