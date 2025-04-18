# -*- coding: utf-8 -*-
from __future__ import print_function
''' Reinforced concrete section. Crack control verification test.
   results are compared with those of the «Prontuario informático del hormigón armado». '''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure


from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces

width= 0.25 # Section width expressed in meters.
depth= 0.25 # Section depth expressed in meters.
cover= 0.025 # Concrete cover expressed in meters.
areaFi12=1.13e-4

NDato= -100.0 # Axial force for crack control checking.
MyDato= 0.0 # Bending moment force for crack control checking.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concrMatTag25= EHE_materials.HA25.defDiagK(preprocessor)
tagB500S= EHE_materials.B500S.defDiagK(preprocessor)

geomSecHA= preprocessor.getMaterialHandler.newSectionGeometry("geomSecHA")
regions= geomSecHA.getRegions
concrete= regions.newQuadRegion(EHE_materials.HA25.nmbDiagK)
concrete.nDivIJ= 10
concrete.nDivJK= 10
concrete.pMin= geom.Pos2d(-width/2.0,-depth/2.0)
concrete.pMax= geom.Pos2d(width/2.0,depth/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagK)
reinforcementInf.numReinfBars= 2
reinforcementInf.barArea= areaFi12
reinforcementInf.p1= geom.Pos2d(cover-width/2.0,cover-depth/2.0) # bottom layer.
reinforcementInf.p2= geom.Pos2d(width/2.0-cover,cover-depth/2.0)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagK)
reinforcementSup.numReinfBars= 2
reinforcementSup.barArea= areaFi12
reinforcementSup.p1= geom.Pos2d(cover-width/2.0,depth/2.0-cover) # top layer.
reinforcementSup.p2= geom.Pos2d(width/2.0-cover,depth/2.0-cover)

materialHandler= preprocessor.getMaterialHandler
secHA= materialHandler.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(geomSecHA.name)
secHA.setupFibers()

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0F0(nodB.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,MyDato,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)



# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)

secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')


scc= zlElement.getSection()
secHAParamsFis.computeWk(scc,EHE_materials.HA25.matTagK,EHE_materials.B500S.matTagK,EHE_materials.HA25.fctm())

ratio1= secHAParamsFis.Wk


''' 
    print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
