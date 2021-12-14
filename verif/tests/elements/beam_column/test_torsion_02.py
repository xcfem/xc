# -*- coding: utf-8 -*-
''' Test based on the example 5.1 of AISC Steel Design Guide 9 Torsional 
    Analysis of Structural Steel Members.'''

from __future__ import division 
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import xc_base
import geom
import xc
import math
from model import predefined_spaces
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from materials.astm_aisc import ASTM_materials as astm
from materials.astm_aisc import torsional_function_curves
from actions import loads
from actions import load_cases as lcases

def torsional_stiffness_factor(case, steelShape, z, L, zT):
    ''' Return the torsional stiffness factor to use in the
        beam elements.

    :param case: loading and end condition case according to "AISC Design Guide number 9".
    :param steelShape: steel shape of the rolled steel section.
    :param z: length from left support to cross-section analyzed.
    :param L: span length
    :param zT: length from left support to cross-section where torque
               is applied.
    '''
    thetaFactor= torsional_function_curves.theta_factor(3, steelShape, z, L, zT)
    return zT/L*z/L/thetaFactor


#units
in2m=0.0254
kip2N=4448.22
m2in=1/in2m
N2kip=1/kip2N
Pa2ksi= 1.45038e-7
#data
L= 15*12*in2m # beam length
T= -60*kip2N*in2m # service load torque (60 kip-in)

FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
prep=preprocessor   #short name
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the

grid=gm.GridModel(prep,xList=[0,L/2,L],yList=[0],zList=[0])
grid.generatePoints()


steel_W=astm.A992   #steel W shapes
beam=grid.genLinOneXYZRegion([(0,0,0),(L,0,0)],'beam')
beam_mat= astm.WShape(steel_W,'W10X49') 

thetaFactor= torsional_function_curves.theta_factor(3, beam_mat, z= 0.5*L, L= L, zT= 0.5*L)
beam_mat.torsionalStiffnessFactor= torsional_stiffness_factor(3, beam_mat, z= 0.5*L, L= L, zT= 0.5*L)

beam_mat.defElasticShearSection3d(prep)

beam_mesh= fem.LinSetToMesh(linSet=beam,matSect=beam_mat,elemSize=0.25,vDirLAxZ=xc.Vector([0,1,0]), elemType='ElasticBeam3d')
beam_mesh.generateMesh(prep)

J= beam_mat.J()/beam_mat.torsionalStiffnessFactor
Jcomp= 1.39/(m2in**4)
ratio0= abs(J-Jcomp)/Jcomp

G= steel_W.G()
Gcomp= 11200.0/Pa2ksi
ratio1= abs(G-Gcomp)/Gcomp

#boundary conditions
extr1= grid.getPntXYZ((0,0,0))
modelSpace.fixNode000_0FF(extr1.getNode().tag)
extr2= grid.getPntXYZ((L,0,0))
modelSpace.fixNodeF00_0FF(extr2.getNode().tag)

xcTotalSet= modelSpace.getTotalSet()
testElem= xcTotalSet.elements[0]
elementTorsionalStiffness= testElem.getTangentStiff()(3,3)
elementLength= testElem.getLength(True)
refElementTorsionalStiffness= G*J/elementLength

#load
midPoint= grid.getPntXYZ((L/2.,0,0))
midNode= midPoint.getNode()
torsBeam= loads.NodalLoad(name='torsBeam',lstNod=[midNode],loadVector=xc.Vector([0,0,0,T,0,0]))

LC= lcases.LoadCase(preprocessor=prep,name="LC",loadPType="default",timeSType="constant_ts")
LC.create()
LC.addLstLoads([torsBeam])

modelSpace.addLoadCaseToDomain("LC")
modelSpace.analyze(calculateNodalReactions=True)

rot= midNode.getDisp[3]

refRot= thetaFactor*(T*L)/(G*J) # radians
ratio2= abs(rot-refRot)/-refRot

'''
print('refRot= ', refRot/rot)
print('theta factor: ', thetaFactor)
print('torsional stiffness factor: ', beam_mat.torsionalStiffnessFactor)
print('beam G= ', testElem.sectionProperties.G/1e6)
print('beam J= ', testElem.sectionProperties.J)
print('element torsional stiffness: ', elementTorsionalStiffness/1e6)
print('element length: ', elementLength, ' m')
print('element torsional stiffness: ', refElementTorsionalStiffness/1e6)
print('T= '+str(T/1e3)+' kN m ('+str(T/kip2N/in2m)+ ' kip in)')
print('l= '+str(L)+' m ('+str(L/in2m)+ ' in)')
print('G= '+str(G/1e6)+' MPa ('+str(G*Pa2ksi)+ ' ksi)')
print('J= '+str(J)+' m4 ('+str(J*(m2in**4))+ ' in4)')
print('ratio0= ', ratio0)
print('ratio1= ', ratio1)
print('rotation: '+str(rot)+' radians')
print('reference rotation: '+str(refRot)+' radians')
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-3) & (ratio1<0.01) & (ratio2<1e-6):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
