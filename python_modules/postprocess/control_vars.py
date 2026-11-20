# -*- coding: utf-8 -*-
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WHICH MUST DISSAPEAR.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"


import os
import sys
import json
import scipy
from misc_utils import log_messages as lmsg
from postprocess.reports import common_formats as fmt
from postprocess import extrapolate_elem_attr as ext
from materials import stresses
from postprocess.control_variables import basic_control_variables as bcv

__all__= ['AxialForceControlVars', 'BiaxialBendingControlVars', 'BiaxialBendingStrengthControlVars', 'CFN', 'CFNMy', 'CFNMyMz', 'CFVy', 'ControlVarsBase', 'CrackControlBaseVars', 'CrackControlVars', 'FatigueControlBaseVars', 'FatigueControlVars', 'N', 'NMy', 'NMyMz', 'RCCrackControlVars', 'RCCrackStraightControlVars', 'RCShearControlVars', 'SIATypeRCShearControlVars', 'ShVy', 'ShearYControlVars', 'SteelShapeUniaxialBendingControlVars', 'SteelShapeBiaxialBendingControlVars', 'UniaxialBendingControlVars', 'VonMisesControlVars', 'RCBucklingControlVars', 'SteelBucklingControlVars', 'StrutAndTieControlVars', 'extrapolate_control_var', 'getControlVarImportModuleStr', 'get_diagram_direction', 'get_element_internal_force_component_data', 'write_control_vars_from_elements', 'write_control_vars_from_elements_for_ansys', 'write_control_vars_from_phantom_elements']

ControlVarsBase= bcv.ControlVarsBase
N= bcv.N
NMy= bcv.NMy
NMyMz= bcv.NMyMz
CFN= bcv.CFN
CFNMy= bcv.CFNMy
ShVy= bcv.ShVy
CFVy= bcv.CFVy
ShearYControlVars= bcv.ShearYControlVars
UniaxialBendingControlVars= bcv.UniaxialBendingControlVars
CFNMyMz= bcv.CFNMyMz
AxialForceControlVars= bcv.AxialForceControlVars
BiaxialBendingControlVars= bcv.BiaxialBendingControlVars
PlaneStressControlVars= bcv.PlaneStressControlVars
ShellControlVars= bcv.ShellControlVars
BucklingControlVarsBase= bcv.BucklingControlVarsBase
from postprocess.control_variables.ec2_annex_f_control_vars import Ec2InPlaneStressControlVars

from postprocess.control_variables.von_mises_control_vars import VonMisesControlVars

from postprocess.control_variables.rc_control_vars import *
from postprocess.control_variables.strut_and_tie_control_vars import *
from postprocess.control_variables.steel_control_vars import *

def get_diagram_direction(elem, component, defaultDirection):
    '''Return the direction vector to represent the diagram over the element

    :param elem: element to deal with.
    :param component: component to show. 
           Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
           'T'
    :param defaultDirection: default direction of the diagram (J: element local
                             j vector or K: element local K vector).
    '''
    if(defaultDirection=='J'): # local J vector.
        elemVDir= elem.getJVector3d(True) #initialGeometry= True
    else: # local K vector.
        elemVDir= elem.getKVector3d(True) #initialGeometry= True
    if((component == 'Qy') or (component == 'Vy')):
        elemVDir= elem.getJVector3d(True) # initialGeometry= True 
    elif((component == 'Qz') or (component == 'Vz')):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'My'):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'Mz'):
        elemVDir= -elem.getJVector3d(True) # initialGeometry= True 
    return elemVDir
    

def get_element_internal_force_component_data(elem, component, defaultDirection):
    '''Return the data to use to represent the diagram over the element

    :param elem: element to deal with.
    :param component: component to show. 
           Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
           'T'
    :param defaultDirection: default direction of the diagram (J: element local
                             j vector or K: element local K vector). If None
                             ignore the direction information of the diagram.
    '''
    # default values
    elemVDir= None
    if(defaultDirection):
        elemVDir= get_diagram_direction(elem, component, defaultDirection)
    value1= 0.0
    value2= 0.0
    values= elem.getValuesAtNodes(component, False)
    if(len(values)>1): # component found.
        value1= values[0]; value2= values[1]
    return [elemVDir,value1,value2]


def read_control_vars(preprocessor, inputFileName):
    ''' Read control var data from the input file an put them as properties
        of the model elements and/or nodes.

    :param preprocessor: pre-processor for the finite element model.
    :param inputFileName: name of the input file containing the data.
    :returns: number of properties read.
    '''
    retval= 0
    try:
        with open(inputFileName) as f:
            dataDict= json.load(f)
    except IOError:
        methodName= sys._getframe(0).f_code.co_name
        errorMsg= "; can't read from file: "+str(inputFileName)
        lmsg.error(methodName+errorMsg)
        return retval
    if('elementData' in dataDict): # Control variables on elements.
        elementData= dataDict['elementData']
        elementHandler= preprocessor.getElementHandler
        for controlVarName in elementData: # iterate on limit states.
            propName= controlVarName+'Sect'
            for eKey in elementData[controlVarName]: # iterate on elements.
                elementTag= int(eKey)
                element= elementHandler.getElement(elementTag)
                elementControlVars= elementData[controlVarName][eKey]
                for propKey in elementControlVars: # iterate on element control vars.
                    expression= elementControlVars[propKey]
                    propValue= eval(expression)
                    sectPropName= propName+propKey
                    element.setProp(sectPropName, propValue)
                    retval+= 1
    if('nodeData' in dataDict): # Control variables on nodes.
        nodeData= dataDict['nodeData']
        nodeHandler= preprocessor.getNodeHandler
        for controlVarName in elementData: # iterate on limit states.
            propName= controlVarName+'Sect'
            for eKey in nodeData: # iterate on nodes.
                nodeTag= int(eKey)
                node= nodeHandler.getNode(nodeTag)
                nodeControlVars= nodeData[eKey]
                for propKey in nodeControlVars: # iterate on node control vars.
                    propValue= eval(nodeControlVars[propKey])
                    node.setProp(propName+propKey, propValue)
                    retval+= 1
    return retval

def get_control_vars_dict(elements, controlVarName, sections):
    '''Return a dictionary with the values of the control variables for each
     of the given elements.

    :param elements: elements to get the control variables from.
    :param controlVarName: name of the control var to populate the dictionary with.
    :param sections: names of the sections of each element.
    '''
    retval= {controlVarName:dict()}
    elementValues= retval[controlVarName]
    for e in elements:
        eTag= e.tag
        if not eTag in elementValues:
            elementValues[eTag]= dict()
        for s in sections:
            if(len(s)>0):
                index= int(s.removeprefix('Sect'))
            else:
                index= 0
            propName= controlVarName+s
            controlVar= e.getProp(propName)        
            elementValues[eTag][index]= controlVar
    return retval            
    
def get_element_data_dict(controlVarsDict, controlVarName):
    ''' Return a dictionary populated with the control variables for each element.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param controlVarName: name of the control var to populate the dictionary 
                           with.
    '''
    elementValues= controlVarsDict[controlVarName]
    retval= {controlVarName:dict()}
    for eTag in elementValues:
        elementControlVars= elementValues[eTag]
        if not eTag in retval[controlVarName]:
            retval[controlVarName][eTag]= dict()
        sz= len(elementControlVars)
        if(sz>1): # Many records for each element.
            for index in elementControlVars:
                controlVar= elementControlVars[index]
                # sectionName= 'Sect'+str(index)
                # propName= controlVarName+sectionName
                retval[controlVarName][eTag][index]= controlVar.getStrConstructor()
        else: # One record for each element.
            index= next(iter(elementControlVars))
            controlVar= elementControlVars[index]
            retval[controlVarName][eTag][index]= controlVar.getStrConstructor()
    return retval

def write_latex_control_vars(outputFile, controlVarsDict):
    ''' Write the values of the control vars in a LaTeX file.

    :param outputFile: output file.
    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    '''
    sectionLines= dict()
    for controlVarName in controlVarsDict:
        elementControlVars= controlVarsDict[controlVarName]
        for eTag in elementControlVars:
            elementControlVars= elementControlVars[eTag]
            for index in elementControlVars:
                controlVar= elementControlVars[index]
                if not index in sectionLines:
                    sectionLines[index]= list()
                outStr= controlVar.getLaTeXString(eTag,1e-3)
                sectionLines[index].append(outStr)
        for index in sectionLines:
            header= 'Section '+str(index)+'\n'
            outputFile.write(header)
            for string in sectionLines[index]:
                outputFile.write(string)
            
def get_capacity_factors_from_control_vars(controlVarsDict):
    ''' Return the capacity factors from the given control vars.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    '''
    retval= dict()
    for controlVarName in controlVarsDict:
        retval[controlVarName]= dict()
        elementValues= controlVarsDict[controlVarName]
        for eTag in elementValues:
            elementControlVars= elementValues[eTag]
            for index in elementControlVars:
                controlVar= elementControlVars[index]
                if not index in retval[controlVarName]:
                    retval[controlVarName][index]= list()
                retval[controlVarName][index].append(controlVar.getCF())
    return retval

def convert_json_keys_to_int(obj):
    '''Recursively convert dictionary keys that are integer strings to integers.

    :param obj: dictionary to change whose keys will be converted to integers.
    '''
    if isinstance(obj, dict):
        new_dict = {}
        for key, value in obj.items():
            try:
                new_key = int(key)
            except (ValueError, TypeError):
                # Leave non-integer keys unchanged
                new_key = key
            new_dict[new_key] = convert_json_keys_to_int(value)
        return new_dict

    elif isinstance(obj, list):
        return [convert_json_keys_to_int(item) for item in obj]

    else:
        return obj

def read_existing_control_vars_dict(inputFileName):
    ''' Read the values of the contral variables that are already written
        in a results file.

    :param inputFileName: name of the input file to read from.
    '''
    tmp= dict()
    if(os.path.isfile(inputFileName)):
        try:
            with open(inputFileName, 'r') as f:
               tmp= json.load(f)
        except IOError:
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= "can't read from file: "
            errorMsg+= str(inputFileName)
            errorMsg+= ". Are you sure you need to read previous results?"
            lmsg.error(methodName+errorMsg)
            exit(1)
    else:
        tmp= dict()
    # Convert keys to integers.
    retval= convert_json_keys_to_int(tmp)
    return retval

def update_control_vars_dict(dataDict, newResults):
    ''' Update the given dictionary with the given results.

    :param dataDict: dictionary to update.
    :param newResults: result to update the dictionary with.
    '''
    if('elementData' in dataDict):
        limitStateData= dataDict['elementData']
        for controlVarKey in newResults:
            if(controlVarKey in limitStateData):
                limitStateData[controlVarKey].update(newResults[controlVarKey])
            else:
                limitStateData[controlVarKey]= newResults[controlVarKey]
    else:
        dataDict['elementData']= newResults
    
            
def write_control_vars_from_phantom_elements(controlVarsDict, outputCfg):
    '''Writes to file the control-variable values calculated for
     the RC elements in the phantom model.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (append or not
           the results to a file, generation or not of lists, ...)
    '''
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    controlVarName= outputCfg.controller.limitStateLabel
    dataDict= None
    jsonFileName= outputFileName+'.json'
    if(outputCfg.appendToResFile):
        dataDict= read_existing_control_vars_dict(jsonFileName)
    else:
        dataDict= dict()
    elementDataDict= get_element_data_dict(controlVarsDict= controlVarsDict, controlVarName= controlVarName)
    update_control_vars_dict(dataDict= dataDict, newResults= elementDataDict)

    # Write the dictionary in a JSON file.
    with open(jsonFileName, 'w') as f:
        json.dump(dataDict, f)       

    if outputCfg.listFile:
        if outputCfg.appendToResFile:
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        write_latex_control_vars(outputFile= texOutput, controlVarsDict= controlVarsDict)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF:
        capacityFactors= get_capacity_factors_from_control_vars(controlVarsDict= controlVarsDict)[controlVarName]
        retval= list()
        for key in capacityFactors:
            retval.append(scipy.mean(capacityFactors[key]))
    return retval

def getControlVarImportModuleStr(preprocessor, outputCfg, sections):
    '''Return the string to import the module where the control var is defined.

    :param preprocessor:    preprocessor from FEA model.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    retval= None
    elems= outputCfg.getCalcSetElements(preprocessor) # elements in set 'setCalc'
    controlVarName= outputCfg.controller.limitStateLabel
    if(len(elems)>0):
        e0= elems[0]
        s= sections[0]
        propName= controlVarName+s
        controlVar= e0.getProp(propName)
        retval= controlVar.getModuleImportString()
    else:
        methodName= sys._getframe(0).f_code.co_name
        errorMsg= "; element set is empty."
        lmsg.error(methodName+errorMsg)
    return retval

def write_control_vars_from_elements(preprocessor, controlVarsDict, outputCfg, sections):
    '''Writes to file the control-variable values calculated for elements in set 'setCalc'. 

    :param preprocessor:    preprocessor from FEA model.
    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    controlVarName= outputCfg.controller.limitStateLabel
    dataDict= None
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    jsonFileName= outputFileName+'.json'
    # Get the existing data dictionary if required.
    if(outputCfg.appendToResFile):
        dataDict= read_existing_control_vars_dict(jsonFileName)
    else:
        dataDict= dict()
    # Write results in JSON format.
    importString= getControlVarImportModuleStr(preprocessor, outputCfg, sections)
    if('importStrings' in dataDict):
        importStringsList= dataDict['importStrings']
        if(importString not in importStringsList):
            importStringsList.append(importString)
    else:
        importStringsList=[importString]
    dataDict['importStrings']= importStringsList
    elementDataDict= get_element_data_dict(controlVarsDict= controlVarsDict, controlVarName= controlVarName)
    update_control_vars_dict(dataDict= dataDict, newResults= elementDataDict)
    # for e in elems:
    #     elementDataDict[e.tag]= dict()
    #     for s in sections:
    #         propName= controlVarName+s
    #         controlVar= e.getProp(propName)
    #         elementDataDict[e.tag][propName]= controlVar.getStrConstructor()
    with open(jsonFileName, 'w') as f:
        json.dump(dataDict, f)
    # Write report in LaTeX format.
    if outputCfg.listFile:
        if outputCfg.appendToResFile:
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        write_latex_control_vars(outputFile= texOutput, controlVarsDict= controlVarsDict)
        # for s in sections:
        #     texOutput.write("Section: "+s+"\n")
        #     propName= controlVarName+s
        #     for e in elems:
        #         controlVar= e.getProp(propName)
        #         outStr= controlVar.getLaTeXString(e.tag,1e-3)
        #         texOutput.write(outStr)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF:
        capacityFactors= get_capacity_factors_from_control_vars(controlVarsDict= controlVarsDict)[controlVarName]
        retval= list()
        for key in capacityFactors:
            retval.append(scipy.mean(capacityFactors[key]))
    return retval


def write_control_vars_from_elements_for_ansys(preprocessor, outputCfg, sectionName1, sectionName2):
    '''
    :param preprocessor: preprocessor for the FE problem.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sectioName1: name of the first section to write the output for.
    :param sectioName2: name of the second section to write the output for.
    '''
    def odd(v):
        if v & 1:
            return True
        else:
            return False
    controlVarName= outputCfg.controller.limitStateLabel
    texOutput1= open("/tmp/texOutput1.tmp","w")
    texOutput1.write("Section 1\n")
    texOutput2= open("/tmp/texOutput2.tmp","w")
    texOutput2.write("Section 2\n")
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    ansysOutput1= open(outputFileName+".mac","w")
    ansysOutput2= open(outputFileName+"esf.mac","w")
    #printCabeceraListadoCapacityFactor("texOutput1","1 ("+ sectionName1 +")")
    #printCabeceraListadoCapacityFactor("texOutput2","2 ("+ sectionName2 +")")
    fcs1= [] #Capacity factors at section 1.
    fcs2= [] #Capacity factors at section 2.
    elems= preprocessor.getSets["total"].elements
    for e in elems:
        eTag= e.getProp("idElem")
        controlVar= e.getProp(controlVarName)
        outStr= controlVar.getLaTeXString(eTag,1e-3)
        if(odd(e.tag)):
            fcs1.append(controlVar.getCF())
            texOutput1.write(outStr)
            ansOut= controlVar.getAnsysStrings(eTag,'1',1e-3)
            for s in ansOut:
                ansysOutput1.write(s)
        else:
            fcs2.append(controlVar.getCF())
            texOutput2.write(outStr)
            ansOut= controlVar.getAnsysStrings(eTag,'2',1e-3)
            for s in ansOut:
                ansysOutput2.write(s)
    #printCierreListadoCapacityFactor("texOutput1")
    #printCierreListadoCapacityFactor("texOutput2")
    texOutput1.close()
    texOutput2.close()
    ansysOutput1.close()
    ansysOutput2.close()
    os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    # os.system("rm -f "+"/tmp/acciones.xci")
    # os.system("rm -f "+"/tmp/cargas.xci")
    # os.system("rm -f "+"/tmp/elems.xci")
    os.system("rm -f "+"/tmp/texOutput1.tmp")
    os.system("rm -f "+"/tmp/texOutput2.tmp")
    retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
    return retval

def get_control_var_values_from_elements(elements, propName, argument):
    ''' Return a dictionary containing the values of the given control var
        in the given element set. The dictiorary key is the element tag.

     :param element: sequence of elements to retrieve the values for.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    for e in elements:
        if(e.hasProp(propName)):
            controlVar= e.getProp(propName)
            value= controlVar(argument)
        else:
            value= None
        retval[e.tag]= value
    return retval

def get_maximum_control_var_value_on_surfaces(xcSet, propName, argument):
    ''' Compute the maximum of a control var value for each surface of the
        given set.

     :param xcSet: set of surfaces
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    sect1CV= propName+'Sect1'
    sect2CV= propName+'Sect2'
    for s in xcSet.surfaces:
        cvDict1= get_control_var_values_from_elements(elements= s.elements, propName= sect1CV, argument= argument)
        cvDict2= get_control_var_values_from_elements(elements= s.elements, propName= sect2CV, argument= argument)
        listCV1= list()
        listCV2= list()
        for key in cvDict1:
            cv1= cvDict1[key]
            if(cv1 is not None):
                listCV1.append(cv1)
            cv2= cvDict2[key]
            if(cv2 is not None):
                listCV2.append(cv2)
        retval[s.tag]= [max(listCV1), max(listCV2)]
    return retval

def get_maximum_control_var_value_on_lines(xcSet, propName, argument):
    ''' Compute the maximum of a control var value for each line of the
        given set.

     :param xcSet: set of lines
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    sect1CV= propName+'Sect1'
    sect2CV= propName+'Sect2'
    for l in xcSet.lines:
        cvDict1= get_control_var_values_from_elements(elements= l.elements, propName= sect1CV, argument= argument)
        cvDict2= get_control_var_values_from_elements(elements= l.elements, propName= sect2CV, argument= argument)
        listCV1= list()
        listCV2= list()
        for key in cvDict1:
            cv1= cvDict1[key]
            if(cv1 is not None):
                listCV1.append(cv1)
            cv2= cvDict2[key]
            if(cv2 is not None):
                listCV2.append(cv2)
        retval[l.tag]= [max(listCV1), max(listCV2)]
    return retval

def extrapolate_control_var_from_elements(preprocessor, elements, propName, argument, initialValue= 0.0):
    '''Extrapolates element's control var values to the nodes.

     :param preprocessor: preprocessor of the FE problem.
     :param elements: sequence of elements to retrieve the control variables for.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
     :param initialValue: initial value for the prop defined at the nodes.
    '''
    nodePropName= propName+'_'+argument
    touchedNodes= ext.create_attribute_at_nodes(elements,nodePropName,initialValue)
    #Calculate totals.
    for e in elements:
        elemNodes= e.getNodes
        if(e.hasProp(propName)):
            controlVar= e.getProp(propName)
        else: # Silently ignore this value (LP 26/07/2022).
            controlVar= None
        for n in elemNodes:
            if(controlVar):
                value= controlVar(argument)
                oldValue= n.getProp(nodePropName)
                n.setProp(nodePropName,oldValue+value)
    #Divide by number of elements in the set that touch the node.
    for tag in touchedNodes: # Nodes touched by the elements in the set.
        n= preprocessor.getNodeHandler.getNode(tag)
        denom= float(touchedNodes[tag])
        newValue= n.getProp(nodePropName)/denom
        n.setProp(nodePropName,newValue)
    return nodePropName

def extrapolate_control_var(elemSet, propName, argument, initialValue= 0.0):
    '''Extrapolates element's control var values to the nodes.

     :param elemSet: set of elements.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
     :param initialValue: initial value for the prop defined at the nodes.
    '''
    elemSet.fillDownwards()
    eSet= elemSet.elements
    return extrapolate_control_var_from_elements(preprocessor= elemSet.getPreprocessor, elements= eSet, propName= propName, argument= argument, initialValue= initialValue)


