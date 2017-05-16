#!/bin/sh
ERT="\\033[1;32m"
NORMAL="\\033[0;39m"
ROUGE="\\033[1;31m"
ROSE="\\033[1;35m"
BLEU="\\033[1;34m"
BLANC="\\033[0;02m"
BLANCLAIR="\\033[1;08m"
JAUNE="\\033[1;33m"
CYAN="\\033[1;36m"

echo ""

START=$(date +%s.%N)

#Test de combinaciones.
echo "$BLEU" "Load combination tests." "$NORMAL"
python tests/combinations/add_loads_01.py
python tests/combinations/suma_hipotesis_02.py
python tests/combinations/combinacion_01.py
python tests/combinations/combinacion_02.py
python tests/combinations/combinacion_03.py
#XXX sqlitepp extremely slow. Seek for another solution.
# python tests/combinations/combinacion_04.py
python tests/combinations/combinacion_05.py
python tests/combinations/combinacion_06.py
python tests/combinations/combinacion_07.py
python tests/combinations/test_pescante_01.py
python tests/combinations/test_pescante_02.py


echo "$BLEU" "Elements tests." "$NORMAL"
echo "$BLEU" "  Truss tests." "$NORMAL"
python tests/elements/truss_test0.py
python tests/elements/truss_test1.py
python tests/elements/truss_test2.py
python tests/elements/truss_temperat.py
echo "$BLEU" "  Coordinate transformations tests." "$NORMAL"
python tests/elements/crd_transf/test_linear_crd_transf_2d_01.py
python tests/elements/crd_transf/test_pdelta_crd_transf_2d_01.py
python tests/elements/crd_transf/test_corot_crd_transf_2d_01.py
python tests/elements/crd_transf/test_linear_crd_transf_3d_01.py
python tests/elements/crd_transf/test_linear_crd_transf_3d_02.py
python tests/elements/crd_transf/test_pdelta_crd_transf_3d_01.py
python tests/elements/crd_transf/test_corot_crd_transf_3d_01.py
python tests/elements/crd_transf/test_corot_crd_transf_3d_02.py
python tests/elements/crd_transf/test_ejes_elemento_01.py
python tests/elements/crd_transf/test_ejes_elemento_02.py
python tests/elements/crd_transf/test_ejes_elemento_03.py
python tests/elements/crd_transf/test_punto_elemento_01.py
python tests/elements/crd_transf/test_crd_transf2d_01.py
python tests/elements/crd_transf/test_crd_transf3d_01.py
python tests/elements/crd_transf/test_pdelta_01.py
python tests/elements/crd_transf/test_pdelta_02.py
echo "$BLEU" "  Beam column tests." "$NORMAL"
python tests/elements/beam_column/beam_test1.py
python tests/elements/beam_column/mensula3d_01.py
python tests/elements/beam_column/mensula3d_02.py
python tests/elements/beam_column/mensula3d_03.py
python tests/elements/beam_column/mensula3d_04.py
python tests/elements/beam_column/mensula3d_05.py
python tests/elements/beam_column/mensula3d_06.py
python tests/elements/beam_column/mensula3d_07.py
python tests/elements/beam_column/mensula3d_08.py
python tests/elements/beam_column/mensula3d_09.py
python tests/elements/beam_column/mensula3d_10.py
python tests/elements/beam_column/elastic_beam2d_sign_criteria_01.py
python tests/elements/beam_column/elastic_beam2d_sign_criteria_02.py
python tests/elements/beam_column/elastic_beam2d_sign_criteria_03.py
python tests/elements/beam_column/elastic_beam3d_sign_criteria_01.py
python tests/elements/beam_column/elastic_beam3d_sign_criteria_02.py
python tests/elements/beam_column/elastic_beam3d_sign_criteria_03.py
python tests/elements/beam_column/test_beam3d_sections_axes_orientation.py
python tests/elements/beam_column/test_force_beam_column_2d_01.py
python tests/elements/beam_column/test_force_beam_column_2d_02.py
python tests/elements/beam_column/test_force_beam_column_2d_03.py
python tests/elements/beam_column/test_force_beam_column_3d_01.py
python tests/elements/beam_column/test_force_beam_column_3d_02.py
python tests/elements/beam_column/test_force_beam_column_3d_03.py
python tests/elements/beam_column/test_force_beam_column_3d_04.py
python tests/elements/beam_column/test_force_beam_column_3d_05.py
python tests/elements/beam_column/test_force_beam_column_3d_06.py
python tests/elements/beam_column/plastic_hinge_on_cantilever.py
python tests/elements/beam_column/test_giro_crdTransf_01.py
python tests/elements/beam_column/test_portico_01.py
python tests/elements/beam_column/test_integration_options_01.py
python tests/elements/beam_column/test_signo_deformaciones01.py
echo "$BLEU" "  Zero length elements tests." "$NORMAL"
python tests/elements/zero_length/zero_length_element_test_01.py
python tests/elements/zero_length/zero_length_element_test_02.py
python tests/elements/zero_length/zero_length_element_test_03.py
python tests/elements/zero_length/zero_length_element_test_04.py
python tests/elements/zero_length/zero_length_element_test_05.py
python tests/elements/zero_length/zero_length_element_test_06.py
python tests/elements/zero_length/criterio_signos_zero_length_section.py
python tests/elements/zero_length/test_signo_deformaciones.py
python tests/elements/zero_length/test_signo_deformaciones01.py
python tests/elements/zero_length/test_signo_deformaciones02.py
python tests/elements/zero_length/test_signo_deformaciones03.py
python tests/elements/zero_length/test_signo_deformaciones04.py
python tests/elements/zero_length/test_winkler.py
python tests/elements/zero_length/test_cuasi_winkler.py
echo "$BLEU" "  Plane elasticity elements tests." "$NORMAL"
python tests/elements/plane/test_tri31.py
python tests/elements/plane/test_four_node_quad_01.py
python tests/elements/plane/test_four_node_quad_02.py
echo "$BLEU" "  Shell elements tests." "$NORMAL"
python tests/elements/shell/test_shell_mitc4_01.py
python tests/elements/shell/test_shell_mitc4_02.py
python tests/elements/shell/test_shell_mitc4_03.py
python tests/elements/shell/test_shell_mitc4_04.py
python tests/elements/shell/test_shell_mitc4_05.py
python tests/elements/shell/test_shell_mitc4_06.py
python tests/elements/shell/test_shell_mitc4_07.py
python tests/elements/shell/test_shell_mitc4_08.py
python tests/elements/shell/test_shell_mitc4_09.py
python tests/elements/shell/test_shell_mitc4_10.py
python tests/elements/shell/test_shell_mitc4_11.py
python tests/elements/shell/test_shell_mitc4_12.py
python tests/elements/shell/test_shell_mitc4_13.py
python tests/elements/shell/test_shell_mitc4_14.py
python tests/elements/shell/test_shell_mitc4_15.py
python tests/elements/shell/test_shell_mitc4_16.py
python tests/elements/shell/test_shell_mitc4_17.py
python tests/elements/shell/test_shell_mitc4_18.py
python tests/elements/shell/test_shell_mitc4_19.py
python tests/elements/shell/test_shell_mitc4_20.py
python tests/elements/shell/test_shell_mitc4_21.py
python tests/elements/shell/test_shell_mitc4_22.py
python tests/elements/shell/test_shell_mitc4_23.py
python tests/elements/shell/test_shell_mitc4_24.py
python tests/elements/shell/test_shell_mitc4_25.py
python tests/elements/shell/test_shell_mitc4_26.py
python tests/elements/shell/test_shell_mitc4_27.py
python tests/elements/shell/test_shell_mitc4_28.py
python tests/elements/shell/test_area_tributaria_01.py
python tests/elements/shell/test_corot_shell_mitc4_01.py
python tests/elements/shell/test_corot_shell_mitc4_02.py
python tests/elements/shell/test_corot_shell_mitc4_03.py
python tests/elements/shell/test_corot_shell_mitc4_04.py
python tests/elements/shell/test_shell_mitc4_natural_coordinates_01.py
python tests/elements/shell/test_transformInternalForces.py

echo "$BLEU" "  Solid elements tests." "$NORMAL"
python tests/elements/volume/test_brick_00.py
python tests/elements/volume/test_brick_01.py

echo "$BLEU" "  Misc elements tests." "$NORMAL"
python tests/elements/spring_test_01.py
python tests/elements/spring_test_02.py
python tests/elements/test_elastomeric_bearing_01.py
python tests/elements/test_pot_bearing_01.py
python tests/elements/test_pot_bearing_02.py
python tests/elements/test_pot_bearing_03.py
python tests/elements/kill_elements_01.py
python tests/elements/kill_elements_02.py

echo "$BLEU" "Solver tests." "$NORMAL"
python tests/solution/superlu_solver_test_01.py

#Test de los manejadores de coacciones.
echo "$BLEU" "  Constraint handler tests." "$NORMAL"
python tests/solution/constraint_handler/plain_handler_test_01.py
python tests/solution/constraint_handler/transformation_handler_test_01.py
python tests/solution/constraint_handler/transformation_handler_test_02.py
python tests/solution/constraint_handler/transformation_handler_test_03.py
python tests/solution/constraint_handler/lagrange_handler_test_01.py

#autovalores
echo "$BLEU" "  Eigenvalue solution tests." "$NORMAL"
python tests/solution/eigenvalues/modos_propios_mensula_01.py
python tests/solution/eigenvalues/modos_propios_mensula_02.py
python tests/solution/eigenvalues/modos_propios_mensula_03.py
python tests/solution/eigenvalues/modos_propios_mensula_04.py
python tests/solution/eigenvalues/modos_propios_mensula_05.py
python tests/solution/eigenvalues/modos_propios_mensula_06.py
python tests/solution/eigenvalues/linear_buckling_column01.py
python tests/solution/eigenvalues/linear_buckling_column02.py
python tests/solution/eigenvalues/linear_buckling_column03.py
python tests/solution/eigenvalues/linear_buckling_column04.py
python tests/solution/eigenvalues/linear_buckling_column05.py
python tests/solution/eigenvalues/test_string_under_tension.py
python tests/solution/eigenvalues/test_analisis_modal_01.py
python tests/solution/eigenvalues/test_analisis_modal_02.py
python tests/solution/eigenvalues/test_analisis_modal_03.py
python tests/solution/eigenvalues/test_analisis_modal_04.py
python tests/solution/eigenvalues/test_analisis_modal_05.py
python tests/solution/eigenvalues/test_cqc_01.py
python tests/solution/eigenvalues/test_band_arpackpp_solver_01.py

#Preprocessor tests
echo "$BLEU" "Preprocessor tests." "$NORMAL"
echo "$BLEU" "  Simpson rule tests." "$NORMAL"
python tests/preprocessor/integra_simpson_fila_i.py
python tests/preprocessor/integra_simpson_fila_j.py
python tests/preprocessor/integra_simpson_fila_k.py

echo "$BLEU" "  Preprocessor entities tests." "$NORMAL"
python tests/preprocessor/cad/linea.py
python tests/preprocessor/cad/linea_02.py
python tests/preprocessor/cad/k_points.py
python tests/preprocessor/cad/linea_tramos.py
python tests/preprocessor/cad/test_sec_lineas_01.py
python tests/preprocessor/cad/test_sec_lineas_02.py
python tests/preprocessor/cad/arco_circunf_01.py
python tests/preprocessor/cad/arco_circunf_02.py
python tests/preprocessor/cad/arco_circunf_03.py
python tests/preprocessor/cad/arco_circunf_04.py
python tests/preprocessor/cad/arco_circunf_05.py
python tests/preprocessor/cad/crea_puntos_linea.py
python tests/preprocessor/cad/test_traslacion_01.py
python tests/preprocessor/cad/test_rotacion_01.py
python tests/preprocessor/cad/test_rotacion_02.py
python tests/preprocessor/cad/test_rotacion_03.py
python tests/preprocessor/cad/test_reflexion_01.py
python tests/preprocessor/cad/test_sis_ref_cartesianas_01.py
python tests/preprocessor/cad/test_sis_ref_cartesianas_02.py
python tests/preprocessor/cad/test_sis_ref_cartesianas_03.py
python tests/preprocessor/cad/test_unif_grid_01.py
python tests/preprocessor/cad/test_unif_grid_02.py
python tests/preprocessor/cad/test_unif_grid_03.py
python tests/preprocessor/cad/test_sup_cuadrilatera_01.py
python tests/preprocessor/cad/test_sup_cuadrilatera_02.py
python tests/preprocessor/cad/test_punto_cruz_01.py
python tests/preprocessor/cad/test_esquema2d.py
python tests/preprocessor/cad/test_esquema3d.py
python tests/preprocessor/cad/test_nearest_node_01.py
python tests/preprocessor/cad/test_nearest_element_01.py
python tests/preprocessor/cad/split_linea_01.py
python tests/preprocessor/cad/split_linea_02.py
python tests/preprocessor/cad/split_linea_03.py
echo "$BLEU" "  Meshing routines tests." "$NORMAL"
python tests/preprocessor/test_surface_meshing_01.py
python tests/preprocessor/test_surface_meshing_02.py
python tests/preprocessor/test_surface_meshing_03.py
python tests/preprocessor/test_surface_meshing_04.py
python tests/preprocessor/test_surface_meshing_05.py
echo "$BLEU" "  Sets handling tests." "$NORMAL"
python tests/preprocessor/sets/mueve_set.py
python tests/preprocessor/sets/test_set_01.py
python tests/preprocessor/sets/une_sets.py
python tests/preprocessor/sets/test_resisting_svd01.py
python tests/preprocessor/sets/test_get_contours_01.py
python tests/preprocessor/sets/test_get_contours_02.py


#Tests condiciones de contorno
echo "$BLEU" "Displacement constraints tests." "$NORMAL"
python tests/constraints/remove_fix_01.py
python tests/constraints/remove_fix_02.py
python tests/constraints/test_elastic_bearing_01.py
python tests/constraints/test_elastic_bearing_02.py
python tests/constraints/test_elastic_bearing_03.py
python tests/constraints/test_rigid_beam_01.py
python tests/constraints/test_rigid_rod_01.py
python tests/constraints/test_fulcrum_01.py
python tests/constraints/test_glue_node_to_element_01.py
python tests/constraints/test_glue_node_to_element_02.py
python tests/constraints/test_glue_node_to_element_03.py
python tests/constraints/test_glue_node_to_element_04.py
python tests/constraints/test_glue_node_to_element_05.py

#Tests de cargas
echo "$BLEU" "Loads tests." "$NORMAL"
python tests/loads/beam2dNodalLoad.py
python tests/loads/beam2dNodalLoad02.py
python tests/loads/beam2dPointLoad.py
python tests/loads/beam2dUniformLoad.py
python tests/loads/beam2dUniformLoad02.py
python tests/loads/beam3dPointLoad.py
python tests/loads/beam3dUniformLoad.py
python tests/loads/beam3dUniformLoad02.py
python tests/loads/beam3dUniformLoad03.py
python tests/loads/test_vector2d_uniform_load_local.py
python tests/loads/test_vector2d_uniform_load_global01.py
python tests/loads/test_vector2d_point_load_local.py
python tests/loads/test_vector2d_point_load_global.py
python tests/loads/test_vector3d_point_load_local.py
python tests/loads/test_vector3d_point_load_global.py
python tests/loads/test_vector3d_uniform_load_local.py
python tests/loads/test_vector3d_uniform_load_global01.py
python tests/loads/test_vector3d_uniform_load_global02.py
python tests/loads/test_vector3d_uniform_load_global03.py
python tests/loads/beam_strain_load_01.py
python tests/loads/beam_strain_load_02.py
python tests/loads/beam_strain_load_03.py
python tests/loads/beam_strain_load_04.py
python tests/loads/beam_strain_load_05.py
python tests/loads/beam_strain_load_06.py
python tests/loads/beam_strain_load_07.py
python tests/loads/beam_strain_load_08.py
python tests/loads/beam_strain_load_09.py
python tests/loads/shell_strain_load_01.py
python tests/loads/testCargaVientoCTE.py
python tests/loads/test_ground_motion_01.py
python tests/loads/test_ground_motion_02.py
python tests/loads/test_ground_motion_03.py
python tests/loads/test_ground_motion_04.py
python tests/loads/test_ground_motion_05.py
python tests/loads/test_ground_motion_06.py
python tests/loads/test_ground_motion_07.py
python tests/loads/test_ground_motion_08.py

#Tests de los materiales
#Materiales uniaxiales.
echo "$BLEU" "Materials tests." "$NORMAL"
echo "$BLEU" "  Uniaxial materials tests." "$NORMAL"
python tests/materials/uniaxial/test_elasticpp_material_01.py
python tests/materials/uniaxial/test_steel01.py
python tests/materials/uniaxial/test_steel02.py
python tests/materials/uniaxial/test_pretensado_steel02.py
python tests/materials/uniaxial/test_concrete01.py
python tests/materials/uniaxial/test_concrete02_01.py
python tests/materials/uniaxial/test_concrete02_02.py
python tests/materials/uniaxial/test_HA25_01.py
python tests/materials/uniaxial/test_HA25_02.py
python tests/materials/uniaxial/test_HP45_01.py
python tests/materials/uniaxial/test_HP45_02.py
echo "$BLEU" "    EHE reinforcing steel." "$NORMAL"
python tests/materials/uniaxial/test_B400S_01.py
python tests/materials/uniaxial/test_B400S_02.py
python tests/materials/uniaxial/test_B500S_01.py
python tests/materials/uniaxial/test_B500S_02.py
echo "$BLEU" "    SIA reinforcing steel." "$NORMAL"
python tests/materials/uniaxial/test_B500A_01.py
python tests/materials/uniaxial/test_B500A_02.py
python tests/materials/uniaxial/test_B500B_01.py
python tests/materials/uniaxial/test_B500B_02.py
echo "$BLEU" "    cable material." "$NORMAL"
python tests/materials/uniaxial/test_cable_material01.py
python tests/materials/uniaxial/test_cable_material02.py
python tests/materials/uniaxial/test_cable_material03.py
python tests/materials/uniaxial/test_cable_material04.py
python tests/materials/uniaxial/test_cable_material05.py
python tests/materials/uniaxial/test_cable_material06.py
python tests/materials/uniaxial/test_cable_material07.py
python tests/materials/uniaxial/test_material_elast_no_traccion.py
#Materiales nD
echo "$BLEU" "  nD materials tests." "$NORMAL"
python tests/materials/test_elastic_isotropic_plane_strain_2d_01.py
python tests/materials/test_elastic_isotropic_plane_stress_2d_01.py
python tests/materials/test_elastic_isotropic_3d_01.py

#Secciones transversales.
#Secciones transversales. Definición geométrica.
echo "$BLEU" "  Section geometry tests." "$NORMAL"
python tests/materials/geom_section/test_spot_01.py
python tests/materials/geom_section/test_segmento_01.py
python tests/materials/geom_section/test_reg_circ_01.py
python tests/materials/geom_section/test_rg_cuadrilatera_01.py
python tests/materials/geom_section/test_rg_cuadrilatera_02.py
python tests/materials/geom_section/test_rg_arco_corona_circular_01.py
python tests/materials/geom_section/test_rg_arco_corona_circular_02.py
python tests/materials/geom_section/test_geom_caract_mec_01.py
python tests/materials/geom_section/test_geom_caract_mec_02.py
python tests/materials/geom_section/test_geom_caract_mec_03.py
python tests/materials/geom_section/test_geom_caract_mec_04.py
python tests/materials/geom_section/test_geom_params_torsion_ehe_01.py
echo "$BLEU" "  Steel sections tests." "$NORMAL"
python tests/materials/steel_shapes/test_perfiles_aisc_w.py
python tests/materials/steel_shapes/test_perfiles_aisc_hss.py
python tests/materials/steel_shapes/test_perfil_metalico_01.py
python tests/materials/steel_shapes/test_perfil_metalico_02.py

#Secciones transversales. Modelo de fibras.
echo "$BLEU" "  Elastic section tests." "$NORMAL"
python tests/materials/elastic_section/test_elastic_section_2d_sign_convention_01.py
python tests/materials/elastic_section/test_elastic_section_2d_01.py
python tests/materials/elastic_section/test_elastic_shear_section_2d_01.py
python tests/materials/elastic_section/test_elastic_section_3d_01.py
python tests/materials/elastic_section/test_elastic_section_3d_02.py
python tests/materials/elastic_section/test_elastic_shear_section_3d_01.py
python tests/materials/elastic_section/test_tangent_stiffness_02.py
python tests/materials/elastic_section/test_giro_seccion_3d_01.py
python tests/materials/elastic_section/test_giro_seccion_3d_02.py
python tests/materials/elastic_section/test_giro_seccion_3d_03.py
python tests/materials/elastic_section/test_giro_seccion_3d_04.py
echo "$BLEU" "  Fiber section tests." "$NORMAL"
python tests/materials/fiber_section/test_reg_cuad_01.py
python tests/materials/fiber_section/test_capa_armadura_recta_01.py
python tests/materials/fiber_section/test_fiber_section_discretization_error_01.py
python tests/materials/fiber_section/test_fiber2d_01.py
python tests/materials/fiber_section/test_fiber3d_01.py
python tests/materials/fiber_section/test_fiber3d_02.py
python tests/materials/fiber_section/test_fiber3d_03.py
python tests/materials/fiber_section/test_fiber3d_04.py
python tests/materials/fiber_section/test_fiber3d_05.py
python tests/materials/fiber_section/test_fiber3d_06.py
python tests/materials/fiber_section/test_fiber3d_07.py
python tests/materials/fiber_section/test_fiber3d_08.py
python tests/materials/fiber_section/test_fiber3d_09.py
python tests/materials/fiber_section/test_fiber_section_01.py
python tests/materials/fiber_section/test_fiber_section_02.py
python tests/materials/fiber_section/test_fiber_section_03.py
python tests/materials/fiber_section/test_fiber_section_04.py
python tests/materials/fiber_section/test_fiber_section_05.py
python tests/materials/fiber_section/test_fiber_section_06.py
python tests/materials/fiber_section/test_fiber_section_07.py
python tests/materials/fiber_section/test_fiber_section_08.py
python tests/materials/fiber_section/test_fiber_section_09.py
python tests/materials/fiber_section/test_fiber_section_10.py
python tests/materials/fiber_section/test_fiber_section_11.py
python tests/materials/fiber_section/test_fiber_section_12.py
python tests/materials/fiber_section/test_fiber_section_13.py
python tests/materials/fiber_section/test_tangent_stiffness_01.py
python tests/materials/fiber_section/test_section_aggregator_01.py
python tests/materials/fiber_section/test_fiber_section_shear3d_01.py
python tests/materials/fiber_section/test_fiber_section_shear3d_02.py
python tests/materials/fiber_section/test_diag_interaccion01.py
python tests/materials/fiber_section/test_diag_interaccion02.py
python tests/materials/fiber_section/test_diag_interaccion03.py
python tests/materials/fiber_section/test_diag_interaccion04.py
python tests/materials/fiber_section/test_diag_interaccion05.py
python tests/materials/fiber_section/test_diag_interaccion06.py
python tests/materials/fiber_section/test_cortante_01.py
python tests/materials/fiber_section/test_cortante_02.py
python tests/materials/fiber_section/plastic_hinge_on_IPE200.py
echo "$BLEU" "  RC sections test." "$NORMAL"
python tests/materials/ehe/test_Ecm_hormigon.py
python tests/materials/ehe/test_EHEconcrete.py
echo "$BLEU" "    Crack amplitude determination tests (EHE-08)." "$NORMAL"
python tests/materials/ehe/test_fisuracion_01.py
python tests/materials/ehe/test_fisuracion_02.py
python tests/materials/ehe/test_fisuracion_03.py
python tests/materials/ehe/test_fisuracion_04.py
python tests/materials/ehe/test_fisuracion_05.py
python tests/materials/ehe/test_fisuracion_06.py
echo "$BLEU" "    Shear resistance determination tests (EHE-08)." "$NORMAL"
python tests/materials/ehe/test_cortante_ehe08_01.py
python tests/materials/ehe/test_cortante_ehe08_02.py
python tests/materials/ehe/test_cortante_ehe08_03.py
python tests/materials/ehe/test_cortante_ehe08_04.py
python tests/materials/ehe/test_cortante_ehe08_05.py
echo "$BLEU" "    Misd. (EHE-08)." "$NORMAL"
python tests/materials/ehe/test_mensula_corta_ehe.py
echo "$BLEU" "   EC2 tests." "$NORMAL"
python tests/materials/ec2/test_EC2Concrete.py
python tests/materials/ec2/test_EC2creep.py
python tests/materials/ec2/test_EC2shrinkage.py
python tests/materials/ec2/test_smearedCracking_01.py
python tests/materials/ec2/test_smearedCracking_02.py
python tests/materials/ec2/test_smearedCracking_03.py
python tests/materials/ec2/test_smearedCracking_04.py
python tests/materials/ec2/test_smearedCracking_05.py
python tests/materials/ec2/test_smearedCracking_06.py
echo "$BLEU" "   EC3 tests." "$NORMAL"
python tests/materials/ec3/test_buckling_resistance_01.py
python tests/materials/ec3/test_lateral_torsional_buckling00.py
python tests/materials/ec3/test_lateral_torsional_buckling01.py
python tests/materials/ec3/test_lateral_torsional_buckling02.py
python tests/materials/ec3/test_lateral_torsional_buckling03.py
python tests/materials/ec3/test_cross_section_verification.py
echo "$BLEU" "    SIA 262 tests." "$NORMAL"
python tests/materials/sia262/shear_01.py
python tests/materials/sia262/test_anchorage_length.py
python tests/materials/sia262/test_crack_control_01.py
echo "$BLEU" "    SIA 263 tests." "$NORMAL"
python tests/materials/sia263/test_coefs_fatique_SIA263.py
echo "$BLEU" "    shell elements." "$NORMAL"
python tests/materials/xLamina/test_xlamina_eluTN.py

echo "$BLEU" "  Plate and membrane materials." "$NORMAL"
python tests/materials/test_material_elastic_membrane_plate_section_01.py
python tests/materials/test_material_elastic_plate_section_01.py
python tests/materials/test_inercia_torsion_cajon.py

echo "$BLEU" "  Other materials tests." "$NORMAL"
python tests/materials/test_elastomeric_bearing_stiffness.py
python tests/materials/test_anchor_bolt01.py
python tests/materials/ehe/test_fluencia_01.py
python tests/materials/ehe/test_fluencia_02.py
python tests/materials/ehe/test_fluencia_03.py
python tests/materials/ehe/test_retraccion_01.py
#python tests/materials/ehe/test_retraccion_02.py
python tests/materials/ehe/test_relajacion_01.py

echo "$BLEU" "  Steel design verifications (EAE)." "$NORMAL"
python tests/materials/eae/verif_casquillo_rigidizado.py
python tests/materials/eae/verif_apoyo_por_soldadura_alma.py
python tests/materials/eae/verif_resistencia_cordon.py

#Tests de database
echo "$BLEU" "Database tests (MySQL, Berkeley db, sqlite,...)." "$NORMAL"
# XXX sqlitepp is toooooo slooooow. Seek another solution.
# python tests/database/test_database_01.py
# python tests/database/test_database_02.py
# python tests/database/test_database_03.py
# python tests/database/test_database_04.py
# python tests/database/test_database_05.py
# python tests/database/test_database_06.py
# python tests/database/test_database_07.py
# XXX
python tests/database/test_database_08.py
python tests/database/test_database_09.py
python tests/database/test_database_10.py
python tests/database/test_database_11.py
python tests/database/test_database_12.py
python tests/database/test_database_13.py
python tests/database/test_database_14.py
python tests/database/test_database_15.py
python tests/database/prueba_sqlite_01.py
python tests/database/prueba_sqlite_02.py
python tests/database/prueba_sqlite_03.py
python tests/database/prueba_readln_05.py

echo "$BLEU" "Verifiyng import/export routines (Salome, Code_Aster,...)." "$NORMAL"
echo "$ROSE" "  MED tests are in quarantine (some debugging pending)." "$NORMAL"
#python tests/utility/med_xc/test_exporta_med01.py
#python tests/utility/med_xc/test_exporta_med02.py

echo "$BLEU" "Verifiying routines for rough calculations,...)." "$NORMAL"
python tests/rough_calculations/test_punzo01.py
python tests/rough_calculations/test_punzo02.py
python tests/rough_calculations/test_min_dim_abut_support.py
python tests/rough_calculations/test_cantilever01.py
python tests/rough_calculations/test_simple_beam_01.py
python tests/rough_calculations/test_simple_beam_02.py
python tests/rough_calculations/test_earth_pressure_kray.py
python tests/rough_calculations/test_suspension_bridge_simple_model.py
python tests/rough_calculations/test_cable_stayed_bridge_simple_model.py
python tests/rough_calculations/test_arch_bridge_simple_model.py
python tests/rough_calculations/test_tied_arch_simple_model.py
python tests/rough_calculations/test_footing_near_slope01.py
python tests/rough_calculations/masonry_bridge_verif_01.py
python tests/rough_calculations/masonryBridgeVerif_test01_minim.py

#Postprocess tests
echo "$BLEU" "Verifiying routines for post processing,...)." "$NORMAL"
python tests/postprocess/test_export_shell_internal_forces.py

#VTK tests
##python tests/vtk/dibuja_edges.py

END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo $DIFF seconds
NT=$(grep -c '^python' $0)
echo ${NT} tests
Q=$(echo "$DIFF / $NT" | bc -l)
echo $Q seconds/test
