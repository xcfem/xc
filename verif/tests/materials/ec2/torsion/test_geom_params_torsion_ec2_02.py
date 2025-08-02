# -*- coding: utf-8 -*-
''' Check torsion parameters of reinforced concrete section according to clause
    6.3 of Eurocode 2.

Reference values obtained from "Eurocode 2 Worked Examples (rev A 31-03-2017) Example 6.7 Shear – Torsion interaction diagrams [EC2 clause 6.3].
'''
from __future__ import print_function

import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_limit_state_checking

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Ec= 20e9
sideLength= 1.0


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor


concrete= typical_materials.defElasticMaterial(preprocessor, 'concrete',Ec)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
concrete= regions.newQuadRegion('concrete')
sectionWidth= 0.3
sectionHeight= 0.5
concrete.pMin= geom.Pos2d(0.0,0.0)
concrete.pMax= geom.Pos2d(sectionWidth, sectionHeight)

t0= None # if not None, actual thickness of the wall for hollow sections.
cover= .035 # cover of longitudinal reinforcement.
pt= EC2_limit_state_checking.compute_effective_hollow_section_parameters(sectionGeometry= sectionGeometryTest, c= cover, t0= t0)

A= pt.A()
u= pt.u()
tef= pt.tef()
tef_ref= A/u
Ak= pt.Ak()
AkRef= (sectionWidth-tef_ref)*(sectionHeight-tef_ref)
uk= pt.uk()
uk_ref= 2*((sectionWidth-tef_ref)+(sectionHeight-tef_ref))
ratio1= abs(A-.15)
ratio2= abs(u-1.6)/1.6
ratio3= abs(tef-tef_ref)/tef_ref
ratio4= abs(Ak-AkRef)/AkRef
ratio5= abs(uk-uk_ref)/uk_ref

print("A= ",A)
print("u= ",u)
print("tef= ",tef)
print("Ak= ",Ak)
print("uk= ",uk)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
print("ratio5= ",ratio5)
''' 
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15) & (ratio4<1e-15) & (ratio5<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
