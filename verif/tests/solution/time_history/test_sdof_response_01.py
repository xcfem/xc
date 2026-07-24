# -*- coding: utf-8 -*-
''' Check SDFResponse class.

See https://openseesdigital.com/2021/02/14/how-many-clicks-does-it-take/
'''

import math
import os
import xc
import numpy as np
from scipy.constants import g

silent= False

# Loading El Centro EQ data (North-south component)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)
timeValues= el_centro_raw[:,0]
accelerationValues= list(el_centro_raw[:,1]*g)

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
    resp = {'T':[0.0],'SD':[0.0], 'SV':[0.0], 'SA':[0.0], 'TA':[0.0]}
    for T in np.arange(T_min, T_max, dT):

        k = (2*math.pi/T)**2 # mass = 1.0
        sdfResponse.setStiffness(k)
        sdfResponse.setDampingRatio(z)
        sr= sdfResponse.getResponse(dtF, dT, accelerationValues, 0.0, 0.0)
        resp['SD'].append(sr['max_displ'])
        resp['SV'].append(sr['max_vel'])
        resp['SA'].append(sr['max_accel'])
        resp['TA'].append(sr['time_max_accel'])
        resp['T'].append(T)
    # Appending keys and values dynamically
    data_frame[z] = resp
    if(not silent):
        print('Done with zeta= '+str(z)+'!')
    

if(not silent):
    import matplotlib.pyplot as plt
    ## Plot accelerogram.
    plt.figure(figsize=(15,3))
    plt.plot(el_centro_raw[:,0], el_centro_raw[:,1], color='k')

    plt.ylabel('$\\ddot{d_g} (g)$', {'size':14})
    plt.xlabel('Time (sec)', {'fontstyle':'italic','size':13})

    plt.grid()
    plt.yticks(fontsize= 14)
    plt.xticks(fontsize= 14)
    plt.xlim([0.0, el_centro_raw[-1,0]]);
    plt.show()

    ## Displacment -----------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SD'],
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

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
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

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
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Acceleration Response Spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    
