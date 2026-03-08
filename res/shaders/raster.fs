uniform sampler2D u_texture;

varying vec2 v_uv;
varying vec3 v_world_normal;

void main()
{

    vec2 uv = v_uv;
    vec4 color = texture2D(u_texture, uv);
    //vec3 color = normalize(v_world_normal);

	//gl_FragColor = vec4( color, 1.0);
    gl_FragColor = color;
}
