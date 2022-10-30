# -*- coding: utf-8 -*-

import os
import filecmp
import loadCombinations
from actions.load_combination_utils import sia260
from actions.load_combination_utils import utils as ec


lcg= sia260.controlCombGenerator
PP1= lcg.insert("SIA260","permanentes",loadCombinations.Action("G","Poids propre"),"permanent","permanentes")

LM6= lcg.insert("SIA260","variables",loadCombinations.Action("LM6","Modèle de charge 6"),"voie_etroite_load_model_6","trafic_ferroviaire")
LM6.getRelaciones.appendIncompatible("LM7")

LM7= lcg.insert("SIA260","variables",loadCombinations.Action("LM7","Modèle de charge 7"),"voie_etroite_load_model_7","trafic_ferroviaire")
LM7.getRelaciones.appendIncompatible("LM6")

LM8= lcg.insert("SIA260","variables",loadCombinations.Action("LM8","Modèle de charge 8"),"voie_etroite_load_model_8","trafic_ferroviaire")
LM8.getRelaciones.appendIncompatible("LM6")
LM8.getRelaciones.appendIncompatible("LM7")

LM7DF= lcg.insert("SIA260","variables",loadCombinations.Action("LM7DF","Démarrage"),"voie_etroite_load_model_7","trafic_ferroviaire")
LM7DF.getRelaciones.appendMain("LM7")
LM7DF.getRelaciones.appendIncompatible("LM7FC")

LM7FC= lcg.insert("SIA260","variables",loadCombinations.Action("LM7FC","Force centrifugue"),"voie_etroite_load_model_7","trafic_ferroviaire")
LM7FC.getRelaciones.appendMain("LM7")

Vent= lcg.insert("SIA260","variables",loadCombinations.Action("V","Vent"),"vent","variables")
Vent.getRelaciones.appendMain("LM8")

TPos= lcg.insert("SIA260","variables",loadCombinations.Action("TPos","Température +"),"temperature","variables")
TPos.getRelaciones.appendIncompatible("TNeg")
TPos.getRelaciones.appendIncompatible("LM8")
TNeg= lcg.insert("SIA260","variables",loadCombinations.Action("TNeg","Température -"),"temperature","variables")
TNeg.getRelaciones.appendIncompatible("TPos")
TNeg.getRelaciones.appendIncompatible("LM8")

LM6DR1= lcg.insert("SIA260","accidentales",loadCombinations.Action("LM6DR1","Dérraillement type 1 (LM6)."),"voie_etroite_choc","accidentales")
LM6DR1.getRelaciones.appendIncompatible("TPos")
LM6DR1.getRelaciones.appendIncompatible("TNeg")
LM6DR1.getRelaciones.appendIncompatible("LM.*")

LM7DR1= lcg.insert("SIA260","accidentales",loadCombinations.Action("LM7DR1","Dérraillement type 1 (LM7)."),"voie_etroite_choc","accidentales")
LM7DR1.getRelaciones.appendIncompatible("TPos")
LM7DR1.getRelaciones.appendIncompatible("TNeg")
LM7DR1.getRelaciones.appendIncompatible("LM.*")

LMDR2= lcg.insert("SIA260","accidentales",loadCombinations.Action("LMDR2","Dérraillement type 2."),"voie_etroite_choc","accidentales")
LMDR2.getRelaciones.appendIncompatible("TPos")
LMDR2.getRelaciones.appendIncompatible("TNeg")
LMDR2.getRelaciones.appendIncompatible("LM.*")

Choc= lcg.insert("SIA260","accidentales",loadCombinations.Action("Choc","Choc."),"voie_etroite_choc","accidentales")
Choc.getRelaciones.appendMain("LM7")
Choc.getRelaciones.appendIncompatible("TPos")
Choc.getRelaciones.appendIncompatible("TNeg")

lcg.genera()


# Get current path.
pth= os.path.dirname(__file__)
if(not pth):
    pth= '.'

comparisonOK= True
values= [('ELSF', lcg.getLoadCombinations.getSLSFrequentCombinations, "load_combs_els_freq.py"), ("ELSQP", lcg.getLoadCombinations.getSLSQuasiPermanentCombinations,"load_combs_els_qp.py"), ('ELU', lcg.getLoadCombinations.getULSTransientCombinations, "load_combs_elu.py"), ("ELUA", lcg.getLoadCombinations.getULSAccidentalCombinations, "load_combs_acc.py")]

for (prefix, loadCombinations, outputFileName) in values:
    ec.writeXCLoadCombinations(prefix= prefix, loadCombinations= loadCombinations, outputFileName= outputFileName)
    # Compare with reference file.
    refFile= pth+'/../../aux/reference_files/ref_'+outputFileName
    comparisonOK= comparisonOK and filecmp.cmp(refFile, outputFileName, shallow=False)
    os.remove(outputFileName)
    
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
