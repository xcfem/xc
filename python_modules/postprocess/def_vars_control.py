# -*- coding: utf-8 -*-
'''
   THIS PROPERTIES MUST BE REPLACED BY THE CLASSES 
   DEFINED IN control_vars.py 
   THIS FILE MUST DISSAPEAR.'''

from __future__ import division
from __future__ import print_function


__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from misc_utils import log_messages as lmsg
from model import model_inquiry
from typing import Iterable



#       ___  _         _                           _      
#      |   \(_)____ __| |__ _ __ ___ _ __  ___ _ _| |_ ___
#      | |) | (_-< '_ \ / _` / _/ -_) '  \/ -_) ' \  _(_-<
#      |___/|_/__/ .__/_\__,_\__\___|_|_|_\___|_||_\__/__/
#                |_|                                      
# Displacements.

def def_var_control_mov(obj, code):
    ''' Define variables for movement control.

    :param obj: obj whose movement will be controlled.
    :param code: movement identifier.
    '''
    if(not obj.hasProp('span')):
        warningMsg= '; span property not defined for: '
        warningMsg+= str(obj.tag) + ' object.'
        funcName= sys._getframe(0).f_code.co_name
        lmsg.warning(funcName+warningMsg)
    obj.setProp(code+'Max',0.0)
    obj.setProp('Comb'+code+'Max',"")
    obj.setProp(code+'Min',0.0)
    obj.setProp('Comb'+code+'Min',"")

def def_vars_control_movs(nodes: Iterable, flags: Iterable):
    ''' Define variables for movement control.

    :param nodes: nodes whose movement will be controlled.
    :param flags: movement identifiers.
    '''
    tags= []
    for n in nodes:
        tags.append(n.tag)
        for f in flags:
            def_var_control_mov(n,f)
    return tags

def def_vars_control_mov_modulus(nodes: Iterable):
    ''' Define variables for control of the movement modulus.

    :param nodes: nodes whose movement will be controlled.
    '''
    tags= []
    for n in nodes:
        if(not n.hasProp('span')):
            warningMsg= '; span property not defined for node: '
            warningMsg+= str(n.tag) + '.'
            funcName= sys._getframe(0).f_code.co_name
            lmsg.warning(funcName+warningMsg)
        tags.append(n.tag)
        n.setProp("dispMax",0.0)
        n.setProp("CombDispMax","")
    return tags

def def_envelope_vars(elems: Iterable, varNames: Iterable, initV= 6.023e23, nNodes= 2):
    ''' Define variables for generalizez stress control.

    :param elems: elements to define the envelope variables for.
    :param varNames: variable names.
    :param initV: initial value.
    :param nNodes: number of nodes for each element in elems.
    '''
    for e in elems:
        for vn in varNames:
            if(nNodes>1):
                # [back node value, front node value, etc.]
                e.setProp(vn+'+',[-initV]*nNodes) # Positive value of envelope
                e.setProp('comb_'+vn+'+',[None]*nNodes) # Combination that gives
                                                        # the maximum value.
                e.setProp(vn+'-',[initV]*nNodes) # Negative value of envelope
                e.setProp('comb_'+vn+'-',[None]*nNodes) # Combination that gives
                                                        # the minimum value.
            else:
                e.setProp(vn+'+',-initV) #Positive value of envelope
                e.setProp('comb_'+vn+'+', None) # Combination that gives
                                                # the maximum value.
                e.setProp(vn+'-',initV) # Negative value of envelope
                e.setProp('comb_'+vn+'-', None) # Combination that gives
                                                # the minimum value.


#       ___             _   _             
#      | _ \___ __ _ __| |_(_)___ _ _  ___
#      |   / -_) _` / _|  _| / _ \ ' \(_-<
#      |_|_\___\__,_\__|\__|_\___/_||_/__/
# Reactions.                                                       
def get_reaction_var_names(dim, numDOFs):
    ''' Return the variable names for the components of the reaction vectors.

    :param dim: dimension of the space (1, 2 or 3).
    :param numDOFs: number of degrees of freedom.
    '''
    if(numDOFs==1):
        retval= ['Rx']
    elif(numDOFs==2):
        retval= ['Rx', 'Ry']
    elif(numDOFs==3):
        if(dim==2):
            retval= ['Rx', 'Ry', 'RRz']
        elif(dim==3):
            retval= ['Rx', 'Ry', 'Rz']
        else:
            methodName= sys._getframe(0).f_code.co_name
            errMsg= methodName+'; not implemented for '+str(numDOFs)
            errMsg+= ' degrees of freedom in a '+str(dim)
            errMsg+= '-dimensional space.'
            lmsg.error(errMsg)
    elif(numDOFs):
        retval= ['Rx', 'Ry', 'Rz', 'RRx', 'RRy', 'RRz']
    else:
        retval= list()
    return retval


def def_reactions_envelope_vars(nodes):
    ''' Define variables to store the maximum and minimum on each DOF for the
        given nodes.

    :param nodes: nodes to add the reactions envelope variables into.
    '''
    if(len(nodes)>0):
        numDOFs= nodes[0].getNumberDOF
        dim= nodes[0].dim
        varNames= get_reaction_var_names(dim= dim, numDOFs= numDOFs)
        if(varNames):
            def_envelope_vars(nodes, varNames= varNames, nNodes= 1)
        else:
            methodName= sys._getframe(0).f_code.co_name
            errMsg= methodName+'; not implemented for '+str(numDOFs)
            errMsg+= ' degrees of freedom in a '+str(dim)
            errMsg+= '-dimensional space.'
            lmsg.error(errMsg)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; no nodes to process.')
    return varNames

def update_reactions_envelope(nodes):
    ''' Update the the maximum and minimum reactino on each DOF for the
        given nodes.

    :param nodes: nodes to update the reactions for.
    '''
    if(len(nodes)>0):
        numDOFs= nodes[0].getNumberDOF
        dim= nodes[0].dim
        varNames= get_reaction_var_names(dim= dim, numDOFs= numDOFs)
        if(varNames):
            for i, compName in enumerate(varNames):
                for n in nodes:
                    currentValue= n.getReaction[i]
                    maxValueLabel= compName+'+'
                    maxValue= n.getProp(maxValueLabel)
                    if(currentValue>maxValue):
                        maxValue= currentValue
                        n.setProp(maxValueLabel, maxValue)
                    minValueLabel= compName+'-'
                    minValue= n.getProp(minValueLabel)
                    if(currentValue<minValue):
                        minValue= currentValue
                        n.setProp(minValueLabel, minValue)
        else:
            methodName= sys._getframe(0).f_code.co_name
            errMsg= methodName+'; not implemented for '+str(numDOFs)
            errMsg+= ' degrees of freedom in a '+str(dim)
            errMsg+= '-dimensional space.'
            lmsg.error(errMsg)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; no nodes to process.')
    return varNames

#       ___     _                     _ 
#      |_ _|_ _| |_ ___ _ _ _ _  __ _| |
#       | || ' \  _/ -_) '_| ' \/ _` | |
#      |___|_||_\__\___|_| |_||_\__,_|_|
#       / _|___ _ _ __ ___ ___          
#      |  _/ _ \ '_/ _/ -_|_-<          
#      |_| \___/_| \__\___/__/          
# Internal forces.                                      
    
def def_vars_envelope_internal_forces_truss_elems(elems):
    '''Defines properties to store extreme values of internal forces.

    :param elems: elements whose generalized stresses will be controlled.
    '''
    def_envelope_vars(elems, ['N'])
        
def def_vars_envelope_internal_forces_beam_elems(elems):
    '''Defines properties to store extreme values of internal forces.

    :param elems: elements nodes whose generalized stresses will be controlled.
    '''
    def_envelope_vars(elems, ['N', 'Mz', 'My', 'T', 'Vy','Vz'])

def update_envelope_internal_forces_truss(trussElem, loadCombinationName):
    '''Update values for extreme values of internal forces in 2D elements.

    :param trussElem: finite element to update internal forces.
    :param loadCombinationName: name of the load combinations to which the
                                internal forces on the elements correspond.
    '''
    trussElem.getResistingForce()
    [[N1], [N2]]= model_inquiry.get_values_at_nodes(trussElem, ['N'], False)
    maxN= trussElem.getProp('N+') # [back node value, front node value]
    maxNcomb= trussElem.getProp('comb_N+') # load combinations.
    minN= trussElem.getProp('N-')
    minNcomb= trussElem.getProp('comb_N-') # load combinations.
    if(N1>maxN[0]):
        maxN[0]= N1
        maxNcomb[0]= loadCombinationName
    if(N1<minN[0]):
        minN[0]= N1
        minNcomb[0]= loadCombinationName
    if(N2>maxN[1]):
        maxN[1]= N2
        maxNcomb[1]= loadCombinationName
    if(N2<minN[1]):
        minN[1]= N2
        minNcomb[1]= loadCombinationName
    trussElem.setProp('N+',maxN)
    trussElem.setProp('comb_N+', maxNcomb)
    trussElem.setProp('N-',minN)
    trussElem.setProp('comb_N-', minNcomb)

def update_envelope_internal_forces_beam_elem_2d(beamElem2D, loadCombinationName):
    '''Update values for extreme values of internal forces in 2D elements.

    :param beamElem2D: finite element to update internal forces.
    :param loadCombinationName: name of the load combinations to which the
                                internal forces on the elements correspond.
    '''
    beamElem2D.getResistingForce()
    [[N1, M1, V1], [N2, M2, V2]]= model_inquiry.get_values_at_nodes(beamElem2D, ['N', 'M', 'V'], False)        
    maxN= beamElem2D.getProp('N+') # [back node value, front node value]
    maxNcomb= beamElem2D.getProp('comb_N+') # load combinations.
    maxM= beamElem2D.getProp('Mz+')
    maxMcomb= beamElem2D.getProp('comb_Mz+')
    maxV= beamElem2D.getProp('Vy+')
    maxVcomb= beamElem2D.getProp('comb_Vy+')
    minN= beamElem2D.getProp('N-')
    minNcomb= beamElem2D.getProp('comb_N-') # load combinations.
    minM= beamElem2D.getProp('Mz-')
    minMcomb= beamElem2D.getProp('comb_Mz-')
    minV= beamElem2D.getProp('Vy-')
    minVcomb= beamElem2D.getProp('comb_Vy-')
    # Back node axial load.
    if(N1>maxN[0]):
        maxN[0]= N1
        maxNcomb[0]= loadCombinationName
    if(N1<minN[0]):
        minN[0]= N1
        minNcomb[0]= loadCombinationName
    # Back node bending moment.
    if(M1>maxM[0]):
        maxM[0]= M1
        maxMcomb[0]= loadCombinationName
    if(M1<minM[0]):
        minM[0]= M1
        minMcomb[0]= loadCombinationName
    # Back node shear load.
    if(V1>maxV[0]):
        maxV[0]= V1
        maxVcomb[0]= loadCombinationName
    if(V1<minV[0]):
        minV[0]= V1
        minVcomb[0]= loadCombinationName
    # Front node axial load.
    if(N2>maxN[1]):
        maxN[1]= N2
        maxNcomb[1]= loadCombinationName
    if(N2<minN[1]):
        minN[1]= N2
        minNcomb[1]= loadCombinationName
    # Front node axial load.
    if(M2>maxM[1]):
        maxM[1]= M2
        maxMcomb[1]= loadCombinationName
    if(M2<minM[1]):
        minM[1]= M2
        minMcomb[1]= loadCombinationName
    # Front node axial load.
    if(V2>maxV[1]):
        maxV[1]= V2
        maxVcomb[1]= loadCombinationName
    if(V2<minV[1]):
        minV[1]= V2
        minVcomb[1]= loadCombinationName
    beamElem2D.setProp('N+',maxN)
    beamElem2D.setProp('comb_N+', maxNcomb)
    beamElem2D.setProp('Mz+',maxM)
    beamElem2D.setProp('comb_M+', maxMcomb)
    beamElem2D.setProp('Vy+',maxV)
    beamElem2D.setProp('comb_V+', maxVcomb)
    beamElem2D.setProp('N-',minN)
    beamElem2D.setProp('comb_N-', minNcomb)
    beamElem2D.setProp('Mz-',minM)
    beamElem2D.setProp('comb_M-', minMcomb)
    beamElem2D.setProp('Vy-',minV)
    beamElem2D.setProp('comb_V-', minVcomb)


def update_envelope_internal_forces_beam_elem_3d(beamElem, loadCombinationName):
    '''Update values for extreme values of internal forces.

    :param beamElem: finite element to update internal forces.
    :param loadCombinationName: name of the load combinations to which the
                                internal forces on the elements correspond.
    '''
    beamElem.getResistingForce()
    [[N1, My1, Mz1, Vy1, Vz1, T1], [N2, My2, Mz2, Vy2, Vz2, T2]]= model_inquiry.get_values_at_nodes(beamElem,['N', 'My', 'Mz', 'Vy', 'Vz', 'T'], False)
    maxN= beamElem.getProp('N+') # [back node value, front node value]
    maxNcomb= beamElem.getProp('comb_N+') # load combinations.
    maxMy= beamElem.getProp('My+')
    maxMycomb= beamElem.getProp('comb_My+')
    maxMz= beamElem.getProp('Mz+')
    maxMzcomb= beamElem.getProp('comb_Mz+')
    maxVy= beamElem.getProp('Vy+')
    maxVycomb= beamElem.getProp('comb_Vy+')
    maxVz= beamElem.getProp('Vz+')
    maxVzcomb= beamElem.getProp('comb_Vz+')
    maxT= beamElem.getProp('T+')
    maxTcomb= beamElem.getProp('comb_T+')
    minN= beamElem.getProp('N-')
    minNcomb= beamElem.getProp('comb_N-') # load combinations.
    minMy= beamElem.getProp('My-')
    minMycomb= beamElem.getProp('comb_My-')
    minMz= beamElem.getProp('Mz-')
    minMzcomb= beamElem.getProp('comb_Mz-')
    minVy= beamElem.getProp('Vy-')
    minVycomb= beamElem.getProp('comb_Vy-')
    minVz= beamElem.getProp('Vz-')
    minVzcomb= beamElem.getProp('comb_Vz-')
    minT= beamElem.getProp('T-')
    minTcomb= beamElem.getProp('comb_T-')
    # Back end node.
    if(N1>maxN[0]):
        maxN[0]= N1
        maxNcomb[0]= loadCombinationName
    if(N1<minN[0]):
        minN[0]= N1
        minNcomb[0]= loadCombinationName
    if(My1>maxMy[0]):
        maxMy[0]= My1
        minMycomb[0]= loadCombinationName
    if(My1<minMy[0]):
        minMy[0]= My1
        minMycomb[0]= loadCombinationName
    if(Mz1>maxMz[0]):
        maxMz[0]= Mz1
        minMzcomb[0]= loadCombinationName
    if(Mz1<minMz[0]):
        minMz[0]= Mz1
        minMzcomb[0]= loadCombinationName
    if(Vy1>maxVy[0]):
        maxVy[0]= Vy1
        minVycomb[0]= loadCombinationName
    if(Vy1<minVy[0]):
        minVy[0]= Vy1
        minVycomb[0]= loadCombinationName
    if(Vz1>maxVz[0]):
        maxVz[0]= Vz1
        minVzcomb[0]= loadCombinationName
    if(Vz1<minVz[0]):
        minVz[0]= Vz1
        minVzcomb[0]= loadCombinationName
    if(T1>maxT[0]):
        maxT[0]= T1
        minTcomb[0]= loadCombinationName
    if(T1<minT[0]):
        minT[0]= T1
        minTcomb[0]= loadCombinationName
    # Front end node.  
    if(N2>maxN[1]):
        maxN[1]= N2
        minNcomb[1]= loadCombinationName
    if(N2<minN[1]):
        minN[1]= N2
        minNcomb[1]= loadCombinationName
    if(My2>maxMy[1]):
        maxMy[1]= My2
        minMycomb[1]= loadCombinationName
    if(My2<minMy[1]):
        minMy[1]= My2
        minMycomb[1]= loadCombinationName
    if(Mz2>maxMz[1]):
        maxMz[1]= Mz2
        minMzcomb[1]= loadCombinationName
    if(Mz2<minMz[1]):
        minMz[1]= Mz2
        minMzcomb[1]= loadCombinationName
    if(Vy2>maxVy[1]):
        maxVy[1]= Vy2
        minVycomb[1]= loadCombinationName
    if(Vy2<minVy[1]):
        minVy[1]= Vy2
        minVycomb[1]= loadCombinationName
    if(Vz2>maxVz[1]):
        maxVz[1]= Vz2
        minVzcomb[1]= loadCombinationName
    if(Vz2<minVz[1]):
        minVz[1]= Vz2
        minVzcomb[1]= loadCombinationName
    if(T2>maxT[1]):
        maxT[1]= T2
        minTcomb[1]= loadCombinationName
    if(T2<minT[1]):
        minT[1]= T2
        minTcomb[1]= loadCombinationName
    beamElem.setProp('N+',maxN)
    beamElem.setProp('comb_N+', maxNcomb)
    beamElem.setProp('My+',maxMy)
    beamElem.setProp('comb_My+', maxMycomb)
    beamElem.setProp('Mz+',maxMz)
    beamElem.setProp('comb_Mz+', maxMzcomb)
    beamElem.setProp('Vy+',maxVy)
    beamElem.setProp('comb_Vy+', maxVycomb)
    beamElem.setProp('Vz+',maxVz)
    beamElem.setProp('comb_Vz+', maxVzcomb)
    beamElem.setProp('T+',maxT)
    beamElem.setProp('comb_T+', maxTcomb)
    beamElem.setProp('N-',minN)
    beamElem.setProp('comb_N-', minNcomb)
    beamElem.setProp('My-',minMy)
    beamElem.setProp('comb_My-', minMycomb)
    beamElem.setProp('Mz-',minMz)
    beamElem.setProp('comb_Mz-', minMzcomb)
    beamElem.setProp('Vy-',minVy)
    beamElem.setProp('comb_Vy-', minVycomb)
    beamElem.setProp('Vz-',minVz)
    beamElem.setProp('comb_Vz-', minVzcomb)
    beamElem.setProp('T-',minT)
    beamElem.setProp('comb_T-', minTcomb)

def update_envelope_internal_forces_element(element, loadCombinationName):
    '''Update values for extreme values of internal forces.

    :param element: finite element whose envelope will be updated.
    :param loadCombinationName: name of the load combinations to which the
                                internal forces on the elements correspond.
    '''
    element.getResistingForce()
    if(element.hasProp('T+')): # 3D beam element.
        update_envelope_internal_forces_beam_elem_3d(element, loadCombinationName)
    elif(element.hasProp('Vy+') and not element.hasProp('Vz+')): # 2D beam element.
        update_envelope_internal_forces_beam_elem_2d(element, loadCombinationName)
    elif(element.hasProp('N+')): # Truss element.
        update_envelope_internal_forces_truss(element, loadCombinationName)
    else:
        errorMsg= '; not implemented yet for elements of type: '
        errorMsg+= str(element.tipo()) + '.'
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+errorMsg)
         
def update_envelope_internal_forces(elements, loadCombinationName):
    ''' Update the internal forces envelope of the given elements.

    :param elements: element whose envelope will be updated.
    :param loadCombinationName: name of the load combinations to which the
                                internal forces on the elements correspond.
    '''
    for e in elements:
        update_envelope_internal_forces_element(e, loadCombinationName)

def extrapolate_internal_forces_envelope_component_to_nodes(elements, component):
    ''' Extrapolate the values of the given component of the envelope to the 
        nodes.

    :param elements: elements whose envelope will be extrapolated to the 
                     connected nodes.
    '''
    visitedNodes= set()
    nodeValues= dict()
    nodeDict= dict()
    for e in elements:
        values= e.getProp(component)
        nodes= e.getNodes
        for i, n in enumerate(nodes):
            nTag= n.tag
            if(nTag not in nodeDict):
                nodeDict[nTag]= n
            if(nTag not in nodeValues):
                nodeValues[nTag]= [values[i]]
            else:
                nodeValues[nTag].append(values[i])
    for nTag in nodeValues:
        n= nodeDict[nTag]
        values= nodeValues[nTag]
        n.setProp(component, sum(values)/len(values))
  
def def_steel_shape_elastic_range_element_parameters(e,shape):
    e.setProp("nmbSecc",shape.name)
    e.setProp("Area",shape.A())
    e.setProp("fyd",shape.steelType.fyd())
    e.setProp("fydV",shape.steelType.fydV())

    e.setProp("Wyel",shape.get('Wyel'))
    e.setProp("Wzel",shape.get('Wzel'))
    e.setProp("AreaQz",shape.get('AreaQz'))
    e.setProp("AreaQy",shape.get('AreaQy'))


def def_steel_shape_elastic_range_parameters_for_set(elems,shape):
    for e in elems:
        def_steel_shape_elastic_range_element_parameters(e,shape)



#       ___ _                         
#      / __| |_ _ _ ___ ______ ___ ___
#      \__ \  _| '_/ -_|_-<_-</ -_|_-<
#      |___/\__|_| \___/__/__/\___/__/
# Stresses                                    

def def_vars_generalized_stress_control(elems, varDef: Iterable):
    ''' Define variables for generalizez stress control.

    :param elems: elements whose generalized stresses will be controlled.
    :param varDef: list of pairs of variable names and initial values to
                   define as property on each element. 
    '''
    for e in elems:
        for vd in varDef:
            e.setProp(vd[0],vd[1])

def def_vars_control_tens_elastic_range_2d(elems):
    ''' Define variables for stress control in 2D elasticity problems.'''
    varNames= [("Sg",0.0), ("SgMax", 0.0), ("SgMin", 0.0), ("NCP", 0.0), ("MzCP", 0.0), ("FCTN", 0.0), ("FCTNCP", 0.0), ("HIPCPTN", ""), ("Tau", 0.0), ("TauMax", 0.0), ("VyCP", 0.0), ("FCV", 0.0), ("FCVCP", 0.0), ("HIPCPV", "")]
    def_vars_generalized_stress_control(elems,varNames) 

def def_vars_control_tens_elastic_range_3d(elems):
    varNames= [("Sg", 0.0), ("SgMax", 0.0), ("SgMin", 0.0), ("NCP", 0.0), ("MyCP", 0.0), ("MzCP", 0.0), ("FCTN", 0.0), ("FCTNCP", 0.0), ("HIPCPTN", ""), ("Tau", 0.0), ("TauMax", 0.0), ("VyCP", 0.0), ("VzCP", 0.0), ("FCV", 0.0), ("FCVCP", 0.0), ("HIPCPV","")]
    def_vars_generalized_stress_control(elems,varNames) 
