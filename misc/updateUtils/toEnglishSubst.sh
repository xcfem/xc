#!/bin/bash
for fl in $(find ../../verif ../../python_modules ../../doc/XCmanual -type f -name *.py -o -name *.tex | grep -v build)
#Modified by LP to allow generic use (no only in xc directories).

#for fl in $(find -type f -name '*.py')

do

# echo $fl
# sed -i 's/mdlr/preprocessor/g' $fl
# sed -i 's/modeler/preprocessor/g' $fl

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
# sed -i 's/reselTractionFibers/reselTensionFibers/g' $fl
# sed -i 's/getNumBarrasTraccion/getNumTensionRebars/g' $fl
# sed -i 's/tagCdiag/concrMatTag/g' $fl
# sed -i 's/tagHA/concrMatTag/g' $fl
# sed -i 's/tagAcero/reinfMatTag/g' $fl
# sed -i 's/createFiberSetsHA/createRCFiberSets/g' $fl
# sed -i 's/recub/cover/g' $fl
# sed -i 's/nBarrasAncho/nRebarsWidth/g' $fl
# sed -i 's/areaBarrasAncho/areaRebarWidth/g' $fl
# sed -i 's/nBarrasCanto/nRebarsDepth/g' $fl
# sed -i 's/areaBarrasCanto/areaRebarDepth/g' $fl
# sed -i 's/RecordSeccionHAPilar/RecordRCColumnSection/g' $fl
# sed -i 's/ancho/width/g' $fl
# sed -i 's/widthrage/anchorage/g' $fl
# sed -i 's/canto/depth/g' $fl
# sed -i 's/nmbDiagArmadura/reinfDiagName/g' $fl
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
# sed -i 's/gmSeccCuadrado/gmSquareSection/g' $fl
# sed -i 's/gmSeccRectangulo/gmRectangSection/g' $fl
# sed -i 's/nmbSeccion/sectionName/g' $fl
# sed -i 's/descSeccion/sectionDescr/g' $fl
# sed -i 's/tipoHormigon/concrType/g' $fl
# sed -i 's/nmbDiagHormigon/concrDiagName/g' $fl
# sed -i 's/tipoArmadura/reinfSteelType/g' $fl
# sed -i 's/RecordArmaduraCortante/RecordShearReinforcement/g' $fl
# sed -i 's/armCortanteZ/shReinfZ/g' $fl
# sed -i 's/armCortanteY/shReinfY/g' $fl
# sed -i 's/nmbGeomSeccion/gmSectionName/g' $fl
# sed -i 's/barrasNeg/negatvRebars/g' $fl
# sed -i 's/barrasPos/positvRebars/g' $fl
# sed -i 's/nmbRespT/respTName/g' $fl
# sed -i 's/nmbRespVy/respVyName/g' $fl
# sed -i 's/nmbRespVz/respVzName/g' $fl
# sed -i 's/tipoDiag/matDiagType/g' $fl
# sed -i 's/RecordSeccionHASimple/RecordRCSimpleSection/g' $fl
# sed -i 's/RecordSeccionHALosa/RecordRCSlabSection/g' $fl
# sed -i 's/defGeomSeccHAPilar/defGeomRCColumnSection/g' $fl
# sed -i 's/defElastNoTracMaterial/defElastNoTensMaterial/g' $fl
# sed -i 's/nmbArchCsv/intForcCombFileName/g' $fl
# sed -i 's/nmbArchSalida/outputFileName/g' $fl
# sed -i 's/trataResultsComb/procesResultVerif/g' $fl
# sed -i 's/csvFile/intForcCombFileName/g' $fl
# sed -i 's/nmbArchComb/intForcCombFileName/g' $fl
# sed -i 's/nmbArchLST/intForcCombFileName/g' $fl
# sed -i 's/efProblem/FEProblem/g' $fl
# sed -i 's/getEFProblem/getFEProblem/g' $fl
# sed -i 's/ConstantesSecc2d/CrossSectionProperties2d/g' $fl
# sed -i 's/ConstantesSecc3d/CrossSectionProperties3d/g' $fl
# sed -i 's/qPuntual/pointLoad/g' $fl
# sed -i 's/cUnifXYZSup/unifVectLoad/g' $fl
# sed -i 's/cUnifSup/unifPressLoad/g' $fl
# sed -i 's/gradTemp/tempGrad/g' $fl
# sed -i 's/eHidrostVol/hydrThrustLoad/g' $fl
# sed -i 's/cInerc/inercLoad/g' $fl
# sed -i 's/earthP/earthPressLoad/g' $fl
sed -i 's/dicSup/dicGeomEnt/g' $fl
done
