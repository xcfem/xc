\c{Test de elaboración propia

       ^ Y
       |

       4----------3
       |          |
       |          |
       |          |
       |          |
       1----------2   -->X

 Un elemento SHELL con lados unidad y nodo 1 en el origen se rota 45
grados en torno al eje paralelo al X por su centro de gravedad moviéndose
como un sólido rígido.

A continuación se le imponen a sus nodos los siguientes movimientos expresados
en coordenadas locales (-1,-1),(-1,1),(1,1),(-1,1).

Las fuerzas en los nodos debidas al movimiento se comparan con las de un elemento
shell convencional que inicialmente está en la posición girada y sufre la misma
deformación.
  }
\path{"/usr/local/lib/macros_xc"}
\archivo_log{"/tmp/borrar.log"} \c{Para no imprimir mensajes de advertencia.}

\expr{E= 2.1e6} \c{Módulo de Young del acero en kg/cm2.}
\expr{nu= 0.3} \c{Coeficiente de Poisson.}
\expr{h= 0.1} \c{Espesor.}
\expr{dens= 1.33} \c{Densidad kg/m2.}

\def_prop["movLocalN1Corot","vector"]{}
\def_prop["movLocalN2Corot","vector"]{}
\def_prop["movLocalN3Corot","vector"]{}
\def_prop["movLocalN4Corot","vector"]{}
\def_prop["fuerzaN1Corot","vector"]{}
\def_prop["fuerzaN2Corot","vector"]{}
\def_prop["fuerzaN3Corot","vector"]{}
\def_prop["fuerzaN4Corot","vector"]{}
\def_prop["R","m_double"]{}
\def_prop["R0","m_double"]{}

\def_prop["movLocalN1","vector"]{}
\def_prop["movLocalN2","vector"]{}
\def_prop["movLocalN3","vector"]{}
\def_prop["movLocalN4","vector"]{}
\def_prop["fuerzaN1","vector"]{}
\def_prop["fuerzaN2","vector"]{}
\def_prop["fuerzaN3","vector"]{}
\def_prop["fuerzaN4","vector"]{}
\def_prop["TangentCorot","m_double"]{}
\def_prop["Tangent","m_double"]{}
\def_prop["InitialTangentCorot","m_double"]{}
\def_prop["InitialTangent","m_double"]{}

\def_prop["cos45","double"]{@cos(@PI/4)}


\c{Tipo de problema}
\incluye{"modelo/gdls_resist_materiales3D.xci"}
\mdlr
  {
    \nodos
      {
        \nod[1]{\coo{0,0.5*(1-@cos45),-0.5*@cos45}}
        \nod[2]{\coo{1,0.5*(1-@cos45),-0.5*@cos45}}
        \nod[3]{\coo{1,0.5*(1+@cos45),0.5*@cos45}}
        \nod[4]{\coo{0,0.5*(1+@cos45),0.5*@cos45}}

        \nod[11]{\coo{0,0.5,-0.5}}
        \nod[12]{\coo{1,0.5,-0.5}}
        \nod[13]{\coo{1,0.5,0.5}}
        \nod[14]{\coo{0,0.5,0.5}}
      }
    \c{Definimos materiales}
    \materiales
      { \elastic_membrane_plate_section["memb1"]{ \nu{nu} \E{E} \h{h} \rho{dens} } }
    \elementos
      {
        \nmb_material{"memb1"}
        \elem_corot_shell_mitc4{ \nodes{1,2,3,4} }
        \elem_shell_mitc4{ \nodes{11,12,13,14} }
      }
    \nodos
      {
        \nod[1]{\setTrialDisp{[0,0.5*@cos45,-0.5*(1-@cos45)]}}
        \nod[2]{\setTrialDisp{[0,0.5*@cos45,-0.5*(1-@cos45)]}}
        \nod[3]{\setTrialDisp{[0,-0.5*@cos45,0.5*(1-@cos45)]}}
        \nod[4]{\setTrialDisp{[0,-0.5*@cos45,0.5*(1-@cos45)]}}

        \nod[11]{\setTrialDisp{[0,0,0]}}
        \nod[12]{\setTrialDisp{[0,0,0]}}
        \nod[13]{\setTrialDisp{[0,0,0]}}
        \nod[14]{\setTrialDisp{[0,0,0]}}

        \nod[1]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n"}}
        \nod[11]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n\n"}}
        \nod[2]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n"}}
        \nod[12]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n\n"}}
        \nod[3]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n"}}
        \nod[13]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n\n"}}
        \nod[4]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n"}}
        \nod[14]{ \print{@coord+[@getTrialDisp[0],@getTrialDisp[1],@getTrialDisp[2]],"\n\n"}}
      }
    \elementos
      {
        \element[0]
          {
            \crd_transf
              {
                \update{}
                \set{R0= @getR0Matrix}
                \set{R= @getRMatrix}
                \set{movLocalN1Corot= @getBasicTrialDisp(0)}
                \set{movLocalN2Corot= @getBasicTrialDisp(1)}
                \set{movLocalN3Corot= @getBasicTrialDisp(2)}
                \set{movLocalN4Corot= @getBasicTrialDisp(3)}
              }
            \set{fuerzaN1Corot= @getNodeResistingForce(0)}
            \set{fuerzaN2Corot= @getNodeResistingForce(1)}
            \set{fuerzaN3Corot= @getNodeResistingForce(2)}
            \set{fuerzaN4Corot= @getNodeResistingForce(3)}
            \set{TangentCorot= @getTangentStiffness}
            \set{InitialTangentCorot= @getInitialStiffness}
          }
        \element[1]
          {
            \crd_transf
              {
                \update{}
                \set{R0= @getR0Matrix}
                \set{movLocalN1= @getBasicTrialDisp(0)}
                \set{movLocalN2= @getBasicTrialDisp(1)}
                \set{movLocalN3= @getBasicTrialDisp(2)}
                \set{movLocalN4= @getBasicTrialDisp(3)}
              }
            \set{fuerzaN1= @getNodeResistingForce(0)}
            \set{fuerzaN2= @getNodeResistingForce(1)}
            \set{fuerzaN3= @getNodeResistingForce(2)}
            \set{fuerzaN4= @getNodeResistingForce(3)}
            \set{Tangent= @getTangentStiffness}
            \set{InitialTangent= @getInitialStiffness}
          }
      }
  }
\def_prop["ratio1","double"]{@abs(@fuerzaN1Corot-@fuerzaN1)}
\def_prop["ratio2","double"]{@abs(@fuerzaN2Corot-@fuerzaN2)}
\def_prop["ratio3","double"]{@abs(@fuerzaN3Corot-@fuerzaN3)}
\def_prop["ratio4","double"]{@abs(@fuerzaN4Corot-@fuerzaN4)}
\def_prop["ratio5","double"]{@abs(@TangentCorot-@Tangent)}
\def_prop["ratio6","double"]{@abs(@InitialTangentCorot-@InitialTangent)}

\print{"matriz R0= ",@R0,"\n"}
\print{"matriz R= ",@R,"\n"}
\print{"mov. local nodo 1: ",@movLocalN1Corot,"\n"}
\print{"mov. local nodo 2: ",@movLocalN2Corot,"\n"}
\print{"mov. local nodo 3: ",@movLocalN3Corot,"\n"}
\print{"mov. local nodo 4: ",@movLocalN4Corot,"\n"}
\print{"fuerza nodo 1 (corot): ",@fuerzaN1Corot,"\n"}
\print{"fuerza nodo 1 : ",@fuerzaN1,"\n"}
\print{"fuerza nodo 2 (corot): ",@fuerzaN2Corot,"\n"}
\print{"fuerza nodo 2 : ",@fuerzaN2,"\n"}
\print{"fuerza nodo 3 (corot): ",@fuerzaN3Corot,"\n"}
\print{"fuerza nodo 3 : ",@fuerzaN3,"\n"}
\print{"fuerza nodo 4 (corot): ",@fuerzaN4Corot,"\n"}
\print{"fuerza nodo 4 : ",@fuerzaN4,"\n"}
\c{
\print{"Tangent (corot): ",@TangentCorot,"\n"}
\print{"Tangent : ",@Tangent,"\n"}
\print{"InitialTangent (corot): ",@TangentCorot,"\n"}
\print{"InitialTangent : ",@Tangent,"\n"}
  }
\print{"ratio1: ",@ratio1,"\n"}
\print{"ratio2: ",@ratio2,"\n"}
\print{"ratio3: ",@ratio3,"\n"}
\print{"ratio4: ",@ratio4,"\n"}
\print{"ratio5: ",@ratio5,"\n"}
\print{"ratio6: ",@ratio6,"\n"}

\if((@ratio1<1e-6) & (@ratio2<1e-6) & (@ratio3<1e-6) & (@ratio4<1e-6) & (@ratio5<1e-9) & (@ratio6<1e-9))
  {
    \then{\print{"test corot shell MITC4 08: pasa.\n"}}
    \else{\print{"test corot shell MITC4 08: falla.\n"}}
  }

