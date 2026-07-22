# -*- coding: utf-8 -*-
''' Test based on the example «Elastic Response Spectra» by Amir Hossein 
Namadchi.

See https://github.com/AmirHosseinNamadchi/OpenSeesPy-Examples/blob/master/Elastic%20Response%20Spectra.ipynb
'''
import os
import json
import numpy as np
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

def analyze_SDOF(period, damping_ratio):
    
    # natural frequency
    omega = (2*np.pi)/period
    # stiffness
    k = omega**2
    # Damping
    c = 2*damping_ratio*omega
    
    # Model Definition
    feProblem= xc.FEProblem()
    preprocessor=  feProblem.getPreprocessor
    nodeHandler= preprocessor.getNodeHandler
    modelSpace= predefined_spaces.SolidMechanics1D(nodeHandler)

    n1= modelSpace.newNode(0.0)
    n2= modelSpace.newNode(0.0)
    matElast= typical_materials.defElasticMaterial(preprocessor, name= "matElast", E= k)
    modelSpace.setDefaultMaterial(matElast)
    modelSpace.setElementDimension(1)
    zl= modelSpace.newElement('ZeroLength', [n1.tag, n2.tag])

    # unit mass is assumed
    n2.mass= xc.Matrix([[1.0]])  # node mass matrix.

    # Rayleigh damping factors.
    alphaM= c # factor applied to elements or nodes mass matrix
    betaK= 0.0 # factor applied to elements current stiffness matrix.
    betaKinit= 0.0 # factor applied to elements initial stiffness matrix.
    betaKcomm= 0.0 # factor applied to elements committed stiffness matrix.
    rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
    # print('damping factors: ', rayleigh)
    # preprocessor.getDomain.setRayleighDampingFactors(rayleigh)
    #zl.setRayleighDampingFactors(rayleigh)
    #zl.setUseRayleighDampingFlag(1) # Compute the element damping matrix
                                    # from its damping coefficients.
    n2.setRayleighDampingFactor(alphaM)

    modelSpace.fixNode0(n1.tag)

    ## Loading
    dt= 0.02
    # loads definition
    loadHandler= preprocessor.getLoadHandler
    lPatterns= loadHandler.getLoadPatterns
    ts= lPatterns.newTimeSeries("constant_ts","ts")
    gm= lPatterns.newLoadPattern("uniform_excitation","gm")
    mr= gm.motionRecord
    hist= mr.history
    timeValues= el_centro_raw[:,0]
    accelerationValues= list(el_centro_raw[:,1]*g)
    accel= lPatterns.newTimeSeries("path_ts","accel")
    accel.path= xc.Vector(accelerationValues)
    accel.timeIncr= timeValues[1]-timeValues[0] 
    hist.accel= accel
    hist.delta= dt # Time integration step.
    #We add the load case to domain.
    lPatterns.addToDomain(gm.name)

    preprocessor.getDomain.setTime(timeValues[0])
    solProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= 1, timeStep= dt, constraintHandlerType= 'transformation', maxNumIter= 10, printFlag= 0)
    solProc.setup()
    analysis= solProc.getAnalysis()
    
    results= {'D':[],'V':[], 'A':[]}
    for i in range(len(timeValues)):
        analysis.analyze(1, dt)
        results['D'].append(n2.getDisp[0])
        results['V'].append(n2.getVel[0])
        results['A'].append(n2.getAccel[0])    
        
    return {'SD': np.max(np.abs(results['D'])),
            'SV': np.max(np.abs(results['V'])),
            'SA': np.max(np.abs(results['A']))}

silent= True

# Loading El Centro EQ data (North-south component)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)


# Define a period range below
T_min= 0.00001
T_max= 5
dT= 0.05
# a list of damping ratios to be included
zeta_list= np.array([0.02, 0.03, 0.05])


# Base units
cm= 1.0
sec= 1.0
# Gravitational constant
g= 981*cm/sec**2

# Use nested loops to analyse the system for various damping ratios and periods.
data_frame = dict()

for z in zeta_list:    
    # re-initialization
    resp = {'T':[0],'SD':[0], 'SV':[0], 'SA':[0]}
    
    for T in np.arange(T_min, T_max, dT):
        SR = analyze_SDOF(T, z)
        resp['SD'].append(SR['SD'])
        resp['SV'].append(SR['SV'])
        resp['SA'].append(SR['SA'])
        resp['T'].append(T)
    
    # Appending keys and values dynamically
    data_frame[z] = resp
    if(not silent):
        print('Done with zeta= '+str(z)+'!')

refFilePath= pth+"/../../aux/reference_files/"
fname= os.path.basename(__file__)
jsonFileName= refFilePath+'/ref_'+fname.replace('.py', '.json')
# # Save output as reference.
# with open(jsonFileName, 'w') as f:
#     json.dump(data_frame, f)

# Check results.
## Load reference values.
with open(jsonFileName, 'r') as f:
     ref_data_frame= json.load(f)
## Compare results.
err= 0.0
tol= 1e-8
for key in ref_data_frame:
    ref_resp= ref_data_frame[key]
    ref_z= float(key)
    resp= data_frame[ref_z]
    for key in ref_resp:
        values= resp[key]
        ref_values= ref_resp[key]
        error= (len(values)-len(ref_values))**2
        if(error<tol):
            for v, rv in zip(values, ref_values):
                error+=(v-rv)**2
error= np.sqrt(error)

# print(error)

from misc_utils import log_messages as lmsg
if error<tol:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Graphic output.
if(not silent):
    import matplotlib.pyplot as plt
    ## Plot accelerogram.
    plt.figure(figsize=(15,3))
    plt.plot(el_centro_raw[:,0], el_centro_raw[:,1], color='k')

    plt.ylabel('$\ddot{d_g} (g)$', {'size':14})
    plt.xlabel('Time (sec)', {'fontstyle':'italic','size':13})

    plt.grid()
    plt.yticks(fontsize= 14)
    plt.xticks(fontsize= 14)
    plt.xlim([0.0, el_centro_raw[-1,0]]);
    plt.show()

    ## Displacment -----------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SD'],
              label=('$\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Displacement (cm)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Displacement Response Spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    # Velocity ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SV'],
              label=('$\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Velocity (cm/sec)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Veloctiy Response Spectrum', 
              {'fontstyle':'italic','size':18});
    plt.show()

    # Acceleration ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], np.array(data_frame[z]['SA'])/g,
              label=('$\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Acceleration Response Spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()
