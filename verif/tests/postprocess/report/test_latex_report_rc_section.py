# -*- coding: utf-8 -*-
''' Check reinforced concrete section report method. The test does not
 verify the output contents, only that the code runs and the file is
 created. The accuracy of the results is tested elsewhere.
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc

from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces

datosScc1LosC= def_simple_RC_section.RCRectangularSection(name= "secHA1LosC", sectionDescr= "Losa. Central portion. Section normal to X axis.", concrType= EHE_materials.HA25, depth= 0.25, width= 1.0, reinfSteelType= EHE_materials.B500S)
rebNeg=def_simple_RC_section.ReinfRow(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025+0.010,width=0.999)
datosScc1LosC.negatvRebarRows= def_simple_RC_section.LongReinfLayers([rebNeg])
rebPos=def_simple_RC_section.ReinfRow(nRebars= 5, areaRebar= EHE_materials.Fi10,nominalCover=0.025,width=0.999)
datosScc1LosC.positvRebarRows= def_simple_RC_section.LongReinfLayers([rebPos])

# datosScc1LosC.nRebarsNeg= 5
# datosScc1LosC.areaRebarNeg= EHE_materials.Fi10
# datosScc1LosC.coverNeg= 0.025+0.010+0.010/2.0
# datosScc1LosC.nRebarsPos= 5
# datosScc1LosC.areaRebarPos= EHE_materials.Fi10
# datosScc1LosC.coverPos= 0.025+0.010/2.0



NDato= 0 # Axial force for crack control checking.
MyDato= 1000 # Bending moment force for crack control checking.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concreteMatTag= EHE_materials.HA25.defDiagK(preprocessor)
reinfSteelMaterialTag= EHE_materials.B500S.defDiagK(preprocessor)


datosScc1LosC.defRCSection(preprocessor, "k")

outputPath= '/tmp'
fname= os.path.basename(__file__)
texFileName= outputPath+'/'+fname.replace('.py', '.tex')
with open(texFileName, 'w') as latexOutput:
  datosScc1LosC.latexReport(preprocessor= preprocessor, os= latexOutput, includeGraphicsPath= outputPath, outputPath= outputPath)

# Check that file exists
testOK= os.path.isfile(texFileName)
if os.path.exists(texFileName):
    os.remove(texFileName)
else:
    lmsg.error('File: '+str(texFileName)+' has not been created')
    
epsFileName= outputPath+'/secHA1LosC.eps'
testOK= testOK and os.path.isfile(epsFileName)
if os.path.exists(epsFileName):
    os.remove(epsFileName)
else:
    lmsg.error('File: '+str(epsFileName)+' has not been created')
    
pngFileName= outputPath+'/secHA1LosC.png'
testOK= testOK and os.path.isfile(pngFileName)
if os.path.exists(pngFileName):
    os.remove(pngFileName)
else:
    lmsg.error('File: '+str(pngFileName)+' has not been created')
    
from misc_utils import log_messages as lmsg
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
