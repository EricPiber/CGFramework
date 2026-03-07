uniform sampler2D u_texture;
varying vec2 v_uv;

void main()
{

    vec2 uv = v_uv;
    vec4 color = texture2D(u_texture, uv);

	gl_FragColor = color;
}
