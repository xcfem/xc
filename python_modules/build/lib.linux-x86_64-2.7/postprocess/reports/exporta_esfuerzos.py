# -*- coding: utf-8 -*-

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportShellInternalForces(nmbComb, elems, fDesc,fConv= 1.0):
  for e in elems:
    e.getResistingForce()
    mat= e.getPhysicalProperties.getVectorMaterials
    n1Medio= str(mat.getMeanGeneralizedStressByName("n1")*fConv)
    n2Medio= str(mat.getMeanGeneralizedStressByName("n2")*fConv)
    n12Medio= str(mat.getMeanGeneralizedStressByName("n12")*fConv)
    m1Medio= str(mat.getMeanGeneralizedStressByName("m1")*fConv)
    m2Medio= str(mat.getMeanGeneralizedStressByName("m2")*fConv)
    m12Medio= str(mat.getMeanGeneralizedStressByName("m12")*fConv)
    q13Medio= str(mat.getMeanGeneralizedStressByName("q13")*fConv)
    q23Medio= str(mat.getMeanGeneralizedStressByName("q23")*fConv)
    fDesc.write(nmbComb+", "+str(e.tag)+", ")
    fDesc.write(n1Medio+", "+n2Medio+", "+n12Medio+", ")
    fDesc.write(m1Medio+", "+m2Medio+", "+m12Medio+", "+q13Medio+", "+q23Medio+"\n")


# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportaEsfuerzosShellSet(mdlr,nmbComb, st, fName):
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
