# -*- coding: utf-8 -*-
''' Read data from a spreadsheet file.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import openpyxl
from misc_utils import log_messages as lmsg
import sys

def is_int(string: str) -> bool:
    # Try to convert the string to a float
    # If the conversion is successful, return True
    try:
        int(string)
        return True
    # If a ValueError is thrown, it means the conversion was not successful
    # This happens when the string contains non-numeric characters
    except ValueError:
        return False
    
def is_float(string: str) -> bool:
    # Try to convert the string to a float
    # If the conversion is successful, return True
    try:
        float(string)
        return True
    # If a ValueError is thrown, it means the conversion was not successful
    # This happens when the string contains non-numeric characters
    except ValueError:
        return False

def is_number(value):
    ''' Return true if the given value can be converted to a number.

    :param value: value to test.
    '''
    retval= False
    if(is_int(value)):
        retval= True
    elif(is_float(value)):
        retval= True
    return retval
    
def value_to_number(value):
    ''' Convert the given value to number if possible, otherwise return the
        value itself.

    :param value: value to convert.
    '''
    retval= value
    is_str= isinstance(value, str)
    if(is_str):
        if(is_int(value)):
            retval= int(value)
        elif(is_float(value)):
            retval= float(value)
    return retval
    
    
def read_shapes_from_spreadsheet(namePrefix, columnOrder, columnUnits, spreadsheetFileName, E= 210000e6, nu= 0.3, post_processing_function= None, dataOnly= False):
    ''' Read steel shapes data from a spreadsheet in xlsx format.

    :param namePrefix: prefix of the names in the spreadsheet that will be
                       imported (for example 'HE' for the HEB steel shapes).
    :param columnOrder: list of the data to be imported in the spreadsheet
                        ordered by its column number (i.e. if the mass per
                        meter is on the third column of the spreadsheet, then
                        the 'P' field name need to be the third item in the
                        given list.
    :param columnUnits: dictionary containing the conversion factors to apply
                        to the values in the spreadsheet in order to express
                        the corresponding quantitiy in the desired units. For
                        example, if the mass per meter is expressed as pound
                        per feet in the spreadsheet and we want to express it
                        in kilograms per meter then we need to put the pair
                        'P':1.48816394 in the dictionary. The key ('P')
                        indicates the corresponding data (weight per meter) and
                        the value (1.48816394) is the factor that will multiply
                        the lb/ft on the spreadsheet to obtain the kg/m in the
                        returned dictionary. If a key is missing the unity (1.0)
                        factor is assumed for that key.
    :param spreadsheetFileName: name of the file to read the data from.
    :param E: elastic modulus for the shape material.
    :param nu: Poisson coefficiente for the shape material.
    :param post_processing_function: function that will post-process the data
                                     (remove undesired characters on some field
                                     and things like that).
    :param dataOnly: read last computed values for cells that contain a formula.
    '''
    wb_obj= openpyxl.load_workbook(spreadsheetFileName, data_only= dataOnly)
    
    columnKeys= dict()
    for count, key in enumerate(columnOrder):
        if(len(key)>0):
            columnKeys[key]= count
    retval= dict() # Dictionary with all the shapes.
    for sheet in wb_obj.worksheets:
        rows= sheet.rows
        for r in rows:
            if(len(r)>0): # if not empty.
                value= r[columnKeys['nmb']].value
                if(value):
                    name= str(value).replace(' ','')
                    if(name.startswith(namePrefix)):
                        shapeRecord= dict()
                        for key in columnOrder:
                            if(len(key)>0):
                                column= columnKeys[key]
                                shapeRecord[key]= r[column].value
                        shapeRecord['E']= 210000e6 # To deprecate
                        shapeRecord['nu']= 0.3 # To deprecate
                        retval[name]= shapeRecord
    if(len(retval)==0):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+"; no records found for name prefix: '"+str(namePrefix)+"'. Returning empty dictionary")
        
    # Convert to the desired units.
    for shapeName in retval:
        shapeRecord= retval[shapeName]
        shapeRecord['nmb']= shapeName # set name
        for key in shapeRecord:
            if key in columnUnits: # convert to the desired units.
                factor= columnUnits[key]
                value= shapeRecord[key]
                if(isinstance(value, str)):
                    value= value.replace(',','') # thousands separator.
                value= float(value)
                newValue= value*factor
                shapeRecord[key]= newValue
            else: # convert to number if possible.
                value= shapeRecord[key]
                if isinstance(value, str):
                    if(',' in value): # thousands separation.
                        value= value.replace(',','')
                    if(is_number(value)):
                        shapeRecord[key]= value_to_number(value)
    if(len(retval)>0):
        if(post_processing_function):
            retval= post_processing_function(retval)
    if(retval is None):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; post processing function returned None (must return the processed dictionary instead).')
        exit(1)
        
    return retval

