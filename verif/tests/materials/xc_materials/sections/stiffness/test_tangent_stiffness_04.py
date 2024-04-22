# -*- coding: utf-8 -*-
''' Verifies that stiffness properties are returned from the correct element
    of the stiffness matrix. Section type: ElasticSection3d. 
    Home made test.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc
from materials.ec3 import EC3_materials

def compare_values(steelShape, xcSection):
    ''' Compare the stiffness values of both sections.

    :param steelShape: steel shape object.
    :param xcSection: XC ElasticSection3d object.
    '''
    retval= 0.0
    EA1= steelShape.EA()
    EA2= xcSection.EA()
    retval+= (EA2-EA1)**2

    EIz1= steelShape.EIz()
    EIz2= xcSection.EIz()
    retval+= (EIz2-EIz1)**2

    EIy1= steelShape.EIy()
    EIy2= xcSection.EIy()
    retval+= (EIy2-EIy1)**2

    GJ1= steelShape.GJ()
    GJ2= xcSection.GJ()
    retval+= (GJ2-GJ1)**2
    
    retval= math.sqrt(retval)
    return retval
    

# Define problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

steelType= EC3_materials.S275JR
steelType.gammaM= 1.00

steelSectionA= EC3_materials.SHSShape(steel=steelType,name='SHS120x120x6')
xcSectionA= steelSectionA.defElasticSection3d(preprocessor= preprocessor)
errA= compare_values(steelShape= steelSectionA, xcSection= xcSectionA)

steelSectionB= EC3_materials.RHSShape(steel=steelType, name='RHS170x100x4')
xcSectionB= steelSectionB.defElasticSection3d(preprocessor= preprocessor)
errB= compare_values(steelShape= steelSectionB, xcSection= xcSectionB)

'''
print('errA= ', errA)
print('errB= ', errB)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (errA<1e-8) & (errB<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
