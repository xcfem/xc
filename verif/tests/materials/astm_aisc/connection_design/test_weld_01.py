# -*- coding: utf-8 -*-
''' Design examples V14.0 - American Institute of Steel Construction.
Weld test- AISC verification. Example J.2 "Fillet weld loaded at an angle".'''

from __future__ import division 
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import sys
import xc_base
import geom
import xc
import math
from model import predefined_spaces
from materials.astm_aisc import ASTM_materials as astm
from materials import typical_materials as tm
from model.mesh import finit_el_model as fem
from actions import loads
from actions import load_cases as lcases
from model.sets import sets_mng as sets
from model.geometry import grid_model as gm
from connections.steel_connections import cbfem_bolt_weld as sc
from misc_utils import units_utils

#References:
# [1] Design examples V14.0 - American Institute of Steel Construction»
#units
pounds2N=4.448

#Weld test- AISC verification.
#Example J.2 "Fillet weld loaded at an angle" of reference [1]
# According to the problem statement members are stiff enough
l=15.4*units_utils.inchToMeter # weld length [m]
t=5/16*units_utils.inchToMeter #weld thickness
D=50   #dead load [kips]
L=150  #live load [kips]
Pu=(1.2*D+1.6*L)*1e3*pounds2N   #Design load [N]
ang0=0  #
ang1=60 #angle in which load is acting [degrees] (load case 1)

tPLt=1.5*units_utils.inchToMeter   #thickness of plates [m].
Lplt=2*units_utils.inchToMeter  # length of plates to model [m] (low value to consider plates
               # stiff enough
esize=Lplt/5
#             *** GEOMETRIC model (points, lines, surfaces) - SETS ***
FEcase= xc.FEProblem()
FEcase.title= 'Weld test'
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler


modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl
sty=outSty.OutputStyle() 
out=outHndl.OutputHandler(modelSpace,sty)

weldMetal=astm.E7018
plateSteel=astm.A992

gridVpl=gm.GridModel(prep,[0,l],[0],[-Lplt,Lplt])
gridVpl.generatePoints()
gridHpl=gm.GridModel(prep,[0,l],[0,Lplt],[0])
gridHpl.generatePoints()

Vplate=gridVpl.genSurfOneXYZRegion(((0,0,-Lplt),(l,0,Lplt)),'Vplate')
Hplate=gridHpl.genSurfOneXYZRegion(((0,0,0),(l,Lplt,0)),'Hplate')

plate_mat=tm.DeckMaterialData(name='plate_mat',thickness=tPLt,material=plateSteel)
plate_mat.setupElasticSection(prep)
Vplate_mesh=fem.SurfSetToMesh(surfSet=Vplate,matSect=plate_mat,elemSize=esize,elemType='ShellMITC4')
Hplate_mesh=fem.SurfSetToMesh(surfSet=Hplate,matSect=plate_mat,elemSize=esize,elemType='ShellMITC4')
fem.multi_mesh(prep,[Vplate_mesh,Hplate_mesh])

#boundary conditions
bound1=sets.get_set_nodes_plane_XY('bound1',Vplate,-Lplt)
bound2=sets.get_set_nodes_plane_XY('bound2',Vplate,Lplt)
for n in bound1.nodes:
    modelSpace.fixNode000_000(n.tag)
for n in bound2.nodes:
    modelSpace.fixNode000_000(n.tag)

#Welds
weldP1=gridHpl.getPntXYZ((0,0,0))
weldP2=gridHpl.getPntXYZ((l,0,0))

weld_t=sc.WeldTyp(t,weldMetal)
W1=sc.FilletWeld(weld_t,Vplate,Hplate,'W1',weldExtrPoints=[weldP1,weldP2])
W1.generateWeld(nDiv=15)
W2=sc.FilletWeld(weld_t,Vplate,Hplate,'W2',weldExtrPoints=[weldP1,weldP2])
W2.generateWeld(nDiv=15,WS1sign=1,WS2sign=-1)

#out.displayFEMesh()

ratio0=W1.length-weldP1.getPos.dist(weldP2.getPos)

#out.displayBlocks()
setLoadNodes=sets.get_set_nodes_plane_XZ('setLoadNodes',Hplate,Lplt)
Fnode=Pu/setLoadNodes.nodes.size

load0deg=loads.NodalLoad(name='load0deg',
                         lstNod=[n for n in setLoadNodes.nodes],
                         loadVector=xc.Vector([0,Fnode,0,0,0,0]))

load60deg=loads.NodalLoad(name='load0deg',
                         lstNod=[n for n in setLoadNodes.nodes],
                         loadVector=xc.Vector([Fnode*math.cos(math.radians(ang1)),Fnode*math.sin(math.radians(ang1)),0,0,0,0]))

LC1=lcases.LoadCase(preprocessor=prep,name="LC1",loadPType="default",timeSType="constant_ts")
LC1.create()
LC1.addLstLoads([load0deg])

LC2=lcases.LoadCase(preprocessor=prep,name="LC2",loadPType="default",timeSType="constant_ts")
LC2.create()
LC2.addLstLoads([load60deg])

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC1')
result= modelSpace.analyze(calculateNodalReactions= True)


Vytot=0
for e in W1.weldSet.elements:
    Vytot+=e.getVy()
ratio1=(abs(Vytot)-Pu/2)/Pu

Vytot=0
for e in W2.weldSet.elements:
    Vytot+=e.getVy()
ratio2=(abs(Vytot)-Pu/2)/Pu
#capacity factor
CF0=W1.getCF_AISCverif(plateSteel)
ratio3=(CF0-1.0059110796832564)/CF0

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC2')
result= modelSpace.analyze(calculateNodalReactions= True)
Fpar=W1.getTotalIntForc()[0]
ratio4=(abs(Fpar)-Pu/2*math.cos(math.radians(ang1)))/Pu
Vytot=0
for e in W1.weldSet.elements:
    Vytot+=e.getVy()
ratio5=(abs(Vytot)-Pu/2*math.sin(math.radians(ang1)))/Pu
Fpar=W2.getTotalIntForc()[0]
ratio6=(abs(Fpar)-Pu/2*math.cos(math.radians(ang1)))/Pu
Vytot=0
for e in W2.weldSet.elements:
    Vytot+=e.getVy()
ratio7=(abs(Vytot)-Pu/2*math.sin(math.radians(ang1)))/Pu
# Weld design strenght (per unit lentgth)
#fiRn=W1.getWeldDesignStrength(Fpar,Fper)/W1.length
#ratio8=(fiRn-13e3)/13e3
#capacity factor
CF60=W1.getCF_AISCverif(plateSteel)
ratio8=(CF60-1.0501509802325004)/CF60

'''
print('ratio0= ', ratio0)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('CF0= ', CF0)
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('ratio6= ', ratio6)
print('ratio7= ', ratio7)
print('CF60= ', CF60)
print('ratio8= ', ratio8)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-7) & (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<5e-4) & (abs(ratio4)<1e-7) & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-7) & (abs(ratio7)<1e-5) & (abs(ratio8)<5e-3):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic stuff
'''
from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl

sty=outSty.OutputStyle() 
out=outHndl.OutputHandler(modelSpace,sty)

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC1')
result= modelSpace.analyze(calculateNodalReactions= True)
out.displayLoads()
out.displayIntForcDiag('N',W1.weldSet)
out.displayIntForcDiag('Qz',W1.weldSet)
out.displayIntForcDiag('Qy',W1.weldSet)

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC2')
result= modelSpace.analyze(calculateNodalReactions= True)
out.displayLoads()
out.displayIntForcDiag('N',W1.weldSet)
out.displayIntForcDiag('Qz',W1.weldSet)
out.displayIntForcDiag('Qy',W1.weldSet)
'''
