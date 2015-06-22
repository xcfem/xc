\nverborrea{0}


\mdlr
  {
    \c{Definimos materiales}
    \materiales{ \elastic_material[elast,1]{\E{3000}} }

    \c{Definimos nodos}
    \ncoo_def_nod{3} \c{Dimensiones de los nodos}
    \ngdl_def_nod{3} \c{Grados de libertad de los nodos}
    \nod_semilla[0,3]{\coo{0.0,0.0,0.0}}
    \elem_semilla{ \truss[1,elast,3]{ \A{10.0} } }
    \cad
      {
        \incluye{test/portico/geom_columna.xc}
      }

    \sets
      {
        \total
          {
            \malla{}
            
            \incluye{test/portico/carga_malla_vtk.xcm}\c{Cargamos nodos y elementos en vtk.}
          }
      }

    \incluye{test/portico/print_nod_elem.xcm}
  }

\c{
\c{Cargamos kpoint y lineas en vtk.}
\mdlr{\cad{\incluye{test/portico/carga_lineas_vtk.xcm}}}
  }


\plot
   {
     \c{****** Creamos las etiquetas para los puntos *******}

     \c{vtkIdFilter genera escalares a partir del identificador del punto}
     \define[vtkIdFilter,ids]
       {
         \c{\set_input{ugrid}}
         \set_input{ugrid_mesh}
         \cell_ids_off{}
         \point_ids_off{}
       }

     \c{ \incluye{test/portico/dibuja_kpt_lineas.xcm} }


     \incluye{test/portico/dibuja_nod_elem.xcm}

     \renderer
       { 
         \add_actor2D{pointLabels} \c{Actor de las etiquetas de los puntos}
         \add_actor2D{cellLabels} \c{Actor de las etiquetas de las celdas}
       }
     \c{****** Fin creación etiquetas *******}

     \incluye{test/portico/muestra_ventana.xcm}
  }