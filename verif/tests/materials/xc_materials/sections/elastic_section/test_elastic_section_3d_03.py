# -*- coding: utf-8 -*-
'''Check the getCrossSectionProperties3d method.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from misc import scc3d_testing_bench

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Section geometry
scc10x20= section_properties.RectangularSection(name="rectang",b=.10,h=.20) # Section geometry.
# Section material
E= 2.1e6
nu= 0.3
rho= 2500
matscc10x20= typical_materials.MaterialData(name='mtrectang',E= E, nu= nu, rho= rho) # Section material.


# Materials definition
elasticSection3d= scc10x20.defElasticSection3d(preprocessor, matscc10x20)

iw= 0.0 # Warping constant.
G= E/(2*(1+nu))
crossSectionProp3d= elasticSection3d.getCrossSectionProperties3d(E, iw, G)

ratio0= (crossSectionProp3d.rho-elasticSection3d.rho)/elasticSection3d.rho
ratio1= (crossSectionProp3d.EA()-elasticSection3d.EA())/elasticSection3d.EA()
#ratio2= (crossSectionProp3d.GAy()-elasticSection3d.GAy())/elasticSection3d.GAy()
# ratio3= (crossSectionProp3d.GAz()-elasticSection3d.GAz())/elasticSection3d.GAz()
ratio4= (crossSectionProp3d.EIy()-elasticSection3d.EIy())/elasticSection3d.EIy()
ratio5= (crossSectionProp3d.EIz()-elasticSection3d.EIz())/elasticSection3d.EIz()
ratio6= (crossSectionProp3d.GJ()-elasticSection3d.GJ())/elasticSection3d.GJ()

'''
# print(crossSectionProp3d)
print(ratio0)
print(ratio1)
# print(ratio2)
# print(ratio3)
print(ratio4)
print(ratio5)
print(ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-15) & (abs(ratio1)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15)& (abs(ratio6)<1e-15) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
