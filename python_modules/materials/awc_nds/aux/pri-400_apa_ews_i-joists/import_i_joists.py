# -*- coding: utf-8 -*-
''' Import shapes from AISC database version 15.0 (metric units)'''
from __future__ import print_function
from __future__ import division

from pyexcel_ods import get_data
import json

# Section axis:

#    XC

#       ^ Y
#       | 

#     ----- 
#       |  
#       | -> Z
#       |    
#     -----
# 'h': Section depth.
# 'EI': Bending stiffness (El) of the I-joist.
# 'Mr': Factored moment capacity (Mr) of the I-joist.
# 'Vr': Factored shear resistance (Vr) of the I-joist.
# 'Ir': Factored intermediate reaction (IR,) of the I-joist
#       with a minimum bearing length of 89 mm (3-1/2 inches)
#       without bearing stiffeners.

# Factored end reaction (ER) of the I-joist. Interpolation between
# 44-mm (1-3/4-in.) and 102-mm (4-in.) bearings is permitted with or
# without bearing , stiffeners.
# 'ER_44_wo_stiff':  ER 44 mm without stiffeners.
# 'ER_44_stiff':     ER 44 mm with stiffeners.
# 'ER_102_wo_stiff': ER 102 mm without stiffeners.
# 'ER_102_stiff':    ER 102 mm with stiffeners.

# 'VLCr': Factored uniform vertical (bearing) load capacity (VLCr).
# 'K': Coefficient of shear deflection (K), which shall
#      be used to calculate uniform load and center-point
#      load deflections of the I-joist in a simple-span
#      application based on Eqs. 1 and 2.

fNameIn= "factored_resistance_apa_ews_performance_rated_i_joists.ods"
fNameOut= '../pr_400_i_joists.json'

columnOrder= ['','nmb', 'h', 'EI', 'Mr', 'Vr', 'Ir', 'ER_44_wo_stiff', 'ER_44_stiff', 'ER_102_wo_stiff', 'ER_102_stiff', 'VLCr', 'K']
numColumns= len(columnOrder)
columnUnits= {} # All units correspond to SI, no need of conversion.

columnKeys= dict()
for count, key in enumerate(columnOrder):
    if(len(key)>0):
        columnKeys[key]= count

shapesDict= dict() # Dictionary with all the shapes.
namePrefix= 'PRI'
data= get_data(fNameIn)
for sheet in data:
    rows= data[sheet]
    for r in rows:
        if(len(r)>=numColumns): # if not empty.
            name= r[columnKeys['nmb']].replace(' ','')
            if(name.startswith(namePrefix)):
                shapeRecord= dict()
                for key in columnOrder:
                    if(len(key)>0):
                        column= columnKeys[key]
                        shapeRecord[key]= r[column]
                #shapeRecord['h']= shapeRecord['b']
                shapeName= name+'_'+str(int(shapeRecord['h']*1000))
                shapesDict[shapeName]= shapeRecord
                
# Some post-processing:
# for shapeName in shapesDict:
#     shapeRecord= shapesDict[shapeName]
#     for key in shapeRecord:
#         if(key=='nmb'):
#             value= shapeRecord[key].replace(' ','')
#             shapeRecord[key]= value
#         if(key=='CheckAvailability'):
#             value= shapeRecord[key]
#             shapeRecord[key]= (value=='#')
                
#         if key in columnUnits:
#             factor= columnUnits[key]
#             value= shapeRecord[key].replace(',','')
#             shapeRecord[key]= float(value+factor)
#     h_flat= shapeRecord['h']-3.0*shapeRecord['e']
#     shapeRecord['h_flat']= h_flat
#     b_flat= shapeRecord['b']-3.0*shapeRecord['e']
#     shapeRecord['b_flat']= b_flat
            
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
