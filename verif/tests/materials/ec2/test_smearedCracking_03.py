# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

''' Evaluation of crack amplitude in a RC rectangular section under
bending moment.
The data are taken from Example 7.3 of the manual «EC2-worked examples»
titled «Evaluation of crack amplitude [EC2 clause 7.3.4]» 

The section is 400mm width by 600 mm height. A bending moment of 
300 kNm is applied
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
from materials.sections.fiber_section import section_report 
import numpy as np
from materials.ec2 import EC2_limit_state_checking
from misc_utils import log_messages as lmsg

# Data from the experiment
width=0.4     # width (cross-section coordinate Y)
depth=0.6     # depth (cross-section coordinate Z)
f_ck=30e6      # concrete characteristic strength [Pa] (concrete C30-37 adopted)
f_ct=2.9*1e6   # concrete tensile strength
               # (in the test the calculated fctm=2896468.15 is used)

cover=0.04     # cover
A_s=2712e-6    # area of bottom reinforcement layer (6 fi 24)
A_sp=452e-6    # area of top reinforcement layer (4 fi 12)

ro_s=A_s/width/depth  # longitudinal steel ratio (not used)

ro_s_eff=0.05215      # effective ratio of reinforcement


M_y=-300e3      # bending moment [Nm]



# Other data
nDivIJ= 20  # number of cells (fibers) in the IJ direction (cross-section coordinate Y)
nDivJK= 20  # number of cells (fibers) in the JK direction (cross-section coordinate Z)

areaFi24=math.pi*(24e-3)**2/4.0
areaFi12=math.pi*(12e-3)**2/4.0


l= 1e-7     # Distance between nodes


# Model definition
problem=xc.FEProblem()              # necesary to create this instance of
                                     # the class xc.FEProblem()
preprocessor=problem.getPreprocessor
nodes= preprocessor.getNodeHandler     # nodes container
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)  # Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)


nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(1.0,0,0)     # node 1 defined by its (x,y,z) global coordinates
nod= nodes.newNodeXYZ(1.0+l,0,0)   # node 2 defined by its (x,y,z) global coordinates

# Materials definition
concrete=EC2_materials.C30 # concrete according to EC2 fck=30 MPa      

# Reinforcing steel.
rfSteel=EC2_materials.S450C # reinforcing steel according to EC2 fyk=450 MPa
steelDiagram= rfSteel.defDiagK(preprocessor) # Definition of steel stress-strain diagram in XC. 


# Parameters for tension stiffening of concrete
paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
concrete.tensionStiffparam=paramTS           # parameters for tension stiffening are assigned to concrete
concrDiagram=concrete.defDiagK(preprocessor) # Definition of concrete stress-strain diagram in XC.

# Section geometry (rectangular 0.3x0.5, 20x20 cells)
geomSectFibers= preprocessor.getMaterialHandler.newSectionGeometry("geomSectFibers")
y1= width/2.0
z1= depth/2.0
# concrete region
regions= geomSectFibers.getRegions
concrSect= regions.newQuadRegion(concrete.nmbDiagK)
concrSect.nDivIJ= nDivIJ
concrSect.nDivJK= nDivJK
concrSect.pMin= geom.Pos2d(-y1,-z1)
concrSect.pMax= geom.Pos2d(+y1,+z1)

# reinforcement layers
reinforcement= geomSectFibers.getReinfLayers
# bottom layer (positive bending)
reinfBottLayer= reinforcement.newStraightReinfLayer(rfSteel.nmbDiagK) # Steel stress-strain diagram to use.
reinfBottLayer.numReinfBars= 6
reinfBottLayer.barArea= areaFi24
yBotL=(width-2*cover-0.024)/2.0
zBotL=-depth/2.0+cover+0.024/2.0
reinfBottLayer.p1= geom.Pos2d(-yBotL,zBotL) # center point position of the starting rebar
reinfBottLayer.p2= geom.Pos2d(yBotL,zBotL) # center point position of the starting rebar
# top layer (negative bending)
reinfTopLayer= reinforcement.newStraightReinfLayer(rfSteel.nmbDiagK) # Steel stress-strain diagram to use.
reinfTopLayer.numReinfBars= 4
reinfTopLayer.barArea= areaFi12
yTopL=(width-2*cover-0.012)/2.0
zTopL=depth/2.0-cover-0.012/2.0
reinfTopLayer.p1= geom.Pos2d(-yTopL,zTopL) # center point position of the starting rebar
reinfTopLayer.p2= geom.Pos2d(yTopL,zTopL) # center point position of the starting rebar

# Section material 
# it is a generic section created to be assigned to the elements specified
# its stress and strain state is neutral (if we ask this section for stress or strain
# values the result is always 0)
materialHandler= preprocessor.getMaterialHandler
sctFibers= materialHandler.newMaterial("fiber_section_3d","sctFibers")
fiberSectionRepr= sctFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(geomSectFibers.name)
sctFibers.setupFibers()


# # report of the section material
# sectParam=section_report.SectionInfo(preprocessor=preprocessor,sectName='example_7.3_EC2W',sectDescr='Test example 7.3 EC2 Worked examples. Section definition',concrete=concrete,rfSteel=rfSteel,concrDiag=concrDiagram,rfStDiag=steelDiagram,geomSection=geomSectFibers,width=width,depth=depth) # Obtains section parameters for report
# sectParam.writeReport(archTex='figures/sections/secEx73.tex', pathFigura='figures/sections/secEx73.eps')


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial='sctFibers'
elements.dimElem= 1 # Dimension of element space
elements.defaultTag= 1
elem= elements.newElement("ZeroLengthSection",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler      # constraints container
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_0F0(2)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
pointLoad=xc.Vector([0,0,0,0,M_y,0])
lp0.newNodalLoad(2,pointLoad)    # applies the point load on node 2 

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name) # reads load pattern "0" and adds it to the domain

# Solve
solProc= predefined_solutions.PlainStaticModifiedNewton(problem, convergenceTestTol= 1e-8)
analOk= solProc.solve()
if(analOk!=0):
    lmsg.error('Failed to solve for: '+lp0.name)
    quit()

# printing results
nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-6)

'''
RN1= nodes.getNode(1).getReaction[0]   # Axial FX reaction (constrained DOF: ux) at node 1
RQY1= nodes.getNode(1).getReaction[1]   # Vertical FY reaction (constrained DOF: uY) at node 1
RQZ1= nodes.getNode(1).getReaction[2]   # Vertical FY reaction (constrained DOF: uZ) at node 1
RMX1= nodes.getNode(1).getReaction[3]   # Bending moment Mx reaction at node 1
RMY1= nodes.getNode(1).getReaction[4]   # Bending moment My reaction at node 1
RMZ1= nodes.getNode(1).getReaction[5]   # Bending moment Mz reaction at node 1


RN2= nodes.getNode(2).getReaction[0]   # Axial FX reaction (constrained DOF: ux) at node 2
RQY2= nodes.getNode(2).getReaction[1]   # Vertical FY reaction (constrained DOF: uY) at node 2
RQZ2= nodes.getNode(2).getReaction[2]   # Vertical FY reaction (constrained DOF: uZ) at node 2
RMX2= nodes.getNode(2).getReaction[3]   # Bending moment Mx reaction at node 2
RMY2= nodes.getNode(2).getReaction[4]   # Bending moment My reaction at node 2
RMZ2= nodes.getNode(2).getReaction[5]   # Bending moment Mz reaction at node 2

print('Rnode1= (',nodes.getNode(1).getReaction[0],',',nodes.getNode(1).getReaction[1],',',nodes.getNode(1).getReaction[2],',',nodes.getNode(1).getReaction[3],',',nodes.getNode(1).getReaction[4],',',nodes.getNode(1).getReaction[5],')')
print('Rnode2= (',nodes.getNode(2).getReaction[0],',',nodes.getNode(2).getReaction[1],',',nodes.getNode(2).getReaction[2],',',nodes.getNode(2).getReaction[3],',',nodes.getNode(2).getReaction[4],',',nodes.getNode(2).getReaction[5],')')
'''
elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
# section of element 1: it's the copy of the material section 'sctFibers' assigned
# to element 1 and specific of this element. It has the tensional state of the element
sccEl1= ele1.getSection()         
fibersSccEl1= sccEl1.getFibers()

# Creation of two separate sets of fibers: concrete and reinforcement steel 
setsRCEl1= fiber_sets.fiberSectionSetupRCSets(scc=sccEl1,concrMatTag=concrete.matTagK,concrSetName="concrSetFbEl1",reinfMatTag=rfSteel.matTagK,reinfSetName="reinfSetFbEl1")

sumAreas= fibersSccEl1.getArea(1.0)  # total sum of the fibers area
                                    # that sum is multiplied by the coefficient
                                    # passed as a parameter

Iz= fibersSccEl1.getIz(1.0,0.0)
IEIy= sccEl1.getInitialTangentStiffness().at(3,3)
IEIz= sccEl1.getInitialTangentStiffness().at(2,2)
TEIy= sccEl1.getTangentStiffness().at(3,3)
TEIz= sccEl1.getTangentStiffness().at(2,2)
Iy= fibersSccEl1.getIy(1.0,0.0)
esfN= fibersSccEl1.getResultant()
esfMy= fibersSccEl1.getMy(0.0)
esfMz= fibersSccEl1.getMz(0.0)
defMz= sccEl1.getSectionDeformationByName("defMz")
defN= sccEl1.getSectionDeformationByName("defN")
x= sccEl1.getNeutralAxisDepth()
Resul= sccEl1.getStressResultant()
Deform= sccEl1.getSectionDeformation()



# Results for the concrete fibers in section of element 1

fibraCEpsMin= -1
fConcrMin= setsRCEl1.concrFibers.getFiberWithMinStrain()
epsCMin= fConcrMin.getMaterial().getStrain() # minimum strain among concrete fibers
sgCMin= fConcrMin.getMaterial().getStress()  # stress in the concrete fiber with minimum strain
YepsCMin= fConcrMin.getPos().x # y coord. of the concrete fiber with minimum strain
ZepsCMin= fConcrMin.getPos().y # z coord. of the concrete fiber with minimum strain

fConcrMax= setsRCEl1.concrFibers.getFiberWithMaxStrain()
epsCMax= fConcrMax.getMaterial().getStrain() # maximum strain among concrete fibers
sgCMax= fConcrMax.getMaterial().getStress()  # stress in the concrete fiber with maximum strain
YepsCMax= fConcrMax.getPos().x # y coord. of the concrete fiber with maximum strain
ZepsCMax= fConcrMax.getPos().y # z coord. of the concrete fiber with maximum strain


# Results for the steel fibers in section of element 1
fReinfMax= setsRCEl1.reinfFibers.getFiberWithMaxStrain()
epsSMax= fReinfMax.getMaterial().getStrain() # maximum strain among steel fibers
sgSMax= fReinfMax.getMaterial().getStress() # stress in the steel fiber with maximum strain
YepsSMax= fReinfMax.getPos().x # y coord. of the steel fiber with maximum strain
ZepsSMax= fReinfMax.getPos().y # z coord. of the steel fiber with maximum strain

'''
print("sumAreas= ",(sumAreas))
print("Iz= ",(Iz))
print("IEIz= ",IEIz)
print("TEIz= ",TEIz)
print("E1= ",(IEIz/Iz))
print("Iy= ",(Iy))
print("IEIy= ",IEIy)
print("TEIy= ",TEIy)
print("E2= ",(IEIy/Iy))
print("Neutral fiber depth: ",x," m")
print("Sum of normal stresses: ",Resul*1e-3," kN")
print("Strain: ",Deform*1e3,"E-3")

print("\nMinimum strain in concrete fibers:: ",(epsCMin*1E3),"E-3")
print("Minimum stress in concrete fibers: ",(sgCMin/1e6),"E6")

print("Y coordinate of the concrete fiber with minimum strain: ",(YepsCMin))
print("Z coordinate of the concrete fiber with minimum strain: ",(ZepsCMin))

print("\nMinimum strain in steel fibers: ",(epsSMin*1E3),"E-3")
print("Minimum stress in steel fibers: ",(sgSMin/1e6),"E6")
print("Y coordinate of the steel fiber with minimum strain: ",(YepsSMin))
print("Z coordinate of the steel fiber with minimum strain: ",(ZepsSMin))


print("\nMaximum strain in steel fibers: ",(epsSMax*1E3),"E-3")
print("Maximum stress in steel fibers: ",(sgSMax/1e6),"E6")
print("Y coordinate of the steel fiber with maximum strain: ",(YepsSMax))
print("Z coordinate of the steel fiber with maximum strain: ",(ZepsSMax))
'''

#              *Crack width calculation*

# depth of the effective area:
hceff=EC2_limit_state_checking.h_c_eff(depth_tot=depth,depht_eff=depth-cover-0.024/2.0,depth_neutral_axis=abs(x))
# print('depth of the effective area: ',hceff,' m')
# effective reinforcement ratio
roseff=EC2_limit_state_checking.ro_eff(A_s=A_s,width=width,h_c_eff=hceff)
# print('effective reinforcement ratio: ',roseff)
# maximum crack spacing
srmax=EC2_limit_state_checking.s_r_max(k1=0.8,k2=0.5,k3=3.4,k4=0.425,cover=cover,fiReinf=0.024,ro_eff=roseff)
# print('maximum crack spacing: ',srmax,' m')
# mean strain in the concrete between cracks
eps_cm=concrete.fctm()/concrete.E0()/2.0
# mean strain in the reinforcemen takin into account the effects of tension stiffening
eps_sm=epsSMax
# crack withs
w_k=srmax*(eps_sm-eps_cm)
# print('crack widths: ',w_k*1e3, ' mm')

xComp= -0.20590984200810822
epsCMinComp= -0.0005346317226149066
sgCMinComp= -13895218.58725083
epsSMaxComp= 0.0009580032571031238
sgSMaxComp= 190382375.699
hceffComp= 0.13
roseffComp= 0.052153846153
srmaxComp= 0.214230088496
eps_cmComp= 4.82744692303e-05
w_kComp= 0.00019489127873297142 # 22-04-2018 to Ana to look: 0.000184426020443

ratio1=(xComp-x)/xComp
ratio2=(epsCMinComp- epsCMin)/epsCMinComp
ratio3=(sgCMinComp- sgCMin)/sgCMinComp
ratio4=(epsSMaxComp- epsSMax)/epsSMaxComp
ratio5=(sgSMaxComp- sgSMax)/sgSMaxComp
ratio6=(hceffComp-hceff)/hceffComp
ratio7=(roseffComp-roseff)/roseffComp
ratio8=(srmaxComp-srmax)/srmaxComp
ratio9=(eps_cmComp-eps_cm)/eps_cmComp
ratio10=(w_kComp-w_k)/w_kComp

'''
print('x= ', x)
print('xComp= ', xComp)
print('ratio1= ', ratio1)
print('epsCMin= ', epsCMin)
print('epsCMinComp= ', epsCMinComp)
print('ratio2= ', ratio2)
print('sgCMin= ', sgCMin)
print('sgCMinComp= ', sgCMinComp)
print('ratio3= ', ratio3)
print('epsSMax= ', epsSMax)
print('epsSMaxComp= ', epsSMaxComp)
print('ratio4= ', ratio4)
print('sgSMax= ', sgSMax)
print('sgSMaxComp= ', sgSMaxComp)
print('ratio5= ', ratio5)
print('hceff= ', hceff)
print('hceffComp= ', hceffComp)
print('ratio6= ', ratio6)
print('roseff= ', roseff)
print('roseffComp= ', roseffComp)
print('ratio7= ', ratio7)
print('srmax= ', srmax)
print('srmaxComp= ', srmaxComp)
print('ratio8= ', ratio8)
print('eps_cm= ', eps_cm)
print('eps_cmComp= ', eps_cmComp)
print('ratio9= ', ratio9)
print('w_k= ', w_k)
print('w_kComp= ', w_kComp)
print('ratio10= ', ratio10)
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')




####  FIGURES & REPORTS
# plotting of section geometric and mechanical properties
# from materials.sections.fiber_section import plotSectionGeometry as pg
# pg.plotSectionGeometry(geomSectFibers,'./secEx74.eps')

# sectInf=section_report.SectionInfo(preprocessor=preprocessor,section=sctFibers) # Obtains section parameters for report
# sectInf.writeReport(archTex='./secEx74.tex', pathFigura='./secEx74.eps')


# # plot cross-section strains and stresses 
# from postprocess import utils_display
# '''
#   fiberSet: set of fibers to be represented
#   title:    general title for the graphic
#   fileName: name of the graphic file (defaults to None: no file generated)
#   nContours: number of contours to be generated (defaults to 100)
# ''' 
# # utils_display.plotStressStrainFibSet(fiberSet=fibersSccEl1,title='cross-section fibers',fileName='problem.jpeg')
# utils_display.plotStressStrainFibSet(fiberSet=setsRCEl1.concrFibers.fSet,title='cross-section concrete fibers',fileName='problem.jpeg')


# # report concrete material
# from postprocess.reports import report_material

# report_material.report_concrete02(concrDiag=concrDiagram,paramTensStiffening=paramTS,grTitle='Ex. 7.3 EC2W. Concrete $\sigma-\epsilon$ curve',grFileName='figures/material/ex7_3EC2W',texFileName='figures/material/ex7_3EC2W.tex')


