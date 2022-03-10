# -*- coding: utf-8 -*-
''' Simply supported built-up column capacity check example 5.7
    taken from the book: Structural Wood Design A Practice-Oriented 
    Approach Using the ASD Method. Abi Aghayere Jason Vigil.
    John Wiley & Sons 2007.'''

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
columnHeight= 12*footToMeter

# Materials.
wood= dimensional_lumber.DouglasFirLarchWood(name='Douglas-Fir-Larch', grade= 'no_1', sub_grade= '')
(bb, hh)= AWCNDS_materials.dimensionLumberSizes['2x6']
columnSectionA= AWCNDS_materials.BuiltUpLumberSection(name= '3x2x6', b=3*bb, h= hh, woodMaterial= wood)
columnSectionB= AWCNDS_materials.BuiltUpLumberSection(name= '3x6x2', b=hh, h= 3*bb, woodMaterial= wood)
columnA= AWCNDS_limit_state_checking.ColumnMember(unbracedLengthB= columnHeight, unbracedLengthH= columnHeight, section= columnSectionA)
columnB= AWCNDS_limit_state_checking.ColumnMember(unbracedLengthB= columnHeight, unbracedLengthH= columnHeight, section= columnSectionB)

# Lumber property adjustments.
wood.CD= 1.0

# Compute stability factors of the columns.
CPA= columnA.getColumnStabilityFactor() # Column stability factor.
CPB= columnB.getColumnStabilityFactor() 
refCPA= 0.16792700488467618
ratioA= abs(CPA-refCPA)/refCPA
refCPB= 0.27987834147446033
ratioB= abs(CPB-refCPB)/refCPB

# print(CPA, CPB)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratioA<1e-10) and (ratioB<1e-10)):
    print("test "+fname+": ok.")
else:
    lmsg.error("test "+fname+": ERROR.")
