# -*- coding: utf-8 -*-

import os
import filecmp
from actions.load_combination_utils import sia260
from actions.load_combination_utils import utils as ec


lcg= sia260.combGenerator # Combination generator.

# Permanent loads.
PP1= lcg.newPermanentAction(actionName= "G", actionDescription= "Poids propre")

# Railway loads.
LM6= lcg.newRailwayTrafficAction(actionName= "LM6", actionDescription= "Modèle de charge 6", combinationFactorsName= "voie_etroite_load_model_6")
LM6.relationships.appendIncompatible("LM7")

LM7= lcg.newRailwayTrafficAction(actionName= "LM7", actionDescription= "Modèle de charge 7", combinationFactorsName= "voie_etroite_load_model_7")
LM7.relationships.appendIncompatible("LM6")

LM8= lcg.newRailwayTrafficAction(actionName= "LM8", actionDescription= "Modèle de charge 8", combinationFactorsName= "voie_etroite_load_model_8", incompatibleActions= ['LM6', 'LM7'])

LM7DF= lcg.newRailwayTrafficAction(actionName= "LM7DF", actionDescription= "Démarrage", combinationFactorsName= "voie_etroite_load_model_7", dependsOn= 'LM7', incompatibleActions= ['LM7FC'])

LM7FC= lcg.newRailwayTrafficAction(actionName= "LM7FC", actionDescription= "Force centrifugue", combinationFactorsName= "voie_etroite_load_model_7", dependsOn= 'LM7')

# Wind load.
Vent= lcg.newWindAction(actionName= "V", actionDescription= "Vent", dependsOn= 'LM8')

# Thermal loads.
TPos= lcg.newThermalAction(actionName= "TPos", actionDescription= "Température +", incompatibleActions= ["TNeg", "LM8"])

TNeg= lcg.newThermalAction(actionName= "TNeg", actionDescription= "Température -", incompatibleActions= ["TPos", "LM8"])

# Accidental loads.
derailmentIncompatibles= ["TPos", "TNeg", "LM.*"] # Use regular expressions.
LM6DR1= lcg.newAccidentalAction(actionName= "LM6DR1", actionDescription= "Dérraillement type 1 (LM6).", combinationFactorsName= "voie_etroite_choc", incompatibleActions= derailmentIncompatibles)


LM7DR1= lcg.newAccidentalAction(actionName= "LM7DR1", actionDescription= "Dérraillement type 1 (LM7).", combinationFactorsName= "voie_etroite_choc", incompatibleActions= derailmentIncompatibles)

LMDR2= lcg.newAccidentalAction(actionName= "LMDR2", actionDescription= "Dérraillement type 2.", combinationFactorsName= "voie_etroite_choc", incompatibleActions= derailmentIncompatibles)

Choc= lcg.newAction(family= "accidentales",actionName= "Choc", actionDescription= "Choc.", combinationFactorsName= "voie_etroite_choc", partialSafetyFactorsName= "accidentales", dependsOn= 'LM7', incompatibleActions=  ["TPos", "TNeg"])

lcg.computeCombinations()

# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'

comparisonOK= True
values= [('ELSF', lcg.getSLSFrequentCombinations(), "load_combs_els_freq.py"), ("ELSQP", lcg.getSLSQuasiPermanentCombinations(),"load_combs_els_qp.py"), ('ELU', lcg.getULSTransientCombinations(), "load_combs_elu.py"), ("ELUA", lcg.getULSAccidentalCombinations(), "load_combs_acc.py")]

for (prefix, loadCombinations, outputFileName) in values:
    ec.writeXCLoadCombinations(prefix= prefix, loadCombinations= loadCombinations, outputFileName= outputFileName)
    # Compare with reference file.
    refFile= pth+'/../../aux/reference_files/ref_'+outputFileName
    comparisonOK= comparisonOK and filecmp.cmp(refFile, outputFileName, shallow=False)
    os.remove(outputFileName) # clean up after yourself.
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
