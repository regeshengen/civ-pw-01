#version 410 core
layout(location = 0) in vec3 aPos;

void main()
{
    // pass-through: position of the single input point (not used directly,
    // geometry shader will build the geometry). We still set gl_Position
    // to avoid warnings when rendering without GS.
    gl_Position = vec4(aPos, 1.0);
}
