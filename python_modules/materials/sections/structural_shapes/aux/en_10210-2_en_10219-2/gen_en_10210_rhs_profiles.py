# -*- coding: utf-8 -*-
'''Implementation of EN 10210-2 annex A formulas with a little help of Gemini
   (and a lot of bugs fixed afterwards).

See https://eurocodeapplied.com/design/en1993/rhs-design-properties.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import json
from materials.ec3 import en_10210_section_calculator
from rhs_shapes_to_generate import rhs_shapes_to_generate
from misc_utils import spreadsheet_utils as su
from misc_utils import log_messages as lmsg

    
def post_process(shapesDict):
    ''' Post-process data readed from the spreadsheet

    :param shapesDict: dictionary containing the data imported from the
                       spreadsheet.
    '''
    # Some post-processing:
    retval= dict()
    for shapeName in shapesDict:
        shapeData= shapesDict[shapeName]
        newShapeName= shapeName.replace(' / ', 'x')
        newShapeName= newShapeName.replace('RHS ', 'RHS')
        newShapeName= newShapeName.replace('.', '_')
        shapeData['nmb']= newShapeName
        retval[newShapeName]= shapeData
    return retval

def read_shapes_from_spreadsheet(post_process):
    fNameIn= "eurocodeapplied_en_10210_rhs_properties.xlsx"

    columnOrder= ['nmb', '', 'h', 'b', 'e', '', '', 'P', '', 'A', 'Avy', 'Avz', 'Iz', 'iz', 'Wzel', 'Wzpl', 'Iy', 'iy', 'Wyel', 'Wypl' , 'It', 'Wt']
    numColumns= len(columnOrder)


    columnUnits= {'b':1e-3,'h':1e-3, 'e':1e-3, 'A':1e-6, 'Avy':1e-6, 'Avz':1e-6, 'Iz':1e-6, 'iz':1e-3, 'Wzel':1e-6, 'Wzpl':1e-6, 'Iy':1e-6, 'iy':1e-3, 'Wyel':1e-6, 'Wypl':1e-6, 'It':1e-9,  'Wt':1e-6}

    namePrefix= 'RHS'

    retval= su.read_shapes_from_spreadsheet(namePrefix= namePrefix, columnOrder= columnOrder, columnUnits= columnUnits, spreadsheetFileName= fNameIn, E= 210000e6, nu= 0.3, post_processing_function= post_process)
    return retval

test_classification= False

# --- Quick Test Execution ---
# Check against data from:
# https://eurocodeapplied.com/design/en1993/rhs-design-properties
rhs_data= read_shapes_from_spreadsheet(post_process)

shapesToCheck= list(rhs_data.keys())
calc= en_10210_section_calculator.RHSSHSectionCalculator()

for key in rhs_data:
    if(key): #if(key in shapesToCheck):
        dimensions= key.removeprefix('RHS').replace('_', '.').split('x')
        dimensions= [float(x)*1e-3 for x in dimensions]
        calculated_properties= calc.calculate_rhs(h=dimensions[0], b= dimensions[1], t= dimensions[2])
        for propKey in rhs_data[key]:
            if(propKey in calculated_properties):
                stored_value= rhs_data[key][propKey]
                calculated_value= calculated_properties[propKey]
                if(isinstance(stored_value, str)):
                    if(stored_value!=calculated_value):
                        lmsg.error('Error: '+str(key)+', '+ str(propKey)+', '+str(stored_value)+', '+str(calculated_value))
                        exit(1)
                else:
                    err= abs(stored_value-calculated_value)/abs(stored_value)
                    if(err>5e-3):
                        lmsg.error('Error: '+str(key)+', '+ str(propKey)+', '+str(stored_value)+', '+str(calculated_value)+', '+str(err*100)+' %')
                        exit(1)
            else:
                lmsg.error('Missing property: '+str(propKey))
                exit(1)

if(test_classification):
    # Example 1: Standard hot-finished section S355
    print("--- Section Classification Test 1 ---")
    res1 = calc.classify_rhs(h=150.0, b=100.0, t=5.0, fy=355.0)
    print(res1)
    for key in res1:
        print(f"{key}", res1[key])

    # Example 2: Extremely thin custom cross section (Simulating higher slenderness)
    print("\n--- Section Classification Test 2 (Thin Walls) ---")
    res2 = calc.classify_rhs(h=200.0, b=200.0, t=3.0, fy=460.0)
    print(f"Pure Compression Class: Class {res2['class_comp']}")
    print(f"Pure Bending Class zz : Class {res2['class_bending_zz']}")

# Generate JSON file containing the design properties of the desired shapes.
shapesDict= dict()
for key in rhs_shapes_to_generate:  
    dimensions= key.removeprefix('RHS').replace('_', '.').split('x')
    dimensions= [float(x)*1e-3 for x in dimensions]
    calculated_properties= calc.calculate_rhs(h=dimensions[0], b= dimensions[1], t= dimensions[2])
    shapesDict[key]= calculated_properties
## Write the JSON file
fNameOut= '../en_10210_rhs_profiles.json'
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
