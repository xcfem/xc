# -*- coding: utf-8 -*-
# TODO: Migrate to YAML or JSON data file

import geom
from materials.ehe import EHE_materials

def define_section(preprocessor, leverArm, barDiam):
    barsSectionGeometry= preprocessor.getMaterialHandler.newSectionGeometry("barsSectionGeometry")
    reinforcement= barsSectionGeometry.getReinfLayers
    reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
    reinforcementInf.numReinfBars= 2
    reinforcementInf.barDiameter= barDiam
    # reinforcementInf.barArea= barArea
    reinforcementInf.setP1P2(geom.Pos2d(-leverArm,leverArm), geom.Pos2d(-leverArm,-leverArm)) # bottom layer.
    reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
    reinforcementSup.numReinfBars= 2
    reinforcementSup.barDiameter= barDiam
    # reinforcementSup.barArea= barArea
    reinforcementSup.setP1P2(geom.Pos2d(leverArm,leverArm), geom.Pos2d(leverArm,-leverArm)) # top layer.
    return barsSectionGeometry, reinforcementInf, reinforcementSup
