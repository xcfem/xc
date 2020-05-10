Test concerning elements birth and death\:

As Mr. Allen George explains [here:](https://www.researchgate.net/post/Can_anyone_explain_me_how_to_do_element_birth_and_death_techniques_in_ansys_apdl_since_i_need_to_simulate_Welding)

FEM solves essentially the equation [u] =[K]-1[F]. where u, K and F represent 
Displacement, Stiffness and Load.

K is the so called Stiffness Matrix composed of the stiffness of each individual element. Element birth and death is simply setting these individual stiffness to zero by setting the Young's Modulus to 0. Well not exactly to zero... it multiplies the Young's modulus with a factor of 10-6. So numerically speaking...0.

