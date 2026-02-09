# -*- coding: utf-8 -*-
# TODO: Migrate to YAML or JSON data file

import geom
from materials.ehe import EHE_materials

def define_section(preprocessor, leverArm, barDiam, barArea):
    barsSectionGeometry= preprocessor.getMaterialHandler.newSectionGeometry("barsSectionGeometry")
    reinforcement= barsSectionGeometry.getReinfLayers
    reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
    reinforcementInf.numReinfBars= 2
    reinforcementInf.barDiam= barDiam
    reinforcementInf.barArea= barArea
    reinforcementInf.p1= geom.Pos2d(-leverArm,leverArm) # bottom layer.
    reinforcementInf.p2= geom.Pos2d(-leverArm,-leverArm)
    reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
    reinforcementSup.numReinfBars= 2
    reinforcementSup.barDiam= barDiam
    reinforcementSup.barArea= barArea
    reinforcementSup.p1= geom.Pos2d(leverArm,leverArm) # top layer.
    reinforcementSup.p2= geom.Pos2d(leverArm,-leverArm)
    return barsSectionGeometry, reinforcementInf, reinforcementSup
