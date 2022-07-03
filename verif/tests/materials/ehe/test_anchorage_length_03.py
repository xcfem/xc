# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to clause
    8.4.4 of EHE:2004. Home made test.'''

from __future__ import division
from __future__ import print_function

import os
import math
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from misc_utils import log_messages as lmsg

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

diameters= [8e-3, 10e-3, 12e-3, 16e-3, 20e-3, 25e-3, 32e-3, 40e-3]
#diameters= [12e-3]

# Define rebar controllers.
rebarControllerPosI= EHE_limit_state_checking.RebarController(concreteCover= 25e-3, pos= 'I', compression= False)
rebarControllerPosII= EHE_limit_state_checking.RebarController(concreteCover= 25e-3, pos= 'II', compression= False)

# Materials
concrete= EHE_materials.HA25
steel= EHE_materials.B500S

lbd_straight_posI_cond= ['posI', 'straight']
lbd_straight_posII_cond= ['posII', 'straight']
lbd_bent_posI_cond= ['posI', 'bent']
lbd_bent_posII_cond= ['posII', 'bent']
for d in diameters:
    tmp= rebarControllerPosI.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'straight')
    tmp= math.ceil(tmp*1e2)*10
    lbd_straight_posI_cond.append(tmp)
    tmp= rebarControllerPosII.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'straight')
    tmp= math.ceil(tmp*1e2)*10
    lbd_straight_posII_cond.append(tmp)
    tmp= rebarControllerPosI.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'bent')
    tmp= math.ceil(tmp*1e2)*10
    lbd_bent_posI_cond.append(tmp)
    tmp= rebarControllerPosII.getDesignAnchorageLength(concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, barShape= 'bent')
    tmp= math.ceil(tmp*1e2)*10
    lbd_bent_posII_cond.append(tmp)

values= [lbd_straight_posI_cond, lbd_straight_posII_cond,lbd_bent_posI_cond,lbd_bent_posII_cond]
refValuesEHE=[['posI', 'straight', 150, 200, 260, 370, 580, 940, 1540, 2400], ['posII', 'straight', 200, 280, 360, 530, 810, 1320, 2160, 3360], ['posI', 'bent', 200, 250, 300, 400, 600, 940, 1540, 2400], ['posII', 'bent', 290, 360, 430, 580, 840, 1320, 2160, 3360]]
#refValuesEC2= [['posI', 'straight', 230, 320, 410, 600, 780, 1010, 1300, 1760], ['posII', 'straight', 330, 450, 580, 850, 1110, 1450, 1850, 2510], ['posI', 'bent', 320, 410, 490, 650, 810, 1010, 1300, 1760], ['posII', 'bent', 460, 580, 700, 930, 1160, 1450, 1850, 2510]]

err= 0.0 

for row1, row2 in zip(values, refValuesEHE):
    for v1, v2 in zip(row1[2:], row2[2:]):
        err+=(v1-v2)**2
err= math.sqrt(err)

'''
print(values)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')


# # Tabular output
# numDiameters= len(diameters)
# diametersCaption= ['']*numDiameters
# diametersCaption[0]= 'Reinforcement in tension, bar diameter (mm)'
# diametersHeader= ['']*numDiameters
# for i, d in enumerate(diameters):
#     diametersHeader[i]= str(int(d*1e3))
    
# header2= ['bar', 'bond', 'concrete', 'steel']+diametersCaption
# header3= ['type', 'condition', 'type', 'type',]+diametersHeader
# headers= [header2, header3]

# results= headers
# results.append(['']*len(header2))
# for row in values:
#     resultRow= [row[1], row[0], concrete.materialName, steel.materialName]
#     resultRow.extend(row[2:])
#     results.append(resultRow)
    
# from tabulate import tabulate
# print(tabulate(results))

# # Write csv file.
# import csv
# fileName= concrete.materialName+'_'+steel.materialName+'_anchorage_lengths.csv'
# with open(fileName, 'w') as csvfile:
#     writer = csv.writer(csvfile)
#     for row in results:
#         writer.writerow(row)



