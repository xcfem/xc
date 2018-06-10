# Source Generated with Decompyle++
# File: defSections.pyc (Python 2.7)

from __future__ import division
import xc_base
import geom
import xc
from materials.ehe import EHE_materials
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
  geomSecHA1 = mdlr.getMaterialHandler.newSectionGeometry('geomSecHA1')
  regions = geomSecHA1.getRegions
  concrete= regions.newQuadRegion(EHE_materials.HA30.nmbDiagD)
  concrete.nDivIJ = 10
  concrete.nDivJK = 10
  concrete.pMin = geom.Pos2d(-ancho / 2, -canto / 2)
  concrete.pMax = geom.Pos2d(ancho / 2, canto / 2)
  reinforcement = geomSecHA1.getReinfLayers
  bottomReinforcement = reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
  bottomReinforcement.numReinfBars = 5
  bottomReinforcement.barArea = areaFi12
  bottomReinforcement.p1 = geom.Pos2d(-ancho / 2 + recneg, -canto / 2 + recneg)
  bottomReinforcement.p2 = geom.Pos2d(ancho / 2 - recneg, -canto / 2 + recneg)
  topReinforcement = reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
  topReinforcement.numReinfBars = 5
  topReinforcement.barArea = areaFi12
  topReinforcement.p1 = geom.Pos2d(-ancho / 2 + recpos, canto / 2 - recpos)
  topReinforcement.p2 = geom.Pos2d(ancho / 2 - recpos, canto / 2 - recpos)
  materiales = mdlr.getMaterialHandler
  secHA1 = materiales.newMaterial('fiber_section_3d', 'secHA1')
  fiberSectionRepr = secHA1.getFiberSectionRepr()
  fiberSectionRepr.setGeomNamed('geomSecHA1')
  secHA1.setupFibers()
  param = xc.InteractionDiagramParameters()
  param.concreteTag = EHE_materials.HA30.tagDiagD
  param.reinforcementTag = EHE_materials.B500S.tagDiagD
  diagIntSecHA1 = materiales.calcInteractionDiagram('secHA1', param)
  return diagIntSecHA1


def getDiagIntSection1(mdlr):
  ancho = 1
  canto = 0.3
  recpos = 0.076 + 0.008
  recneg = 0.076 + 0.008
  geomSecHA2 = mdlr.getMaterialHandler.newSectionGeometry('geomSecHA2')
  regions = geomSecHA2.getRegions
  concrete= regions.newQuadRegion(EHE_materials.HA30.nmbDiagD)
  concrete.nDivIJ = 10
  concrete.nDivJK = 10
  concrete.pMin = geom.Pos2d(-ancho / 2, -canto / 2)
  concrete.pMax = geom.Pos2d(ancho / 2, canto / 2)
  reinforcement = geomSecHA2.getReinfLayers
  bottomReinforcement = reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
  bottomReinforcement.numReinfBars = 7
  bottomReinforcement.barArea = areaFi20
  bottomReinforcement.p1 = geom.Pos2d(-ancho / 2 + recneg, -canto / 2 + recneg)
  bottomReinforcement.p2 = geom.Pos2d(ancho / 2 - recneg, -canto / 2 + recneg)
  topReinforcement = reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
  topReinforcement.numReinfBars = 7
  topReinforcement.barArea = areaFi20
  topReinforcement.p1 = geom.Pos2d(-ancho / 2 + recpos, canto / 2 - recpos)
  topReinforcement.p2 = geom.Pos2d(ancho / 2 - recpos, canto / 2 - recpos)
  materiales = mdlr.getMaterialHandler
  secHA2 = materiales.newMaterial('fiber_section_3d', 'secHA2')
  fiberSectionRepr = secHA2.getFiberSectionRepr()
  fiberSectionRepr.setGeomNamed('geomSecHA2')
  secHA2.setupFibers()
  param = xc.InteractionDiagramParameters()
  param.concreteTag = EHE_materials.HA30.tagDiagD
  param.reinforcementTag = EHE_materials.B500S.tagDiagD
  diagIntSecHA2 = materiales.calcInteractionDiagram('secHA2', param)
  return diagIntSecHA2

