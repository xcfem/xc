# -*- coding: utf-8 -*-
''' Reinforced concrete frame pushover analysis example migrated from OpenSees.

See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

# Units: kips, in, sec  
#
# Written: GLF/MHS/fmk
# Date: January 2001

# Import the FE model and gravity Analysis
# ----------------------------------------------------

# Import local modules.
import rc_frame_gravity

modelSpace= rc_frame_gravity.modelSpace
n3= rc_frame_gravity.n3
n4= rc_frame_gravity.n4

# Set the gravity loads to be constant & reset the time in the domain
modelSpace.setLoadConstant(t= 0.0)


# Start of additional modelling for lateral loads

## Define lateral loads

# Set some parameters
H = 10.0  # Reference lateral load

# Set lateral load pattern with a Linear TimeSeries
hLoad= modelSpace.newLoadPattern(name= 'hLoad', setCurrent= True)

# Create nodal loads at nodes 3 & 4
#    nd    FX  FY  MZ
hLoad.newNodalLoad(n3.tag, xc.Vector([H, 0, 0]))
hLoad.newNodalLoad(n4.tag, xc.Vector([H, 0, 0]))

# Start of modifications to analysis for push over

# Set some parameters
dU = 0.1  # Displacement increment

solProc= rc_frame_gravity.solProc
solProc.setDisplacementControlIntegrator(node= n3, dof= 1, increment= dU, numIter= 1, dUmin= dU, dUmax= dU)

# Finally perform the analysis
# ------------------------------

# Set some parameters
maxU = 15.0  # Max displacement
currentDisp = 0.0
ok = 0

quit()

test('NormDispIncr', 1.0e-12, 1000)
algorithm('ModifiedNewton', '-initial')

while ok == 0 and currentDisp < maxU:

    ok = analyze(1)

    # if the analysis fails try initial tangent iteration
    if ok != 0:
        print("modified newton failed")
        break
    # print "regular newton failed .. lets try an initail stiffness for this step"
    # test('NormDispIncr', 1.0e-12,  1000)
    # # algorithm('ModifiedNewton', '-initial')
    # ok = analyze(1)
    # if ok == 0:
    #     print "that worked .. back to regular newton"

    # test('NormDispIncr', 1.0e-12,  10)
    # algorithm('Newton')

    currentDisp = nodeDisp(3, 1)

results = open('results.out', 'a+')

if ok == 0:
    results.write('PASSED : RCFramePushover.py\n')
    print("Passed!")
else:
    results.write('FAILED : RCFramePushover.py\n')
    print("Failed!")

results.close()

# Print the state at node 3
# print node 3



# Change the integration scheme to be displacement control
#                             node dof init Jd min max
integrator('DisplacementControl', 3, 1, dU, 1, dU, dU)

# ------------------------------
# Start of recorder generation
# ------------------------------

# Stop the old recorders by destroying them
# remove recorders

# Create a recorder to monitor nodal displacements
# recorder Node -file node32.out -time -node 3 4 -dof 1 2 3 disp

# Create a recorder to monitor element forces in columns
# recorder EnvelopeElement -file ele32.out -time -ele 1 2 forces

# --------------------------------
# End of recorder generation
# ---------------------------------


# ------------------------------
