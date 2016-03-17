# -*- coding: utf-8 -*-
# Cuantía geométrica mínima en pilares.
def cuantiaGeomMinPilares(Ac):
  return 0.004*Ac 

# Cuantía mecánica mínima en pilares.
def cuantiaMecMinPilares(Nd, fyd):
  return 0.1*Nd/fyd 

# Cuantía máxima en pilares.
def cuantiaMaxPilares(Ac, fcd, fyd):
  return min((fcd*Ac)/fyd,0.08*Ac)
 
# Verificación de la cuantía de la reinforcement longitudinal en pilares.
def verifCuantiaPilar(Ac, As, fcd, fyd, Nd):
  ctMinGeom= cuantiaGeomMinPilares(Ac)
  if(As<ctMinGeom):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima geométrica: ",ctMinGeom*1e4," cm2\n"
  ctMinMec= cuantiaMecMinPilares(Nd,fyd)
  if(As<ctMinMec):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, inferior a la mínima mecánica: ",ctMinMec*1e4," cm2\n"
  ctMax= cuantiaMaxPilares(Ac,fcd,fyd)
  if(As>ctMax):
    print "Cuantía de reinforcement dispuesta: ",As*1e4," cm2, superior a la máxima: ",ctMax*1e4," cm2\n"


