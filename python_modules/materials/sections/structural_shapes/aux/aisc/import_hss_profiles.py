# -*- coding: utf-8 -*-
''' Import HSS profiles from AISC database version 15.0'''
from __future__ import print_function
import openpyxl
from misc_utils import log_messages as lmsg
from openpyxl.utils.cell import coordinate_from_string, column_index_from_string
import unicodedata

sheet_columns= dict()
sheet_columns['W']= 'CI'
sheet_columns['A']= 'CJ'
sheet_columns['d']= 'CK'
sheet_columns['ddet']= 'CL'
sheet_columns['Ht']= 'CM'
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
sheet_columns['x']= 'DF'
sheet_columns['y']= 'DG'
sheet_columns['eo']= 'DH'
sheet_columns['xp']= 'DI'
sheet_columns['yp']= 'DJ'
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
sheet_columns['C']= 'ED'
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

fNameOut= 'dict_aisc_hss_shapes.py'
dictName= 'HSS'
out= open(fNameOut,mode='w')

# Give the location of the file
loc= ('./aisc-shapes-database-v15.0.xlsx')
wb = openpyxl.load_workbook(loc)
sheet= wb['Database v15.0']

nameColumn= 'B'
weightColumn= 'CI'
for row in sheet.iter_rows():
    shapetype= row[0].value
    col= column_index_from_string(sheet_columns['Ht'])-1
    circular= isinstance(row[col].value,unicode)
    if(shapetype=='HSS' and (not circular)):
        col= column_index_from_string('B')
        name= row[col].value
        out.write(dictName+"['" + name + "']= ")
        out.write("{")
        out.write("\'nmb\':\'" + name +"\', ")
        col= column_index_from_string(sheet_columns['W'])-1
        out.write("\'P\':" + str(row[col].value)+", ")
        col= column_index_from_string(sheet_columns['A'])-1
        out.write("\'A\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['Ht'])-1
        out.write("\'h\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['h'])-1
        out.write("\'h_flat\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['B'])-1
        out.write("\'b\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['b'])-1
        out.write("\'b_flat\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['tnom'])-1
        out.write("\'tnom\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['tdes'])-1
        out.write("\'t\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['b/tdes'])-1
        out.write("\'bSlendernessRatio\':" + str(row[col].value)+", ")
        col= column_index_from_string(sheet_columns['h/tdes'])-1
        out.write("\'hSlendernessRatio\':" + str(row[col].value)+", ")
        col= column_index_from_string(sheet_columns['Ix'])-1
        out.write("\'Iz\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['Zx'])-1
        out.write("\'Wzpl\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['Sx'])-1
        out.write("\'Wzel\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['rx'])-1
        out.write("\'iz\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['Iy'])-1
        out.write("\'Iy\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['Zy'])-1
        out.write("\'Wypl\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['Sy'])-1
        out.write("\'Wyel\':" + str(row[col].value)+"e-6, ")
        col= column_index_from_string(sheet_columns['ry'])-1
        out.write("\'iy\':" + str(row[col].value)+"e-3, ")
        col= column_index_from_string(sheet_columns['J'])-1
        out.write("\'It\':" + str(row[col].value)+"e-9, ")
        col= column_index_from_string(sheet_columns['C'])-1
        out.write("\'C\':" + str(row[col].value)+"e-9, ")
        out.write("\'E\': 210000e6, ")
        out.write("\'nu\': 0.3")
        out.write(" }\n")

out.close()        
