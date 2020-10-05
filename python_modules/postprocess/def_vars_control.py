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
import xc_base
import geom
import xc



def defVarControlMov(obj, code):
  if(not obj.hasProp('span')):
    lmsg.warning('span property not defined for: '+str(obj.tag) + ' object.')
  obj.setProp(code+'Max',0.0)
  obj.setProp('Comb'+code+'Max',"")
  obj.setProp(code+'Min',0.0)
  obj.setProp('Comb'+code+'Min',"")

def defVarsControlMovs(nodes,flags):
  tags= []
  for n in nodes:
    tags.append(n.tag)
    for f in flags:
      defVarControlMov(n,f)
  return tags

# Deprecated.
# def defVarsControlMovU(nodes):
#   return defVarsControlMovs(nodes,{'U'})

# def defVarsControlMovV(nodes):
#   return defVarsControlMovs(nodes,{'V'})

# def defVarsControlMovW(nodes):
#   return defVarsControlMovs(nodes,{'W'})

# def defVarsControlMovUV(nodes):
#   return defVarsControlMovs(nodes,{'U','V'})
# def defVarsControlMovUVW(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W'})

# def defVarsControlMovUVWRXRYRZ(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W','RX','RY','RZ'})

def defVarsControlMovModulus(nodes):
  tags= []
  for n in nodes:
    if(not n.hasProp('span')):
      lmsg.warning('span property not defined for node: '+str(n.tag) + '.')
    tags.append(n.tag)
    n.setProp("dispMax",0.0)
    n.setProp("CombDispMax","")
  return tags


def defVarsControlTensRegElastico2d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")

def defVarsControlTensRegElastico3d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MyCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("VzCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")

def defVarsEnvelopeInternalForcesBeamElems(elems):
  '''Defines properties to store extreme values of internal forces.'''
  for e in elems:
    # [back node value, front node value]
    e.setProp('N+',[-6.023e23,-6.023e23]) #Positive axial force envelope
    e.setProp('N-',[6.023e23,6.023e23]) #Negative axial force envelope
    e.setProp('Mz+',[-6.023e23,-6.023e23]) #Positive bending moment envelope
    e.setProp('Mz-',[6.023e23,6.023e23]) #Negative bending moment envelope
    e.setProp('My+',[-6.023e23,-6.023e23]) #Positive bending moment envelope
    e.setProp('My-',[6.023e23,6.023e23]) #Negative bending moment envelope
    e.setProp('Vy+',[-6.023e23,-6.023e23]) #Positive y shear envelope
    e.setProp('Vy-',[6.023e23,6.023e23]) #Negative y shear  envelope
    e.setProp('Vz+',[-6.023e23,-6.023e23]) #Positive y shear envelope
    e.setProp('Vz-',[6.023e23,6.023e23]) #Negative y shear  envelope
    e.setProp('T+',[-6.023e23,-6.023e23]) #Positive torque envelope
    e.setProp('T-',[6.023e23,6.023e23]) #Negative torque envelope

def updateEnvelopeInternalForcesBeamElem2D(beamElem2D):
    '''Update values for extreme values of internal forces in 2D elements.

    :param beamElem2D: finite element to update internal forces.
    '''
    beamElem2D.getResistingForce()
    N1= 0.0; M1= 0.0; V1= 0.0
    N2= 0.0; M2= 0.0; V2= 0.0
    axialForces= beamElem2D.getValuesAtNodes('N', False)
    if(len(axialForces)>1): # 'N' found.
        N1= axialForces[0]
        N2= axialForces[1]
    bending= beamElem2D.getValuesAtNodes('M', False)
    if(len(bending)>1): # 'M' found.
        M1= bending[0]
        M2= bending[1]
    shear= beamElem2D.getValuesAtNodes('V', False)
    if(len(shear)>1): # 'V' found.
        V1= shear[0]
        V2= shear[1]
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
    N1= 0.0; My1= 0.0; Mz1= 0.0; Vy1= 0.0; Vz1= 0.0
    N2= 0.0; My2= 0.0; Mz2= 0.0; Vy2= 0.0; Vz2= 0.0
    axialForces= beamElem.getValuesAtNodes('N', False)
    if(len(axialForces)>1): # 'N' found.
        N1= axialForces[0]
        N2= axialForces[1]
    bendingY= beamElem.getValuesAtNodes('My', False)
    if(len(bendingY)>1): # 'My' found.
        My1= bendingY[0]
        My2= bendingY[1]
    bendingZ= beamElem.getValuesAtNodes('Mz', False)
    if(len(bendingZ)>1): # 'Mz' found.
        Mz1= bendingZ[0]
        Mz2= bendingZ[1]
    shearY= beamElem.getValuesAtNodes('Vy', False)
    if(len(shearY)>1): # 'Vy' found.
        Vy1= shearY[0]
        Vy2= shearY[1]
    shearZ= beamElem.getValuesAtNodes('Vz', False)
    if(len(shearZ)>1): # 'Vz' found.
        Vz1= shearZ[0]
        Vz2= shearZ[1]
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
  e.setProp("nmbSecc",shape.sectionName)
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
