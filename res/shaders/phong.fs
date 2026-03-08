// camera
uniform vec3 u_cam_eye;

// material
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;
uniform sampler2D u_texture;
uniform sampler2D u_normal_texture;

// light
uniform vec3 u_Ia;
uniform vec3 u_m;
uniform vec3 u_I;

// interactivity
uniform int u_ct;
uniform int u_st;
uniform int u_nt;

uniform mat4 u_model;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

void main()
{
    vec2 uv = v_uv;
    vec4 color = texture2D(u_texture, uv);
    
    // parameters
    vec3 Ip;
    vec3 N;
    vec3 P = v_world_position;
    vec3 V = normalize(u_cam_eye - P);
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    
    if (u_ct == 0) {
        Ka = u_ka;
        Kd = u_kd;
    } else {
        Ka = color.xyz;
        Kd = color.xyz;
    }
    
    if (u_st == 0) {
        Ks = u_ks;
    } else {
        Ks = vec3(color.w);
    }
    
    if (u_nt == 0) {
        N = normalize(v_world_normal);
    } else {
        vec4 normal = texture2D(u_normal_texture, uv);  // 0 to 1
        normal *= 2.0;    // 0 to 2
        normal -= 1.0;    // -1 to 1
        
        // local to world coord
        N = (u_model * normal).xyz;
        
        // smoothing
        N = normalize(mix(N, v_world_normal, 0.25));
    }
    
    // ambient
    Ip = Ka * u_Ia;
    
    // parameters
    vec3 L = normalize(u_m - P);
    vec3 R = reflect((-1.0)*L, N);
    float d_sq = 1.0;   // WITH THIS LOOKS TERRIBLE (since light should be nearer, and then not good result) --> pow(distance(P, u_m), 2.0);
    
    // diffuse
    vec3 diffuse = clamp(dot(L, N), 0.0, 1.0)*Kd;
    
    // specular
    vec3 specular = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess) * Ks;
    
    vec3 sum = diffuse + specular;
    sum *= (u_I/d_sq);
    Ip += sum;
    

    //vec3 color = normalize(v_world_normal);

	//gl_FragColor = vec4( color, 1.0);
    gl_FragColor = vec4(Ip, 1.0);
}
