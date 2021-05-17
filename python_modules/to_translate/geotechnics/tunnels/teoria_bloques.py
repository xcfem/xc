# -*- coding: utf-8 -*-
# Macros that are useful in block theory calculations (see Goodman & Shi, Block theory and its application to rock engineering 1985)
from __future__ import print_function


'''Return the dip plane defined by:
   alpha: dip angle.
   beta: dip direction.
   p: point for which the plane passes through.'''
def computeDipPlane(alpha,beta,pos3d p):
  A= math.sin(alpha)*sin(beta)
  B= math.sin(alpha)*cos(beta)
  C= math.cos(alpha)
  D= -(A*p.x+B*p.y+C*p.z)

  retval= ecuacion_general([A,B,C,D])
  return retval

def processPolyhedralAngle(nmbAng,nmbTunel):
  {
    print("\n\\subsection{Polyhedral angle: ",nmbAng,"}\n")
    print("\n\\begin{alltt}\n")
    if(not(JPvacio))
        \then
          {
            if(containsTunnelAxis)
                \then
                  {
                    print("  Contains tunnel axis= Yes.\n")
                    print("\\end{alltt}\n")
                  }
                else:
                  {
                    print("  Contains tunnel axis= No.\n")
                    print("  Vectores externos:\n")
                    print("    I1= ",I1," I2= ",I2,"\n")
                    print("    angI1= ",rad2deg(angI1),"\n")
                    print("    angI2= ",rad2deg(angI2),"\n")
                    print("    angI1I2= ",rad2deg(angI1I2),"\n")
                    if(angI1I2>=PI)
                        \then
                          {
                            print("  La cuña abarca toda la sección (bloque inamovible).\n")
                            print("\\end{alltt}\n")
                          }
                        else:
                          {
                            print("  Pirámide:\n")
                            print("    points of tangency= ",ptsTang,"\n")
                            print("    cúspide= ",p3,"\n")
                            \psplot
                              {
                                \pageSize{"letter"}
                                \open{nmbTunel+nmbAng+".ps"}
                                \fspace{0.0, 0.0, 30.0, 30.0} # specify user coor system
                                \lineWidth{0.1} # line thickness in user coordinates
                                \penColorName{"blue"}  # path will be drawn in red
                                \erase{} # erase Plotter's graphics display
                                \tunnelSection
                                  {
                                    nv= getNumVertices
                                    ptPlot= at(1)
                                    \moveTo{ptPlot.x,ptPlot.y}
                                    i= 0
                                    \for
                                      {
                                        \inicio{i=2}} \continua{i<=nv} \incremento{\set{i=i+1}
                                        \bucle
                                           {
                                             ptPlot= at(i)
                                             \cont{ptPlot.x,ptPlot.y}
                                           }
                                      }
                                    ptPlot= at(1)
                                    \cont{ptPlot.x,ptPlot.y}
                                  }
                                \penColorName{"red"}  # path will be drawn in red
                                \moveTo{p1.x,p1.y}
                                \cont{p3.x,p3.y}
                                \cont{p2.x,p2.y}
                                \flush{}
                                \close{}
                                print("\\end{alltt}\n")
                                print("\\begin{figure}\n")
                                print("  \\centering\n")
                                print("  \\includegraphics[width=40mm]{",nmbTunel+nmbAng+".ps","}\n")
                                print("  \\caption{",caption+" Pirámide "+ nmbAng,"}\n")
                                print("\\end{figure}\n")
                              }
                          }
                      }
                  }
              }
          }
        else:
          {
            print("  El bloque generado es inamovible.\n")
            print("\\end{alltt}\n")
          }
      }
  }

'''Computes the points of tangency of the external vectors with the section.'''
def computePointsOfTangency(sectionName,vector2d vt1,vector2d vt2):
  {
    retval= 
    \sectionName{retval= getPosTangAprox(vt1)}
    pt0= 
    pt0= retval.at(1)
    \retval{\clear{}}
    \sectionName{retval= getPosTangAprox(vt2)}
    pt1= 
    pt1= retval.at(1)
    \retval{\clear{}}
    \retval{insert(pt0) insert(pt1)}
    return retval
  }
