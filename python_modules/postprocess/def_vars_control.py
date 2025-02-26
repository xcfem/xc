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
        lmsg.warning('span property not defined for: '+str(obj.tag) + ' object.')
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
            lmsg.warning('span property not defined for node: '+str(n.tag) + '.')
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
                e.setProp(vn+'+',[-initV]*nNodes) #Positive value of envelope
                e.setProp(vn+'-',[initV]*nNodes) # Negative value of envelope
            else:
                e.setProp(vn+'+',-initV) #Positive value of envelope
                e.setProp(vn+'-',initV) # Negative value of envelope


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

    :param elems: nodes whose generalized stresses will be controlled.
    '''
    def_envelope_vars(elems, ['N'])
        
def def_vars_envelope_internal_forces_beam_elems(elems):
    '''Defines properties to store extreme values of internal forces.

    :param elems: nodes whose generalized stresses will be controlled.
    '''
    def_envelope_vars(elems, ['N', 'Mz', 'My', 'T', 'Vy','Vz'])

def update_envelope_internal_forces_truss(trussElem):
    '''Update values for extreme values of internal forces in 2D elements.

    :param trussElem: finite element to update internal forces.
    '''
    trussElem.getResistingForce()
    [[N1], [N2]]= model_inquiry.getValuesAtNodes(trussElem, ['N'], False)
    maxN= trussElem.getProp('N+') # [back node value, front node value]
    minN= trussElem.getProp('N-')
    if(N1>maxN[0]):
        maxN[0]= N1
    if(N1<minN[0]):
        minN[0]= N1
    if(N2>maxN[1]):
        maxN[1]= N2
    if(N2<minN[1]):
        minN[1]= N2
    trussElem.setProp('N+',maxN)
    trussElem.setProp('N-',minN)

def update_envelope_internal_forces_beam_elem_2d(beamElem2D):
    '''Update values for extreme values of internal forces in 2D elements.

    :param beamElem2D: finite element to update internal forces.
    '''
    beamElem2D.getResistingForce()
    [[N1, M1, V1], [N2, M2, V2]]= model_inquiry.getValuesAtNodes(beamElem2D, ['N', 'M', 'V'], False)
    maxN= beamElem2D.getProp('N+') # [back node value, front node value]
    maxM= beamElem2D.getProp('Mz+')
    maxV= beamElem2D.getProp('Vy+')
    minN= beamElem2D.getProp('N-')
    minM= beamElem2D.getProp('Mz-')
    minV= beamElem2D.getProp('Vy-')
    # Back node axial load.
    if(N1>maxN[0]):
        maxN[0]= N1
    if(N1<minN[0]):
        minN[0]= N1
    # Back node bending moment.
    if(M1>maxM[0]):
        maxM[0]= M1
    if(M1<minM[0]):
        minM[0]= M1
    # Back node shear load.
    if(V1>maxV[0]):
        maxV[0]= V1
    if(V1<minV[0]):
        minV[0]= V1
    # Front node axial load.
    if(N2>maxN[1]):
        maxN[1]= N2
    if(N2<minN[1]):
        minN[1]= N2
    # Front node axial load.
    if(M2>maxM[1]):
        maxM[1]= M2
    if(M2<minM[1]):
        minM[1]= M2
    # Front node axial load.
    if(V2>maxV[1]):
        maxV[1]= V2
    if(V2<minV[1]):
        minV[1]= V2
    beamElem2D.setProp('N+',maxN)
    beamElem2D.setProp('Mz+',maxM)
    beamElem2D.setProp('Vy+',maxV)
    beamElem2D.setProp('N-',minN)
    beamElem2D.setProp('Mz-',minM)
    beamElem2D.setProp('Vy-',minV)


def update_envelope_internal_forces_beam_elem_3d(beamElem):
    '''Update values for extreme values of internal forces.

    :param beamElem: finite element to update internal forces.
     '''
    beamElem.getResistingForce()
    [[N1, My1, Mz1, Vy1, Vz1, T1], [N2, My2, Mz2, Vy2, Vz2, T2]]= model_inquiry.getValuesAtNodes(beamElem,['N', 'My', 'Mz', 'Vy', 'Vz', 'T'], False)
    maxN= beamElem.getProp('N+') # [back node value, front node value]
    maxMy= beamElem.getProp('My+')
    maxMz= beamElem.getProp('Mz+')
    maxVy= beamElem.getProp('Vy+')
    maxVz= beamElem.getProp('Vz+')
    maxT= beamElem.getProp('T+')
    minN= beamElem.getProp('N-')
    minMy= beamElem.getProp('My-')
    minMz= beamElem.getProp('Mz-')
    minVy= beamElem.getProp('Vy-')
    minVz= beamElem.getProp('Vz-')
    minT= beamElem.getProp('T-')
    # Back end node.
    if(N1>maxN[0]):
        maxN[0]= N1
    if(N1<minN[0]):
        minN[0]= N1
    if(My1>maxMy[0]):
        maxMy[0]= My1
    if(My1<minMy[0]):
        minMy[0]= My1
    if(Mz1>maxMz[0]):
        maxMz[0]= Mz1
    if(Mz1<minMz[0]):
        minMz[0]= Mz1
    if(Vy1>maxVy[0]):
        maxVy[0]= Vy1
    if(Vy1<minVy[0]):
        minVy[0]= Vy1
    if(Vz1>maxVz[0]):
        maxVz[0]= Vz1
    if(Vz1<minVz[0]):
        minVz[0]= Vz1
    if(T1>maxT[0]):
        maxT[0]= T1
    if(T1<minT[0]):
        minT[0]= T1
    # Front end node.  
    if(N2>maxN[1]):
        maxN[1]= N2
    if(N2<minN[1]):
        minN[1]= N2
    if(My2>maxMy[1]):
        maxMy[1]= My2
    if(My2<minMy[1]):
        minMy[1]= My2
    if(Mz2>maxMz[1]):
        maxMz[1]= Mz2
    if(Mz2<minMz[1]):
        minMz[1]= Mz2
    if(Vy2>maxVy[1]):
        maxVy[1]= Vy2
    if(Vy2<minVy[1]):
        minVy[1]= Vy2
    if(Vz2>maxVz[1]):
        maxVz[1]= Vz2
    if(Vz2<minVz[1]):
        minVz[1]= Vz2
    if(T2>maxT[1]):
        maxT[1]= T2
    if(T2<minT[1]):
        minT[1]= T2
    beamElem.setProp('N+',maxN)
    beamElem.setProp('My+',maxMy)
    beamElem.setProp('Mz+',maxMz)
    beamElem.setProp('Vy+',maxVy)
    beamElem.setProp('Vz+',maxVz)
    beamElem.setProp('T+',minT)
    beamElem.setProp('N-',minN)
    beamElem.setProp('My-',minMy)
    beamElem.setProp('Mz-',minMz)
    beamElem.setProp('Vy-',minVy)
    beamElem.setProp('Vz-',minVz)
    beamElem.setProp('T-',minT)
  
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
