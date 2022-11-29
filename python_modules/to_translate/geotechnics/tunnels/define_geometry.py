# -*- coding: utf-8 -*-

import math
import geom
import teoria_bloques

def getUpAndDownHalfSpaces(plane):
    '''Return the up and down half spaces that correspond to
       the argument plane.'''
    PL= geom.HalfSpace()
    PL.setPlane(plane)
    PU= geom.HalfSpace()
    PU.setPlane(plane)
    PU.swap()
    return PL, PU

def define_halfspaces_3planes(axisBeta, alpha1, beta1, alpha2, beta2, alpha3, beta3, p):
    P1= teoria_bloques.computeDipPlane(alpha1,beta1,p)
    P2= teoria_bloques.computeDipPlane(alpha2,beta2,p)
    P3= teoria_bloques.computeDipPlane(alpha3,beta3,p)

    # Tunnel axis
    tunnelAxisVector= geom.Vector3d(math.sin(axisBeta), math.cos(axisBeta),0)
    vectorITunel= geom.Vector3d(0,0,1).cross(tunnelAxisVector)
    vectorJTunel= tunnelAxisVector.cross(vectorITunel)
    tunnelAxis= tunnelAxis(org= p, axis= setVectoresIJ([vectorITunel,vectorJTunel]))

    # Define the up and down half spaces that correspond
    #  to the planes P1, P2, P3 and P4
    P1L, P1U= getUpAndDownHalfSpaces(P1)
    P2L, P2U= getUpAndDownHalfSpaces(P2)
    P3L, P3U= getUpAndDownHalfSpaces(P3)
    return [(P1L, P1U), (P2L, P2U), (P3L, P3U)]
    
def define_halfspaces_3planes(axisBeta, alpha1, beta1, alpha2, beta2, alpha3, beta3, alpha4, beta4, p):
    P1= teoria_bloques.computeDipPlane(alpha1,beta1,p)
    P2= teoria_bloques.computeDipPlane(alpha2,beta2,p)
    P3= teoria_bloques.computeDipPlane(alpha3,beta3,p)
    P4= teoria_bloques.computeDipPlane(alpha4,beta4,p)

    # Tunnel axis
    tunnelAxisVector= [math.sin(axisBeta),math.cos(axisBeta),0]
    vectorITunel= geom.Vector3d(0,0,1).cross(tunnelAxisVector)
    vectorJTunel= tunnelAxisVector.cross(vectorITunel)
    tunnelAxis= tunnelAxis( org= p, axis= setVectoresIJ([vectorITunel,vectorJTunel]))
    # Define the up and down half spaces that correspond
    #  to the planes P1, P2, P3 and P4
    P1L, P1U= getUpAndDownHalfSpaces(P1)
    P2L, P2U= getUpAndDownHalfSpaces(P2)
    P3L, P3U= getUpAndDownHalfSpaces(P3)
    P4L, P4U= getUpAndDownHalfSpaces(P4)
    return [(P1L, P1U), (P2L, P2U), (P3L, P3U), (P4L, P4U)]

def defJointPyramids4Planes(fourPlanes):
    ''' Define the «Joint Pyramids» corresponding to the four 
        planes: P1,P2,P3 y P4.'''
    [(P1L, P1U), (P2L, P2U), (P3L, P3U), (P4L, P4U)]= fourPlanes
    JP0000= geom.BlockPyramid( (P1U,P2U,P3U,P4U) )
    JP0001= geom.BlockPyramid( (P1U,P2U,P3U,P4L) )
    JP0010= geom.BlockPyramid( (P1U,P2U,P3L,P4U) )
    JP0011= geom.BlockPyramid( (P1U,P2U,P3L,P4L) )

    JP0100= geom.BlockPyramid( (P1U,P2L,P3U,P4U) )
    JP0101= geom.BlockPyramid( (P1U,P2L,P3U,P4L) )
    JP0110= geom.BlockPyramid( (P1U,P2L,P3L,P4U) )
    JP0111= geom.BlockPyramid( (P1U,P2L,P3L,P4L) )

    JP1000= geom.BlockPyramid( (P1L,P2U,P3U,P4U) )
    JP1001= geom.BlockPyramid( (P1L,P2U,P3U,P4L) )
    JP1010= geom.BlockPyramid( (P1L,P2U,P3L,P4U) )
    JP1011= geom.BlockPyramid( (P1L,P2U,P3L,P4L) )

    JP1100= geom.BlockPyramid( (P1L,P2L,P3U,P4U) )
    JP1101= geom.BlockPyramid( (P1L,P2L,P3U,P4L) )
    JP1110= geom.BlockPyramid( (P1L,P2L,P3L,P4U) )
    JP1111= geom.BlockPyramid( (P1L,P2L,P3L,P4L) )

    return [(JP0000, JP0001, JP0010, JP0011), (JP0100, JP0101, JP0110, JP0111), (JP1000, JP1001, JP1010, JP1011), (JP1100, JP1101, JP1110, JP1111)]

def defJointPyramids3Planes(threePlanes):
    ''' Define the «Joint Pyramids» corresponding to the three
        planes: P1,P2 and P3.'''
    JP000= geom.BlockPyramid( (P1U,P2U,P3U) )
    JP001= geom.BlockPyramid( (P1U,P2U,P3L) )
    JP010= geom.BlockPyramid( (P1U,P2L,P3U) )
    JP011= geom.BlockPyramid( (P1U,P2L,P3L) )
    JP100= geom.BlockPyramid( (P1L,P2U,P3U) )
    JP101= geom.BlockPyramid( (P1L,P2U,P3L) )
    JP110= geom.BlockPyramid( (P1L,P2L,P3U) )
    JP111= geom.BlockPyramid( (P1L,P2L,P3L) )
    return [(JP000, JP001, JP010, JP011), (JP100, JP101, JP110, JP111)]
