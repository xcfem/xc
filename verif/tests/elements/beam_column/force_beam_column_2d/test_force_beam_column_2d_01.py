# -*- coding: utf-8 -*-
''' Home made test. Horizontal cantilever under tension load at its end.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Geometry
width= .001
depth= .01
A= width*depth
E= 210e9
I= width*depth**3/12.0
nu= 0.3
G= E/(2.0*(1+nu))
L= 1.5 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude en N

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(L,0.0)


# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

# Materials definition
section= typical_materials.defElasticShearSection2d(preprocessor, "section",A,E,G,I,1.0)


# Elements definition
elemHandler= preprocessor.getElementHandler
elemHandler.defaultTransformation= lin.name # Coordinate transformation for the new elements
elemHandler.defaultMaterial= section.name
beam2d= elemHandler.newElement("ForceBeamColumn2d",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode000(n1.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([F,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(10)

# Check results.
delta= n2.getDisp[0]  # Node 2 xAxis displacement
deltateor= (F*L/(E*A))
ratio1= (abs((delta-deltateor)/deltateor))

beam2d.getResistingForce()
scc= beam2d.getSections()[0]
N0= scc.getStressResultantComponent("N")
ratio2= (abs((N0-F)/F))
# Check getN1 and getN2 (LP 28/04/2024).
N1= beam2d.getN1
N2= beam2d.getN2
ratio3= math.sqrt((F-N1)**2+(F-N2)**2)

''' 
print("F= ",(F))
print("delta: ",delta)
print("deltaTeor: ",deltateor)
print("ratio1= ",ratio1)
print("N0= ",N0)
print("ratio2= ",ratio2)
print('F= ', F/1e3, 'N1= ', N1/1e3, ' N2= ', N2/1e3, ' ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10 and abs(ratio2)<1e-10 and abs(ratio3)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayIntForcDiag('N')
# # oh.displayIntForcDiag('M')
# #oh.displayLocalAxes()
