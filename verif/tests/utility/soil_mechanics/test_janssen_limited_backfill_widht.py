# -*- coding: utf-8 -*-
'''
   Test of the function k_janssen, from the results in figure 2 of the
   paper: "Narrow backfill lateral earth pressure." Ahmad Al-Hassan et al.
   page 3
'''
from __future__ import print_function
from geotechnics import earth_pressure
import math

gammaSoil= 1.6*9.81
H= 9.0
B= H/10.0
delta= math.radians(20)
k0= 1-math.sin(math.radians(30)) # earth pressure at rest.
x= list()
results01= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  x.append(z/H)
  results01.append(sigma_h)

B= 2*H/10.0
results02= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  results02.append(sigma_h)

B= 3*H/10.0
results03= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  results03.append(sigma_h)

B= 5*H/10.0
results05= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  results05.append(sigma_h)

B= H
results1= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  results1.append(sigma_h)

B= 1000*H
results10= list()
for i in range(0,11):
  z= i*H/10.0
  k= earth_pressure.k_janssen(k0,delta,B,z)
  sigma_h= k*gammaSoil*z
  results10.append(sigma_h)

#At rest
resultsRest= list()
for i in range(0,11):
  z= i*H/10.0
  sigma_h= k0*gammaSoil*z
  resultsRest.append(sigma_h)

theorResults01= [0.0, 5.920534739914547, 10.034784660696005, 12.893825803809793, 14.880607456700933, 16.26124562826842, 17.220667494287465, 17.887381145953636, 18.350688382385975, 18.67264608460811, 18.89637831641687]
theorResults02= [0.0, 6.457777435372942, 11.841069479829095, 16.328655731588995, 20.06956932139201, 23.1880463157639, 25.787651607619587, 27.95471831538771, 29.761214913401865, 31.267135311209003, 32.52249125653684]
theorResults03= [0.0, 6.651548819733391, 12.543141241475114, 17.761604219743642, 22.38384450830533, 26.477982068382772, 30.104353982088014, 33.31640366618837, 36.161468490934276, 38.681477411429384, 40.91356889283952]
theorResults05= [0.0, 6.812246606913355, 13.146221074689603, 19.03550178829836, 24.511309671663707, 29.602673699572737, 34.33658478937433, 38.73813888829619, 42.83067001493126, 46.635873960187645, 50.173923303480024]
theorResults1=  [0.0, 6.936205672390086, 13.62449321382671, 20.07372386668914, 26.292442149379205, 32.28888717686471, 38.07100357659672, 43.646452014263666, 49.02261934332741, 54.206628391788236, 59.205347399145474]
theorResults10= [0.0, 7.063071461833055, 14.125885853560618, 21.18844318455475, 28.250743464165968, 35.31278670172325, 42.37457290662018, 49.43610208820731, 56.49737425581358, 63.55838941881105, 70.61914758655028]

results= [results01, results02, results03, results05, results1, results10]
theorResults= [theorResults01, theorResults02, theorResults03, theorResults05, theorResults1, theorResults10]
ratio= 0.0

for r,tr in zip(results,theorResults):
  for v,tv in zip(r,tr):
    ratio+= (v-tv)**2
    
'''
print('B/H= 0.1', results01)
print('B/H= 0.2', results02)
print('B/H= 0.3', results03) 
print('B/H= 0.5', results05)
print('B/H= 1', results1)
print('B/H= 1000', results10)
print('At rest pressure', resultsRest)
print(ratio)
'''

#Drawing stuff
# import matplotlib.pyplot as plt


# dshs = [10, 5, 100, 5]  # 10 points on, 5 off, 100 on, 5 off
# dshs2 = [30, 5, 10, 5]  # 30 points on, 5 off, 10 on, 5 off

# fig, ax = plt.subplots()
# line01, = ax.plot(results01, x, '--', linewidth=2, label='B/H= 0.1')
# line01.set_dashes(dshs)

# line02, = ax.plot(results02, x, dashes=dshs2, label='B/H= 0.2')
# line03, = ax.plot(results03, x, dashes=dshs, label='B/H= 0.3')
# line05, = ax.plot(results05, x, dashes=dshs2 , label='B/H= 0.5')
# line1, = ax.plot(results1, x, dashes=dshs, label='B/H= 1')
# line10, = ax.plot(results10, x, dashes=dshs2 , label='B/H= 1000')
# lineRest, = ax.plot(resultsRest, x, dashes=dshs, label='pressure at rest')

# ax.legend(loc='upper right')

# plt.ylim(x[-1],x[0])
# plt.xlabel('$kN/m^2$')
# plt.ylabel('$z/H$')
# plt.title('Distribution of lateral earth pressure with varying backfill widths.')
# plt.show()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio<1e-15):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
