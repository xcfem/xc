## -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__cppyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

areaCordones= 140e-6 # Área de los cordones expresada en metros cuadrados.
areaFiLong= 0.5e-4 # Fi8 área de la armadura longitudinal expresada en metros cuadrados.
def gmSecHP02(nmbGeomSecc,concrDiagName,nmbDiagAceroArmar,nmbDiagAceroPret):
  # Hormigón
  geomSecc= preprocessor.getMaterialLoader.newSectionGeometry(nmbGeomSecc)
  regiones= geomSecc.getRegions

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Transicion 1 ala inferior - alma
  horm.nDivJK= 5
  horm.pMin= geom.Pos2d(-0.813,-0.35)
  horm.pMax= geom.Pos2d(-0.593,0.35)

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Ala inferior
  horm.nDivJK= 5
  horm.setQuad(geom.Cuadrilatero2d(geom.Pos2d(-0.593, 0.35), geom.Pos2d(-0.593, -0.35), geom.Pos2d(-0.523, -0.15), geom.Pos2d(-0.523, 0.15)))


  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Transicion 2 ala inferior - alma
  horm.nDivJK= 5
  horm.setQuad(geom.Cuadrilatero2d(geom.Pos2d(-0.523, 0.15), geom.Pos2d(-0.523, -0.15), geom.Pos2d(-0.413, -0.075), geom.Pos2d(-0.413, 0.075)))

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Alma 
  horm.nDivJK= 5
  horm.pMin= geom.Pos2d(-0.413,-0.075)
  horm.pMax= geom.Pos2d(0.587,0.075)

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Transicion 1 alma - ala superior
  horm.nDivJK= 5
  horm.setQuad(geom.Cuadrilatero2d(geom.Pos2d(0.587, 0.075), geom.Pos2d(0.587, -0.075), geom.Pos2d(0.687, -0.2), geom.Pos2d(0.687, 0.2)))

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 5 # Transicion 2 alma - ala superior
  horm.nDivJK= 5
  horm.setQuad(geom.Cuadrilatero2d(geom.Pos2d(0.687, 0.2), geom.Pos2d(0.687, -0.2), geom.Pos2d(0.787, -0.6), geom.Pos2d(0.787, 0.6)))

  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= 4 # Ala superior
  horm.nDivJK= 4
  horm.pMin= geom.Pos2d(0.787,-0.6)
  horm.pMax= geom.Pos2d(0.837,0.6)

  reinforcement= geomSecc.getReinfLayers

  # Armadura activa
  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroPret)
  armadura.numReinfBars= 13 # Armadura activa inferior ala inferior.
  armadura.barArea= areaCordones
  armadura.p1= geom.Pos2d(-0.773,-0.31) 
  armadura.p2= geom.Pos2d(-0.773,0.31)

  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroPret)
  armadura.numReinfBars= 13 # Armadura activa intermedia ala inferior.
  armadura.barArea= areaCordones
  armadura.p1= geom.Pos2d(-0.713,-0.31) 
  armadura.p2= geom.Pos2d(-0.713,0.31)

  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroPret)
  armadura.numReinfBars= 7
  armadura.barArea= areaCordones
  armadura.p1= geom.Pos2d(-0.653,-0.155) # Armadura activa superior ala inferior.
  armadura.p2= geom.Pos2d(-0.653,0.155)

  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroPret)
  armadura.numReinfBars= 3
  armadura.barArea= areaCordones
  armadura.p1= geom.Pos2d(0.797,-0.478) # Armadura activa ala superior.
  armadura.p2= geom.Pos2d(0.797,0.478)

  # Armadura pasiva
  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  armadura.numReinfBars= 5
  armadura.barArea= areaFiLong
  armadura.p1= geom.Pos2d(-0.413,-0.031)
  armadura.p2= geom.Pos2d(0.587,-0.031)

  armadura= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  armadura.numReinfBars= 5
  armadura.barArea= areaFiLong
  armadura.p1= geom.Pos2d(-0.413,0.031)
  armadura.p2= geom.Pos2d(0.587,0.031)

  return geomSecc


geomSecPret02= gmSecHP02("geomSecHormigonPret02",EHE_concrete.HP45.nmbDiagD,EHE_reinforcing_steel.B500S.nmbDiagD,aceroPretEHE.Y1860S7.nmbDiagD)

