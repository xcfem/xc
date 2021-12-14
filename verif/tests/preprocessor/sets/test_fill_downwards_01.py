# -*- coding: utf-8 -*-
'''Test sets and grid model. Generation/selection of geometric and FE 
   entities using grid model, boolean operations with the created sets.'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import os
import xc_base
import geom
import xc
from model import predefined_spaces
from model.geometry import grid_model as gm
from materials import typical_materials as tm
from materials.sections import section_properties as sectpr
from model.mesh import finit_el_model as fem
from model.sets import sets_mng as sets


# *** PROBLEM
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #defines dimension of
                  #the space: nodes by three coordinates (x,y,z) and 
                  #three DOF for each node (Ux,Uy,Uz)

# *** GEOMETRY ***

# coordinates in global X,Y,Z axes for the generation of grid 
xList=[i for i in range(11)]
yList=[i for i in range(21)]
zList=[i for i in range(6)]

#Points' generation
gridGeom=gm.GridModel(prep,xList,yList,zList)
gridGeom.generatePoints()

# Parameters

# Ranges to be used lately
floor1_rg=gm.IJKRange((2,5,2),(8,15,2))

# Surfaces' generation
floor1= gridGeom.genSurfOneRegion(ijkRange=floor1_rg,setName='floor1')
# for s in floor1.getSurfaces:
#     print(s.name, s.tag, s, s.getKPoints())

floor1.fillDownwards()
sz= floor1.getLines.size

# print('Lines in set floor1: ', sz)
# for l in floor1.getLines:
#     print(l.name, l.tag, l.getKPoints(), l)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (sz==136):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
