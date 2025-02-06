# -*- coding: utf-8 -*-
''' Import/export internal forces data from and to a CSV file.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

# import openpyxl
import json
import csv

csv_header= ['load_combination', 'element', 'element_type', 'position', 'N', 'Vy', 'Vz', 'T', 'My' , 'Mz']

def get_internal_forces_dict_rows(internal_forces_dict, unitConversionFactor= 1.0):
    ''' Get a list of rows containing the internal forces in the given 
        dictionary.

    :param internal_forces_dict: Python dictionary containing the internal
           forces data.
    :param unitConversionFactor: factor to apply to the internal forces to get
                                 them in the desired unit.
    '''
    retval= list()
    for combName in internal_forces_dict: # for each load combination.
        element_data= internal_forces_dict[combName]
        for elementId in element_data: # for each element.
            elementTag= int(elementId)
            elementResults= element_data[elementId]
            elementType= elementResults['type']
            elementInternalForces= elementResults['internalForces']
            for posId in elementInternalForces: # for each position in the element.
                posTag= int(posId)
                posInternalForces= elementInternalForces[posId]
                N= posInternalForces['N']*unitConversionFactor
                Vy= posInternalForces['Vy']*unitConversionFactor
                Vz= posInternalForces['Vz']*unitConversionFactor
                T= posInternalForces['T']*unitConversionFactor
                My= posInternalForces['My']*unitConversionFactor
                Mz= posInternalForces['Mz']*unitConversionFactor
                retval.append([combName, elementTag, elementType, posTag, N, Vy, Vz, T, My, Mz])
    return retval
        
def internal_forces_dict_to_csv(internal_forces_dict, csvFileName, unitConversionFactor= 1.0):
    ''' Write the given internal forces data into a CSV file.

    :param internal_forces_dict: Python dictionary containing the internal
           forces data.
    :param csvFileName: CSV file name.
    :param unitConversionFactor: factor to apply to the internal forces to get
                                 them in the desired unit.
    '''
    rows= list([csv_header])+get_internal_forces_dict_rows(internal_forces_dict, unitConversionFactor= unitConversionFactor)
    with open(csvFileName, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        for row in rows:
            csvwriter.writerow(row)
        

def json_to_csv(jsonFileName:str, csvFileName:str, unitConversionFactor= 1.0):
    ''' Read the internal forces data from a JSON file and writes them in a CSV
        file.

    :param jsonFileName: name of the JSON file.
    :param csvFileName: name of the CSV file.
    :param unitConversionFactor: factor to apply to the internal forces to get
                                 them in the desired unit.
    '''
    with open(jsonFileName, 'r') as f:
        internal_forces_dict = json.load(f)
    internal_forces_dict_to_csv(internal_forces_dict, csvFileName, unitConversionFactor= unitConversionFactor)

def get_csv_rows(csvFileName:str):
    ''' Read the internal forces data from a CSV file and return them in a list
        of rows.

    :param csvFileName: name of the CSV file.
    '''
    retval= list()
    with open(csvFileName, 'r') as csvfile:
        csvreader = csv.reader(csvfile)
        for row in csvreader:
            retval.append(row)
    return retval
    
def rows_to_dict(rows:list, unitConversionFactor= 1.0):
    ''' Read the internal forces data from the given rows and return them in a 
        Python dictionary.

    :param csvFileName: name of the CSV file.
    :param unitConversionFactor: factor to apply to the internal forces to get
                                 them in the desired unit.
    '''
    headers= rows[0]
    load_combination_column= headers.index('load_combination')
    element_column= headers.index('element')
    element_type_column= headers.index('element_type')
    position_column= headers.index('position')
    N_column= headers.index('N')
    Vy_column= headers.index('Vy')
    Vz_column= headers.index('Vz')
    T_column= headers.index('T')
    My_column= headers.index('My')
    Mz_column= headers.index('Mz')
    retval= dict()
    for row in rows[1:]:
        load_combination= row[load_combination_column]
        element= int(row[element_column])
        element_type= row[element_type_column]
        position= int(row[position_column])
        N= float(row[N_column])*unitConversionFactor
        Vy= float(row[Vy_column])*unitConversionFactor
        Vz= float(row[Vz_column])*unitConversionFactor
        T= float(row[T_column])*unitConversionFactor
        My= float(row[My_column])*unitConversionFactor
        Mz= float(row[Mz_column])*unitConversionFactor
        # Create dictionary tree.
        if(load_combination not in retval):
            retval[load_combination]= dict()
        load_combination_data= retval[load_combination]
        if(element not in load_combination_data):
            load_combination_data[element]= dict()
        element_data= load_combination_data[element]
        element_data['type']= element_type
        if('internalForces' not in element_data):
            element_data['internalForces']= dict()
        element_internal_forces= element_data['internalForces']
        if(position not in element_internal_forces):
            element_internal_forces[position]= dict()
        position_data= element_internal_forces[position]
        # Populate the dictionary.
        position_data.update({'N':N, 'Vy':Vy, 'Vz':Vz, 'T':T, 'My':My, 'Mz':Mz})
    return retval

def csv_to_json(csvFileName:str, jsonFileName:str, unitConversionFactor= 1.0):
    ''' Read the internal forces data from a CSV file and writes them in a JSON
        file.

    :param csvFileName: name of the CSV file.
    :param jsonFileName: name of the JSON file.
    :param unitConversionFactor: factor to apply to the internal forces to get
                                 them in the desired unit.
    '''
    rows= get_csv_rows(csvFileName)
    tmp= rows_to_dict(rows= rows, unitConversionFactor= unitConversionFactor)
    with open(jsonFileName, 'w') as jsonFile:
        json.dump(tmp, jsonFile)

        

    
