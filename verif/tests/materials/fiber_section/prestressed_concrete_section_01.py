# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 0.2 # Section width expressed in meters.
depth= 0.4 # Section width expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
areaCordones= 140e-6 # Área de los cordones expressed in square meters.

def gmSecHP01(nmbGeomSecc,concrDiagName,prestressingSteelDiagramName):
  # Concrete
  geomSecc= preprocessor.getMaterialHandler.newSectionGeometry(nmbGeomSecc)
  regions= geomSecc.getRegions
  horm= regions.newQuadRegion(concrDiagName)
  horm.nDivIJ= 11
  horm.nDivJK= 11
  horm.pMin= geom.Pos2d(-depth/2.0,-width/2.0)
  horm.pMax= geom.Pos2d(depth/2.0,width/2.0)
  # Armadura
  reinforcement= geomSecc.getReinfLayers
  reinforcementInf= reinforcement.newStraightReinfLayer(prestressingSteelDiagramName)
  reinforcementInf.numReinfBars= 2
  reinforcementInf.barArea= areaCordones
  reinforcementInf.p1= geom.Pos2d(cover-depth/2.0,cover-width/2.0)
  reinforcementInf.p2= geom.Pos2d(cover-depth/2.0,width/2.0-cover)
  reinforcementSup= reinforcement.newStraightReinfLayer(prestressingSteelDiagramName)
  reinforcementSup.numReinfBars= 2
  reinforcementSup.barDiam= 16e-3
  reinforcementSup.barArea= areaCordones
  reinforcementSup.p1= geom.Pos2d(depth/2.0-cover,cover-width/2.0)
  reinforcementSup.p2= geom.Pos2d(depth/2.0-cover,width/2.0-cover)
  return geomSecc

geomSecPret01= gmSecHP01("prestressedConcretSectionGeom01",EHE_materials.HP45.nmbDiagD,EHE_materials.Y1860S7.nmbDiagD)

