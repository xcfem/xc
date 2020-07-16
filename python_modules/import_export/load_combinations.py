# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

'''Import/export load combinations.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

import pyexcel as pe

def to_str(s):
    try:
        return str(s)
    except ValueError:
        return ''
def to_int(s):
    try:
        return int(s)
    except ValueError:
        return None
def to_float(s):
    try:
        return float(s)
    except ValueError:
        return None

def import_load_combinations_from_sheet(pyExcelBook, sheetName, nameColumsRow, combNameColumn,actionLabels):
    ''' Import load combinations from an spreadsheet sheet

    :param pyExcelBook: spreadsheet book to import from.
    :param sheetName: name of the seet to import from.
    :param nameColumnsRow: row number of the column names.
    :param combNameColumn: label of the column that contains the 
                           combination name.
    :param actionLabels: labels of the actions.
    '''
    retval= dict()
    uls_sheet= pyExcelBook[sheetName]
    uls_sheet.name_columns_by_row(1)
    records = uls_sheet.to_records()
    for record in records:
        keys = sorted(record.keys())
        combName= to_str(record[combNameColumn])
        combExpr= ''
        if(len(combName)>0):
            for a in actionLabels:
                coef= to_float(record[a])
                if(coef):
                    if(coef!= 0):
                        if(len(combExpr)>0):
                            combExpr+= '+'
                        combExpr+= str(coef)+'*'+a
            if(len(combExpr)>0):
                retval[combName]= combExpr
    return retval
    
def import_load_combinations_from_book(fileName, sheetNames, nameColumsRow, combNameColumn,actionLabels):
    ''' Import load combinations from an spreadsheet sheet

    :param fileName: spreadsheet file name import from.
    :param sheetNames: names of the seets to import from.
    :param nameColumnsRow: row number of the column names.
    :param combNameColumn: label of the column that contains the 
                           combination name.
    :param actionLabels: labels of the actions.
    '''
    retval= dict()
    book= pe.get_book(file_name= fileName)
    for name in sheetNames:
        retval[name]= import_load_combinations_from_sheet(book, name, nameColumsRow, combNameColumn, actionLabels)
    return retval
