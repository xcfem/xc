# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import filecmp
from actions.load_combination_utils import iap11 # Spanish IAP-11
from misc_utils import log_messages as lmsg

lcg= iap11.combGenerator

G1= lcg.newPermanentAction(actionName= "G1", actionDescription= "Peso propio")
G2= lcg.newPermanentAction(actionName= "G2", actionDescription= "Carga muerta")
G3= lcg.newPermanentAction(actionName= "G3", actionDescription=  "Reológicas")
P= lcg.newPrestressingAction(actionName= "P", actionDescription=  "Pretensado")
Q1= lcg.newFootbridgeAction(actionName= "Q1", actionDescription= "SC uso.")
Q2= lcg.newFootbridgeAction(actionName= "Q2", actionDescription= "Veh. servicio.", incompatibleActions=['Q1'])
Q3= lcg.newFootbridgeAction(actionName= "Q3", actionDescription= "Carga barandilla.", incompatibleActions=['Q.*'])
Qcon= lcg.newThermalAction(actionName= "Qcon", actionDescription= "Contracción térmica.", incompatibleActions=['Qexp', 'Qw'])
Qexp= lcg.newThermalAction(actionName= "Qexp", actionDescription= "Contracción térmica.", incompatibleActions=['Qexp', 'Qw'])
Qw= lcg.newFootbridgeWindAction(actionName= "Qw", actionDescription= "Contracción térmica.")

lcg.computeCombinations()
combContainer= lcg.getCombContainer()

outputFileName= 'test_iap11_03.json'
outputPath= '/tmp/'+outputFileName
combContainer.writeJSON(outputFileName= outputPath)

# Check results.
pth= os.path.dirname(__file__) # Get current path.
if(not pth):
    pth= '.'
refFile= pth+'/../../../aux/reference_files/ref_'+outputFileName
comparisonOK= filecmp.cmp(refFile, outputPath, shallow=False)

# ## Generate PDF.
# combContainer.exportToPDF(fileName= outputFileName.replace('.json', '.pdf'))

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if comparisonOK:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.remove(outputPath) # Your garbage, you clean it.
