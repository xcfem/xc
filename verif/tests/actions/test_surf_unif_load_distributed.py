# -*- coding: utf-8 -*-
'''Home-made test on distribute uniform load applied on a non meshed surfaces
among the nodes that touch those surfaces 
'''
import os
import xc_base
import geom
import xc
from materials import typical_materials as tm
from materials.ehe import EHE_materials
from model import predefined_spaces
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from actions import loads
from actions import load_cases as lcases

# Default configuration of environment variables.
from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl

__author__= "Ana Ortega (AO_O)  Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2019, AO_O  LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"


#data
Lsquare=1.0 # square side
qUnif=5e3   # unif load on surfaces 

sty=outSty.OutputStyle() 

FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the
# dimension of the space: nodes by three coordinates (x,y,z) and 
# six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)
out=outHndl.OutputHandler(modelSpace,sty)
xList=[0,Lsquare/5,Lsquare]
yList=[0,Lsquare/3,Lsquare]
zList=[1]
gridGeom= gm.GridModel(prep,xList,yList,zList)
gridGeom.generatePoints()

loadedSurf=gridGeom.genSurfOneXYZRegion(xyzRange=((0,0,1),(1,1,1)),setName='loadedSurf')

beams=gridGeom.genLinOneXYZRegion(((0,0,1),(1,1,1)),'beams')

concrete=EHE_materials.HA30
concrProp=tm.MaterialData(name='concrProp',E=concrete.Ecm(),nu=concrete.nuc,rho=concrete.density())

from materials.sections import section_properties as sectpr
geomSect=sectpr.RectangularSection(name='geomSect',b=0.2,h=0.2)

linel_mat= tm.BeamMaterialData(name= 'linel_mat', section=geomSect, material=concrProp)
linel_mat.setupElasticShear3DSection(preprocessor=prep)

beams_mesh=fem.LinSetToMesh(linSet=beams,matSect=linel_mat,elemSize=0.5,vDirLAxZ=xc.Vector([0,0,1]),elemType='ElasticBeam3d',coordTransfType='linear')
beams_mesh.generateMesh(prep)


surfUnifLoad= loads.UnifLoadSurfNodesDistributed(name= 'surfUnifLoad',surfSet=loadedSurf,loadVector=xc.Vector([0,0,-qUnif,0,0,0]))

LC1=lcases.LoadCase(preprocessor=prep,name="LC1",loadPType="default",timeSType="constant_ts")
LC1.create()
LC1.addLstLoads([surfUnifLoad])

modelSpace.addLoadCaseToDomain('LC1')

lp=modelSpace.getCurrentLoadPattern()
lIter= lp.loads.getNodalLoadIter
totalLoad=0
nLoad=lIter.next()
while(nLoad):
    vLoad= nLoad.getForce
    v= xc.Vector([vLoad[0], vLoad[1], vLoad[2]])
    totalLoad+=v.Norm()
    nLoad= lIter.next()

totalCompLoad=Lsquare*Lsquare*qUnif
#out.displayLoads()

ratio0=(totalLoad-totalCompLoad)/totalCompLoad
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<2e-7):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
