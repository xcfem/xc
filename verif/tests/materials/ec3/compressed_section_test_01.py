# -*- coding: utf-8 -*-
'''
Verification test based on the example:

Example 09 Bracing and bracing connections
Steel Building Design: Worked examples for students 
In accordance with Eurocodes and the UK National Annexes
Published by:
The Steel Construction Institute
ISBN 978-1-85942-191-8

'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from materials.ec3 import EC3_materials
from materials.ec3 import EC3_limit_state_checking

#Materials
## Steel material
steel= EC3_materials.S355JR
steel.gammaM= 1.00
## Section
section= EC3_materials.CHSShape(steel= steel,name='CHS_219.1_10.0')

sectionClass= section.getClassInternalPartInCompression()

## Design value of applied axial force.
NEd= 839e3

## Design resistance of the cross-section for uniform compression
NcRd= section.getNcRd()
refNcRd= 65.70e-4*355e6

err= abs(NcRd-refNcRd)/refNcRd

'''
print('section class: ',sectionClass)
print('Design resistance of the cross-section for uniform compression: Nc,Rd= ',NcRd/1e3,'kN')
print(refNcRd/1e3,'kN')
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
