# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_reinforcing_steel
from materials.ehe import EHE_concrete

defSec= dict(width= 0.2, # Section width expressed in meters.
depth= 0.4, # Section depth expressed in meters.
cover= 0.05, # Concrete cover expressed in meters.
nIJ= 11, # Número de divisiones en la dirección IJ.
nJK= 11, # Número de divisiones en la dirección JK.
areaCordones= 140e-6, # Área de los cordones expressed in square meters.
diamBarra= 16e-3, # Diámetro de las barras expressed in meters.
areaBarra= 2.01e-4 # Área de las barras expressed in square meters.
  )

def gmSecHA01(nmbGeomSecc,defSec,concrDiagName,nmbDiagAceroArmar):
  geomSecc= preprocessor.getMaterialLoader.newSectionGeometry(nmbGeomSecc)
  regiones= geomSecc.getRegions
  horm= regiones.newQuadRegion(concrDiagName)
  horm.nDivIJ= defSec['nIJ']
  horm.nDivJK= defSec['nJK']
  horm.pMin= geom.Pos2d(-(defSec['depth']/2.0),-(defSec['width']/2.0))
  horm.pMax= geom.Pos2d(defSec['depth']/2.0,defSec['width']/2.0)
  reinforcement= geomSecc.getReinfLayers
  reinforcementInf= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  reinforcementInf.numReinfBars= 2
  reinforcementInf.barDiam= 16e-3
  reinforcementInf.barArea= defSec['areaBarra']
  reinforcementInf.p1= geom.Pos2d(defSec['cover']-defSec['depth']/2.0,defSec['cover']-defSec['width']/2.0) # Armadura inferior.
  reinforcementInf.p2= geom.Pos2d(defSec['cover']-defSec['depth']/2.0,defSec['width']/2.0-defSec['cover'])
  reinforcementSup= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  reinforcementSup.numReinfBars= 2
  reinforcementSup.barDiam= 16e-3
  reinforcementSup.barArea= defSec['areaBarra']
  reinforcementSup.p1= geom.Pos2d(defSec['depth']/2.0-defSec['cover'],defSec['cover']-defSec['width']/2.0) # Armadura superior.
  reinforcementSup.p2= geom.Pos2d(defSec['depth']/2.0-defSec['cover'],defSec['width']/2.0-defSec['cover'])
  return geomSecc


geomSecHormigon01= gmSecHA01(nmbGeomSecc="geomSecHormigon01",defSec=defSec,concrDiagName=EHE_concrete.HA25.nmbDiagD,nmbDiagAceroArmar=EHE_reinforcing_steel.B500S.nmbDiagD)
