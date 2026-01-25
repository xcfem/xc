# -*- coding: utf-8 -*-
''' Check display that the displayBlocks method works. The test does not
 verify the output contents, only that the method runs and the file is
 created.'''

from __future__ import print_function

import geom
import xc
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Create FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Definimos geometria

pt1= modelSpace.newKPoint(0.0,0.0,5.0)
pt2= modelSpace.newKPoint(0.0,0.0,10.0)
pt3= modelSpace.newKPoint(5.0,0.0,10.0)
pt4= modelSpace.newKPoint(0.0,5.0,10.0)

l1= modelSpace.newLine(pt1,pt2)
l2= modelSpace.newLine(pt1,pt3)
l3= modelSpace.newLine(pt1,pt4)
l4= modelSpace.newLine(pt2,pt4)
l5= modelSpace.newLine(pt2,pt3)
l6= modelSpace.newLine(pt4,pt3)

import os
fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.tiff')

from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
oh.displayBlocks(displayLocalAxes= False, fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

import os
from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
