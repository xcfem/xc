# -*- coding: utf-8 -*-
# Tomado del example B46 del SOLVIA Verification Manual
import xc_base
import geom
import xc

from model import predefined_spaces
from materials import typical_materials
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

NumDiv= 10

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Define materials
elast= typical_materials.defElasticMembranePlateSection(preprocessor, "elast",EMat,nuMat,espChapa*dens,espChapa)


points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntIDPos3d(1, geom.Pos3d(0.0,0.0,0.0) )
pt2= points.newPntIDPos3d(2, geom.Pos3d(b,0.0,0.0) )
pt3= points.newPntIDPos3d(3, geom.Pos3d(b,L,0.0) )
pt4= points.newPntIDPos3d(4, geom.Pos3d(0,L,0.0) )
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s= surfaces.newQuadSurfacePts(1,2,3,4)
s.nDivI= 1
s.nDivJ= NumDiv




seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= "elast"
seedElemHandler.defaultTag= 1
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))


f1= preprocessor.getSets.getSet("f1")
f1.genMesh(xc.meshDir.I)
# Constraints


ln= preprocessor.getMultiBlockTopology.getLineWithEndPoints(pt1.tag,pt2.tag)
lNodes= ln.nodes
for n in lNodes:
  n.fix(xc.ID([0,1,2,3,4,5]),xc.Vector([0,0,0,0,0,0])) # UX,UY,UZ,RX,RY,RZ

# Solution procedure
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl


solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")


cHandler= sm.newConstraintHandler("transformation_constraint_handler")

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")

analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("frequency_soln_algo")
integ= analysisAggregation.newIntegrator("eigen_integrator",xc.Vector([]))

#soe= analysisAggregation.newSystemOfEqn("sym_band_eigen_soe")
#solver= soe.newSolver("sym_band_eigen_solver")
soe= analysisAggregation.newSystemOfEqn("band_arpackpp_soe")
soe.shift= 0.0
solver= soe.newSolver("band_arpackpp_solver")
solver.tol= 1e-3
solver.maxNumIter= 5

analysis= solu.newAnalysis("eigen_analysis","analysisAggregation","")


analOk= analysis.analyze(2)
eig1= analysis.getEigenvalue(1)
eig2= analysis.getEigenvalue(2)




omega1= math.sqrt(eig1)
T1= 2*math.pi/omega1
f1calc= 1.0/T1
omega2= math.sqrt(eig2)
T2= 2*math.pi/omega2
f2calc= 1.0/T2



Lambda= 1.87510407
f1teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inertia1/m)
ratio1= abs(f1calc-f1teor)/f1teor
f2teor= Lambda**2/(2*math.pi*L**2)*math.sqrt(EMat*inertia2/m)
ratio2= abs(f2calc-f2teor)/f2teor

'''
print "omega1= ",omega1
print "T1= ",T1
print "f1calc= ",f1calc
print "f1teor= ",f1teor
print "ratio1= ",ratio1
print "omega2= ",omega2
print "T2= ",T2
print "f2calc= ",f2calc
print "f2teor= ",f2teor
print "ratio2= ",ratio2
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio2)<1e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
