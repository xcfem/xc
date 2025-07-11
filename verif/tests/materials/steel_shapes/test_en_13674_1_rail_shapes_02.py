# -*- coding: utf-8 -*-
''' Home made test. EN13674−1 UIC54 rail cantilever under vertical load 
    at his end.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.structural_shapes import bs_en_13674_1_rail_sections
from materials.en_13674_1 import EN_13674_1_materials

# Geometry
L= 5 # Bar length (m)

# Load
F= 50e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L,0.0,0.0)

steel= EN_13674_1_materials.R260
gammaM0= 1.05
steel.gammaM= gammaM0 
rail= bs_en_13674_1_rail_sections.RailShape(steel= steel, name= "RAIL_UIC54") # Section geometry.
profil= rail.defElasticShearSection3d(preprocessor)

# Elements definition
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(rail)

elem= modelSpace.newElement("ElasticBeam3d", [n1.tag, n2.tag])
elem.rho= rail.get('P')

# Constraints
modelSpace.fixNode000_000(n1.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0') 
lp0.newNodalLoad(n2.tag, xc.Vector([0,0,-F,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

delta= n2.getDisp[2]  # Node 2 zAxis displacement

elem.getResistingForce()
M= elem.getMz1
V= elem.getVy()

EIz= profil.getEI1()
deltateor= -F*L**3/3/EIz
ratio1= (abs(delta-deltateor)/deltateor)
ratio2= ((M-F*L)/(F*L))
ratio3= ((F-V)/F)

'''
print("delta= ",delta)
print("deltaTeor= ",deltateor)
print("ratio1= ",ratio1)
print("M= ",M)
print("ratio2= ",ratio2)
print("V1= ",V)
print("ratio3= ",ratio3)
'''

testOK= (abs(ratio1)<5e-2) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5)
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
