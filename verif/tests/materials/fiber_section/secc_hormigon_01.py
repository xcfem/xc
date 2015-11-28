# -*- coding: utf-8 -*-

from materials.ehe import EHE_reinforcing_steel
from materials.ehe import EHE_concrete

defSec= dict(ancho= 0.2, # Ancho de la sección expresado en metros.
canto= 0.4, # Canto de la sección expresado en metros.
recub= 0.05, # Recubrimiento de la sección expresado en metros.
nIJ= 11, # Número de divisiones en la dirección IJ.
nJK= 11, # Número de divisiones en la dirección JK.
areaCordones= 140e-6, # Área de los cordones expresada en metros cuadrados.
diamBarra= 16e-3, # Diámetro de las barras expresado en metros.
areaBarra= 2.01e-4 # Área de las barras expresado en metros cuadrados.
  )

def gmSecHA01(nmbGeomSecc,defSec,nmbDiagHormigon,nmbDiagAceroArmar):
  geomSecc= preprocessor.getMaterialLoader.newSectionGeometry(nmbGeomSecc)
  regiones= geomSecc.getRegions
  horm= regiones.newQuadRegion(nmbDiagHormigon)
  horm.nDivIJ= defSec['nIJ']
  horm.nDivJK= defSec['nJK']
  horm.pMin= geom.Pos2d(-(defSec['canto']/2.0),-(defSec['ancho']/2.0))
  horm.pMax= geom.Pos2d(defSec['canto']/2.0,defSec['ancho']/2.0)
  armaduras= geomSecc.getReinfLayers
  armaduraInf= armaduras.newStraightReinfLayer(nmbDiagAceroArmar)
  armaduraInf.numReinfBars= 2
  armaduraInf.barDiam= 16e-3
  armaduraInf.barArea= defSec['areaBarra']
  armaduraInf.p1= geom.Pos2d(defSec['recub']-defSec['canto']/2.0,defSec['recub']-defSec['ancho']/2.0) # Armadura inferior.
  armaduraInf.p2= geom.Pos2d(defSec['recub']-defSec['canto']/2.0,defSec['ancho']/2.0-defSec['recub'])
  armaduraSup= armaduras.newStraightReinfLayer(nmbDiagAceroArmar)
  armaduraSup.numReinfBars= 2
  armaduraSup.barDiam= 16e-3
  armaduraSup.barArea= defSec['areaBarra']
  armaduraSup.p1= geom.Pos2d(defSec['canto']/2.0-defSec['recub'],defSec['recub']-defSec['ancho']/2.0) # Armadura superior.
  armaduraSup.p2= geom.Pos2d(defSec['canto']/2.0-defSec['recub'],defSec['ancho']/2.0-defSec['recub'])
  return geomSecc


geomSecHormigon01= gmSecHA01("geomSecHormigon01",defSec,EHE_concrete.HA25.nmbDiagD,EHE_reinforcing_steel.B500S.nmbDiagD)
