# -*- coding: utf-8 -*-
''' This test addresses a  time-series calculation of shrinkage
  Two beams are built with the following sequence:
  day 0: beam-1 is built (authogenus shrinkage of beam-1 starts)
  day 3: end of beam-1 curing (drying shrinkage of beam-1 starts)
  day 15:  beam-2 is built (authogenus shrinkage of beam-2 starts) 
  day 18: end of beam-2 curing (drying shrinkage of beam-2 starts)
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model.geometry import grid_2Dmodel as gm
from postprocess.config import default_config
from materials.ehe import EHE_materials

#data
w_beam=1
th_beam=1
concr=EHE_materials.HA30
concr.cemType='N'   #cement class
RH=70                   #ambient relative humidity(%)
ts=3       # age of concrete [days] when drying shrinkage of beams begins 
t_calc=[t for t in range(0,5)]+[t for t in range(5,20,5)]+ [t for t in range(20,100,10)]
t0_beam2=15 # age (days) of beam 1 when beam2 is build
ts_beam2=3 # age of concrete [days] when drying shrinkage of beams begins 
# end data

Ac=w_beam*th_beam
u=2*(w_beam+th_beam)
h0_beam1=2*Ac/u
h0_beam2=h0_beam1
shrinkage_beam1=[concr.getShrEpscs(t,ts,RH,h0_beam1) for t in t_calc]
shrinkage_beam2=[concr.getShrEpscs(t-t0_beam2,ts_beam2,RH,h0_beam2) for t in t_calc]

shr_05=concr.getShrEpscd(5,ts,RH,h0_beam1)+concr.getShrEpsca(5) 
N05Ref=Ac*shr_05*concr.Ecm()

shr_10=concr.getShrEpscd(10,ts,RH,h0_beam1)+concr.getShrEpsca(10) 
N10Ref=Ac*shr_10*concr.Ecm()

shr_15=concr.getShrEpscd(15,ts,RH,h0_beam1)+concr.getShrEpsca(15)
N15Ref=Ac*shr_15*concr.Ecm()
shr_20=concr.getShrEpscd(20,ts,RH,h0_beam1)+concr.getShrEpsca(20)
N20Ref=Ac*shr_20*concr.Ecm()
shr_25=concr.getShrEpscd(25,ts,RH,h0_beam1)+concr.getShrEpsca(25)
shr_30=concr.getShrEpscd(30,ts,RH,h0_beam1)+concr.getShrEpsca(30)
N30Ref=Ac*shr_30*concr.Ecm()
N25Ref=Ac*shr_25*concr.Ecm()
N25Interp=(N30Ref+N20Ref)/2
# Geometry
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 2
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) #Defines the


xList=[0,1]
yList=[0,1]
gridGeom= gm.Grid2DModel(prep,xList,yList)
gridGeom.generatePoints()

beam1=gridGeom.genLinOneXYRegion(
    xyRange=((0,0),(1,0)),
    setName='beam1')
beam2=gridGeom.genLinOneXYRegion(
    xyRange=((0,1),(1,1)),
    setName='beam2')


beam_mat=typical_materials.defElasticShearSection2d(
    prep, "beam_mat",A=w_beam*th_beam,
    E=concr.Ecm(),G=concr.Ecm()/(2.0+1/0.3),
    I=1/12*w_beam*th_beam**3,alpha= 1)

#mesh
lin= modelSpace.newLinearCrdTransf("lin")
seedElemHandler= prep.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 2
seedElemHandler.defaultMaterial= 'beam_mat'
seedElemHandler.defaultTransformation='lin'
beam2d= seedElemHandler.newElement("ElasticBeam2d")
beam2d.h=th_beam
beam1.genMesh(xc.meshDir.I)
beam2.genMesh(xc.meshDir.I)
e1=beam1.getElements[0]
e2=beam2.getElements[0]

# Constraints
constraints= prep.getBoundaryCondHandler
for x in [0,1]:
    for y in yList:
        p=gridGeom.getPntXY([x,y])
        n=p.getNode()
        constraints.newSPConstraint(n.tag,0,0.0)
        constraints.newSPConstraint(n.tag,1,0.0)


## Define loads.
#### Create load case 0 (shrinkage in beam1)
ts= modelSpace.newTimeSeries(name= 'ts', tsType= "path_time_ts")
ts.time= xc.Vector(t_calc)
ts.path= xc.Vector(shrinkage_beam1) # factors that apply to the deformation value defined in the load patterns 
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
eleLoad= lp0.newElementalLoad("beam_strain_load")
etags=list()
for e in beam1.elements:
    etags.append(e.tag)
eleLoad.elementTags= xc.ID(etags)
shrDeformation= xc.DeformationPlane(1.0)
eleLoad.backEndDeformationPlane= shrDeformation
eleLoad.frontEndDeformationPlane= shrDeformation

#### Create load case 1 (shrinkage in beam-2)
ts_beam2= modelSpace.newTimeSeries(name= 'ts_beam2', tsType= "path_time_ts")
ts_beam2.time= xc.Vector(t_calc)
ts_beam2.path= xc.Vector(shrinkage_beam2) # factors that apply to the deformation value defined in the load patterns 
lp1= modelSpace.newLoadPattern(name= '1')
modelSpace.setCurrentLoadPattern(lp1.name)
eleLoad2= lp1.newElementalLoad("beam_strain_load")
etags2=list()
for e in beam2.elements:
    etags2.append(e.tag)
eleLoad2.elementTags= xc.ID(etags2)
shrDeformation= xc.DeformationPlane(1.0)
eleLoad2.backEndDeformationPlane= shrDeformation
eleLoad2.frontEndDeformationPlane= shrDeformation

# t=0. Add load case "0" to domain
modelSpace.addLoadCaseToDomain(lp0.name)

 ## Solution.
# Static analysis.
#solProc= predefined_solutions.PlainKrylovNewton(feProblem, convergenceTestTol= 1e-3, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 50, printFlag= 1)
solProc= predefined_solutions.PlainKrylovNewton(FEcase, convergenceTestTol= 1e-3, convTestType= 'norm_unbalance_conv_test', maxNumIter= 50, printFlag= 0)
solProc.setup()
solProc.integrator.dLambda1= 1
#t=5
result= solProc.analysis.analyze(5)
modelSpace.calculateNodalReactions(True, 1e-4)
N05Calc_beam1=e1.getN()

# t=10
result= solProc.analysis.analyze(5)
modelSpace.calculateNodalReactions(True, 1e-4)
N10Calc_beam1=e1.getN()

# t=15
result= solProc.analysis.analyze(5)
N15Calc_beam1=e1.getN()

modelSpace.addLoadCaseToDomain(lp1.name)

# t=20
result= solProc.analysis.analyze(5)
N20Calc_beam1=e1.getN()
N20Calc_beam2=e2.getN()

#t=25 (the value of shrinkage for t=25 is not given in the time-series path, so it's interpolated between t=20 ad t=30) 
result= solProc.analysis.analyze(5)
N25Calc_beam1=e1.getN()
N25Calc_beam2=e2.getN()

# t=30
result= solProc.analysis.analyze(5)
N30Calc_beam1=e1.getN()
N30Calc_beam2=e2.getN()


ratio1=abs((N05Ref+N05Calc_beam1)/N05Ref) + abs((N10Ref+N10Calc_beam1)/N10Ref) + abs((N15Ref+N15Calc_beam1)/N15Ref) + abs((N20Ref+N20Calc_beam1)/N20Ref)
ratio2=abs((N20Calc_beam2-N05Calc_beam1)/N05Calc_beam1)+abs((N30Calc_beam2-N15Calc_beam1)/N15Calc_beam1)
ratio3=abs((N25Interp+N25Calc_beam1)/N25Interp)
ratio4=abs((N25Ref+N25Calc_beam1)/N25Ref)

# print('N05Ref=',N05Ref)
# print('N05Calc=',N05Calc_beam1)
# print('N10Ref=',N10Ref)
# print('N10Calc=',N10Calc_beam1)
# print('N15Ref=',N15Ref)
# print('N15Calc=',N15Calc_beam1)
# print('N20Ref=',N20Ref)
# print('N20calc_beam1=',N20Calc_beam1)
# print('N20calc_beam2=',N20Calc_beam2)
# print('N25Ref=',N25Ref)
# print('N25calc_beam1=',N25Calc_beam1)
# print('N25calc_beam2=',N25Calc_beam2)
# print('N25Interp',N25Interp)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) and (abs(ratio2) <1e-15) and (abs(ratio3)<1e-15) and (abs(ratio4)<5e-3) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

#graphic stuff
# from postprocess import output_styles as outSty
# from postprocess import output_handler as outHndl
# sty=outSty.OutputStyle() 
# out=outHndl.OutputHandler(modelSpace,sty)
# out.displayFEMesh([beam1,beam2])
# out.displayReactions()
# out.displayIntForcDiag('N')
