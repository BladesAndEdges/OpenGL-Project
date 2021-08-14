#version 450 core

out vec4 FragColor;
  
in vec3 out_Colour; 

void main()
{
    FragColor = vec4(out_Colour, 1.0f);
} 