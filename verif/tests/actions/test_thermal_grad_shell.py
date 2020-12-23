# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

''' home made test
    This test reproduces shell_grad_strain_load_02.py test by using a
    straight load case generation
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
__copyright__= "Copyright 2019, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"

L= 1.0#  Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of steel 1/ºC
b= 1 # width of the element
#A= h*h # bar area expressed in square meters
#I= (h)**4/12 # Cross section moment of inertia (m4)
Ttop=10 # Temperature at top side (Celsius degrees)
Tbottom=0 # Temperature at bottom side (Celsius degrees)
thickness=2e-2

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0.0,b,0.0)
nod2= nodes.newNodeXYZ(L,b,0.0)
nod3= nodes.newNodeXYZ(L,0.0,0.0)
nod4= nodes.newNodeXYZ(0,0.0,0.0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor=preprocessor, name="memb1",E=E,nu=0.3,rho=0.0,h=thickness)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem1= elements.newElement("ShellMITC4",xc.ID([nod4.tag,nod3.tag,nod2.tag,nod1.tag]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler

spc= constraints.newSPConstraint(nod1.tag,0,0.0)
spc= constraints.newSPConstraint(nod1.tag,1,0.0)
spc= constraints.newSPConstraint(nod1.tag,2,0.0)
spc= constraints.newSPConstraint(nod1.tag,4,0.0)
spc= constraints.newSPConstraint(nod4.tag,0,0.0)
spc= constraints.newSPConstraint(nod4.tag,1,0.0)
spc= constraints.newSPConstraint(nod4.tag,2,0.0)
spc= constraints.newSPConstraint(nod4.tag,4,0.0)
elSet=preprocessor.getSets.getSet('total')

from actions.roadway_trafic import IAP_load_models as slm
from actions.imposed_strain import imp_strain as imps

lp0= slm.gradient_thermal_LC(lcName='lp0', lstGradThStrnData=[imps.gradThermalStrain(elemSet=elSet, elThick=thickness, DOF=3, alpha=alpha, Ttop=Ttop, Tbottom=Tbottom)])
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
lPatterns.addToDomain('lp0')
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
elem1.getResistingForce()

# Displacements free nodes 
uz_n2= nod2.getDispXYZ[2]
uz_n3= nod3.getDispXYZ[2]

# theoretical displacement
curvature= alpha*(Tbottom-Ttop)/thickness  #rad/m
R=1.0/curvature+thickness/2.
deltaz_theor=R*(1-math.cos(curvature))

ratio1= uz_n2-deltaz_theor
ratio2= uz_n3-deltaz_theor

'''
print('Ttop= ', Ttop, ' Tbottom= ', Tbottom)
print('curvature= ', curvature)
print('R= ', R)
print("uz_n2= ",uz_n2)
print("uz_n3= ",uz_n3)
print("deltaz_theor=",deltaz_theor)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<2e-7) & (abs(ratio2)<2e-7):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
  
# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# #oh.displayFEMesh()
# oh.displayLocalAxes()
