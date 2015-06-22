# Source Generated with Decompyle++
# File: defSections.pyc (Python 2.7)

from __future__ import division
import xc_base
import geom
import xc
from materials.ehe import hormigonesEHE
from materials.ehe import acerosEHE
areaFi8 = 5e-05
areaFi10 = 7.85e-05
areaFi12 = 0.000113
areaFi16 = 0.000201
areaFi20 = 0.000314
areaFi25 = 0.0004608

def getDiagIntSection2(mdlr):
  ancho = 1
  canto = 0.3
  recpos = 0.06 + 0.008
  recneg = 0.06 + 0.008
  geomSecHA1 = mdlr.getMaterialLoader.newSectionGeometry('geomSecHA1')
  regiones = geomSecHA1.getRegions
  hormigon = regiones.newQuadRegion(hormigonesEHE.HA30.nmbDiagD)
  hormigon.nDivIJ = 10
  hormigon.nDivJK = 10
  hormigon.pMin = geom.Pos2d(-ancho / 2, -canto / 2)
  hormigon.pMax = geom.Pos2d(ancho / 2, canto / 2)
  armaduras = geomSecHA1.getReinfLayers
  armaduraInf = armaduras.newStraightReinfLayer(acerosEHE.B500S.nmbDiagD)
  armaduraInf.numReinfBars = 5
  armaduraInf.barArea = areaFi12
  armaduraInf.p1 = geom.Pos2d(-ancho / 2 + recneg, -canto / 2 + recneg)
  armaduraInf.p2 = geom.Pos2d(ancho / 2 - recneg, -canto / 2 + recneg)
  armaduraSup = armaduras.newStraightReinfLayer(acerosEHE.B500S.nmbDiagD)
  armaduraSup.numReinfBars = 5
  armaduraSup.barArea = areaFi12
  armaduraSup.p1 = geom.Pos2d(-ancho / 2 + recpos, canto / 2 - recpos)
  armaduraSup.p2 = geom.Pos2d(ancho / 2 - recpos, canto / 2 - recpos)
  materiales = mdlr.getMaterialLoader
  secHA1 = materiales.newMaterial('fiber_section_3d', 'secHA1')
  fiberSectionRepr = secHA1.getFiberSectionRepr()
  fiberSectionRepr.setGeomNamed('geomSecHA1')
  secHA1.setupFibers()
  param = xc.InteractionDiagramParameters()
  param.tagHormigon = hormigonesEHE.HA30.tagDiagD
  param.tagArmadura = acerosEHE.B500S.tagDiagD
  diagIntSecHA1 = materiales.calcInteractionDiagram('secHA1', param)
  return diagIntSecHA1


def getDiagIntSection1(mdlr):
  ancho = 1
  canto = 0.3
  recpos = 0.076 + 0.008
  recneg = 0.076 + 0.008
  geomSecHA2 = mdlr.getMaterialLoader.newSectionGeometry('geomSecHA2')
  regiones = geomSecHA2.getRegions
  hormigon = regiones.newQuadRegion(hormigonesEHE.HA30.nmbDiagD)
  hormigon.nDivIJ = 10
  hormigon.nDivJK = 10
  hormigon.pMin = geom.Pos2d(-ancho / 2, -canto / 2)
  hormigon.pMax = geom.Pos2d(ancho / 2, canto / 2)
  armaduras = geomSecHA2.getReinfLayers
  armaduraInf = armaduras.newStraightReinfLayer(acerosEHE.B500S.nmbDiagD)
  armaduraInf.numReinfBars = 7
  armaduraInf.barArea = areaFi20
  armaduraInf.p1 = geom.Pos2d(-ancho / 2 + recneg, -canto / 2 + recneg)
  armaduraInf.p2 = geom.Pos2d(ancho / 2 - recneg, -canto / 2 + recneg)
  armaduraSup = armaduras.newStraightReinfLayer(acerosEHE.B500S.nmbDiagD)
  armaduraSup.numReinfBars = 7
  armaduraSup.barArea = areaFi20
  armaduraSup.p1 = geom.Pos2d(-ancho / 2 + recpos, canto / 2 - recpos)
  armaduraSup.p2 = geom.Pos2d(ancho / 2 - recpos, canto / 2 - recpos)
  materiales = mdlr.getMaterialLoader
  secHA2 = materiales.newMaterial('fiber_section_3d', 'secHA2')
  fiberSectionRepr = secHA2.getFiberSectionRepr()
  fiberSectionRepr.setGeomNamed('geomSecHA2')
  secHA2.setupFibers()
  param = xc.InteractionDiagramParameters()
  param.tagHormigon = hormigonesEHE.HA30.tagDiagD
  param.tagArmadura = acerosEHE.B500S.tagDiagD
  diagIntSecHA2 = materiales.calcInteractionDiagram('secHA2', param)
  return diagIntSecHA2

