# -*- coding: utf-8 -*-
''' Check the values of mass properties (area, inertia, etc.) returned by
    object of RCRectangularSection class. 
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section

structureConcrete= EHE_materials.HA30
reinfSteel= EHE_materials.B500S

c= 1.4
b= 0.45

flatColumnsRCSection= def_simple_RC_section.RCRectangularSection(name= 'flatColumnsRCSection', sectionDescr= 'columns section', concrType= structureConcrete, reinfSteelType= reinfSteel, width= c, depth= b, swapReinforcementAxes= True)

J= flatColumnsRCSection.J()
# Reference value obtained from Bentley RAM Structural System Help. Concrete Beam Torsional Stiffness
# https://docs.bentley.com/LiveContent/web/RAM%20Structural%20System%20Help%20Complete-v5/en/GUID-F812B55C-A8C7-4BE0-BCAC-179CCECE56D1.html
refJ= c*b**3*(1/3.0-0.21*b/c)
ratio1= abs(J-refJ)/refJ

elasticMatData= flatColumnsRCSection.getElasticMaterialData()
sectionProperties= flatColumnsRCSection.getCrossSectionProperties3D(material= elasticMatData)
ratio2= abs(J-sectionProperties.J)




'''
print(J)
print(refJ)
print(ratio1)
print(sectionProperties)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<.01) and (ratio2==0.0)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
