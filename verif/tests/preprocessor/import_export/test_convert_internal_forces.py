# -*- coding: utf-8 -*-
''' Check import/export internal forces data from and to a CSV file.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import os
import sys
import math
import json
from import_export import convert_internal_forces as cv


pth= os.path.dirname(__file__)
if(not pth):
  pth= "."

internalForcesFName= pth+"/../../aux/internal_forces/intForce_ULS_9565.json"
thisFileName= os.path.basename(__file__)
csvTemporaryFileName= '/tmp/'+thisFileName.replace('.py', '.csv')
jsonTemporaryFileName= '/tmp/'+thisFileName.replace('.py', '.json')


cv.json_to_csv(internalForcesFName, csvTemporaryFileName, unitConversionFactor= 1e-3)
cv.csv_to_json(csvTemporaryFileName, jsonTemporaryFileName, unitConversionFactor= 1e3)

with open(internalForcesFName, 'r') as f:
    internal_forces_dict_a = json.load(f)
with open(jsonTemporaryFileName, 'r') as f:
    internal_forces_dict_b = json.load(f)

error= 0.0
for combName in internal_forces_dict_a: # for each load combination.
    element_data_a= internal_forces_dict_a[combName]
    element_data_b= internal_forces_dict_b[combName]
    for elementId in element_data_a: # for each element.
        elementTag= int(elementId)
        element_results_a= element_data_a[elementId]
        element_results_b= element_data_b[elementId]
        element_type_a= element_results_a['type']
        element_type_b= element_results_b['type']
        if(element_type_a!=element_type_b):
            err_msg= 'Something went wrong. Element types are different.' 
            lmsg.error(err_msg)
            exit(1)
        element_internal_forces_a= element_results_a['internalForces']
        element_internal_forces_b= element_results_b['internalForces']
        for posId in element_internal_forces_a: # for each position in the element.
            posTag= int(posId)
            pos_internal_forces_a= element_internal_forces_a[posId]
            N_a= pos_internal_forces_a['N']
            Vy_a= pos_internal_forces_a['Vy']
            Vz_a= pos_internal_forces_a['Vz']
            T_a= pos_internal_forces_a['T']
            My_a= pos_internal_forces_a['My']
            Mz_a= pos_internal_forces_a['Mz']
            pos_internal_forces_b= element_internal_forces_b[posId]
            N_b= pos_internal_forces_b['N']
            Vy_b= pos_internal_forces_b['Vy']
            Vz_b= pos_internal_forces_b['Vz']
            T_b= pos_internal_forces_b['T']
            My_b= pos_internal_forces_b['My']
            Mz_b= pos_internal_forces_b['Mz']
            error+= (N_a-N_b)**2+(Vy_a-Vy_b)**2+(Vz_a-Vz_b)**2+(T_a-T_b)**2+(My_a-My_b)**2 +(Mz_a-Mz_b)**2

error= math.sqrt(error)

# print(error)

from misc_utils import log_messages as lmsg
if (error<1e-8):
    print('test '+thisFileName+': ok.')
else:
    lmsg.error(thisFileName+' ERROR.')
    
os.system('rm -f -r '+csvTemporaryFileName) # Clean after yourself.
os.system('rm -f -r '+jsonTemporaryFileName)

