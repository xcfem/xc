# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_reinforcing_steel
from materials.ehe import EHE_concrete

defSec= dict(width= 0.2, # Ancho de la sección expresado en metros.
depth= 0.4, # Canto de la sección expresado en metros.
cover= 0.05, # Recubrimiento de la sección expresado en metros.
nIJ= 11, # Número de divisiones en la dirección IJ.
nJK= 11, # Número de divisiones en la dirección JK.
areaCordones= 140e-6, # Área de los cordones expresada en metros cuadrados.
diamBarra= 16e-3, # Diámetro de las barras expresado en metros.
areaBarra= 2.01e-4 # Área de las barras expresado en metros cuadrados.
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
  armaduraInf= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  armaduraInf.numReinfBars= 2
  armaduraInf.barDiam= 16e-3
  armaduraInf.barArea= defSec['areaBarra']
  armaduraInf.p1= geom.Pos2d(defSec['cover']-defSec['depth']/2.0,defSec['cover']-defSec['width']/2.0) # Armadura inferior.
  armaduraInf.p2= geom.Pos2d(defSec['cover']-defSec['depth']/2.0,defSec['width']/2.0-defSec['cover'])
  armaduraSup= reinforcement.newStraightReinfLayer(nmbDiagAceroArmar)
  armaduraSup.numReinfBars= 2
  armaduraSup.barDiam= 16e-3
  armaduraSup.barArea= defSec['areaBarra']
  armaduraSup.p1= geom.Pos2d(defSec['depth']/2.0-defSec['cover'],defSec['cover']-defSec['width']/2.0) # Armadura superior.
  armaduraSup.p2= geom.Pos2d(defSec['depth']/2.0-defSec['cover'],defSec['width']/2.0-defSec['cover'])
  return geomSecc


geomSecHormigon01= gmSecHA01("geomSecHormigon01",defSec,EHE_concrete.HA25.nmbDiagD,EHE_reinforcing_steel.B500S.nmbDiagD)
