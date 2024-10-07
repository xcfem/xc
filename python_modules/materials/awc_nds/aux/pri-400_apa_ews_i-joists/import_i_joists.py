# -*- coding: utf-8 -*-
''' Import shapes from AISC database version 15.0 (metric units)'''
from __future__ import print_function
from __future__ import division

import json
from misc_utils import spreadsheet_utils as su

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

fNameIn= "asd_design_properties_apa_ews_performance_rated_i_joists.xlsx"
#fNameOut= '../pr_400_i_joists.json'
fNameOut= './pp.json'

columnOrder= ['','nmb', 'b', 'h', 'EI', 'Mr', 'Vr', 'Ir', 'ER_44_wo_stiff', 'ER_44_stiff', 'ER_102_wo_stiff', 'ER_102_stiff', 'VLCr', 'K']
columnUnits= {} # All units correspond to SI, no need of conversion.

def post_process(shapesDict):
    ''' Post-process data readed from the spreadsheet

    :param shapesDict: dictionary containing the data imported from the
                       spreadsheet.
    '''
    # Some post-processing:
    retval= dict()
    for shapeName in shapesDict:
        shapeRecord= dict()
        shapeRecord.update(shapesDict[shapeName])
        retval[shapeName]= shapeRecord
    return retval

namePrefix= 'PRI'
shapesDict= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process, dataOnly= True)

jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
