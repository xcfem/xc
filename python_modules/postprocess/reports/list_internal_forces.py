# -*- coding: utf-8 -*-
# THIS MODULE TRULY NEEDS TO BE REWRITED.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2014, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def listBarSetInternalForces(preprocessor,nmbComb, setName, fmt, fName):
    ''' Print internal forces on the elements in the set argument.'''
    str= "" 
    k= 0
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elements:
        str= nmbComb+" & "+e.tag+" & "
        k= 0
        sections= e.getSections()
        for s in sections:
            fName.write(str,k," & ")
            fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
            fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3),"\\\\\n")
            k=k+1


def listLineSetInternalForces(preprocessor, nmbComb, setNameLineas, fmt, fName, encab, tit):
    ''' Print internal forces on the elements of the lines in the
        set argument.'''
    fName.write("\\",encab,"{",tit,"}\n")
    caption= "Bars on set: "+setNameLineas
    defCampos= "|l|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
    cabeceraSupertabular(fName,9,defCampos,idsCampos,caption) 

    s= preprocessor.getSets.getSet(setNameLineas)
    lines= s.getLines()
    for l in lines:
        nmb= l.getName() 
        fName.write("\hline\n")
        fName.write("\multicolumn{9}{|l|}{Internal forces of bars on the line: ",nmb,"}\\\\\n")
        fName.write("\hline\n")
        listBarSetInternalForces(nmbComb,nmb,fmt,fName) 
    cierraSupertabular(fName)

def listBarSetInternalForcesAndCF(preprocessor, nmbComb, setName, fmt, fName, nmbDiag):
    '''Print internal forces and capacity factor of the elements contained in the
       argument set.'''
    str= "" 
    k= 0
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elements:
        str= nmbComb+" & "+e.tag+" & "
        k= 0
        sections= e.getSections()
        for s in sections:
            fName.write(str,k," & ")
            fName.write(fmt.format(e.getProp("N")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
            fName.write(fmt.format(e.getProp("Mx")/1e3)," & ",fmt.format(e.getProp("My")/1e3)," & ",fmt.format(e.getProp("Mz")/1e3)," & ",fmt.format(getCapacityFactor(nmbDiag)),"\\\\\n")
            k=k+1

def listLineSetInternalForcesAndCF(nmbComb, setNameLineas, fmt, fName, encab, tit, nmbDiag):
    '''Print internal forces and capacity factor of the elements contained in the
       lines of the argument set.'''
    fName.write("\\",encab,"{",tit,"}\n")
    caption= "Bars of the set: "+setNameLineas
    defCampos= "|l|r|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz & FC \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m & - "
    cabeceraSupertabular(fName,10,defCampos,idsCampos,caption) 

    s= preprocessor.getSets.getSet(setNameLineas)
    lines= s.getLines()
    for l in lines:
        nmb= l.getName()
        fName.write("\hline\n")
        fName.write("\multicolumn{10}{|l|}{Internal forces on the bars of the line: ",nmb,"}\\\\\n")
        fName.write("\hline\n")
        listBarSetInternalForcesAndCF(nmbComb,nmb,fmt,fName,nmbDiag) 
    cierraSupertabular(fName) 

def listaDatosEsfuerzosTrussSet(nmbComb, setName, fmt, fName):
    ''' Print internal forces on the truss elements in the set argument.'''
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elements:
        fName.write(nmbComb," & ",e.tag," & ",fmt.format(e.getStrain()*1e2)," & ",fmt.format(e.getStress()/1e6)," & ",fmt.format(e.getN()/1e3),"\\\\\n")

def listaEsfuerzosTrussSet(nmbComb, setName, fmt, fName, encab, tit):
    fName.write("\\",encab,"{",tit,"}\n")
    caption= "Bars of the set: "+setName
    defCampos= "|l|r|r|r|r|"
    idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
    cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 
    listaDatosEsfuerzosTrussSet(nmbComb,setName,fmt,fName) 
    cierraSupertabular(fName) 

def listaEsfuerzosTrussLineas(nmbComb, setNameLineas, fmt, fName, encab, tit):
    ''' Print internal forces on the truss elements of the lines in the
        set argument.'''
    fName.write("\\",encab,"{",tit,"}\n")
    caption= "Bars of the set: "+setNameLineas
    defCampos= "|l|r|r|r|r|"
    idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
    cabeceraSupertabular(fName,5,defCampos,idsCampos,caption) 

    s= preprocessor.getSets.getSet(setNameLineas)
    lines= s.getLines()
    for l in lines:
        nmb= l.getName()
        fName.write("\hline\n")
        fName.write("\multicolumn{5}{|l|}{Esfuerzos en elementos linea: ",nmb,"}\\\\\n")
        fName.write("\hline\n")
        listaDatosEsfuerzosTrussSet(nmbComb,nmb,fmt,fName) 
    cierraSupertabular(fName) 

def listaEsfuerzosZeroLengthSet(nmbComb, setName, fmt, fName, encab, tit):
    ''' Print internal forces on the zero length elements in the 
        set argument.'''
    fName.write("\\",encab,"{",tit,"}\n")
    caption= "Elementos del conjunto: "+setName
    defCampos= "|l|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & N & Vx & Vy & T & Mx & My \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
    cabeceraSupertabular(fName,8,defCampos,idsCampos,caption) 

    str= "" 
    k= 0
    VX= 0
    VY= 0
    N= 0
    T= 0
    momY= 0
    momZ= 0
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elements:
        str= nmbComb+" & "+e.tag+" & "
        mats= e.getMaterials()
        VX= mats[0].getStress()
        VY= mats[1].getStress()
        N= mats[2].getStress()
        T= mats[3].getStress()
        momY= mats[4].getStress()
        momZ= mats[5].getStress()

        fName.write(str)
        fName.write(fmt.format(N/1e3)," & ",fmt.format(VX/1e3)," & ",fmt.format(VY/1e3)," & ")
        fName.write(fmt.format(T/1e3)," & ",fmt.format(momY/1e3)," & ",fmt.format(momZ/1e3),"\\\\\n")
    cierraSupertabular(fName) 

def listaEsfuerzosElasticBeam3dSet(preprocessor, nmbComb, setName, fmt, fName):
    ''' Print internal forces on the truss elements of the lines in the
        set argument.'''
    s= preprocessor.getSets.getSet(setName)
    elems= s.elements
    for e in elements:
        str= nmbComb+" & "+e.tag+" & "
        fName.write(str,1," & ")
        fName.write(fmt.format(e.getProp("N1")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
        fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My1")/1e3)," & ",fmt.format(e.getProp("Mz1")/1e3),"\\\\\n")
        fName.write(str,2," & ")
        fName.write(fmt.format(e.getProp("N2")/1e3)," & ",fmt.format(e.getProp("Vy")/1e3)," & ",fmt.format(e.getProp("Vz")/1e3)," & ")
        fName.write(fmt.format(e.getProp("T")/1e3)," & ",fmt.format(e.getProp("My2")/1e3)," & ",fmt.format(e.getProp("Mz2")/1e3),"\\\\\n")
