# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

# This test carries out some verifications on the orientation of the elements'
# local axes, of the Reinforced Concrete (RC) sections and their respective
# axes, and the disposition of rebars (positive, negative, ..)
# The model consists of three 3d cantilever beams with the same geometry,
# but different orientations:
#    -The first one, parallel to X global axis, is designed with the strong
#    axis of the RC section parallel to the global Y axis
#    -The second cantilever beam follows the direction of the global Y axis
#    and the strong axis is vertical (parallel to Z global)
#    -The third, in Z global axis direction, has its strong axis parallel to
#    the bisector of XY (through an angle of 45ª with the positive X axis)
# The three beams share their first node at (0,0,0), which is fixed in all its
# 6 DOF.
# The same bending moment is applied to the three elements in the appropriate
# directions in order to produce the same results for the three cases.
# The criterion elected is to have tensional stresses in the Y-negative
# face of the beam 

import math
import os
import xc_base
import geom
import xc
import numpy as np
import csv

from materials.sections.fiber_section import def_simple_RC_section
from postprocess import RC_material_distribution
from materials.sia262 import SIA262_materials
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from materials.sia262 import SIA262_limit_state_checking
from model import model_inquiry as minq
from misc import matrix_utils
from postprocess.control_vars import *
from postprocess.config import default_config
import logging

#Hide INFO messages from modules.
rootLogger = logging.getLogger()
rootLogger.setLevel(logging.ERROR)


hbeam=0.5     #heigh of the rectangular RC section (in Y local axis direction)
wbeam=0.25    #width of the rectangular RC section (in Z local axis direction)
Lbeam=1.00    #length of the beams

#Loads
M=1e3   
F=1e3

# Rebars diameters: fi-10 in positive face (compressed), fi-16 in
# negative face (tensioned)
areaFi10= SIA262_materials.section_barres_courantes[10e-3] 
areaFi16= SIA262_materials.section_barres_courantes[16e-3]

concrete= SIA262_materials.c30_37
reinfSteel= SIA262_materials.SpecialII1956SIA161

# Define available sections for the elements (spatial distribution of RC sections).
# It refers to the reinforced concrete sections associated with the element
# (i.e. for shell elements we typically define two RC sections, one for each
# main direction; in the case of beam elements the most common way is to define
# RC sections in the front and back ends of the elements)

reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSectionDistribution.sectionDefinition #creates an RC sections container

#Generic layers (rows of rebars). Other instance variables that we can define
#for ReinfRows are coverLat and nRebars.If we define nRebars that
#value overrides the rebarsSpacing
fi10s75r30=def_simple_RC_section.ReinfRow(rebarsDiam=10e-3,areaRebar= areaFi10,rebarsSpacing=0.075,width=0.25,nominalCover=0.030)
fi16s75r30=def_simple_RC_section.ReinfRow(rebarsDiam=16e-3,areaRebar= areaFi16,rebarsSpacing=0.075,width=0.25,nominalCover=0.030)

#instances of def_simple_RC_section.RCSlabBeamSection that defines the
#variables that make up THE TWO reinforced concrete sections in the two
#reinforcement directions of a slab or the front and back ending sections
#of a beam element
beamRCsect=def_simple_RC_section.RCSlabBeamSection(name='beamRCsect',sectionDescr='beam section',concrType=concrete, reinfSteelType=reinfSteel,width=wbeam,depth=hbeam)
beamRCsect.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi10s75r30])
beamRCsect.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi16s75r30])
beamRCsect.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi10s75r30])
beamRCsect.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi16s75r30])
beamRCsect.creaTwoSections()
sections.append(beamRCsect)

feProblem= xc.FEProblem()
feProblem.errFileName= "/tmp/erase.err" # Don't print errors.
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the dimension of
                  #the space: nodes by three coordinates (x,y,z) and six
                  #DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)


nodes.defaultTag= 0 #First node number.
nod= nodes.newNodeXYZ(0,0.0,0.0)
nod= nodes.newNodeXYZ(Lbeam,0.0,0.0)
nod= nodes.newNodeXYZ(0.0,Lbeam,0.0)
nod= nodes.newNodeXYZ(0.0,0.0,Lbeam)


# Geometric transformations
trfs= preprocessor.getTransfCooHandler
# Coord. trasformation for beam in global X direction
#local Z axis of the element
# parallel to global -Y (it matches
# the direction of the local Z axis of the reinforced
# concrete sections (parallel to the width)
ltXbeam= modelSpace.newLinearCrdTransf("ltXbeam", xc.Vector([0,-1,0])) 

# Coord. trasformation for beam in global Y direction
# local Z axis of the element
# (parallel to global -Z)
ltYbeam= modelSpace.newLinearCrdTransf("ltYbeam",xc.Vector([0,0,-1]))

# Coord. trasformation for beam in global Z direction
# local Z axis
# of the element (45º with global +X)
ltZbeam= modelSpace.newLinearCrdTransf("ltZbeam",xc.Vector([1/math.sqrt(2),1/math.sqrt(2),0,1]))


# The local Z axis of the elements matches the direction of the local Z axis
# of the reinforced concrete sections (parallel to the width),
# that is why we define:
IzElem=beamRCsect.lstRCSects[0].getIz_RClocalZax()
IyElem=beamRCsect.lstRCSects[0].getIy_RClocalYax()
JElem= beamRCsect.lstRCSects[0].getJTorsion()   


# Materials definition
# A:            cross-sectional area of the section
# E:            Young’s modulus of material
# G:            Shear modulus of the material          
# Iz:           second moment of area about the local z-axis
# Iy:           second moment of area about the local y-axis 
# J:            torsional moment of inertia of the section

scc= typical_materials.defElasticSection3d(preprocessor=preprocessor, name="scc",A=beamRCsect.lstRCSects[0].getAc(),E=beamRCsect.lstRCSects[0].fiberSectionParameters.concrType.Ecm(),G=beamRCsect.lstRCSects[0].fiberSectionParameters.concrType.Gcm(),Iz=IzElem,Iy=IyElem,J=JElem)


# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "scc"
elements.defaultTag= 1 #Tag for next element.

elements.defaultTransformation= "ltXbeam"
beam3dX= elements.newElement("ElasticBeam3d",xc.ID([0,1]))
elements.defaultTransformation= "ltYbeam"
beam3dY= elements.newElement("ElasticBeam3d",xc.ID([0,2]))
elements.defaultTransformation= "ltZbeam"
beam3dZ= elements.newElement("ElasticBeam3d",xc.ID([0,3]))

    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(0,0,0.0) # Node 0
spc= constraints.newSPConstraint(0,1,0.0)
spc= constraints.newSPConstraint(0,2,0.0)
spc= constraints.newSPConstraint(0,3,0.0)
spc= constraints.newSPConstraint(0,4,0.0)
spc= constraints.newSPConstraint(0,5,0.0)

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
#Nodal Loads defined as [Fx,Fy,Fz,Mx,My,Mz] in the global coordinate system
lcXbeam= lPatterns.newLoadPattern("default","lcXbeam")
lcXbeam.newNodalLoad(1,xc.Vector([0,0,F,0,-M,0])) #loads applied at front end
lcYbeam= lPatterns.newLoadPattern("default","lcYbeam")
lcYbeam.newNodalLoad(2,xc.Vector([F,0,0,0,0,-M])) #loads applied at front end
lcZbeam= lPatterns.newLoadPattern("default","lcZbeam")
lcZbeam.newNodalLoad(3,xc.Vector([F/math.sqrt(2),-F/math.sqrt(2),0,M/math.sqrt(2),M/math.sqrt(2),0])) #loads applied at front end

# lcbeams= lPatterns.newLoadPattern("default","lcbeams")
# lcbeams.newNodalLoad(1,xc.Vector([0,0,F,0,-M,0])) # loads applied at front end
#                                                   # of beam X
# lcbeams.newNodalLoad(2,xc.Vector([F,0,0,0,0,-M])) #loads applied at front end
#                                                   # of beam Y
# lcbeams.newNodalLoad(3,xc.Vector([F/math.sqrt(2),-F/math.sqrt(2),0,M/math.sqrt(2),M/math.sqrt(2),0]))                             #loads applied at front end
#                                                   # of beam Z

# lPatterns.addToDomain("lcbeams")

# Solution
# analysis= predefined_solutions.simple_static_linear(feProblem)
# result= analysis.analyze(1)


# Load combinations
combContainer= combs.CombContainer()
combContainer.ULS.perm.add('allLoads', '1.0*lcXbeam+1.0*lcYbeam+1.0*lcZbeam')
totalSet= preprocessor.getSets.getSet('total')
cfg=default_config.EnvConfig(language='en',intForcPath= '',verifPath= '',annexPath= 'annex/',grWidth='120mm')
cfg.projectDirTree.workingDirectory= '/tmp/'
lsd.LimitStateData.envConfig= cfg
lsd.normalStressesResistance.saveAll(combContainer,totalSet) 

# Spatial distribution of reinforced concrete sections.
reinfConcreteSectionDistribution.assign(elemSet=totalSet.getElements,setRCSects=beamRCsect)

#Checking normal stresses.
limitStateLabel= lsd.normalStressesResistance.label
lsd.normalStressesResistance.controller= SIA262_limit_state_checking.BiaxialBendingNormalStressController(limitStateLabel)
lsd.normalStressesResistance.outputDataBaseFileName= 'resVerif'

# Using runChecking method we create the phantom model and run the checking on it. Unlike other check methods that also creates the phantom model this one doesn't clear the model after carrying out the verification. This method returns a tuple with the FE model (phantom model) and the result of verification

(FEcheckedModel,checkResult)=reinfConcreteSectionDistribution.runChecking(lsd.normalStressesResistance, matDiagType="d",threeDim= True)  

#Set with all the elements in the phantom model 
elements= FEcheckedModel.getPreprocessor.getSets.getSet('total').getElements

# Elements of the phantom model (ordered by the number of the section)
# associated with each of the beam elements
FMeBx=minq.get_attached_PhModElems(elemTag=beam3dX.tag,setElPhMod=elements)
#print(FMeBx[0].tag,FMeBx[1].tag)
FMeBy=minq.get_attached_PhModElems(elemTag=beam3dY.tag,setElPhMod=elements)
#print(FMeBy[0].tag,FMeBy[1].tag)
FMeBz=minq.get_attached_PhModElems(elemTag=beam3dZ.tag,setElPhMod=elements)
#print(FMeBz[0].tag,FMeBz[1].tag)

#checks
matIntForc=np.array(6*[np.array([0.0,0.0,0.0])]) #array to which import the resulting
                                           #[Fx,Fy,Fz] expressed in the element
                                           #local axes for the two sections of
                                           #each element
internalForces= lsd.readIntForcesDict("/tmp/intForce_ULS_normalStressesResistance.json")
internalForcesValues= internalForces[2]
for key in internalForcesValues.keys():
    intForces= internalForcesValues[key]
    for f in intForces:
        nrow=2*(int(f.tagElem)-1)+int(f.idSection)
        matIntForc[nrow]= np.array([float(f.N),float(f.Vy),float(f.Vz)]) #[Fx,Fy,Fz]
        
#We'll check the result of applying the coord. matrix to the vector of forces
#applied in the GCS is equal to the internal forces read from the file
#Local axes beam3dX 
lAxBeamX= matrix_utils.matrixToNumpyArray(beam3dX.getLocalAxes(True))
#Loads [Fx,Fy,Fz] applied on the element (GCS)
FbeamXGCS=np.array([0,0,F])
#internal forces read from the results file for elem1, section1
IFbeamXsec1=matIntForc[0]
ratio1_0=sum(np.dot(IFbeamXsec1,lAxBeamX)-FbeamXGCS)
#internal forces read from the results file for elem1, section2
IFbeamXsec2=matIntForc[1]
ratio1_1=sum(np.dot(IFbeamXsec2,lAxBeamX)-FbeamXGCS)

#Local axes beam3dY 
lAxBeamY=matrix_utils.matrixToNumpyArray(beam3dY.getLocalAxes(True))
#Loads [Fx,Fy,Fz] applied on the element (GCS)
FbeamYGCS=np.array([F,0,0])
#internal forces read from the results file for elem2, section1
IFbeamYsec1=matIntForc[2]
ratio2_0=sum(np.dot(IFbeamYsec1,lAxBeamY)-FbeamYGCS)
#internal forces read from the results file for elem2, section2
IFbeamYsec2=matIntForc[3]
ratio2_1=sum(np.dot(IFbeamYsec2,lAxBeamY)-FbeamYGCS)

#Local axes beam3dZ 
lAxBeamZ=matrix_utils.matrixToNumpyArray(beam3dZ.getLocalAxes(True))
#Loads [Fx,Fy,Fz] applied on the element (GCS)
FbeamZGCS=np.array([F/math.sqrt(2),-F/math.sqrt(2),0])
#internal forces read from the results file for elem2, section1
IFbeamZsec1=matIntForc[4]
ratio3_0=sum(np.dot(IFbeamZsec1,lAxBeamZ)-FbeamZGCS)
#internal forces read from the results file for elem2, section2
IFbeamZsec2=matIntForc[5]
ratio3_1=sum(np.dot(IFbeamZsec2,lAxBeamZ)-FbeamZGCS)

#results checks
# Internal forces (expressed in local axes) must give the same
# values for the RC section 1 in all the three beams
elsec1x=FMeBx[0]
Rsec1beamX=matrix_utils.vectorToNumpyArray(elsec1x.getResistingForce())
elsec1y=FMeBy[0]
Rsec1beamY=matrix_utils.vectorToNumpyArray(elsec1y.getResistingForce())
elsec1z=FMeBz[0]
Rsec1beamZ=matrix_utils.vectorToNumpyArray(elsec1z.getResistingForce())

ratio1=(Rsec1beamX-Rsec1beamY).sum()+(Rsec1beamX-Rsec1beamZ).sum()+(Rsec1beamY-Rsec1beamZ).sum()

# Internal forces (expressed in local axes) must give the same
# values for the RC section 2 in all the three beams
elsec2x=FMeBx[1]
Rsec2beamX=matrix_utils.vectorToNumpyArray(elsec2x.getResistingForce())
elsec2y=FMeBy[1]
Rsec2beamY=matrix_utils.vectorToNumpyArray(elsec2y.getResistingForce())
elsec2z=FMeBz[1]
Rsec2beamZ=matrix_utils.vectorToNumpyArray(elsec2z.getResistingForce())

ratio2=(Rsec2beamX-Rsec2beamY).sum()+(Rsec2beamX-Rsec2beamZ).sum()+(Rsec2beamY-Rsec2beamZ).sum()

#Check the internal forces written in the checking file
execfile('/tmp/resVerif.py')

sec1MzTeor=(M+F*Lbeam) # expressed in mkN
sec1beamXMz=preprocessor.getElementHandler.getElement(1).getProp("ULS_normalStressesResistanceSect1").Mz
sec1beamYMz=preprocessor.getElementHandler.getElement(2).getProp("ULS_normalStressesResistanceSect1").Mz
sec1beamZMz=preprocessor.getElementHandler.getElement(3).getProp("ULS_normalStressesResistanceSect1").Mz
ratio3=(sec1beamXMz-sec1MzTeor)+(sec1beamYMz-sec1MzTeor)+(sec1beamZMz-sec1MzTeor)

sec2MzTeor=M # expressed in mkN
sec2beamXMz=preprocessor.getElementHandler.getElement(1).getProp("ULS_normalStressesResistanceSect2").Mz
sec2beamYMz=preprocessor.getElementHandler.getElement(2).getProp("ULS_normalStressesResistanceSect2").Mz
sec2beamZMz=preprocessor.getElementHandler.getElement(3).getProp("ULS_normalStressesResistanceSect2").Mz
ratio4=(sec2beamXMz-sec2MzTeor)+(sec2beamYMz-sec2MzTeor)+(sec2beamZMz-sec2MzTeor)

ratios=[ratio1_0,ratio1_1,ratio2_0,ratio2_1,ratio3_0,ratio3_1,ratio1,ratio2,ratio3,ratio4]

result= 0.0
for r in ratios:
  result+= r*r
result= math.sqrt(result)
feProblem.errFileName= "cerr" # Display errors if any.
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (result<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
