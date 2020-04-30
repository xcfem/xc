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
sheet_columns['d']= 'CK' # Overall depth of member, or width of shorter leg for angles, or width of the outstanding legs of long legs back-to-back double angles, or the width of the back-to-back legs of short legs back-to-back double angles.
sheet_columns['ddet']= 'CL' # Detailing value of member depth.
sheet_columns['Ht']= 'CM' # Overall depth of square or rectangular HSS.
sheet_columns['h']= 'CN' # Depth of the flat wall of square or rectangular HSS.
sheet_columns['OD']= 'CO' # Outside diameter of round HSS or pipe.
sheet_columns['bf']= 'CP' # Flange width.
sheet_columns['bfdet']= 'CQ' # Detailing value of flange width
sheet_columns['B']= 'CR' # Overall width of square or rectangular HSS,
sheet_columns['b']= 'CS' # Width of the flat wall of square or rectangular HSS, or width of the longer leg for angles, or width of the back-to-back legs of long legs back-to-back double angles, or width of the outstanding legs of short legs back-to-back double angles.
sheet_columns['ID']= 'CT' # Inside diameter of round HSS or pipe.
sheet_columns['tw']= 'CU' # Web thickness.
sheet_columns['twdet']= 'CV' # Detailing value of web thickness.
sheet_columns['twdet/2']= 'CW' # Detailing value of tw/2
sheet_columns['tf']= 'CX' # Flange thickness.
sheet_columns['tfdet']= 'CY' # Detailing value of flange thickness.
sheet_columns['t']= 'CZ' # Thickness of angle leg
sheet_columns['tnom']= 'DA' # HSS and pipe nominal wall thickness
sheet_columns['tdes']= 'DB' # HSS and pipe design wall thickness
sheet_columns['kdes']= 'DC' # Design distance from outer face of flange to web toe of fillet
sheet_columns['kdet']= 'DD' # Detailing distance from outer face of flange to web toe of fille
sheet_columns['k1']= 'DE' # Detailing distance from center of web to flange toe of fillet
sheet_columns['x']= 'DF' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member centroidal axis.
sheet_columns['y']= 'DG' # Vertical distance from designated member edge, as defined in the AISC Steel Construction Manual, to member centroidal axis.
sheet_columns['eo']= 'DH' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member shear center.
sheet_columns['xp']= 'DI' # Horizontal distance from designated member edge, as defined in the AISC Steel Construction Manual, to member plastic neutral axis.
sheet_columns['yp']= 'DJ' # Vertical distance from designated member edge, as defined in the AISC Steel Construction Manual, to member plastic neutral axis.
sheet_columns['bf/2tf']= 'DK' # Slenderness ratio.
sheet_columns['b/t']= 'DL' # Slenderness ratio for angles.
sheet_columns['b/tdes']= 'DM' # Slenderness ratio for square or rectangular HSS.
sheet_columns['h/tw']= 'DN' # Slenderness ratio
sheet_columns['h/tdes']= 'DO' # Slenderness ratio for square or rectangular HSS.
sheet_columns['D/t']= 'DP' # Slenderness ratio for round HSS and pipe, or tee shapes.
sheet_columns['Ix']= 'DQ'  # Moment of inertia about the x-axis, in.4 (mm4 /106)
sheet_columns['Zx']= 'DR'  # Plastic section modulus about the x-axis, in.3 (mm3 /103)
sheet_columns['Sx']= 'DS'  # Elastic section modulus about the x-axis, in.3 (mm3 /103)
sheet_columns['rx']= 'DT'  # Radius of gyration about the x-axis, in. (mm)
sheet_columns['Iy']= 'DU'  # Moment of inertia about the y-axis, (mm4 /106)
sheet_columns['Zy']= 'DV'  # Plastic section modulus about the y-axis, in.3 (mm3 /103)
sheet_columns['Sy']= 'DW'  # Elastic section modulus about the y-axis, in.3 (mm3 /103)
sheet_columns['ry']= 'DX'  # Radius of gyration about the y-axis (with no separation for double angles back-to-back), in. (mm)
sheet_columns['Iz']= 'DY' # Moment of inertia about the z-axis, (mm4 /106)
sheet_columns['rz']= 'DZ' # Radius of gyration about the z-axis, (mm)
sheet_columns['Sz']= 'EA' # Elastic section modulus about the z-axis, (mm3 /103)
sheet_columns['J']= 'EB' # Torsional moment of inertia
sheet_columns['Cw']= 'EC' # Warping constant.
sheet_columns['C']= 'ED' # HSS torsional constant
sheet_columns['Wno']= 'EE' # Normalized warping function, as used in Design Guide 9
sheet_columns['Sw1']= 'EF' # Warping statical moment at point 1 on cross section, as used in AISC Design Guide 9 and shown in Figures 1 and 2, in.4 (mm4 /106)
sheet_columns['Sw2']= 'EG' # Warping statical moment at point 2 on cross section, as used in AISC Design Guide 9 and shown in Figure 2, in.4 (mm4 /106)
sheet_columns['Sw3']= 'EH' # Warping statical moment at point 3 on cross section, as used in AISC Design Guide 9 and shown in Figure 2, in.4 (mm4 /106)
sheet_columns['Qf']= 'EI' # Statical moment for a point in the flange directly above the vertical edge of the web, as used in AISC Design Guide 9, in.3 (mm3 /103)
sheet_columns['Qw']= 'EJ' # Statical moment for a point at mid-depth of the cross section, as used in AISC Design Guide 9, in.3 (mm3 /103)
sheet_columns['ro']= 'EK' # Polar radius of gyration about the shear center, in. (mm)
sheet_columns['H']= 'EL' # Flexural constant
sheet_columns['tan(α)']= 'EM' # Tangent of the angle between the y-y and z-z axes for single angles, where a is shown in Figure 3
sheet_columns['Iw']= 'EN' # Moment of inertia about the w-axis for single angles, in.4 (mm4 /106)
sheet_columns['zA']= 'EO' # Distance from point A to center of gravity along z-axis, as shown in Figure 3, in. (mm)
sheet_columns['zB']= 'EP' # Distance from point B to center of gravity along z-axis, as shown in Figure 3, in. (mm)
sheet_columns['zC']= 'EQ' # Distance from point C to center of gravity along z-axis, as shown in Figure 3, in. (mm)
sheet_columns['wA']= 'ER' # Distance from point A to center of gravity along w-axis, as shown in Figure 3, in. (mm)
sheet_columns['wB']= 'ES' # Distance from point B to center of gravity along w-axis, as shown in Figure 3, in. (mm)
sheet_columns['wC']= 'ET' # Distance from point C to center of gravity along w-axis, as shown in Figure 3, in. (mm)
sheet_columns['SwA']= 'EU' # Elastic section modulus about the w-axis at point A on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['SwB']= 'EV' # Elastic section modulus about the w-axis at point B on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['SwC']= 'EW' # Elastic section modulus about the w-axis at point C on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['SzA']= 'EX' # Elastic section modulus about the z-axis at point A on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['SzB']= 'EY' # Elastic section modulus about the z-axis at point B on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['SzC']= 'EZ' # Elastic section modulus about the z-axis at point C on cross section, as shown in Figure 3, in.3 (mm3 /103)
sheet_columns['rts']= 'FA' # Effective radius of gyration, in. (mm)
sheet_columns['ho']= 'FB' # Distance between the flange centroids, in. (mm)
sheet_columns['PA']= 'FC' # Shape perimeter minus one flange surface (or short leg surface for a single angle), as used in Design Guide 19, in. (mm)
sheet_columns['PA2']= 'FD' # Single angle shape perimeter minus long leg surface, as used in AISC Design Guide 19, in. (mm)
sheet_columns['PB']= 'FE' # Shape perimeter, as used in AISC Design Guide 19, in. (mm)
sheet_columns['PC']= 'FF' # Box perimeter minus one flange surface, as used in Design Guide 19, in. (mm)
sheet_columns['PD']= 'FG' # Box perimeter, as used in AISC Design Guide 19, in. (mm)
sheet_columns['T']= 'FH' # Distance between web toes of fillets at top and bottom of web, in. (mm)
sheet_columns['WGi']= 'FI' # The workable gage for the inner fastener holes in the flange that provides for entering and tightening clearances and edge distance and spacing requirements. The actual size, combination, and orientation of fastener components should be compared with the geometry of the cross section to ensure compatibility. See AISC Manual Part 1 for additional information, in. (mm)
sheet_columns['WGo']= 'FJ' # The bolt spacing between inner and outer fastener holes when the workable gage is compatible with four holes across the flange. See AISC Manual Part 1 for additional information, in. (mm)

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
        writeVariable(row, variableName= 'd', key= 'h', unitConv= 'e-3')
        writeVariable(row, variableName= 'ddet', key= 'hDet', unitConv= 'e-3')
        writeVariable(row, variableName= 'Ht', key= 'h', unitConv= 'e-3')
        writeVariable(row, variableName= 'h', key= 'h_flat', unitConv= 'e-3')
        writeVariable(row, variableName= 'bf', key= 'b', unitConv= 'e-3')
        writeVariable(row, variableName= 'bfdet', key= 'bDet', unitConv= 'e-3')
        writeVariable(row, variableName= 'B', key= 'b', unitConv= 'e-3')
        writeVariable(row, variableName= 'b', key= 'b_flat', unitConv= 'e-3')
        writeVariable(row, variableName= 'tw', key= 'tw', unitConv= 'e-3')
        writeVariable(row, variableName= 'twdet', key= 'twDet', unitConv= 'e-3')
        writeVariable(row, variableName= 'tf', key= 'tf', unitConv= 'e-3')
        writeVariable(row, variableName= 'tfdet', key= 'tfDet', unitConv= 'e-3')
        writeVariable(row, variableName= 't', key= 't', unitConv= 'e-3')
        writeVariable(row, variableName= 'tnom', key= 'tnom', unitConv= 'e-3')
        writeVariable(row, variableName= 'tdes', key= 't', unitConv= 'e-3')
        writeVariable(row, variableName= 'kdes', key= 'kDes', unitConv= 'e-3')
        writeVariable(row, variableName= 'kdet', key= 'kDet', unitConv= 'e-3')
        writeVariable(row, variableName= 'k1', key= 'k1', unitConv= 'e-3')
        writeVariable(row, variableName= 'x', key= 'x', unitConv= 'e-3')
        writeVariable(row, variableName= 'y', key= 'y', unitConv= 'e-3')
        writeVariable(row, variableName= 'eo', key= 'eo', unitConv= 'e-3')
        writeVariable(row, variableName= 'xp', key= 'xp', unitConv= 'e-3')
        writeVariable(row, variableName= 'yp', key= 'yp', unitConv= 'e-3')
        writeVariable(row, variableName= 'bf/2tf', key= 'bSlendernessRatio')
        writeVariable(row, variableName= 'b/t', key= 'bSlendernessRatio')
        writeVariable(row, variableName= 'b/tdes', key= 'bSlendernessRatio')
        writeVariable(row, variableName= 'h/tw', key= 'hSlendernessRatio')
        writeVariable(row, variableName= 'h/tdes', key= 'hSlendernessRatio')
        writeVariable(row, variableName= 'D/t', key= 'slendernessRatio')
        writeVariable(row, variableName= 'Ix', key= 'Iz', unitConv= 'e-6')
        writeVariable(row, variableName= 'Zx', key= 'Wzpl', unitConv= 'e-6')
        writeVariable(row, variableName= 'Sx', key= 'Wzel', unitConv= 'e-6')
        writeVariable(row, variableName= 'rx', key= 'iz', unitConv= 'e-3')
        writeVariable(row, variableName= 'Iy', key= 'Iy', unitConv= 'e-6')
        writeVariable(row, variableName= 'Zy', key= 'Wypl', unitConv= 'e-6')
        writeVariable(row, variableName= 'Sy', key= 'Wyel', unitConv= 'e-6')
        writeVariable(row, variableName= 'ry', key= 'iy', unitConv= 'e-3')
        writeVariable(row, variableName= 'Iz', key= 'Ix', unitConv= 'e-6')
        writeVariable(row, variableName= 'rz', key= 'ix', unitConv= 'e-3')
        writeVariable(row, variableName= 'Sz', key= 'Wxel', unitConv= 'e-6')
        writeVariable(row, variableName= 'J', key= 'It', unitConv= 'e-9')
        writeVariable(row, variableName= 'Cw', key= 'Cw', unitConv= 'e-9') # unit conversion correct?
        writeVariable(row, variableName= 'C', key= 'C', unitConv= 'e-9')
        writeVariable(row, variableName= 'Wno', key= 'Wno', unitConv='e-6')
        writeVariable(row, variableName= 'Sw1', key= 'Sw1', unitConv='e-6')
        writeVariable(row, variableName= 'Sw2', key= 'Sw2', unitConv='e-6')
        writeVariable(row, variableName= 'Sw3', key= 'Sw3', unitConv='e-6')
        writeVariable(row, variableName= 'Qf', key= 'Qf', unitConv='e-6')
        writeVariable(row, variableName= 'Qw', key= 'Qw', unitConv='e-6')
        writeVariable(row, variableName= 'ro', key= 'ro', unitConv='e-3')
        writeVariable(row, variableName= 'H', key= 'H')
        writeVariable(row, variableName= 'tan(α)', key= 'tan(α)')
        writeVariable(row, variableName= 'Iw', key= 'Iw', unitConv='e-6')
        writeVariable(row, variableName= 'zA', key= 'zA', unitConv='e-3')
        writeVariable(row, variableName= 'zB', key= 'zB', unitConv='e-3')
        writeVariable(row, variableName= 'zC', key= 'zC', unitConv='e-3')
        writeVariable(row, variableName= 'wA', key= 'wA', unitConv='e-3')
        writeVariable(row, variableName= 'wB', key= 'wB', unitConv='e-3')
        writeVariable(row, variableName= 'wC', key= 'wC', unitConv='e-3')
        writeVariable(row, variableName= 'SwA', key= 'SwA', unitConv='e-6')
        writeVariable(row, variableName= 'SwB', key= 'SwB', unitConv='e-6')
        writeVariable(row, variableName= 'SwC', key= 'SwC', unitConv='e-6')
        writeVariable(row, variableName= 'SzA', key= 'SzA', unitConv='e-6')
        writeVariable(row, variableName= 'SzB', key= 'SzB', unitConv='e-6')
        writeVariable(row, variableName= 'SzC', key= 'SzC', unitConv='e-6')
        writeVariable(row, variableName= 'rts', key= 'rts', unitConv='e-3')
        writeVariable(row, variableName= 'ho', key= 'ho', unitConv='e-3')
        writeVariable(row, variableName= 'PA', key= 'PA', unitConv='e-3')
        writeVariable(row, variableName= 'PA2', key= 'PA2', unitConv='e-3')
        writeVariable(row, variableName= 'PB', key= 'PB', unitConv='e-3')
        writeVariable(row, variableName= 'PC', key= 'PC', unitConv='e-3')
        writeVariable(row, variableName= 'PD', key= 'PD', unitConv='e-3')
        writeVariable(row, variableName= 'T', key= 'T', unitConv='e-3')
        writeVariable(row, variableName= 'WGi', key= 'WGi', unitConv='e-3')
        writeVariable(row, variableName= 'WGo', key= 'WGo', unitConv='e-3')
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
