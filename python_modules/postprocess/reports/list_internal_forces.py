# -*- coding: utf-8 -*-
# THIS MODULE TRULY NEEDS TO BE REWRITED.

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2014, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def listBarSetInternalForces(nmbComb, xcSet, fmt, outputFile):
    ''' Print internal forces on the elements in the set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outStr= "" 
    for e in xcSet.elements:
        outStr= nmbComb+" & "+e.tag+" & "
        sections= e.getSections()
        for k, s in enumerate(sections):
            outputFile.write(outStr,k+" & ")
            outputFile.write(fmt.format(e.getProp("N")/1e3)+" & "+fmt.format(e.getProp("Vy")/1e3)+" & "+fmt.format(e.getProp("Vz")/1e3)+" & ")
            outputFile.write(fmt.format(e.getProp("Mx")/1e3)+" & "+fmt.format(e.getProp("My")/1e3)+" & "+fmt.format(e.getProp("Mz")/1e3)+"\\\\\n")


def listLineSetInternalForces(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Print internal forces on the elements of the lines in the
        set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Bars on set: "+xcSet.name
    defCampos= "|l|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m "
    cabeceraSupertabular(outputFile,9,defCampos,idsCampos,caption) 

    for l in xcSet.lines:
        nmb= l.getName() 
        outputFile.write("\hline\n")
        outputFile.write("\multicolumn{9}{|l|}{Internal forces of bars on the line: "+nmb+"}\\\\\n")
        outputFile.write("\hline\n")
        listBarSetInternalForces(nmbComb,nmb,fmt,outputFile) 
    cierraSupertabular(outputFile)

def listBarSetInternalForcesAndCF(nmbComb, xcSet, fmt, outputFile, nmbDiag):
    '''Print internal forces and capacity factor of the elements contained 
       in the argument set.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outStr= "" 
    for e in xcSet.elements:
        outStr= nmbComb+" & "+e.tag+" & "
        sections= e.getSections()
        for k, s in enumerate(sections):
            outputFile.write(outStr,k+" & ")
            outputFile.write(fmt.format(e.getProp("N")/1e3)+" & "+fmt.format(e.getProp("Vy")/1e3)+" & "+fmt.format(e.getProp("Vz")/1e3)+" & ")
            outputFile.write(fmt.format(e.getProp("Mx")/1e3)+" & "+fmt.format(e.getProp("My")/1e3)+" & "+fmt.format(e.getProp("Mz")/1e3)+" & "+fmt.format(getCapacityFactor(nmbDiag))+"\\\\\n")

def listLineSetInternalForcesAndCF(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title, nmbDiag):
    '''Print internal forces and capacity factor of the elements contained 
       in the lines of the argument set.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    :param nmbDiag: name of the interaction diagram.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Bars of the set: "+xcSet.name
    defCampos= "|l|r|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & Secc. & N & Vy & Vz & Mx & My & Mz & FC \\\\\n - & - & - & kN & kN & kN & kN m & kN m & kN m & - "
    cabeceraSupertabular(outputFile,10,defCampos,idsCampos,caption) 

    for l in xcSet.lines:
        nmb= l.getName()
        outputFile.write("\hline\n")
        outputFile.write("\multicolumn{10}{|l|}{Internal forces on the bars of the line: "+nmb+"}\\\\\n")
        outputFile.write("\hline\n")
        listBarSetInternalForcesAndCF(nmbComb,l,fmt,outputFile,nmbDiag) 
    cierraSupertabular(outputFile) 

def listaDatosEsfuerzosTrussSet(nmbComb, xcSet, fmt, outputFile):
    ''' Print internal forces on the truss elements in the set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    '''
    for e in xcSet.elements:
        outputFile.write(nmbComb+" & "+e.tag+" & "+fmt.format(e.getStrain()*1e2)+" & "+fmt.format(e.getStress()/1e6)+" & "+fmt.format(e.getN()/1e3)+"\\\\\n")

def listaEsfuerzosTrussSet(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Print internal forces on the truss elements in the set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Bars of the set: "+xcSet.name
    defCampos= "|l|r|r|r|r|"
    idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
    cabeceraSupertabular(outputFile,5,defCampos,idsCampos,caption) 
    listaDatosEsfuerzosTrussSet(nmbComb,xcSet,fmt,outputFile) 
    cierraSupertabular(outputFile)

def listaEsfuerzosTrussLineas(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Print internal forces on the truss elements of the lines in the
        set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Bars of the set: "+xcSet.name
    defCampos= "|l|r|r|r|r|"
    idsCampos= "Caso & Id & $\\epsilon$ & $\\sigma$ & axil \\\\\n - & - & \\% & MPa & kN "
    cabeceraSupertabular(outputFile,5,defCampos,idsCampos,caption) 

    for l in xcSet.lines:
        nmb= l.getName()
        outputFile.write("\hline\n")
        outputFile.write("\multicolumn{5}{|l|}{Esfuerzos en elementos linea: "+nmb+"}\\\\\n")
        outputFile.write("\hline\n")
        listaDatosEsfuerzosTrussSet(nmbComb,nmb,fmt,outputFile) 
    cierraSupertabular(outputFile) 

def listaEsfuerzosZeroLengthSet(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Print internal forces on the zero length elements in the 
        set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Elementos del conjunto: "+xcSet.name
    defCampos= "|l|r|r|r|r|r|r|r|"
    idsCampos= "Caso & Id & N & Vx & Vy & T & Mx & My \\\\\n - & - & kN & kN & kN & kN m & kN m & kN m "
    cabeceraSupertabular(outputFile,8,defCampos,idsCampos,caption) 

    outStr= "" 
    VX= 0
    VY= 0
    N= 0
    T= 0
    momY= 0
    momZ= 0
    for e in xcSet.elements:
        outStr= nmbComb+" & "+e.tag+" & "
        mats= e.getMaterials()
        VX= mats[0].getStress()
        VY= mats[1].getStress()
        N= mats[2].getStress()
        T= mats[3].getStress()
        momY= mats[4].getStress()
        momZ= mats[5].getStress()

        outputFile.write(outStr)
        outputFile.write(fmt.format(N/1e3)+" & "+fmt.format(VX/1e3)+" & "+fmt.format(VY/1e3)+" & ")
        outputFile.write(fmt.format(T/1e3)+" & "+fmt.format(momY/1e3)+" & "+fmt.format(momZ/1e3)+"\\\\\n")
    cierraSupertabular(outputFile) 

def listaEsfuerzosElasticBeam3dSet(nmbComb, xcSet, fmt, outputFile):
    ''' Print internal forces on the truss elements of the lines in the
        set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    '''
    for e in xcSet.elements:
        outStr= nmbComb+" & "+e.tag+" & "
        outputFile.write(outStr,1+" & ")
        outputFile.write(fmt.format(e.getProp("N1")/1e3)+" & "+fmt.format(e.getProp("Vy")/1e3)+" & "+fmt.format(e.getProp("Vz")/1e3)+" & ")
        outputFile.write(fmt.format(e.getProp("T")/1e3)+" & "+fmt.format(e.getProp("My1")/1e3)+" & "+fmt.format(e.getProp("Mz1")/1e3)+"\\\\\n")
        outputFile.write(outStr,2+" & ")
        outputFile.write(fmt.format(e.getProp("N2")/1e3)+" & "+fmt.format(e.getProp("Vy")/1e3)+" & "+fmt.format(e.getProp("Vz")/1e3)+" & ")
        outputFile.write(fmt.format(e.getProp("T")/1e3)+" & "+fmt.format(e.getProp("My2")/1e3)+" & "+fmt.format(e.getProp("Mz2")/1e3)+"\\\\\n")
