uniform vec2 u_resolution;
uniform int u_task;
uniform int u_exercise;
uniform float u_pi;
uniform float u_aspect;
uniform float u_time;
uniform sampler2D u_texture;

varying vec2 v_uv;

void main()
{
    vec2 uv = v_uv;
    
    vec3 color;
    vec4 color4;
    vec3 r = vec3(1,0,0);
    vec3 g = vec3(0,1,0);
    vec3 b = vec3(0,0,1);
    vec3 black = vec3(0);
    vec3 white = vec3(1);
    
    if(u_exercise == 0) {
        uv.x *= u_aspect;
        if(u_task == 0) {
            color = mix(b, r, uv.x/u_aspect);
        } else if (u_task == 1) {
            
            vec2 center = vec2(0.5*u_aspect, 0.5);
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
            sq_uv.x /= u_aspect;
            
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
        color4 = vec4(color, 1.0);
    } else if (u_exercise == 1) {
        
        color4 = texture2D(u_texture, uv);

        if (u_task == 0) {
            vec3 grayscale = vec3(0.299, 0.587, 0.114);
            vec3 temp = vec3(dot(color4.xyz, grayscale));
            color4 = vec4(temp, color4.w);
        } else if (u_task == 1) {
            color4 = vec4(1.0-color4.xyz, color4.w);
        } else if (u_task == 2) {
            float yellow = mix(color4.x, color4.y, 0.5);
            color4.xyz = vec3(yellow, yellow, 0.0);
        } else if (u_task == 3) {
            vec3 grayscale = vec3(0.299, 0.587, 0.114);
            float temp = dot(color4.xyz, grayscale);
            color4.xyz = vec3(1.0*step(0.5, temp));
        } else if (u_task == 4) {
            float grad = 1.0-(distance(gl_FragCoord.xy, u_resolution/2.0)/u_resolution.y);
            color4.xyz *= grad;
        } else if (u_task == 5) {
            // 5x5 box blur:
            vec2 texel = 1.0 / u_resolution;
            
            vec4 r1_c1 = texture2D(u_texture, uv - 2.0*texel);
            vec4 r1_c2 = texture2D(u_texture, vec2(uv.x-texel.x, uv.y-2.0*texel.y));
            vec4 r1_c3 = texture2D(u_texture, vec2(uv.x, uv.y-2.0*texel.y));
            vec4 r1_c4 = texture2D(u_texture, vec2(uv.x+texel.x, uv.y-2.0*texel.y));
            vec4 r1_c5 = texture2D(u_texture, vec2(uv.x+2.0*texel.x, uv.y-2.0*texel.y));

            vec4 r2_c1 = texture2D(u_texture, vec2(uv.x-2.0*texel.x, uv.y-texel.y));
            vec4 r2_c2 = texture2D(u_texture, uv - texel);
            vec4 r2_c3 = texture2D(u_texture, vec2(uv.x, uv.y-texel.y));
            vec4 r2_c4 = texture2D(u_texture, vec2(uv.x+texel.x, uv.y-texel.y));
            vec4 r2_c5 = texture2D(u_texture, vec2(uv.x+2.0*texel.x, uv.y-texel.y));

            vec4 r3_c1 = texture2D(u_texture, vec2(uv.x-2.0*texel.x, uv.y));
            vec4 r3_c2 = texture2D(u_texture, vec2(uv.x-texel.x, uv.y));
            vec4 r3_c3 = color4;
            vec4 r3_c4 = texture2D(u_texture, vec2(uv.x+texel.x, uv.y));
            vec4 r3_c5 = texture2D(u_texture, vec2(uv.x+2.0*texel.x, uv.y));

            vec4 r4_c1 = texture2D(u_texture, vec2(uv.x-2.0*texel.x, uv.y+texel.y));
            vec4 r4_c2 = texture2D(u_texture, vec2(uv.x-texel.x, uv.y+texel.y));
            vec4 r4_c3 = texture2D(u_texture, vec2(uv.x, uv.y+texel.y));
            vec4 r4_c4 = texture2D(u_texture, uv + texel);
            vec4 r4_c5 = texture2D(u_texture, vec2(uv.x+2.0*texel.x, uv.y+texel.y));

            vec4 r5_c1 = texture2D(u_texture, vec2(uv.x-2.0*texel.x, uv.y+2.0*texel.y));
            vec4 r5_c2 = texture2D(u_texture, vec2(uv.x-texel.x, uv.y+2.0*texel.y));
            vec4 r5_c3 = texture2D(u_texture, vec2(uv.x, uv.y+2.0*texel.y));
            vec4 r5_c4 = texture2D(u_texture, vec2(uv.x+texel.x, uv.y+2.0*texel.y));
            vec4 r5_c5 = texture2D(u_texture, uv + 2.0*texel);
            
            color4.xyz = (
            r5_c1.xyz + r5_c2.xyz + r5_c3.xyz + r5_c4.xyz + r5_c5.xyz +
            r4_c1.xyz + r4_c2.xyz + r4_c3.xyz + r4_c4.xyz + r4_c5.xyz +
            r3_c1.xyz + r3_c2.xyz + r3_c3.xyz + r3_c4.xyz + r3_c5.xyz +
            r2_c1.xyz + r2_c2.xyz + r2_c3.xyz + r2_c4.xyz + r2_c5.xyz +
            r1_c1.xyz + r1_c2.xyz + r1_c3.xyz + r1_c4.xyz + r1_c5.xyz
            ) / 25.0;
        }
    } else if (u_exercise == 2) {
        
        if (u_task == 0) {
            // ROTATE
            uv.x *= u_aspect;       // avoid deformation when rotating
            vec2 center = vec2(0.5*u_aspect, 0.5);
            vec2 horizontal = vec2(1.0, 0.0);       // to compute phi

            float rad = distance(uv, center);
            float phi = acos(dot(normalize(uv-center),horizontal));     // angle w.r.t. horizontal
            // since cos() outputs the same values for angles 0-pi and for pi-2pi
            // the following deals with angles for pi-2pi:
            phi = phi*step(center.y, uv.y) - phi*(1.0-step(center.y, uv.y));
            
            uv.x = center.x + rad*cos(-(u_pi/8.0)*u_time + phi);
            uv.y = center.y + rad*sin(-(u_pi/8.0)*u_time + phi);
            // note that ang. freq. < 0, since uvs "rotate" in the opposite way than the image
            uv.x /= u_aspect;       // restoring uv.x value, after computation, for proper sampling
            // borders of the image will be the same image:
            uv.x = mod(uv.x, 1.0);
            uv.y = mod(uv.y, 1.0);

        } else if (u_task == 1) {
            // PIXELIZATION
            uv.x *= u_aspect;       // stretching uv.x, for computations
            float param = cos((u_pi/8.0)*u_time);   // now param goes between -1 and 1
            param += 1.0; // between 0 and 2
            param /= 2.0; // between 0 and 1, so it can be used to compute the size of the pixels
            
            // apply more or less the same as in task=3, exercise=0
            float n_sq_min = 9.0;
            float sq_size = 1.0/n_sq_min;
            sq_size *= param;           // animate the size (increase and decrease with time)
            vec2 remain = vec2(mod(uv.x, sq_size), mod(uv.y, sq_size));
            uv -= remain;
            uv.x /= u_aspect;       // squeezing it back, to proper display
            
        }
        
        color4 = texture2D(u_texture, uv);
        
    }
    
    gl_FragColor = vec4(color4);
}
