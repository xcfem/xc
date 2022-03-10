# -*- coding: utf-8 -*-
''' Simply supported column capacity check example 5.6
    taken from the document Residential Structural Design Guide
    Second Edition. October 2017. U.S. Department of Housing and 
    Urban Development Office of Policy Development and Research.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
import xc

from materials.sections import section_properties
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import AWCNDS_limit_state_checking
from materials.awc_nds import dimensional_lumber

inchToMeter= 2.54/100.0
footToMeter= 0.3048
psiToPa= 6894.76
psfToPa= 47.88026
poundToNewton= 4.4482216282509

# Geometry.
columnHeight= 7.3*footToMeter

# Materials.
wood= dimensional_lumber.SprucePineFirWood(name='Spruce-Pine-Fir', grade= 'no_2', sub_grade= '')
columnSection= AWCNDS_materials.DimensionLumberSection(name= '4x4', woodMaterial= wood)
column= AWCNDS_limit_state_checking.ColumnMember(unbracedLengthB= columnHeight, unbracedLengthH= columnHeight, section= columnSection)

# Lumber property adjustments.
wood.CD= 1.0

# Load.
designLoad= -4800*poundToNewton

# Compute adjusted compressive capacity.
CP= column.getColumnStabilityFactor() # Column stability factor.
refCP= 0.4378295192248415
ratio1= abs(CP-refCP)/refCP
Fc_aster2= column.getFcAdj()
ratio2= abs(Fc_aster2-3992269.7055212082)/3992269.7055212082
fc= designLoad/column.crossSection.A()
efficiency= -fc/Fc_aster2
ratio3= abs(efficiency-0.6767125949470081)/0.6767125949470081
# Check the value returned by the general-purpose method:
efficiency2= column.getBiaxialBendingEfficiency(Nd= designLoad, Myd= 0.0, Mzd= 0.0, chiN= CP, chiLT= 1.0)
ratio4= abs(efficiency-efficiency2)/efficiency2

'''
print('Column stability factor CP= '+'{:.2f}'.format(CP))
print('Compressive strength: '+'{:.2f}'.format(Fc_aster2/1e6)+' MPa ('+'{:.2f}'.format(Fc_aster2/psiToPa)+' psi).')
print('Design stress: '+'{:.2f}'.format(fc/1e6)+' MPa ('+'{:.2f}'.format(fc/psiToPa)+' psi).')
print('Efficiency: '+'{:.2f}'.format(efficiency))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) and (ratio2<1e-10) and (ratio3<1e-10) and (ratio4<1e-10)):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")
