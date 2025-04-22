# -*- coding: utf-8 -*-
''' Check eigenvectors graphic output. The test does not verify the output 
contents, only that the method runs and the file is  created.
'''
import os
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1 # Cantilever length in meters
b= 0.05 # Cross section width in meters
h= 0.10 # Cross section depth in meters
A= b*h # Cross section area en m2
I= 1/12.0*b*h**3 # Moment of inertia in m4
theta= math.radians(30)
E= 2.0E11 # Elastic modulus in N/m2
dens= 7800 # Steel density in kg/m3
m= A*dens

NumDiv= 10
# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Define materials
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ElasticBeam2d")
beam2d.h= h
beam2d.rho= m

pt1= modelSpace.newKPoint(0.0,0.0,0.0)
pt2= modelSpace.newKPoint(L*math.cos(theta),L*math.sin(theta),0.0)
l= modelSpace.newLine(pt1,pt2)
l.nDiv= NumDiv

# Generate mesh.
setTotal= preprocessor.getSets.getSet("total")
setTotal.genMesh(xc.meshDir.I)

# Constraints
constraints= preprocessor.getBoundaryCondHandler

constrainedNodeTag= pt1.getNode().tag
spc= constraints.newSPConstraint(constrainedNodeTag,0,0.0) # Node 2,gdl 0
spc= constraints.newSPConstraint(constrainedNodeTag,1,0.0) # Node 2,gdl 1
spc= constraints.newSPConstraint(constrainedNodeTag,2,0.0) # Node 2,gdl 2


# Solution procedure
analysis= predefined_solutions.frequency_analysis(feProblem)
analOk= analysis.analyze(1)

# Compute results
eig1= analysis.getEigenvalue(1)


# Graphic stuff.
from postprocess import output_handler
#########################################################
# Graphic stuff.
oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display the eigenvectors
# oh.displayEigenvectors()

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayReactions(fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
