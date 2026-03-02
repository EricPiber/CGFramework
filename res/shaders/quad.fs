uniform vec2 u_resolution;
uniform int u_task;
uniform float u_pi;

varying vec2 v_uv;

void main()
{
    vec2 uv = v_uv;
    uv.x *= (u_resolution.x/u_resolution.y);
    
    vec3 color;
    vec3 r = vec3(1,0,0);
    vec3 g = vec3(0,1,0);
    vec3 b = vec3(0,0,1);
    vec3 black = vec3(0);
    vec3 white = vec3(1);
    
    if(u_task == 0) {
        color = mix(b, r, uv.x);
    } else if (u_task == 1) {
        
        vec2 center = vec2(0.5, 0.5);
        vec3 gray = vec3(211.0/255.0);
        float rad = distance(center, uv)*2.0;
        
        color = mix(black, gray, rad);
        
    } else if (u_task == 2) {
        
        float n_stripes = 5.0;
        float sq_size = 1.0/n_stripes;
        float red = mod(uv.x, sq_size);
        float blue = mod(uv.y, sq_size);
        
        color = r*step(2.0*sq_size/3.0, red) + b*step(2.0*sq_size/3.0, blue);
        
    } else if (u_task == 3) {
        
        float n_sq = 16.0;
        float sq_size = 1.0/n_sq;
        float remain_x = mod(uv.x, sq_size);
        float remain_y = mod(uv.y, sq_size);
        vec2 sq_uv = vec2(uv.x - remain_x, uv.y - remain_y);
        
        color = vec3(sq_uv, 0.0);
        
    } else if (u_task == 4) {
        
        float n_sq = 8.0;
        float sq_size = 1.0/n_sq;
        float remain_x = mod(uv.x, sq_size);
        float remain_y = mod(uv.y, sq_size);
        
        float temp_x = step(sq_size/2.0, remain_x);
        float temp_y = step(sq_size/2.0, remain_y);
        
        color = black;
        color += white * mod(temp_x + temp_y, 2.0);
        /*
         note that:
         - if only one temp returns 1, then color = white
         - if both, or none return 1, then mod(sum, 2) = 0, so color = black
         - each sq is thought as 4 small white and black squares
         */
        
    } else if (u_task == 5) {
        
        float alpha = uv.x * 2.0 * u_pi;
        float f_sin = sin(alpha);   // A = 2; from -1 to 1
        f_sin /= 4.0;               // A = 0.5; from -0.25 to 0.25
        f_sin += 0.5;               // A = 0.5; from 0.25 to 0.75
        // now f_sin is in uv coord
        
        float temp = step(f_sin, uv.y);     // above or below sinusoide
        
        float sin_min_max = 0.25*temp + 0.75*(1.0-temp);
        // above? --> compute vertical distance until 0.25
        // below? --> compute vertical distance until 0.75
        float grad_y = distance(vec2(uv.x, sin_min_max), uv)/0.75;
        // normalized value, to compute interpolation

        color = mix(g, black, grad_y);
        
    }
    else {
        color = vec3(uv, 0.0);
    }
    
    gl_FragColor = vec4(color, 1.0);
}
