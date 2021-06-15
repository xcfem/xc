# -*- coding: utf-8 -*-
from __future__ import division 
from __future__ import print_function

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
#References:
# [1] Design examples V14.0 - American Institute of Steel Construction»
#units
in2m=0.0254
pounds2N=4.448

#Weld test- AISC verification.
#Example J.2 "Fillet weld loaded at an angle" of reference [1]
l=15.4*in2m # weld length [m]
t=5/16*in2m #weld thickness
D=50   #dead load [kips]
L=150  #live load [kips]
Pu=(1.2*D+1.6*L)*1e3*pounds2N   #Design load [N]
ang0=0  #
ang1=60 #angle in which load is acting [degrees] (load case 1)

tPLt=1.5*in2m   #thickness of plates [m].
Lplt=2*in2m  # length of plates to model [m] (low value to consider plates
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
lW=geom.Segment3d(geom.Pos3d(0,0,0),geom.Pos3d(l,0,0))
weld_t=sc.WeldTyp(t,weldMetal)
W=sc.MultiFilletWeld(weld_t,Vplate,Hplate,[lW],'W','weld both sides')
W.generateWeld(nDiv=15,WS1sign=1,bothSidesOfWS2=True)
#out.displayFEMesh()

W1=W.lstWelds[0]
W2=W.lstWelds[1]
ratio0=W1.length-lW.getLength()


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
for e in  W1.weldSet.elements:
    Vytot+=e.getVy()
ratio1=(abs(Vytot)-Pu/2)/Pu
Vytot=0
for e in  W2.weldSet.elements:
    Vytot+=e.getVy()
ratio2=(abs(Vytot)-Pu/2)/Pu
#capacity factor
CF0=W1.getCF_AISCverif(plateSteel)
ratio3=(CF0-0.99125)/CF0


modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC2')
result= modelSpace.analyze(calculateNodalReactions= True)
Fpar=W1.getTotalIntForc()[0]
ratio4=(abs(Fpar)-Pu/2*math.cos(math.radians(ang1)))/Pu
Vytot=0
for e in  W1.weldSet.elements:
    Vytot+=e.getVy()
ratio5=(abs(Vytot)-Pu/2*math.sin(math.radians(ang1)))/Pu
Fpar=W2.getTotalIntForc()[0]
ratio6=(abs(Fpar)-Pu/2*math.cos(math.radians(ang1)))/Pu
Vytot=0
for e in  W2.weldSet.elements:
    Vytot+=e.getVy()
ratio7=(abs(Vytot)-Pu/2*math.sin(math.radians(ang1)))/Pu
# Weld design strenght (per unit lentgth)
#capacity factor
CF60=W1.getCF_AISCverif(plateSteel)
ratio8=(CF60-1.035)/CF60

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-7) & (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-3) & (abs(ratio4)<1e-7) & (abs(ratio5)<1e-7) & (abs(ratio6)<1e-7) & (abs(ratio7)<1e-7) & (abs(ratio8)<5e-3) :
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

'''
#Graphic stuff
from postprocess.config import default_config
from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl

sty=outSty.OutputStyle() 
out=outHndl.OutputHandler(modelSpace,sty)

out.displayLoads()
out.displayIntForcDiag('N',W1.weldSet)
out.displayIntForcDiag('Qz',W1.weldSet)
out.displayIntForcDiag('Qy',W1.weldSet)
'''
