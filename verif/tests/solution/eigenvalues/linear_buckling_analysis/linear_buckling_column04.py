# -*- coding: utf-8 -*-
from __future__ import print_function
''' Linear buckling analysis of a column under axial load.
    Taken from figure 6.22 de "Finite Element Procedures"
    de "Klaus Jurgen Bathe".

    We use 6 elements instead of 2 to capture the P-d (small delta) effect.
    To capture the geometric non-linearity "inside" the element you need to 
    use a different element formulation, e.g., nonlinear strain in the 
    displacement-based formulation or curvature-based displacement 
    interpolation (CBDI) in the force-based formulation.

    See the article 'Meshing for Column Loads <https://portwooddigital.com/2020/05/10/meshing-for-column-loads/amp/>'
 '''
import xc_base
import geom
import xc

import math
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
# from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"
# from postprocess import output_handler

L= 10 # Column length in meters
b= 0.2 # Cross section width in meters
h= 0.2 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
E=1e4/I # Elastic modulus en N/m2
nu= 0.3
G= E/(2*(1+nu))
P= -100 # Carga vertical sobre la columna.

NumDiv= 6

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Materials definition
scc= typical_materials.defElasticShearSection2d(preprocessor, "scc",A,E,G,I,1.0)


# Geometric transformation(s)
lin= modelSpace.newPDeltaCrdTransf("lin")

# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ForceBeamColumn2d",xc.ID([0,0]))
beam2d.h= h
beam2d.rho= 0.0

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(2,geom.Pos3d(0.0,L,0.0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pt1.tag,pt2.tag)
l.nDiv= NumDiv


setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

n1= pt1.getNode() # Back end node.
n2= pt2.getNode() # Front end node.

# Constraints
constraints= preprocessor.getBoundaryCondHandler

#
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1,gdl 0 
spc= constraints.newSPConstraint(n1.tag,1,0.0) # Node 1,gdl 1
spc= constraints.newSPConstraint(n2.tag,0,0.0) # Node 2,gdl 0

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,P,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
linearBucklingAnalysis= predefined_solutions.LinearBucklingAnalysis(prb= feProblem, numModes= 2, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= "norm_disp_incr_conv_test", convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= "band_gen_lin_soe", solverType= "band_gen_lin_lapack_solver", solnAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= "band_arpackpp_soe", eigenSolverType= "band_arpackpp_solver")
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

eig1= linearBucklingAnalysis.analysis.getEigenvalue(1)

deltay= n2.getDisp[1] 
 

deltayTeor= P*L/(E*A)
ratio1= abs(deltay-deltayTeor)/deltayTeor
blCalc= eig1*P
blTeor= -1.0*math.pi**2*E*I/(L**2)
ratio2= abs(blCalc-blTeor)/blTeor

''' 
print("deltay= ",(deltay))
print("deltayTeor= ",(deltayTeor))
print("eig1= ",(eig1))
print("ratio1= ",(ratio1))
print("blCalc= ",(blCalc/1e3)," kN")
print("blTeor= ",(blTeor/1e3)," kN")
print("ratio2= ",(ratio2))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<0.03):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
#Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
# oh.displayLocalAxes()
#oh.displayStrongWeakAxis(setToDisplay= frameSet)
# oh.displayLoads()
#oh.displayEigenvectors(mode= 1)
# oh.displayEigenResult(eigenMode= 1, defFScale= .5)
# oh.displayReactions(reactionCheckTolerance= 1e-2)
# oh.displayDispRot(itemToDisp='uX', defFScale= 15)
# oh.displayDispRot(itemToDisp='uY', defFScale= 15)
