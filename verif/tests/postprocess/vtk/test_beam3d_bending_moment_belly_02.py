# -*- coding: utf-8 -*-
'''
Check that the My bending moment diagram is oriented towards the the "belly" 
due that belly.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc_base
import geom
import xc

from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from actions import loads
from actions import load_cases as lcases
from postprocess import control_vars as cv

E= 2.0e11 # Young modulus.
A= 31e-4 # Beam cross-section area.
h= math.sqrt(A) # Beam cross-section depth.
I= 2810e-8 # Inertia of the beam section.
Iy= I
Iz= I
sry= 2.42 # Shear coefficient.
Ay= A/sry
Az= A-Ay
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))
p= 1e5 # Transverse load.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection3d(preprocessor, name= "scc",A= A,E= E,G= G,Iy= Iy, Iz= Iz,J= Iy/100.0, alpha_y= Ay/A, alpha_z= Az/A)

# Problem geometry
L= 1
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(L,0,0)

n3= nodes.newNodeXYZ(0,L,0)
n4= nodes.newNodeXYZ(L,L,0)

n5= nodes.newNodeXYZ(0,2*L,0)
n6= nodes.newNodeXYZ(L,2*L,0)

n7= nodes.newNodeXYZ(0,3*L,0)
n8= nodes.newNodeXYZ(L,3*L,0)

n9= nodes.newNodeXYZ(0,4*L,0)
n10= nodes.newNodeXYZ(L,4*L,0)

n11= nodes.newNodeXYZ(0,5*L,0)
n12= nodes.newNodeXYZ(L,5*L,0)

offset= 5
n101= nodes.newNodeXYZ(offset+0,0,0)
n102= nodes.newNodeXYZ(offset+L,0,0)

n103= nodes.newNodeXYZ(offset+0,L,0)
n104= nodes.newNodeXYZ(offset+L,L,0)

n105= nodes.newNodeXYZ(offset+0,2*L,0)
n106= nodes.newNodeXYZ(offset+L,2*L,0)

n107= nodes.newNodeXYZ(offset+0,3*L,0)
n108= nodes.newNodeXYZ(offset+L,3*L,0)

n109= nodes.newNodeXYZ(offset+0,4*L,0)
n110= nodes.newNodeXYZ(offset+L,4*L,0)

n111= nodes.newNodeXYZ(offset+0,5*L,0)
n112= nodes.newNodeXYZ(offset+L,5*L,0)


## Geometric transformations
linA= modelSpace.newLinearCrdTransf("linA",xc.Vector([0,0,1]))
linB= modelSpace.newLinearCrdTransf("linB",xc.Vector([0,0,-1]))

elementData= [[linA, n1, n2], [linB, n3, n4],
              [linA, n5, n6], [linB, n7, n8],
              [linA, n9, n10], [linB, n11, n12],
              [linA, n101, n102], [linB, n103, n104],
              [linA, n105, n106], [linB, n107, n108],
              [linA, n109, n110], [linB, n111, n112]]

## Define element
elements= preprocessor.getElementHandler
elements.defaultMaterial= scc.name
elemList= list()
for eData in elementData:
    elements.defaultTransformation= eData[0].name
    nA= eData[1].tag; nB= eData[2].tag
    elemList.append(elements.newElement("ElasticBeam3d",xc.ID([nA,nB])))

# Boundary conditions
## Define constraints
for eData in elementData:
    nA= eData[1].tag; nB= eData[2].tag
    modelSpace.fixNode000_0FF(nA)
    modelSpace.fixNode000_FFF(nB)

## Define loads.
bendingMFrontPos= loads.NodalLoad(name='bendingMFrontNeg',lstNod=[n2,n4,n10,n12],loadVector=xc.Vector([0,0,0,0,-1e3,0]))
bendingMBackPos= loads.NodalLoad(name='bendingMBackNeg',lstNod=[n5,n7,n9,n11],loadVector=xc.Vector([0,0,0,0,1e3,0]))

bendingMFrontNeg= loads.NodalLoad(name='bendingMFrontNeg',lstNod=[n102,n104,n110,n112],loadVector=xc.Vector([0,0,0,0,1e3,0]))
bendingMBackNeg= loads.NodalLoad(name='bendingMBackNeg',lstNod=[n105,n107,n109,n111],loadVector=xc.Vector([0,0,0,0,-1e3,0]))

LC= lcases.LoadCase(preprocessor=preprocessor,name="LC",loadPType="default",timeSType="constant_ts")
LC.create()
LC.addLstLoads([bendingMFrontPos, bendingMBackPos, bendingMFrontNeg, bendingMBackNeg])

modelSpace.addLoadCaseToDomain("LC")
modelSpace.analyze(calculateNodalReactions=True)

xcTotalSet= modelSpace.getTotalSet()
MyValues= list()
for e in xcTotalSet.elements:
    MyValues.append(cv.getElementInternalForceComponentData(e, 'My'))


refMyValues= [[geom.Vector3d(0, 0, 1),   0.0, -1000],
              [geom.Vector3d(0, 0,-1),   0.0,  1000],
              [geom.Vector3d(0, 0, 1), -1000,   0.0],
              [geom.Vector3d(0, 0,-1),  1000,   0.0],
              [geom.Vector3d(0, 0, 1), -1000, -1000],
              [geom.Vector3d(0, 0,-1),  1000,  1000],
              [geom.Vector3d(0, 0, 1),   0.0,  1000],
              [geom.Vector3d(0, 0,-1),   0.0, -1000],
              [geom.Vector3d(0, 0, 1),  1000,   0.0],
              [geom.Vector3d(0, 0,-1), -1000,   0.0],
              [geom.Vector3d(0, 0, 1),  1000,  1000],
              [geom.Vector3d(0, 0,-1), -1000, -1000]]

error= 0.0
for values, refValues in zip(MyValues,refMyValues):
    error+= (values[0]-refValues[0]).getModulus() # Direction vector.
    error+= abs(values[1]-refValues[1])
    error+= abs(values[2]-refValues[2])

'''
print(MyValues)
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if error<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

'''    
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display the mesh
# oh.displayFEMesh()
oh.displayLocalAxes()
#oh.displayStrongWeakAxis()
oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uZ')
oh.displayIntForcDiag('My')
'''
