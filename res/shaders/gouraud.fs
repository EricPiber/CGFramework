uniform sampler2D u_texture;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec3 v_Ip;

void main()
{

    vec2 uv = v_uv;
    vec4 color = texture2D(u_texture, uv);

    gl_FragColor = vec4(v_Ip, 1.0);
}
