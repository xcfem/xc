# -*- coding: utf-8 -*-
''' Check SDFResponse class.

Test inspired on the example «Elastic Response Spectra» by Amir Hossein 
Namadchi.

See https://github.com/AmirHosseinNamadchi/OpenSeesPy-Examples/blob/master/Elastic%20Response%20Spectra.ipynb

See also https://openseesdigital.com/2021/02/14/how-many-clicks-does-it-take/
'''

import json
import math
import os
import xc
import numpy as np
from scipy.constants import g

silent= True

# Loading El Centro EQ data (North-south component)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/LOR_20110511_164726.acc'
lorca_raw= np.loadtxt(accelFilePath, skiprows= 5)
timeValues= lorca_raw[:,0]
accelerationValues= list(lorca_raw[:,1]*g)

# Define a period range below
T_min= 0.00001
T_max= 5
dtF= timeValues[1]-timeValues[0] # time step for input data.
dT= dtF # time step for analysis.

# SDOF response object.
m= 1.0 # mass.
zeta= .05 # damping ratio (to be set later).
k= 0.0 # stiffness (to be set later).
Fy= 1e16 # yielding strength.
alpha= .01 # strain-hardening ratio.
sdfResponse= xc.SDOFResponse(m, zeta, k, Fy, alpha)

# a list of damping ratios to be included
zeta_list= np.array([0.02, 0.03, 0.05])

# Use nested loops to analyse the system for various damping ratios and periods.
data_frame = dict()
for z in zeta_list:    
    resp = {'T':[0.0],'SD':[0.0], 'SV':[0.0], 'SA':[0.0], 'TA':[0.0], 'STA':[0.0], 'TTA':[0.0]}
    for T in np.arange(T_min, T_max, dT):

        k = (2*math.pi/T)**2 # mass = 1.0
        sdfResponse.setStiffness(k)
        sdfResponse.setDampingRatio(z)
        sr= sdfResponse.getResponse(dtF, dT, accelerationValues, 0.0, 0.0)
        resp['SD'].append(sr['max_displ'])
        resp['SV'].append(sr['max_vel'])
        resp['SA'].append(sr['max_accel'])
        resp['TA'].append(sr['time_max_accel'])
        resp['STA'].append(sr['max_true_accel'])
        resp['TTA'].append(sr['time_max_true_accel'])
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

if(not silent):
    import matplotlib.pyplot as plt
    ## Plot accelerogram.
    plt.figure(figsize=(15,3))
    plt.plot(lorca_raw[:,0], lorca_raw[:,1], color='k')

    plt.ylabel('$\\ddot{d_g} (g)$', {'size':14})
    plt.xlabel('Time (sec)', {'fontstyle':'italic','size':13})

    plt.grid()
    plt.yticks(fontsize= 14)
    plt.xticks(fontsize= 14)
    plt.xlim([0.0, lorca_raw[-1,0]]);
    plt.show()

    ## Displacment -----------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SD'],
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative displacement (m)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (s)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative displacement response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    # Velocity ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SV'],
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative velocity (m/s)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (s)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative veloctiy response spectrum', 
              {'fontstyle':'italic','size':18});
    plt.show()

    # Acceleration ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], np.array(data_frame[z]['SA'])/g,
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative acceleration response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()
    
    # True acceleration ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], np.array(data_frame[z]['STA'])/g,
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('True acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('True acceleration response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    
