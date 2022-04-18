# -*- coding: utf-8 -*-

def listNodeDisplacements(nmbComb, nodeList, fmt, outputFile, ltxSectioning, title):
    ''' Print the displacements of the nodes contained in the list argument.

    :param nmbComb: name of the load combination.
    :param nodeList: node list.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= title
    defCampos= "|l|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
    cabeceraSupertabular(outputFile,8,defCampos,idsCampos,caption)

    for nod in nodeList:
        outputFile.write(nmbComb+" & "+nod.tag+" & ")
        disp= nod.getDisp()
        outputFile.write(fmt.format(disp[0]*1e3)+" & "+fmt.format(disp[1]*1e3)+" & "+fmt.format(disp[2]*1e3)+" & ")
        outputFile.write(fmt.format(disp[3])+" & "+fmt.format(disp[4])+" & "+fmt.format(disp[5])+"\\\\\n")
    cierraSupertabular(outputFile)

def listPointsDisplacements(nmbComb, pointList, fmt, outputFile, ltxSectioning, title):
    ''' Writes a list of the displacements of the nodes associated to the points.

    :param nmbComb: name of the combination for which the displacements
                    were obtained.
    :param pointList: list of points.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= title
    defCampos= "|l|r|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdP & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & - & mm & mm & mm & rad & rad & rad "
    cabeceraSupertabular(outputFile,9,defCampos,idsCampos,caption)

    # Construct the list of starting nodes.
    for pnt in pointList:
        iNode= pnt.getNodeTag()
        outputFile.write(nmbComb+" & "+iPoint+" & "+iNode+" & ")
        nod= nodes.getNode(iNode)
        disp= nod.getDisp()
        outputFile.write(fmt.format(disp[0]*1e3)+" & "+fmt.format(disp[1]*1e3)+" & "+fmt.format(disp[2]*1e3)+" & ")
        outputFile.write(fmt.format(disp[3])+" & "+fmt.format(disp[4])+" & "+fmt.format(disp[5])+"\\\\\n")
    cierraSupertabular(outputFile)

# Imprime los desplazamientos de los nodes contenidos en el conjunto que se pasa como parámetro.
def listNodeDisplacementsSet(nmbComb, xcSet, fmt, outputFile):
    ''' Prints the displacements of the nodes contained in the set argument.

    :param nmbComb: name of the load combination.
    :param xcSet: set containing the points points.
    :param fmt: format for numbers.
    :param outputFile: output file name.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: title.
    '''
    for n in xcSet.nodes:
        outputFile.write(nmbComb+" & "+n.tag+" & ")
        disp= n.getDisp()
        outputFile.write(fmt.format(disp[0]*1e3)+" & "+fmt.format(disp[1]*1e3)+" & "+fmt.format(disp[2]*1e3)+" & ")
        outputFile.write(fmt.format(disp[3])+" & "+fmt.format(disp[4])+" & "+fmt.format(disp[5])+"\\\\\n")

def listNodeDisplacementsSET(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Lists the displacements of the nodes contained in the argument set.

    :param nmbComb: load combination.
    :param xcSet: XC set containing the nodes.
    :param fmt: format string for the displacements.
    :param outputFile: output file.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: section title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Nodes from set: "+xcSet.name
    defCampos= "|l|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
    cabeceraSupertabular(outputFile,8,defCampos,idsCampos,caption)

    listNodeDisplacementsSet(nmbComb= nmbComb, xcSet= xcSet, fmt= fmt, outputFile= outputFile)

    cierraSupertabular(outputFile)

# from model import model_inquiry

# Imprime los desplazamientos of the nodes contenidos en las lineas del conjunto que se pasa como parámetro.
def listNodeDisplacementsLineas(nmbComb, xcSet, fmt, outputFile, ltxSectioning, title):
    ''' Print the displacements of the nodes belonging to the lines 
        contained in the argument set.

    :param nmbComb: load combination.
    :param xcSet: XC set containing the lines.
    :param fmt: format string for the displacements.
    :param outputFile: output file.
    :param ltxSectioning: latex sectioning command (chapter, section,...).
    :param title: section title.
    '''
    outputFile.write("\\"+ltxSectioning+"{"+title+"}\n")
    caption= "Nodes from set: "+xcSet.name
    defCampos= "|l|r|r|r|r|r|r|r|"
    idsCampos= "Caso & IdN & Ux & Uy & Uz & Rx & Ry & Rz \\\\\n - & - & mm & mm & mm & rad & rad & rad "
    cabeceraSupertabular(outputFile,8,defCampos,idsCampos,caption)

    for l in xcSet.lines:
        nmb= l.getName()
        outputFile.write("\hline\n")
        outputFile.write("\\multicolumn{8}{|l|}{Desplazamiento of the nodes de la linea: "+nmb+"}\\\\\n")
        outputFile.write("\hline\n")
        listNodeDisplacementsSet(nmbComb= nmbComb, xcSet= l, fmt= fmt, outputFile= outputFile)
        cierraSupertabular(outputFile)

class RecordListadoDesplazamientos(object):
    formato= '{:7.2f}'
    sectionHeadingA= "subsection"
    title= "Displacements"
    sectionHeadingB= "subsubsection"
    pointsLists= [] 
    listaCabeceras= [] 

    def generaListadoDesplazamientos(self, nmbComb, outputFile):
        ''' Creates a displacements listing.


        :param nmbComb: load combination.
        :param outputFile: output file.
        '''
        outputFile.write("\\"+sectionHeadingA+"{"+title+"}\n")
        j= 0
        for pl in pointsLists:
            listPointsDisplacements(nmbComb= nmbComb, pointList= pl, fmt= '{:7.3f}', outputFile= outputFile, ltxSectioning= sectionHeadingB, title= listaCabeceras[j])
            j+= 1

