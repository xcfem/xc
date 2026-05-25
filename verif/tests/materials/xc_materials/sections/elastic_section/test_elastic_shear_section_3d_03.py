# -*- coding: utf-8 -*-
'''Check the getCrossSectionProperties3d method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from materials.sections import section_properties
from misc import scc3d_testing_bench

# Rectangular cross-section definition
scc10x20= section_properties.RectangularSection(name="rectang",b=.10,h=.20) # Section geometry.
E= 2.1e6
nu= 0.3
rho= 2500
matscc10x20=typical_materials.MaterialData(name='mtrectang', E=E, nu=nu, rho= rho) # Section material.


feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
# Materials definition
elasticShearSection3d= scc10x20.defElasticShearSection3d(preprocessor,matscc10x20)


iw= 0.0 # Warping constant.
G= matscc10x20.G()
crossSectionProp3d= elasticShearSection3d.getCrossSectionProperties3d(E, iw, G)

ratio0= (crossSectionProp3d.rho-elasticShearSection3d.rho)/elasticShearSection3d.rho
ratio1= (crossSectionProp3d.EA()-elasticShearSection3d.EA())/elasticShearSection3d.EA()
ratio2= (crossSectionProp3d.GAy()-elasticShearSection3d.GAy())/elasticShearSection3d.GAy()
ratio3= (crossSectionProp3d.GAz()-elasticShearSection3d.GAz())/elasticShearSection3d.GAz()
ratio4= (crossSectionProp3d.EIy()-elasticShearSection3d.EIy())/elasticShearSection3d.EIy()
ratio5= (crossSectionProp3d.EIz()-elasticShearSection3d.EIz())/elasticShearSection3d.EIz()
ratio6= (crossSectionProp3d.GJ()-elasticShearSection3d.GJ())/elasticShearSection3d.GJ()

'''
# print(crossSectionProp3d)
print('ratio0= ', ratio0)
print('ratio1= ', ratio1)
print('G= ', G/1e6)
print('crossSectionProp3d.GAy()= ', crossSectionProp3d.GAy())
print('elasticShearSection3d.GAy()= ', elasticShearSection3d.GAy())
print('ratio2= ', ratio2)
print('crossSectionProp3d.GAz()= ', crossSectionProp3d.GAz())
print('elasticShearSection3d.GAz()= ', elasticShearSection3d.GAz())
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('ratio6= ', ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-15) & (abs(ratio1)<1e-15) & (abs(ratio2)<1e-15) & (abs(ratio3)<1e-15) & (abs(ratio4)<1e-15) & (abs(ratio5)<1e-15) & (abs(ratio6)<1e-15) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
