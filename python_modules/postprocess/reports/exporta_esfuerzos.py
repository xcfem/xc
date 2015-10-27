# -*- coding: utf-8 -*-

import math

def transformInternalForces(iForces,theta):
  cos2T= math.cos(2*theta)
  sin2T= math.sin(2*theta)
  tmpA= (iForces[0]+iForces[1])/2.0
  tmpB= (iForces[0]-iForces[1])/2.0*cos2T+iForces[2]*sin2T
  retval= [0.0,0.0,0.0]
  retval[0]= tmpA+tmpB 
  retval[2]= -(iForces[0]-iForces[1])/2.0*sin2T+iForces[2]*cos2T
  retval[1]= tmpA-tmpB
  return retval
  

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportShellInternalForces(nmbComb, elems, fDesc,fConv= 1.0):
  for e in elems:
    e.getResistingForce()
    mat= e.getPhysicalProperties.getVectorMaterials
    n1Raw= mat.getMeanGeneralizedStressByName("n1")*fConv
    n2Raw= mat.getMeanGeneralizedStressByName("n2")*fConv
    n12Raw= mat.getMeanGeneralizedStressByName("n12")*fConv
    m1Raw= mat.getMeanGeneralizedStressByName("m1")*fConv
    m2Raw= mat.getMeanGeneralizedStressByName("m2")*fConv
    m12Raw= mat.getMeanGeneralizedStressByName("m12")*fConv
    q13Raw= mat.getMeanGeneralizedStressByName("q13")*fConv
    q23Raw= mat.getMeanGeneralizedStressByName("q23")*fConv
    strEsf= ''
    if(e.hasProp('theta')):
      theta= e.getProp('theta')
      N= transformInternalForces([n1Raw,n2Raw,n12Raw],theta)
      M= transformInternalForces([m1Raw,m2Raw,m12Raw],theta)
      Q= transformInternalForces([q13Raw,q23Raw,0.0],theta)
      strEsf+= str(N[0])+", "+str(N[1])+", "+str(N[2])+", "
      strEsf+= str(M[0])+", "+str(M[1])+", "+str(M[2])+", "
      strEsf+= str(Q[0])+", "+str(Q[1])
    else:
      strEsf+= str(n1Raw)+", "+str(n2Raw)+", "+str(n12Raw)+", "
      strEsf+= str(m1Raw)+", "+str(m2Raw)+", "+str(m12Raw)+", "
      strEsf+= str(q13Raw)+", "+str(q23Raw)

    fDesc.write(nmbComb+", "+str(e.tag)+", "+strEsf+'\n')

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportaEsfuerzosShellSet(preprocessor,nmbComb, st, fName):
  elems= st.getElements
  exportShellInternalForces(nmbComb,elems,fName)

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportBeamInternalForces(nmbComb, elems, fName):
  for e in elems:
    e.getResistingForce()
    N1= str(e.getN1)
    Qy1= str(e.getVy1)
    Qz1= str(e.getVz1)
    T1= str(e.getT1)
    My1= str(e.getMy1) # Momento en el extremo dorsal de la barra
    Mz1= str(e.getMz1) # Momento en el extremo dorsal de la barra
    fName.write(nmbComb+", "+str(e.tag*10+1)+",")
    fName.write(N1+", "+Qy1+", "+Qz1+", ")
    fName.write(T1+", "+My1+", "+Mz1+"\n")
    N2= str(e.getN2)
    Qy2= str(e.getVy2)
    Qz2= str(e.getVz2)
    T2= str(e.getT2)
    My2= str(e.getMy2) # Momento en el extremo frontal de la barra
    Mz2= str(e.getMz2) # Momento en el extremo frontal de la barra
    fName.write(nmbComb+", "+str(e.tag*10+2)+",")
    fName.write(N2+", "+Qy2+", "+Qz2+", ")
    fName.write(T2+", "+My2+", "+Mz2+"\n")
