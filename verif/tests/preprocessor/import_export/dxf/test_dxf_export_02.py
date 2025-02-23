# Home made test
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import os
import logging
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from import_export import mesh_entities

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

feProblem.logFileName= "/tmp/erase.log" # Nicely avoid warning messages.

#               
#  4 +--------------------+ 3
#    |                    |
#    |                    |
#    |                    |
#  1 +--------------------+ 2
#

# Define mesh. 
## Define nodes.
CooMax= 10.0
n1= modelSpace.newNodeXY(0.0,0.0)
n2= modelSpace.newNodeXY(CooMax,0.0)
n3= modelSpace.newNodeXY(CooMax,CooMax)
n4= modelSpace.newNodeXY(0.0,CooMax)

## Define material.
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

## Define elements.
modelSpace.setDefaultMaterial(elast2d)
quad4n = modelSpace.newElement("FourNodeQuad", [n1.tag, n2.tag, n3.tag, n4.tag])

xcTotalSet= modelSpace.getTotalSet()
me= mesh_entities.MeshData()
numEntitiesRead= me.readFromXCSet(xcTotalSet)
ok1= (abs(numEntitiesRead-5)==0) # four nodes and one element.
# Avoid info messages
import ezdxf
logging.getLogger(ezdxf.__name__).setLevel(logging.ERROR)
fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.dxf')
me.writeDxfFile(outputFileName)
fileSize= os.path.getsize(outputFileName)
ok2= (fileSize>3000) and (fileSize<20000) # file exists and has a size that
                                          # seems reasonable.

'''
print(numEntitiesRead)
print(fileSize)
'''

from misc_utils import log_messages as lmsg
if(ok1 & ok2):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

