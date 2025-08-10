# -*- coding: utf-8 -*-
''' Test functions that report results from PotBearing class objects.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import json
import xc
from model import predefined_spaces
from materials import bridge_bearings
from solution import predefined_solutions
from actions import combinations as combs
from postprocess.config import default_config # Post-processing.

def store_pot_results(potBearings, lcName, potResults):
    ''' Return a dictionary containing the forces on the bearing pots of the
        pergola.

    :param potBearings: potBearings to extract the results from.
    :param lcName: load case name to which the results correspond.
    :param potResults: Python dictionary to store the results.
    '''
    for pb in potBearings:
        results= pb.getResultsDict()
        if(pb.name not in potResults):
            potResults[pb.name]= dict()
        potResults[pb.name][lcName]= results
    return potResults

FX= 1.0e4 # Force magnitude
FY= 2.0e4 
FZ= 3.0e4
MX= 4.0
MY= 5.0 
MZ= 6.0
loadVector= xc.Vector([FX,FY,FZ,MX,MY,MZ])

ang=30 # counterclockwise angle (degrees) between local and global X-axes
angRad=math.radians(ang)
vDirXlocal=xc.Vector([math.cos(angRad),math.sin(angRad),0])
vDirYlocal=xc.Vector([-math.sin(angRad),math.cos(angRad),0])


# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Some POT materials.
## d: pot diameter.
## unidirX: unidirectional POT in local-X direction (uY constrained).
## unidirY: unidirectional POT in local-Y direction (uX constrained).
## deltaFrict: Displacement when the friction force is reached.
## Fperp: mean compressive force perperdicular to the pot surface.
pot_matA= bridge_bearings.PTFEPotBearingMat(d= 993e-3, unidirX=False,unidirY=False)
pot_matA.name= 'A'
pot_matB= bridge_bearings.PTFEPotBearingMat(d= 396e-3, unidirX=True, unidirY=False, deltaFrict=20e-3, Fperp= 1049.79e3)
pot_matB.name= 'B'
## Bidirectional pot bearing.
pot_matC= bridge_bearings.PTFEPotBearingMat(d= 330e-3, unidirX=False, unidirY=False, deltaFrict=20e-3, Fperp= 1461.49e3)
pot_matC.name= 'C'

pot_bearing_materials= [pot_matA, pot_matB, pot_matC]

for pot_mat in pot_bearing_materials:
    pot_mat.defineMaterials(preprocessor)

# Create nodes for each pot bearing
pot_bearing_dict= dict()
pot_bearing_list= list()
for i, pot_mat in enumerate(pot_bearing_materials):
    nod1= nodes.newNodeXYZ(i,0,0)
    nod2= nodes.newNodeXYZ(i,0,0)
    potBearing= bridge_bearings.PotBearing(pot_mat,nod1,nod2,orientation=[vDirXlocal, vDirYlocal])
    potBearing.name= pot_mat.name
    pot_bearing_list.append(potBearing)
    # POT bearing element with local X-axis 30 degrees counterclockwise from global X-axis
    pot_bearing_dict[pot_mat.name]= {'nod1':nod1, 'nod2':nod2, 'pot_bearing':potBearing}
    # Constraints
    modelSpace.fixNode('000_000', nod1.tag)
    modelSpace.fixNode('FFF_000', nod2.tag) # Fix rotations on node 2.
    
# Define load case.
lp= modelSpace.newLoadPattern(name= 'G')
for key in pot_bearing_dict:
    pot_bearing_data= pot_bearing_dict[key]
    nod2= pot_bearing_data['nod2']
    if(key!='A'):
        pot_factor= 0.25
    else:
        pot_factor= 1.0
    lp.newNodalLoad(nod2.tag, pot_factor*loadVector)

# Define load combinations.
combContainer= combs.CombContainer()
uls_combinations= list()
for i in range(0, 10):
    comb_name= 'ULS'+"{:02d}".format(i)
    factor= 1+i/10.0
    uls_combinations.append((comb_name, factor))
sls_combinations= list()
for i in range(0, 5):
    comb_name= 'SLS'+"{:02d}".format(i)
    factor= (1+i/10.0)/1.4
    sls_combinations.append((comb_name, factor))

for lc in uls_combinations:
    lc_name= lc[0]
    lc_factor= lc[1]
    lc_expr= '{:.2f}'.format(lc_factor)+'*G'
    combContainer.ULS.perm.add(lc_name, lc_expr)
for lc in sls_combinations:
    lc_name= lc[0]
    lc_factor= lc[1]
    lc_expr= '{:.2f}'.format(lc_factor)+'*G'
    combContainer.SLS.freq.add(lc_name, lc_expr)



#        ___                     _                         _ _                 
#       / __|___ _ __  _ __ _  _| |_ ___   _ _ ___ ____  _| | |_ ___  ___ _ _  
#      | (__/ _ \ '  \| '_ \ || |  _/ -_) | '_/ -_|_-< || | |  _(_-< / _ \ ' \ 
#       \___\___/_|_|_| .__/\_,_|\__\___| |_| \___/__/\_,_|_|\__/__/ \___/_||_|
#                 _   |_|                  _                                   
#       _ __  ___| |_  | |__  ___ __ _ _ _(_)_ _  __ _ ___                     
#      | '_ \/ _ \  _| | '_ \/ -_) _` | '_| | ' \/ _` (_-<                     
#      | .__/\___/\__| |_.__/\___\__,_|_| |_|_||_\__, /__/                     
#      |_|                                       |___/                         
# Compute results on pot bearings.
    
## Get temporary environment.
cfg= default_config.get_temporary_env_config()
cfg.projectDirTree.createTree() # Create project directory tree.
resultsPath= cfg.projectDirTree.getFullVerifPath()+'pot_bearings/'
os.mkdir(resultsPath)
## Get the ULS persistent and accidental load combinations.
loadCombinations= combContainer.getCorrespondingLoadCombinations(designSituations= ['uls_permanent', 'uls_accidental', 'uls_earthquake'])
ulsResultDict= dict()
for sc in loadCombinations:
    for key in sc:
        lc= sc[key]
        comb= modelSpace.newLoadCombination(lc.name, lc.expr)
        modelSpace.addLoadCaseToDomain(lc.name)
        result= modelSpace.analyze(calculateNodalReactions= True)
        if(result!=0):
            lmsg.error('Can\'t solve.')
        else:
            store_pot_results(potBearings= pot_bearing_list, lcName= lc.name, potResults= ulsResultDict)
        modelSpace.removeLoadCaseFromDomain(lc.name)
        modelSpace.revertToStart()
## Write the ULS results.        
fileName= resultsPath+'results_on_pots_uls.json'
with open(fileName, 'w') as f:
    json.dump(ulsResultDict, f)
    
## Get the SLS frequent load combinations.
loadCombinations= combContainer.getCorrespondingLoadCombinations(designSituations= ['sls_frequent'])
slsResultDict= dict()
for sc in loadCombinations:
    for key in sc:
        lc= sc[key]
        comb= modelSpace.newLoadCombination(lc.name, lc.expr)
        modelSpace.addLoadCaseToDomain(lc.name)
        result= modelSpace.analyze(calculateNodalReactions= True)
        if(result!=0):
            lmsg.error('Can\'t solve.')
        else:
            store_pot_results(potBearings= pot_bearing_list, lcName= lc.name, potResults= slsResultDict)
        modelSpace.removeLoadCaseFromDomain(lc.name)
        modelSpace.revertToStart()
        
## Write the SLS results.        
fileName= resultsPath+'results_on_pots_sls.json'
with open(fileName, 'w') as f:
    json.dump(slsResultDict, f)

#       ___                   _                     _ _                 
#      | _ \___ _ __  ___ _ _| |_   _ _ ___ ____  _| | |_ ___  ___ _ _  
#      |   / -_) '_ \/ _ \ '_|  _| | '_/ -_|_-< || | |  _(_-< / _ \ ' \ 
#      |_|_\___| .__/\___/_|  \__| |_| \___/__/\_,_|_|\__/__/ \___/_||_|
#       _ __  _|_| |_  | |__  ___ __ _ _ _(_)_ _  __ _ ___              
#      | '_ \/ _ \  _| | '_ \/ -_) _` | '_| | ' \/ _` (_-<              
#      | .__/\___/\__| |_.__/\___\__,_|_| |_|_||_\__, /__/              
#      |_|                                       |___/                  
# Report results on pot bearings.
    
## Report pot characteristics.
pot_characteristics_report= bridge_bearings.report_pot_characteristics(pot_bearings= pot_bearing_list, caption= 'Pot bearings. Dimensiones y características mecánicas', label='tb:pot_bearing_caract')

## Report reaction extreme values for SLS combinations.
sls_reaction_extreme_values_report= str()
### Read results from file
resultsPath= cfg.projectDirTree.getFullVerifPath()+'pot_bearings/'
fileName= resultsPath+'results_on_pots_sls.json'
if os.path.isfile(fileName):
    with open(fileName,'r') as f: # Open JSON file
        sls_results_dict= json.load(f)
        # Reaction extreme values.
        sls_extreme_values= bridge_bearings.get_pot_reaction_extreme_values(sls_results_dict)
        sls_reaction_extreme_values_report= bridge_bearings.get_pot_reaction_extreme_values_table(sls_extreme_values,caption='Pot bearings. Maximum and minimum load values on SLS combinations',label='tb:pot_bearing_ELS_Rmaxmin')
        # Rotation extreme values.
        sls_extreme_values= bridge_bearings.get_pot_rotation_extreme_values(sls_results_dict)
        sls_rotation_extreme_values_report= bridge_bearings.get_pot_rotation_extreme_values_table(sls_extreme_values,caption='Pot bearings. Maximum rotations on SLS combinations',label='tb:pot_bearing_ELS_RotMaxPosNeg')
        # Displacement extreme values.
        sls_extreme_values=  bridge_bearings.get_pot_displacement_extreme_values(sls_results_dict)
        sls_displacement_extreme_values_report=  bridge_bearings.get_pot_displacement_extreme_values_table(sls_extreme_values,caption='Pot bearings. Maximum displacements on SLS combinations',label='tb:pot_bearing_ELS_DispMaxPosNeg')
else:
    lmsg.error('File '+fileName+'does not exist')
    exit(1)

## Report reaction extreme values for ULS combinations.
uls_reaction_extreme_values_report= str()
### Read results from file
fileName= resultsPath+'results_on_pots_uls.json'
if os.path.isfile(fileName):
    with open(fileName,'r') as f: # Open JSON file
        uls_results_dict= json.load(f)
        # Reaction extreme values.
        uls_extreme_values= bridge_bearings.get_pot_reaction_extreme_values(uls_results_dict)
        uls_reaction_extreme_values_report= bridge_bearings.get_pot_reaction_extreme_values_table(uls_extreme_values,caption='Pot bearings. Maximum and minimum load values on ULS combinations',label='tb:pot_bearing_ELU_Rmaxmin')
        # Rotation extreme values.
        uls_extreme_values= bridge_bearings.get_pot_rotation_extreme_values(uls_results_dict)
        uls_rotation_extreme_values_report= bridge_bearings.get_pot_rotation_extreme_values_table(uls_extreme_values,caption='Pot bearings. Maximum rotations on ULS combinations',label='tb:pot_bearing_ELU_RotMaxPosNeg')
        # Displacement extreme values.
        uls_extreme_values=  bridge_bearings.get_pot_displacement_extreme_values(uls_results_dict)
        uls_displacement_extreme_values_report=  bridge_bearings.get_pot_displacement_extreme_values_table(uls_extreme_values,caption='Pot bearings. Maximum displacement on ULS combinations',label='tb:pot_bearing_ELU_DispMaxPosNeg')
else:
    lmsg.error('File '+fileName+'does not exist')
    exit(1)

# Write output
reportPath= cfg.projectDirTree.getFullTextReportPath()
outputFileName= reportPath+'pot_bearings_tables.tex'
with open(outputFileName, 'w') as f:
    f.write(pot_characteristics_report)
    f.write('\n\n')
    f.write(uls_reaction_extreme_values_report)
    f.write('\n')
    f.write(sls_reaction_extreme_values_report)
    f.write('\n')
    f.write(sls_displacement_extreme_values_report)
    f.write('\n')
    f.write(sls_rotation_extreme_values_report)
    f.write('\n')
    
# Check that file exists
testOK= os.path.isfile(outputFileName)

fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

cfg.cleandirs() # Clean after yourself.
