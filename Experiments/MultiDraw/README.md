# MultiDraw

Draw the sponza model with a single draw call using glMultiDrawElements. Drawing each mess separately takes 394 draw calls. With glMultiDrawElements each mess has to use the same shader program, uniforms etc.