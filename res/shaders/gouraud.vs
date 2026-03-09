// Global variables from the CPU
// entity
uniform mat4 u_model;

//camera
uniform mat4 u_viewprojection;
uniform vec3 u_cam_eye;

// material
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;

// light
uniform vec3 u_Ia;
uniform vec3 u_m;
uniform vec3 u_I;

// Variables to pass to the fragment shader
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec3 v_Ip;

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// Convert local position to world space
	vec3 world_position = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;
    
    // Convert local normal to world space
    vec3 world_normal = (u_model * vec4( gl_Normal.xyz, 0.0)).xyz;

    // Pass them to the fragment shader interpolated
    v_world_position = world_position;
    v_world_normal = world_normal;

	// Project the vertex using the model view projection matrix
	gl_Position = u_viewprojection * vec4(world_position, 1.0); //output of the vertex shader
    
    // computing v_Ip
    vec3 Ip;
    vec3 N = normalize(world_normal);
    vec3 P = world_position;
    vec3 V = normalize(u_cam_eye - P);
    
    // ambient
    Ip = u_ka * u_Ia;
    
    // parameters
    vec3 L = normalize(u_m - P);
    vec3 R = reflect(-L, N);
    float d = 1.0;   // WITH THIS LOOKS TERRIBLE (since light should be nearer, and then not good result) --> pow(distance(P, u_m), 2.0);
    
    // diffuse
    vec3 diffuse = clamp(dot(L, N), 0.0, 1.0)*u_kd;
    
    // specular
    vec3 specular = pow(clamp(dot(R, V), 0.0, 1.0), u_shininess) * u_ks;
    
    vec3 sum = diffuse + specular;
    sum *= (u_I/d);
    Ip += sum;
    
    v_Ip = Ip;
}
