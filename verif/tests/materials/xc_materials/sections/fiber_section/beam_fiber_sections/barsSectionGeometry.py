# -*- coding: utf-8 -*-
# XXX Migrate to YAML

barsSectionGeometry= preprocessor.getMaterialHandler.newSectionGeometry("barsSectionGeometry")
reinforcement= barsSectionGeometry.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementInf.numReinfBars= 2
reinforcementInf.barDiam= 16e-3
reinforcementInf.barArea= areaFi16
reinforcementInf.p1= geom.Pos2d(-brazo,brazo) # bottom layer.
reinforcementInf.p2= geom.Pos2d(-brazo,-brazo)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementSup.numReinfBars= 2
reinforcementSup.barDiam= 16e-3
reinforcementSup.barArea= areaFi16
reinforcementSup.p1= geom.Pos2d(brazo,brazo) # top layer.
reinforcementSup.p2= geom.Pos2d(brazo,-brazo)
