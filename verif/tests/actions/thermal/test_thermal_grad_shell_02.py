# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

''' home made test
    This test computes the effect on an "in plane" temperature gradient.
'''

import xc_base
import geom
import xc
import math
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@ciccp.es"

L= 1.0 # Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of steel 1/ºC
b= 0.1 # width of the element
Ttop= 100.0 # Temperature at top side (Celsius degrees)
Tbottom= 0.0 # Temperature at bottom side (Celsius degrees)
thickness= 0.2

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor=preprocessor, name="memb1",E=E,nu=0.3,rho=0.0,h=b)

# Mesh generation
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= memb1.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(L,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(L,thickness,0.0))
pt4= points.newPoint(geom.Pos3d(0.0,thickness,0.0))
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 20
s.nDivJ= 2

s.genMesh(xc.meshDir.I)

xcTotalSet= modelSpace.getTotalSet()

# Constraints
constraints= preprocessor.getBoundaryCondHandler

n1= pt1.getNode()
modelSpace.fixNode('000_F0F', n1.tag)
n4= pt4.getNode()
modelSpace.fixNode('0F0_F0F', n4.tag)

# Load definition.
def tempGradient(y):
    ''' Return the temperature corresponding to "y" coordinate'''
    return (Ttop-Tbottom)/thickness*y
  
lp0= modelSpace.newLoadPattern(name= '0')
for e in xcTotalSet.elements:
    eleLoad= lp0.newElementalLoad("shell_strain_load")
    eleLoad.elementTags= xc.ID([e.tag])
    # Gauss points positions
    gaussPoints= e.getGaussModel().getGaussPoints()
    for i, p in enumerate(gaussPoints):
        gpPos= e.getCartesianCoordinates(p, True)
        AT= tempGradient(gpPos.y)
        eleLoad.setStrainComp(i,0,alpha*AT)
        eleLoad.setStrainComp(i,0,alpha*AT)
        #print(i, gpPos.y, AT)
    
modelSpace.addLoadCaseToDomain(lp0.name)

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

# Displacements of free nodes
n2= pt2.getNode()
uy_n2= n2.getDispXYZ[1]
n3= pt3.getNode()
uy_n3= n3.getDispXYZ[1]
uy= (uy_n2+uy_n3)/2.0 # Average displacement

# theoretical displacement
curvature= alpha*(Tbottom-Ttop)/thickness  #rad/m
R=1.0/curvature+thickness/2.
deltay_theor=R*(1-math.cos(curvature))
ratio1= abs(uy-deltay_theor)/deltay_theor

'''
print('Ttop= ', Ttop, ' Tbottom= ', Tbottom)
print('curvature= ', curvature)
print('R= ', R)
print("uy_n2= ",uy_n2)
print("uy_n3= ",uy_n3)
print("uy= ",uy)
print("deltay_theor=",deltay_theor)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<.05):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayFEMesh()
# oh.displayLocalAxes()
