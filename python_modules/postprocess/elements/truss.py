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

def defParamsTiranteRegElastico(elems,fydSteel):
  for e in elems:
    e.setProp("fyd",fydSteel)
    
def defVarsControlTensRegElastico(elems):
  for e in elems:
    e.setProp(Sg,0)
    e.setProp(SgMax,0)
    e.setProp(SgMin,0)
    e.setProp(NCP,0)
    e.setProp(FCTN,0)
    e.setProp(FCTNCP,0)
    e.setProp(HIPCPTN,"")
