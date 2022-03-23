# -*- coding: utf-8 -*-
''' SOLVIA Verification Manual. Example A50.
    Arpack solver and ShellMITC4 element version.'''

from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc

from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
# from postprocess import output_handler
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1 # Cantilever length in meters
b= 0.05 # Cross section width in meters
h= 0.1 # Cross section depth in meters
nuMat= 0.3 # Poisson's ratio.
EMat= 2.0E11 # Young modulus en N/m2.
espChapa= h # Thickness en m.
area= b*espChapa # Cross section area en m2
inertia1= 1/12.0*espChapa*b**3 # Moment of inertia in m4
inertia2= 1/12.0*b*espChapa**3 # Moment of inertia in m4
dens= 7800 # Density of the steel en kg/m3
m= b*h*dens

numDiv= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elementRho= 0.0 #dens 
elast= typical_materials.defElasticMembranePlateSection(preprocessor, name= "elast", E= EMat, nu= nuMat, rho= elementRho, h= espChapa)

# Define geometry
pt1= modelSpace.newKPoint(0.0,0.0,0.0)
pt2= modelSpace.newKPoint(b,0.0,0.0)
pt3= modelSpace.newKPoint(b,L,0.0)
pt4= modelSpace.newKPoint(0,L,0.0)
s= modelSpace.newQuadSurface(pt1,pt2,pt3,pt4)
s.nDivI= int(numDiv/20)
s.nDivJ= numDiv

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

s.genMesh(xc.meshDir.I)

# Mass distribution
templateMatrix=  xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])
totalMass= 0.0
s.resetTributaries()
s.computeTributaryAreas(False)
for n in s.nodes:
    tributaryArea= n.getTributaryArea()
    tributaryMass= tributaryArea*dens*h
    totalMass+= tributaryMass
    n.mass= tributaryMass*templateMatrix
ratio0= abs(totalMass-m)/m

# Constraints
ln= preprocessor.getMultiBlockTopology.getLineWithEndPoints(pt1.tag,pt2.tag)
lNodes= ln.nodes
for n in lNodes:
    n.fix(xc.ID([0,1,2,3,4,5]),xc.Vector([0,0,0,0,0,0])) # UX,UY,UZ,RX,RY,RZ

# Solution procedure
numModes= 2
analysis= predefined_solutions.frequency_analysis(feProblem, systemPrefix= 'band_arpackpp', shift= 0.0)

analOk= analysis.analyze(numModes)
eig1= analysis.getEigenvalue(1)
eig2= analysis.getEigenvalue(2)

omega1= math.sqrt(eig1)
T1= 2*math.pi/omega1
f1calc= 1.0/T1
omega2= math.sqrt(eig2)
T2= 2*math.pi/omega2
f2calc= 1.0/T2
periods= [T1, T2]

Lambda= 1.87510407
f1teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inertia1/m)
ratio1= abs(f1calc-f1teor)/f1teor
f2teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inertia2/m)
ratio2= abs(f2calc-f2teor)/f2teor

'''
print(totalMass,m)
print("ratio0= ",ratio0)
print("\n omega1= ",omega1)
print("T1= ",T1)
print("f1calc= ",f1calc)
print("f1teor= ",f1teor)
print("ratio1= ",ratio1)
print("\n omega2= ",omega2)
print("T2= ",T2)
print("f2calc= ",f2calc)
print("f2teor= ",f2teor)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-10 and abs(ratio2)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# for mode in range(1,numModes+1):
#     T= periods[mode-1]
#     f= 1.0/T
#     print('T_'+str(mode)+'= ',T, 's')
#     print('f_'+str(mode)+'= ',f, 'Hz')
#     oh.displayEigenvectors(mode)
