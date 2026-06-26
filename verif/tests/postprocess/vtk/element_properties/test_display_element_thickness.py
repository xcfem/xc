# -*- coding: utf-8 -*-
''' Show how to display a graphic sowhing the an element property like its thickness.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import xc
from model import predefined_spaces
from materials import typical_materials
from postprocess import output_handler

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)

# Define mesh.
## Nodes.
nodeRows= list()
for i in range(0,11):
    nodeRow= list()
    x= float(i)
    for j in range(0,11):
        y= float(j)
        nodeRow.append(modelSpace.newNode(x, y, 0))
    nodeRows.append(nodeRow)
    
# Materials definition
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E= 2.1e9, nu= 0.3, rho= 0.0, h= 0.2)

# Define materials.
elements= list()
modelSpace.setDefaultMaterial(shellMat)
row0= nodeRows[0]
for row1 in nodeRows[1:]:
    n0= row0[0]
    for j, n1 in enumerate(row0[1:]):
        n2= row1[j+1]
        n3= row1[j]
        element= modelSpace.newElement('ShellMITC4', [n0.tag, n1.tag, n2.tag, n3.tag])
        # Set new thickness.
        centroid= element.getPosCentroid(True)
        newThickness= max(0.01, 0.01*centroid.x*centroid.y)
        for mat in element.physicalProperties.getVectorMaterials:
            mat.h= newThickness
        elements.append(element)
        n0= n1
    row0= row1

# Graphic stuff.
oh= output_handler.OutputHandler(modelSpace)

## Define property at nodes.
setToDisplay= modelSpace.getTotalSet()
# oh.displayElementThickness(setToDisplay= setToDisplay)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayElementThickness(setToDisplay= setToDisplay, fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

import os
from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
