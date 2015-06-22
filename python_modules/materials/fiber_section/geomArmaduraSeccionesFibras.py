# -*- coding: utf-8 -*-


'''
 Capa de armadura paralela a los paramentos inferior y superior y
    situada a una altura h sobre el centro.
'''
def gmArmCapaHoriz(sectionGeom, nmbFiberMat, nBarras, areaBarras, canto, ancho, rcub, h):
  armaduras= sectionGeom.getReinfLayers
  armaduraH= armaduras.newStraightReinfLayer(nmbFiberMat)
  armaduraH.numReinfBars= nBarras
  armaduraH.barArea= areaBarras
  armaduraH.p1= geom.Pos2d(h,rcub-ancho/2) # Armadura superior.
  armaduraH.p2= geom.Pos2d(h-rcub,ancho/2-rcub)
  return armaduraH

# Definición de la armadura situada sobre el paramento inferior.
def gmArmInferior(sectionGeom,nmbFiberMat, nBarras, areaBarras, canto, ancho, rcub):
  return gmArmCapaHoriz(sectionGeom, nmbFiberMat, nBarras, areaBarras, canto, ancho, rcub, rcub-canto/2)

# Definición de la armadura situada bajo el paramento superior.
def gmArmSuperior(sectionGeom, nmbFiberMat, nBarras, areaBarras, canto, ancho, rcub):
  return gmArmCapaHoriz(sectionGeom, nmbFiberMat, nBarras, areaBarras, canto, ancho, rcub, canto/2-rcub)

#  Capa de armadura de piel situada a una altura h sobre el centro de la sección.
def gmArmPiel(nmbFiberMat, areaBarras, canto, ancho, rcub, h):
  return gmArmCapaHoriz(sectionGeom, nmbFiberMat, 2, areaBarras, canto, ancho, rcub, h)

