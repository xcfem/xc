# -*- coding: utf-8 -*-
''' Test calculation of anchorage lengths according to clause
    8.4.4 of EC2:2004. Home made test.'''

from __future__ import division
from __future__ import print_function

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

diameters= [8e-3, 10e-3, 12e-3, 16e-3, 20e-3, 25e-3, 32e-3, 40e-3]
#diameters= [12e-3]

# Define rebar controllers.
rebarControllerGood= EC2_limit_state_checking.RebarController(concreteCover= 25e-3, spacing= 150e-3, eta1= 1.0, compression= False)
rebarControllerPoor= EC2_limit_state_checking.RebarController(concreteCover= 25e-3, spacing= 150e-3, eta1= 0.7, compression= False)

# Materials
concrete= EC2_materials.C25
steel= EC2_materials.S500C

ratioOfOverlapedTensionBars= [0.5,1.0]
bondCondition= ['good', 'poor']

results= list()

for ratio in ratioOfOverlapedTensionBars:
    for bCond in bondCondition:
        row= [ratio, bCond]
        if(bCond=='good'):
            rebarController= rebarControllerGood
        else:
            rebarController= rebarControllerPoor
        for d in diameters:
            tmp= rebarController.getLapLength(concrete= concrete, rebarDiameter= d, steel= steel, steelEfficiency= 1.0, ratioOfOverlapedTensionBars= ratio)
            tmp= round(tmp*1e2)*10
            row.append(tmp)
        results.append(row)

refValues= [[0.5, 'good', 320, 440, 570, 840, 1100, 1430, 1830, 2480], [0.5, 'poor', 460, 630, 820, 1190, 1570, 2040, 2610, 3550], [1.0, 'good', 340, 470, 610, 890, 1170, 1510, 1940, 2630], [1.0, 'poor', 480, 670, 870, 1270, 1660, 2160, 2770, 3760]]

err= 0.0 
for row1, row2 in zip(results, refValues):
    for v1, v2 in zip(row1, row2):
        if(not isinstance(v1, str)):
            err+=(v1-v2)**2
err= math.sqrt(err)

'''
print(results)
print('err= ', err)
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
    
# header2= ['% lapped', 'bond', 'concrete', 'steel']+diametersCaption
# header3= ['in one location', 'condition', 'type', 'type',]+diametersHeader
# headers= [header2, header3]

# tbResults= headers
# tbResults.append(['']*len(header2))
# for row in results:
#     resultRow= [row[0]*100.0, row[1], concrete.materialName, steel.materialName]
#     resultRow.extend(row[2:])
#     tbResults.append(resultRow)
    
# from tabulate import tabulate
# print(tabulate(tbResults))

# # Write csv file.
# import csv
# fileName= concrete.materialName+'_'+steel.materialName+'_lap_lengths.csv'
# with open(fileName, 'w') as csvfile:
#     writer = csv.writer(csvfile)
#     for row in tbResults:
#         writer.writerow(row)



