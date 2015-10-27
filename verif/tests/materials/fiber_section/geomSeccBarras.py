# -*- coding: utf-8 -*-
geomSecBarras= preprocessor.getMaterialLoader.newSectionGeometry("geomSecBarras")
armaduras= geomSecBarras.getReinfLayers
armaduraInf= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraInf.numReinfBars= 2
armaduraInf.barDiam= 16e-3
armaduraInf.barArea= areaFi16
armaduraInf.p1= geom.Pos2d(-brazo,brazo) # Armadura inferior.
armaduraInf.p2= geom.Pos2d(-brazo,-brazo)
armaduraSup= armaduras.newStraightReinfLayer(EHE_reinforcing_steel.B500S.nmbDiagD)
armaduraSup.numReinfBars= 2
armaduraSup.barDiam= 16e-3
armaduraSup.barArea= areaFi16
armaduraSup.p1= geom.Pos2d(brazo,brazo) # Armadura superior.
armaduraSup.p2= geom.Pos2d(brazo,-brazo)
