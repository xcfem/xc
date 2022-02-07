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
import xc_base
import geom
import xc

def defVarControlMov(obj, code):
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

def defVarsControlMovs(nodes: Iterable, flags: Iterable):
    ''' Define variables for movement control.

    :param nodes: nodes whose movement will be controlled.
    :param flags: movement identifiers.
    '''
    tags= []
    for n in nodes:
        tags.append(n.tag)
        for f in flags:
            defVarControlMov(n,f)
    return tags

def defVarsControlMovModulus(nodes: Iterable):
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

def defVarsGeneralizedStressControl(elems, varDef: Iterable):
    ''' Define variables for generalizez stress control.

    :param elems: elements whose generalized stresses will be controlled.
    :param varDef: list of pairs of variable names and initial values to
                   define as property on each element. 
    '''
    for e in elems:
        for vd in varDef:
            e.setProp(vd[0],vd[1])


def defVarsControlTensRegElastico2d(elems):
    ''' Define variables for stress control in 2D elasticity problems.'''
    varNames= [("Sg",0.0), ("SgMax", 0.0), ("SgMin", 0.0), ("NCP", 0.0), ("MzCP", 0.0), ("FCTN", 0.0), ("FCTNCP", 0.0), ("HIPCPTN", ""), ("Tau", 0.0), ("TauMax", 0.0), ("VyCP", 0.0), ("FCV", 0.0), ("FCVCP", 0.0), ("HIPCPV", "")]
    defVarsGeneralizedStressControl(elems,varNames) 

def defVarsControlTensRegElastico3d(elems):
    varNames= [("Sg", 0.0), ("SgMax", 0.0), ("SgMin", 0.0), ("NCP", 0.0), ("MyCP", 0.0), ("MzCP", 0.0), ("FCTN", 0.0), ("FCTNCP", 0.0), ("HIPCPTN", ""), ("Tau", 0.0), ("TauMax", 0.0), ("VyCP", 0.0), ("VzCP", 0.0), ("FCV", 0.0), ("FCVCP", 0.0), ("HIPCPV","")]
    defVarsGeneralizedStressControl(elems,varNames) 

def defEnvelopeVars(elems: Iterable, varNames: Iterable, initV= 6.023e23):
    ''' Define variables for generalizez stress control.

    :param elems: nodes whose generalized stresses will be controlled.
    :param varNames: variable names.
    '''
    for e in elems:
        for vn in varNames:
            # [back node value, front node value]
            e.setProp(vn+'+',[-initV,-initV]) #Positive value of envelope
            e.setProp(vn+'-',[initV,initV]) # Negative value of envelope
    
def defVarsEnvelopeInternalForcesTrussElems(elems):
    '''Defines properties to store extreme values of internal forces.

    :param elems: nodes whose generalized stresses will be controlled.
    '''
    defEnvelopeVars(elems, ['N'])
        
def defVarsEnvelopeInternalForcesBeamElems(elems):
    '''Defines properties to store extreme values of internal forces.

    :param elems: nodes whose generalized stresses will be controlled.
    '''
    defEnvelopeVars(elems, ['N', 'Mz', 'My', 'T', 'Vy','Vz'])

def updateEnvelopeInternalForcesTruss(trussElem):
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

def updateEnvelopeInternalForcesBeamElem2D(beamElem2D):
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
    if(N1>maxN[0]):
      maxN[0]= N1
    if(N1<minN[0]):
      minN[0]= N1
    if(M1>maxM[0]):
      maxM[0]= M1
    if(M1<minM[0]):
      minM[0]= M1
    if(V1>maxV[0]):
      maxV[0]= V1
    if(V1<minV[0]):
      minV[0]= V1
    if(N2>maxN[1]):
      maxN[1]= N2
    if(N2<minN[1]):
      minN[1]= N2
    if(M2>maxM[1]):
      maxM[1]= M2
    if(M2<minM[1]):
      minM[1]= M2
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


def updateEnvelopeInternalForcesBeamElem(beamElem):
    '''Update values for extreme values of internal forces.

    :param beamElem: finite element to update internal forces.
     '''
    beamElem.getResistingForce()
    [[N1, My1, Mz1, Vy1, Vz1], [N2, My2, Mz2, Vy2, Vz2]]= model_inquiry.getValuesAtNodes(beamElem,['N', 'My', 'Mz', 'Vy', 'Vz'], False)
    maxN= beamElem.getProp('N+') # [back node value, front node value]
    maxMy= beamElem.getProp('My+')
    maxMz= beamElem.getProp('Mz+')
    maxVy= beamElem.getProp('Vy+')
    maxVz= beamElem.getProp('Vz+')
    minT= beamElem.getProp('T+')
    minN= beamElem.getProp('N-')
    minMy= beamElem.getProp('My-')
    minMz= beamElem.getProp('Mz-')
    minVy= beamElem.getProp('Vy-')
    minVz= beamElem.getProp('Vz-')
    minT= beamElem.getProp('T-')
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
  
def defSteelShapeElasticRangeElementParameters(e,shape):
  e.setProp("nmbSecc",shape.name)
  e.setProp("Area",shape.A())
  e.setProp("fyd",shape.steelType.fyd())
  e.setProp("fydV",shape.steelType.fydV())

  e.setProp("Wyel",shape.get('Wyel'))
  e.setProp("Wzel",shape.get('Wzel'))
  e.setProp("AreaQz",shape.get('AreaQz'))
  e.setProp("AreaQy",shape.get('AreaQy'))


def defSteelShapeElasticRangeParametersForSet(elems,shape):
  for e in elems:
    defSteelShapeElasticRangeElementParameters(e,shape)
