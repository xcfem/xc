#!/bin/bash
for fl in $(find ../../verif ../../python_modules -type f -name *.py | grep -v build)
do
echo $fl
# sed -i 's/nmbSet/setName/g' $fl
# sed -i 's/fibrasHormigon/concrFibers/g' $fl
# sed -i 's/fibrasArmadura/reinfFibers/g' $fl
# sed -i 's/traction/tension/g' $fl
# sed -i 's/creaSetsFibras/createFiberSets/g' $fl
# sed -i 's/tagDiag/matTag/g' $fl
# sed -i 's/tagCDiag/concrMatTag/g' $fl
# sed -i 's/setNameC/concrSetName/g' $fl
# sed -i 's/tagSdiag/reinfMatTag/g' $fl
# sed -i 's/setNameS/reinfSetName/g' $fl
#sed -i 's/reselTractionFibers/reselTensionFibers/g' $fl
#sed -i 's/getNumBarrasTraccion/getNumTensionRebars/g' $fl
#sed -i 's/tagCdiag/concrMatTag/g' $fl
#sed -i 's/tagHA/concrMatTag/g' $fl
#sed -i 's/tagAcero/reinfMatTag/g' $fl
#sed -i 's/createFiberSetsHA/createRCFiberSets/g' $fl
# sed -i 's/recub/cover/g' $fl
# sed -i 's/nBarrasAncho/nRebarsWidth/g' $fl
# sed -i 's/areaBarrasAncho/areaRebarWidth/g' $fl
# sed -i 's/nBarrasCanto/nRebarsDepth/g' $fl
# sed -i 's/areaBarrasCanto/areaRebarDepth/g' $fl
#sed -i 's/RecordSeccionHAPilar/RecordRCColumnSection/g' $fl
#sed -i 's/ancho/width/g' $fl
#sed -i 's/widthrage/anchorage/g' $fl
#sed -i 's/canto/depth/g' $fl
#sed -i 's/nmbDiagArmadura/reinfDiagName/g' $fl
# sed -i 's/nmbFamilia/familyName/g' $fl
# sed -i 's/nRamas/nShReinfBranches/g' $fl
# sed -i 's/areaRama/areaShReinfBranch/g' $fl
# sed -i 's/espaciamientoRamas/shReinfSpacing/g' $fl
# sed -i 's/angAlphaRamas/angAlphaShReinf/g' $fl
# sed -i 's/angThetaBielas/angThetaConcrStruts/g' $fl
# sed -i 's/diamBarras/rebarsDiam/g' $fl
# sed -i 's/sepBarras/rebarsSpacing/g' $fl
# sed -i 's/nBarras/nRebars/g' $fl
# sed -i 's/areaBarras/areaRebar/g' $fl
# sed -i 's/areaRebarEHE/areaBarrasEHE/g' $fl
# sed -i 's/centraBarras/centerRebars/g' $fl
# sed -i 's/nmbFiberMat/fiberMatName/g' $fl
# sed -i 's/rcub/cover/g' $fl
# sed -i 's/gmArmCapaHoriz/gmHorizRowRebars/g' $fl
# sed -i 's/gmArmInferior/gmBottomRowRebars/g' $fl
# sed -i 's/mArmSuperior/gmTopRowRebars/g' $fl
# sed -i 's/gmArmPiel/gmSideFaceRebars/g' $fl
done
