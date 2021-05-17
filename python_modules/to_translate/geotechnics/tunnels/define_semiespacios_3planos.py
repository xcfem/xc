# -*- coding: utf-8 -*-
P1= 
P1= computeDipPlane(alpha1,beta1,p)
P2= 
P2= computeDipPlane(alpha2,beta2,p)
P3= 
P3= computeDipPlane(alpha3,beta3,p)

# Tunnel axis
tunnelAxisVector= [sin(axisBeta),cos(axisBeta),0]
vectorITunel= cross([0,0,1],tunnelAxisVector)
vectorJTunel= cross(tunnelAxisVector,vectorITunel)
tunnelAxis= 
\tunnelAxis
  {
    \setOrg{p}
    \axis{\setVectoresIJ{[vectorITunel,vectorJTunel]}}
  }

'''Define the up and down half spaces that correspond to
   the planes P1,P2 and P3'''
P1L= 
\P1L{\setPlane{P1}}
P1U= 
\P1U{\setPlane{P1} \swap{}}

P2L= 
\P2L{\setPlane{P2}}
P2U= 
\P2U{\setPlane{P2} \swap{}}

P3L= 
\P3L{\setPlane{P3}}
P3U= 
\P3U{\setPlane{P3} \swap{}}
