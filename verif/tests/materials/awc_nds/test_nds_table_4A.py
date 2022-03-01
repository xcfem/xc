# -*- coding: utf-8 -*-
''' Check computation net dimensions of dimensional lumber
    according to table 1A of 2018 National Design Specification Supplement
    (page 13).
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
from materials.awc_nds import dimensional_lumber

inchToMeter= 2.54/100.0

# Thickness.
nominalThk= [2.00, 2.50, 3.00, 3.50, 4.00, 4.50]
dryThk= [1.50, 2.00, 2.50, 3.00, 3.50, 4.00]
greenThk= [1.5625, 2.0625, 2.5625, 3.0625, 3.5625, 4.0625]


error= 0.0
## Dry thickness.
for nthk, dthk in zip(nominalThk, dryThk):
    tmp= dimensional_lumber.getDressedThickness(nthk*inchToMeter, dressedSize= 'Dry')/inchToMeter
    error+= (dthk-tmp)**2
    
## Green thickness.
for nthk, gthk in zip(nominalThk, greenThk):
    tmp= dimensional_lumber.getDressedThickness(nthk*inchToMeter, dressedSize= 'Green')/inchToMeter
    error+= (gthk-tmp)**2
    
# Width.
nominalWidth= [2, 3, 4, 5, 6, 8, 10, 12, 14, 16]
dryWidth= [1.50, 2.50, 3.50, 4.50, 5.50, 7.25, 9.25, 11.25, 13.25, 15.25]
greenWidth= [1.5625, 2.5625, 3.5625, 4.625, 5.625, 7.50, 9.50, 11.50, 13.50, 15.50]

## Dry thickness.
for nwdth, dwdth in zip(nominalWidth, dryWidth):
    tmp= dimensional_lumber.getDressedWidth(nwdth*inchToMeter, dressedSize= 'Dry')/inchToMeter
    error+= (dwdth-tmp)**2
    
## Green thickness.
for nwdth, gwdth in zip(nominalWidth, greenWidth):
    tmp= dimensional_lumber.getDressedWidth(nwdth*inchToMeter, dressedSize= 'Green')/inchToMeter
    error+= (gwdth-tmp)**2
    
## Nominal from Dry thickness.
for nthk, dthk in zip(nominalThk, dryThk):
    tmp= dimensional_lumber.getNominalThickness(dthk*inchToMeter, dressedSize= 'Dry')/inchToMeter
    error+= (nthk-tmp)**2
    
## Nominal from Green thickness.
for nthk, gthk in zip(nominalThk, greenThk):
    tmp= dimensional_lumber.getNominalThickness(gthk*inchToMeter, dressedSize= 'Green')/inchToMeter
    error+= (nthk-tmp)**2
    
## Nominal from Dry width.
for nwdth, dwdth in zip(nominalWidth, dryWidth):
    tmp= dimensional_lumber.getNominalWidth(dwdth*inchToMeter, dressedSize= 'Dry')/inchToMeter
    error+= (nwdth-tmp)**2
    
## Nominal from Green width.
for nwdth, gwdth in zip(nominalWidth, greenWidth):
    tmp= dimensional_lumber.getNominalWidth(gwdth*inchToMeter, dressedSize= 'Green')/inchToMeter
    error+= (nwdth-tmp)**2

error= math.sqrt(error)
#print(error)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


