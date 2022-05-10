# -*- coding: utf-8 -*-

import geom

P1= computeDipPlane(alpha1,beta1,p)
P2= computeDipPlane(alpha2,beta2,p)
P3= computeDipPlane(alpha3,beta3,p)
P4= computeDipPlane(alpha4,beta4,p)

# Tunnel axis
tunnelAxisVector= [sin(axisBeta),cos(axisBeta),0]
vectorITunel= cross([0,0,1],tunnelAxisVector)
vectorJTunel= cross(tunnelAxisVector,vectorITunel)
tunnelAxis= tunnelAxis( org= p, axis= setVectoresIJ([vectorITunel,vectorJTunel]))
def getUpAndDownHalfSpaces(plane):
    '''Return the up and down half spaces that correspond to
       the argument plane.'''
    PL= geom.HalfSpace()
    PL.setPlane(plane)
    PU= geom.HalfSpace()
    PU.setPlane(plane)
    PU.swap()
    return PL, PU
    
    
'''Define the up and down half spaces that correspond
   to the planes P1, P2, P3 and P4'''
P1L, P1U= getUpAndDownHalfSpaces(P1)
P2L, P2U= getUpAndDownHalfSpaces(P2)
P3L, P3U= getUpAndDownHalfSpaces(P3)
P4L, P4U= getUpAndDownHalfSpaces(P4)
