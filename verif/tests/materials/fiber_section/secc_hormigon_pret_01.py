# -*- coding: utf-8 -*-


ancho= 0.2 # Ancho de la sección expresado en metros.
canto= 0.4 # Ancho de la sección expresado en metros.
recub= 0.05 # Recubrimiento de la sección expresado en metros.
areaCordones= 140e-6 # Área de los cordones expresada en metros cuadrados.

def gmSecHP01(nmbGeomSecc,nmbDiagHormigon,nmbDiagAceroPret):
  # Hormigón
  geomSecc= preprocessor.getMaterialLoader.newSectionGeometry(nmbGeomSecc)
  regiones= geomSecc.getRegions
  horm= regiones.newQuadRegion(nmbDiagHormigon)
  horm.nDivIJ= 11
  horm.nDivJK= 11
  horm.pMin= geom.Pos2d(-canto/2.0,-ancho/2.0)
  horm.pMax= geom.Pos2d(canto/2.0,ancho/2.0)
  # Armadura
  armaduras= geomSecc.getReinfLayers
  armaduraInf= armaduras.newStraightReinfLayer(nmbDiagAceroPret)
  armaduraInf.numReinfBars= 2
  armaduraInf.barArea= areaCordones
  armaduraInf.p1= geom.Pos2d(recub-canto/2.0,recub-ancho/2.0)
  armaduraInf.p2= geom.Pos2d(recub-canto/2.0,ancho/2.0-recub)
  armaduraSup= armaduras.newStraightReinfLayer(nmbDiagAceroPret)
  armaduraSup.numReinfBars= 2
  armaduraSup.barDiam= 16e-3
  armaduraSup.barArea= areaCordones
  armaduraSup.p1= geom.Pos2d(canto/2.0-recub,recub-ancho/2.0)
  armaduraSup.p2= geom.Pos2d(canto/2.0-recub,ancho/2.0-recub)
  return geomSecc

geomSecPret01= gmSecHP01("geomSecHormigonPret01",EHE_concrete.HP45.nmbDiagD,aceroPretEHE.Y1860S7.nmbDiagD)

