# -*- coding: utf-8 -*-
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

This properties can be shown in a figure plotted using FibSectFeaturesToplot 
function
'''
__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "


import os
import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import fiber_sets
from materials.sections.fiber_section import plot_fiber_section 

#Data from the experiment
width=0.4 # width (cross-section coordinate Y)
depth=0.6 # depth (cross-section coordinate Z)
cover=0.04 # cover
A_s=2712e-6 # area of bottom reinforcement layer (6 fi 24)
A_sp=452e-6 # area of top reinforcement layer (4 fi 12)
M_y=-300e3 # bending moment [Nm]

#Other data
nDivIJ= 20  #number of cells (fibers) in the IJ direction (cross-section coordinate Y)
nDivJK= 20  #number of cells (fibers) in the JK direction (cross-section coordinate Z)
areaFi24=math.pi*(24e-3)**2/4.0
areaFi12=math.pi*(12e-3)**2/4.0
l= 1e-7     # Distance between nodes


# Model definition
feProblem=xc.FEProblem()              
preprocessor=feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler     #nodes container
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)  #Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

#Nodes
nodA= nodes.newNodeXYZ(1.0,0.0,0.0)     #node A defined by its (x,y) global coordinates
nodB= nodes.newNodeXYZ(1.0+l,0.0,0.0)   #node B defined by its (x,y) global coordinates

# Materials definition
concrete=EC2_materials.C30 #concrete according to EC2 fck=30 MPa      

#Reinforcing steel.
rfSteel=EC2_materials.S450C #reinforcing steel according to EC2 fyk=450 MPa
steelDiagram= rfSteel.defDiagK(preprocessor) #Definition of steel stress-strain diagram in XC. 

concrDiagram=concrete.defDiagK(preprocessor) #Definition of concrete stress-strain diagram in XC.

# Section geometry (rectangular 0.3x0.5, 20x20 cells)
geomSectFibers= preprocessor.getMaterialHandler.newSectionGeometry("geomSectFibers")
y1= width/2.0
z1= depth/2.0
#concrete region
regions= geomSectFibers.getRegions
concrSect= regions.newQuadRegion(concrete.getKDiagName())
concrSect.nDivIJ= nDivIJ
concrSect.nDivJK= nDivJK
concrSect.pMin= geom.Pos2d(-y1,-z1)
concrSect.pMax= geom.Pos2d(+y1,+z1)

#reinforcement layers
reinforcement= geomSectFibers.getReinfLayers
#bottom layer (positive bending)
reinfBottLayer= reinforcement.newStraightReinfLayer(rfSteel.getKDiagName()) #Steel stress-strain diagram to use.
reinfBottLayer.numReinfBars= 6
reinfBottLayer.barArea= areaFi24
yBotL=(width-2*cover-0.024)/2.0
zBotL=-depth/2.0+cover+0.024/2.0
reinfBottLayer.p1= geom.Pos2d(-yBotL,zBotL) # center point position of the starting rebar
reinfBottLayer.p2= geom.Pos2d(yBotL,zBotL) # center point position of the starting rebar
#top layer (negative bending)
reinfTopLayer= reinforcement.newStraightReinfLayer(rfSteel.getKDiagName()) #Steel stress-strain diagram to use.
reinfTopLayer.numReinfBars= 4
reinfTopLayer.barArea= areaFi12
yTopL=(width-2*cover-0.012)/2.0
zTopL=depth/2.0-cover-0.012/2.0
reinfTopLayer.p1= geom.Pos2d(-yTopL,zTopL) # center point position of the starting rebar
reinfTopLayer.p2= geom.Pos2d(yTopL,zTopL) # center point position of the starting rebar

# Create fiber section.
sctFibers= geomSectFibers.getFiberSection3d("sctFibers")

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= sctFibers.name
ele1= elements.newElement("ZeroLengthSection",xc.ID([nodA.tag,nodB.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler      #constraints container
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0FF(nodB.tag)

# Loads definition
loads= preprocessor.getLoadHandler   #loads container
lpatt= loads.getLoadPatterns

#Load modulation.
ts= lpatt.newTimeSeries("constant_ts","ts")
lpatt.currentTimeSeries= "ts"
#Load case definition
lp0= lpatt.newLoadPattern("default","0")

pointLoad= xc.Vector([0.0,0.0,0.0,0,M_y,0.0])  #flexión simple recta

lp0.newNodalLoad(nodB.tag,pointLoad)    #applies the point load on node B 

#We add the load case to domain.
lpatt.addToDomain("0")           #reads load pattern "0" and adds it to the domain

# Solve
solProc= predefined_solutions.PlainStaticModifiedNewton(feProblem, convergenceTestTol= 1e-9)
analOk= solProc.solve()
if(analOk!=0):
    lmsg.error('Failed to solve for: '+lp0.name)
    quit()

nodes.calculateNodalReactions(True,1e-7)
#nodB.checkReactionForce(0.5)

#section of element 1: it's the copy of the material section 'sctFibers' 
#assigned to element 1 and specific of this element. It has the tensional state
#of the element
sccEl1= ele1.getSection()         
fibersSccEl1= sccEl1.getFibers()

#Equilibrium.
RNA= nodA.getReaction[0]
RNB= nodB.getReaction[0]

#Neutral axis depth
x= sccEl1.getNeutralAxisDepth()
#Neutral axis plane (parallel to section's Y-axis)
zNA=sccEl1.getNeutralAxis().getParamB()
#checking neutral axis depth vs. neutral axis plane calculations
checkNA=zNA-x
ratio3=(checkNA-depth/2.)/(depth/2.)

#Bending plane (parallel to section's Z-axis)
slopeBP=sccEl1.getBendingPlaneTrace().getParamA()
#Compression plane (parallel to section's Y-axis)
zCP=sccEl1.getCompressedPlaneTrace().getParamB()
#Tension plane (parallel to section's Y-axis)
zTP=sccEl1.getTensionedPlaneTrace().getParamB()
#Internal forces axis (uniaxial bending, parallel to section's Y-axis)
zIFA=sccEl1.getInternalForcesAxes().getParamB()
#Lever arm
levArm=sccEl1.getLeverArmSegment().getLength()
#Lever arm
levArm2=sccEl1.getMechanicLeverArm()
#check lever arm
ratio9=(levArm-(zCP-zTP))
#Effective depth
effDepth=sccEl1.getEffectiveDepthSegment().getLength()
#check effective depth
ratio11=effDepth-(depth/2.-zTP)
# maximum effective concrete height
heffmax_EC2=min(2.5*(depth-effDepth),(depth+x)/3.,depth/2.0)
#Limit of concrete effective area (parallel to section's Y-axis)
zEffConcA=sccEl1.getEffectiveConcreteAreaLimitLine(heffmax_EC2).getParamB()


fname= os.path.basename(__file__)
# outputFileName= './'+fname.removesuffix('.py')+'.png'
outputFileName= '/tmp/'+fname.removesuffix('.py')+'.png'

# Plot results.
fsPlot=plot_fiber_section.FibSectFeaturesToplot(fiberSection=sccEl1)
fsPlot.colorNeutralAxis = 'r'
fsPlot.colorBendingPlane='g'
fsPlot.colorCompressionPlane='b'
fsPlot.colorTensionPlane='m'
fsPlot.colorIntForcAxis='c'
fsPlot.colorLeverArm='orange'
fsPlot.colorEffDepth='purple'
fsPlot.colorEffConcrArea='brown'
fsPlot.MaxEffHeight=heffmax_EC2
fsPlot.colorGrossEffConcrAreaContours='m'
fig1,ax2d= fsPlot.generatePlot()
fig1.savefig(outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

import os
from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.


