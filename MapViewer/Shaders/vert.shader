#version 410 core                                                 
                                                                  
in vec4 position;
in vec3 color;

out vec3 point_color;
                        
uniform mat4 ModelToWorld_Matrix;                                                      
uniform mat4 WorldToView_Matrix;                                             
uniform mat4 Projective_Matrix;                                           
                                                                    
void main(void)                                                     
{                                
	
	point_color = color;  
   
	vec4 viewPos = WorldToView_Matrix * position;
	gl_Position = Projective_Matrix * viewPos; 


	gl_PointSize = 2.0 ;    

	

}                                               