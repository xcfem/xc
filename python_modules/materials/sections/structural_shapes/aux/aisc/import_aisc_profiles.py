# -*- coding: utf-8 -*-
''' Import shapes from AISC database version 15.0 (metric units)'''
from __future__ import print_function
from __future__ import division
import openpyxl
from misc_utils import log_messages as lmsg
from openpyxl.utils.cell import coordinate_from_string, column_index_from_string
import unicodedata

# Section axis:

#    AISC            XC

#       ^ Y           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> X        | -> Z
#       |             |
#     -----         -----

# The axis used in AISC catalogue are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

sheet_columns= dict()
sheet_columns['W']= 'CI'
sheet_columns['A']= 'CJ'
sheet_columns['d']= 'CK'
sheet_columns['ddet']= 'CL'
sheet_columns['Ht']= 'CM' #Overall depth of square or rectangular HSS
sheet_columns['h']= 'CN'
sheet_columns['OD']= 'CO'
sheet_columns['bf']= 'CP'
sheet_columns['bfdet']= 'CQ'
sheet_columns['B']= 'CR'
sheet_columns['b']= 'CS'
sheet_columns['ID']= 'CT'
sheet_columns['tw']= 'CU'
sheet_columns['twdet']= 'CV'
sheet_columns['twdet/2']= 'CW'
sheet_columns['tf']= 'CX'
sheet_columns['tfdet']= 'CY'
sheet_columns['t']= 'CZ'
sheet_columns['tnom']= 'DA'
sheet_columns['tdes']= 'DB'
sheet_columns['kdes']= 'DC'
sheet_columns['kdet']= 'DD'
sheet_columns['k1']= 'DE'
sheet_columns['x']= 'DF' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member centroidal axis.
sheet_columns['y']= 'DG' # Vertical distance from designated member edge, as defined in the AISC Steel Construction Manual, to member centroidal axis.
sheet_columns['eo']= 'DH' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member shear center.
sheet_columns['xp']= 'DI' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member plastic neutral axis.
sheet_columns['yp']= 'DJ' # Vertical distance from designated member edge, as defined in the AISC Steel Construction Manual, to member plastic neutral axis.
sheet_columns['bf/2tf']= 'DK'
sheet_columns['b/t']= 'DL'
sheet_columns['b/tdes']= 'DM'
sheet_columns['h/tw']= 'DN'
sheet_columns['h/tdes']= 'DO'
sheet_columns['D/t']= 'DP'
sheet_columns['Ix']= 'DQ'
sheet_columns['Zx']= 'DR'
sheet_columns['Sx']= 'DS'
sheet_columns['rx']= 'DT'
sheet_columns['Iy']= 'DU'
sheet_columns['Zy']= 'DV'
sheet_columns['Sy']= 'DW'
sheet_columns['ry']= 'DX'
sheet_columns['Iz']= 'DY'
sheet_columns['rz']= 'DZ'
sheet_columns['Sz']= 'EA'
sheet_columns['J']= 'EB'
sheet_columns['Cw']= 'EC'
sheet_columns['C']= 'ED' #HSS torsional constant
sheet_columns['Wno']= 'EE'
sheet_columns['Sw1']= 'EF'
sheet_columns['Sw2']= 'EG'
sheet_columns['Sw3']= 'EH'
sheet_columns['Qf']= 'EI'
sheet_columns['Qw']= 'EJ'
sheet_columns['ro']= 'EK'
sheet_columns['H']= 'EL'
sheet_columns['tan(α)']= 'EM'
sheet_columns['Iw']= 'EN'
sheet_columns['zA']= 'EO'
sheet_columns['zB']= 'EP'
sheet_columns['zC']= 'EQ'
sheet_columns['wA']= 'ER'
sheet_columns['wB']= 'ES'
sheet_columns['wC']= 'ET'
sheet_columns['SwA']= 'EU'
sheet_columns['SwB']= 'EV'
sheet_columns['SwC']= 'EW'
sheet_columns['SzA']= 'EX'
sheet_columns['SzB']= 'EY'
sheet_columns['SzC']= 'EZ'
sheet_columns['rts']= 'FA'
sheet_columns['ho']= 'FB'
sheet_columns['PA']= 'FC'
sheet_columns['PA2']= 'FD'
sheet_columns['PB']= 'FE'
sheet_columns['PC']= 'FF'
sheet_columns['PD']= 'FG'
sheet_columns['T']= 'FH'
sheet_columns['WGi']= 'FI'
sheet_columns['WGo']= 'FJ'

def getDictName(shapeType):
    ''' Return the dictionary name from the shape type.'''
    retval= shapeType
    if(shapeType=='2L'):
        retval= 'TwoL'
    return retval

def writeVariable(row, variableName, key,unitConv= ''):
    ''' Write the variable in the Python dictionary.'''
    col= column_index_from_string(sheet_columns[variableName])-1
    value= row[col].value
    if(value!=u'\u2013'):
        out.write("\'"+key+"\':"+str(value)+unitConv+", ")

createdDictionaries= list()

def writeShape(out, row):
    ''' Write shape record from row data.'''
    shapetype= row[0].value
    if(shapetype):
        dictName= getDictName(shapetype)
        if(dictName not in createdDictionaries):
            out.write(dictName+"= dict()\n")
            createdDictionaries.append(dictName)
        col= column_index_from_string(sheet_columns['Ht'])-1
        col= column_index_from_string('B')
        name= row[col].value
        out.write(dictName+"['" + name + "']= ")
        out.write("{")
        out.write("\'nmb\':\'" + name +"\', ")
        writeVariable(row, variableName= 'W', key= 'P') 
        writeVariable(row, variableName= 'A', key= 'A', unitConv= 'e-6') 
        writeVariable(row, variableName= 'Ht', key= 'h', unitConv= 'e-3') 
        writeVariable(row, variableName= 'h', key= 'h_flat', unitConv= 'e-3') 
        writeVariable(row, variableName= 'B', key= 'b', unitConv= 'e-3') 
        writeVariable(row, variableName= 'b', key= 'b_flat', unitConv= 'e-3') 
        writeVariable(row, variableName= 'tnom', key= 'tnom', unitConv= 'e-3') 
        writeVariable(row, variableName= 'tdes', key= 't', unitConv= 'e-3')
        writeVariable(row, variableName= 'x', key= 'x', unitConv= 'e-3')
        writeVariable(row, variableName= 'y', key= 'y', unitConv= 'e-3')
        writeVariable(row, variableName= 'eo', key= 'eo', unitConv= 'e-3')
        writeVariable(row, variableName= 'xp', key= 'xp', unitConv= 'e-3')
        writeVariable(row, variableName= 'yp', key= 'yp', unitConv= 'e-3')
        writeVariable(row, variableName= 'b/tdes', key= 'bSlendernessRatio') 
        writeVariable(row, variableName= 'h/tdes', key= 'hSlendernessRatio') 
        writeVariable(row, variableName= 'Ix', key= 'Iz', unitConv= 'e-6') 
        writeVariable(row, variableName= 'Zx', key= 'Wzpl', unitConv= 'e-6') 
        writeVariable(row, variableName= 'Sx', key= 'Wzel', unitConv= 'e-6') 
        writeVariable(row, variableName= 'rx', key= 'iz', unitConv= 'e-3') 
        writeVariable(row, variableName= 'Iy', key= 'Iy', unitConv= 'e-6') 
        writeVariable(row, variableName= 'Zy', key= 'Wypl', unitConv= 'e-6') 
        writeVariable(row, variableName= 'Sy', key= 'Wyel', unitConv= 'e-6') 
        writeVariable(row, variableName= 'ry', key= 'iy', unitConv= 'e-3') 
        writeVariable(row, variableName= 'J', key= 'It', unitConv= 'e-9') 
        writeVariable(row, variableName= 'Cw', key= 'Cw', unitConv= 'e-18') # unit conversion correct? 
        writeVariable(row, variableName= 'C', key= 'C', unitConv= 'e-9') 
        out.write("\'E\': 210000e6, ")
        out.write("\'nu\': 0.3")
        out.write(" }\n")

# Give the location of the file
loc= ('./aisc-shapes-database-v15.0.xlsx')
wb = openpyxl.load_workbook(loc)
sheet= wb['Database v15.0']
fNameOut= 'aisc_shapes_dictionaries.py'

out= open(fNameOut,mode='w')

weightColumn= 'CI'
for row in sheet.iter_rows(min_row= 2):
    writeShape(out,row)

out.close()        
