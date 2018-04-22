# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

''' Evaluation of several geometrical and mechanical properties of a 
RC rectangular section under uniaxial bending moment.
The data are taken from Example 7.3 of the manual «EC2-worked examples»
titled «Evaluation of crack amplitude [EC2 clause 7.3.4]» 

The section is 400mm width by 600 mm height. A bending moment of 
300 kNm is applied.

The properties checked include:
- neutral axis
- bending plane
- compression plane
- tension plane
- internal forces axis
- lever arm
- effective depth
- effective concrete area

This properties can be shown in a figure plotted using fibSectFeaturesToplot 
function


'''

import xc_base
import geom
import xc
import math
from solution import predefined_solutions
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials import concrete_base
from materials import typical_materials
from materials.sections.fiber_section import fiber_sets
from materials.sections.fiber_section import sectionReport 
import matplotlib.pyplot as plt
from materials.ec2 import EC2_limit_state_checking

#Data from the experiment
width=0.4     #width (cross-section coordinate Y)
depth=0.6     #depth (cross-section coordinate Z)
f_ck=30e6      #concrete characteristic strength [Pa] (concrete C30-37 adopted)
f_ct=2.9*1e6   # concrete tensile strength
               # (in the test the calculated fctm=2896468.15 is used)

cover=0.04     #cover
A_s=2712e-6    #area of bottom reinforcement layer (6 fi 24)
A_sp=452e-6    #area of top reinforcement layer (4 fi 12)

ro_s=A_s/width/depth  #longitudinal steel ratio (not used)

ro_s_eff=0.05215      #effective ratio of reinforcement


M_y=-300e3      #bending moment [Nm]



#Other data
nDivIJ= 20  #number of cells (fibers) in the IJ direction (cross-section coordinate Y)
nDivJK= 20  #number of cells (fibers) in the JK direction (cross-section coordinate Z)

areaFi24=math.pi*(24e-3)**2/4.0
areaFi12=math.pi*(12e-3)**2/4.0


l= 1e-7     # Distance between nodes


# Model definition
problem=xc.FEProblem()              #necesary to create this instance of
                                     #the class xc.FEProblem()
preprocessor=problem.getPreprocessor
nodes= preprocessor.getNodeHandler     #nodes container
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)  #Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)


nodA= nodes.newNodeXYZ(1.0,0.0,0.0)     #node A defined by its (x,y) global coordinates
nodB= nodes.newNodeXYZ(1.0+l,0.0,0.0)   #node B defined by its (x,y) global coordinates

# Materials definition
concrete=EC2_materials.C30 #concrete according to EC2 fck=30 MPa      

#Reinforcing steel.
rfSteel=EC2_materials.S450C #reinforcing steel according to EC2 fyk=450 MPa
steelDiagram= rfSteel.defDiagK(preprocessor) #Definition of steel stress-strain diagram in XC. 

'''
#Parameters for tension stiffening of concrete
paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
concrete.tensionStiffparam=paramTS           #parameters for tension stiffening are assigned to concrete
'''
concrDiagram=concrete.defDiagK(preprocessor) #Definition of concrete stress-strain diagram in XC.
# Section geometry (rectangular 0.3x0.5, 20x20 cells)
geomSectFibers= preprocessor.getMaterialHandler.newSectionGeometry("geomSectFibers")
y1= width/2.0
z1= depth/2.0
#concrete region
regiones= geomSectFibers.getRegions
concrSect= regiones.newQuadRegion(concrete.nmbDiagK)
concrSect.nDivIJ= nDivIJ
concrSect.nDivJK= nDivJK
concrSect.pMin= geom.Pos2d(-y1,-z1)
concrSect.pMax= geom.Pos2d(+y1,+z1)

#reinforcement layers
reinforcement= geomSectFibers.getReinfLayers
#bottom layer (positive bending)
reinfBottLayer= reinforcement.newStraightReinfLayer(rfSteel.nmbDiagK) #Steel stress-strain diagram to use.
reinfBottLayer.numReinfBars= 6
reinfBottLayer.barArea= areaFi24
yBotL=(width-2*cover-0.024)/2.0
zBotL=-depth/2.0+cover+0.024/2.0
reinfBottLayer.p1= geom.Pos2d(-yBotL,zBotL) # center point position of the starting rebar
reinfBottLayer.p2= geom.Pos2d(yBotL,zBotL) # center point position of the starting rebar
#top layer (negative bending)
reinfTopLayer= reinforcement.newStraightReinfLayer(rfSteel.nmbDiagK) #Steel stress-strain diagram to use.
reinfTopLayer.numReinfBars= 4
reinfTopLayer.barArea= areaFi12
yTopL=(width-2*cover-0.012)/2.0
zTopL=depth/2.0-cover-0.012/2.0
reinfTopLayer.p1= geom.Pos2d(-yTopL,zTopL) # center point position of the starting rebar
reinfTopLayer.p2= geom.Pos2d(yTopL,zTopL) # center point position of the starting rebar

#Section material 
#it is a generic section created to be assigned to the elements specified
#its stress and strain state is neutral (if we ask this section for stress or strain
#values the result is always 0)
materiales= preprocessor.getMaterialHandler
sctFibers= materiales.newMaterial("fiber_section_3d","sctFibers")

fiberSectionRepr= sctFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSectFibers")
sctFibers.setupFibers()


# #report of the section material
# sectParam=sectionReport.SectionInfo(preprocessor=preprocessor,sectName='example_7.3_EC2W',sectDescr='Test example 7.3 EC2 Worked examples. Section definition',concrete=concrete,rfSteel=rfSteel,concrDiag=concrDiagram,rfStDiag=steelDiagram,geomSection=geomSectFibers,width=width,depth=depth) #Obtains section parameters for report
# sectParam.writeReport(archTex='figures/sections/secEx73.tex', pathFigura='figures/sections/secEx73.eps')


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial='sctFibers'
elements.dimElem= 3 # Dimension of element space
ele1= elements.newElement("ZeroLengthSection",xc.ID([nodA.tag,nodB.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler      #constraints container
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0FF(nodB.tag)
# Loads definition
cargas= preprocessor.getLoadHandler   #loads container

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")

pointLoad= xc.Vector([0.0,0.0,0.0,0,M_y,0.0])  #flexión simple recta

lp0.newNodalLoad(nodB.tag,pointLoad)    #applies the point load on node B 

#We add the load case to domain.
casos.addToDomain("0")           #reads load pattern "0" and adds it to the domain

# Solve
solution=  predefined_solutions.SolutionProcedure()
solution.convergenceTestTol= 1e-7
analysis= solution.simpleStaticModifiedNewton(problem)
#analysis= predefined_solutions.simple_static_modified_newton(problem)
analOk= analysis.analyze(1)


nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)
#nodB.checkReactionForce(0.5)

#section of element 1: it's the copy of the material section 'sctFibers' assigned
#to element 1 and specific of this element. It has the tensional state of the element
sccEl1= ele1.getSection()         
fibersSccEl1= sccEl1.getFibers()

#Creation of two separate sets of fibers: concrete and reinforcement steel 
setsRCEl1= fiber_sets.fiberSectionSetupRCSets(scc=sccEl1,concrMatTag=concrete.matTagK,concrSetName="concrSetFbEl1",reinfMatTag=rfSteel.matTagK,reinfSetName="reinfSetFbEl1")

#Equilibrium.
RNA= nodA.getReaction[0]
RNB= nodB.getReaction[0]
ratio0= math.sqrt(RNA**2+RNB**2)/(width*depth*f_ck)

#Neutral axis depth
x= sccEl1.getNeutralAxisDepth()
xTeor= -0.184594790641
ratio1=(x-xTeor)/x
#Neutral axis plane (parallel to section's Y-axis)
zNA=sccEl1.getNeutralAxis().getParamB()
zNATeor= 0.115405209359 #0.13263453242377543
ratio2=(zNA-zNATeor)/zNA
#checking neutral axis depth vs. neutral axis plane calculations
checkNA=zNA-x
ratio3=(checkNA-depth/2.)/(depth/2.)

#Bending plane (parallel to section's Z-axis)
slopeBP=sccEl1.getBendingPlaneTrace().getParamA()
#Compression plane (parallel to section's Y-axis)
zCP=sccEl1.getCompressedPlaneTrace().getParamB()
zCPTeor= 0.237277791614
ratio4=(zCP-zCPTeor)/zCP
#Tension plane (parallel to section's Y-axis)
zTP=sccEl1.getTensionedPlaneTrace().getParamB()
zTPTeor=-0.248
ratio5=(zTP-zTPTeor)/zTP
#Internal forces axis (uniaxial bending, parallel to section's Y-axis)
zIFA=sccEl1.getInternalForcesAxis().getParamB()
zIFATeor=0.0
ratio6=zIFA-zIFATeor
#Lever arm
levArm=sccEl1.getSegmentoBrazoMecanico().getLongitud()
levArmTeor= 0.485277791614
ratio7=(levArm-levArmTeor)/levArm
#Lever arm
levArm2=sccEl1.getMechanicLeverArm()
ratio8=(levArm-levArm2)/levArm
#check lever arm
ratio9=(levArm-(zCP-zTP))
#Effective depth
effDepth=sccEl1.getSegmentoCantoUtil().getLongitud()
effDepthTeor=0.548
ratio10=(effDepth-effDepthTeor)/effDepth
#check effective depth
ratio11=effDepth-(depth/2.-zTP)
# maximum effective concrete height
heffmax_EC2=min(2.5*(depth-effDepth),(depth+x)/3.,depth/2.0)
heffmax_EC2_Teor=0.13
ratio12=(heffmax_EC2-heffmax_EC2_Teor)/heffmax_EC2
#Limit of concrete effective area (parallel to section's Y-axis)
zEffConcA=sccEl1.getEffectiveConcreteAreaLimitLine(heffmax_EC2).getParamB()
zEffConcATeor=-(depth/2.-heffmax_EC2)
ratio13=(zEffConcA-zEffConcATeor)/zEffConcA

'''
print 'ratio0= ', ratio0
print 'x= ', x
print 'xTeor= ', xTeor
print 'ratio1= ', ratio1
print 'zNA=', zNA
print 'zNATeor=', zNATeor
print 'ratio2= ', ratio2
print 'ratio3= ', ratio3
print 'zCP=', zCP
print 'zCPTeor=', zCPTeor
print 'ratio4= ', ratio4
print 'ratio5= ', ratio5
print 'ratio6= ', ratio6
print 'levArm=', levArm
print 'levArmTeor=', levArmTeor
print 'ratio7= ', ratio7
print 'ratio8= ', ratio8
print 'ratio9= ', ratio9
print 'ratio10= ', ratio10
print 'ratio11= ', ratio11
print 'ratio12= ', ratio12
print 'ratio13= ', ratio13
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio0)<1e-7) & (abs(ratio1)<1e-5) & (abs(slopeBP)>1e15) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5) & (abs(ratio5)<1e-5) & (abs(ratio6)<1e-5) & (abs(ratio7)<1e-5) & (abs(ratio8)<1e-5) & (abs(ratio9)<1e-5) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5) & (abs(ratio12)<1e-5)  & (abs(ratio13)<1e-5)  )  :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

'''
from materials.sections.fiber_section import plot_fiber_section as pfs

fsPlot=pfs.fibSectFeaturesToplot(fiberSection=sccEl1)
fsPlot.colorNeutralAxis = 'r'
fsPlot.colorBendingPlane='g'
fsPlot.colorCompressionPlane='b'
fsPlot.colorTensionPlane='m'
fsPlot.colorIntForcAxis='c'
fsPlot.colorLeverArm='orange'
fsPlot.colorEffDepth='purple'
fsPlot.colorEffConcrArea='brown'
fsPlot.MaxEffHeight=heffmax_EHE
fsPlot.colorGrossEffConcrAreaContours='m'
fig1,ax2d=fsPlot.generatePlot()
fig1.show()
fig1.savefig('fig1.png')
'''


