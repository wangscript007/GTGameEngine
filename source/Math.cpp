// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Math.hpp>

namespace GT
{
    //////////////////////////////////
    // vec4

    const vec4 vec4::zero = vec4( 0.0f,  0.0f,  0.0f,  0.0f);
    const vec4 vec4::one  = vec4( 1.0f,  1.0f,  1.0f,  1.0f);

    const vec4 vec4::posx = vec4( 1.0f,  0.0f,  0.0f,  0.0f);
    const vec4 vec4::posy = vec4( 0.0f,  1.0f,  0.0f,  0.0f);
    const vec4 vec4::posz = vec4( 0.0f,  0.0f,  1.0f,  0.0f);
    const vec4 vec4::posw = vec4( 0.0f,  0.0f,  0.0f,  1.0f);

    const vec4 vec4::negx = vec4(-1.0f,  0.0f,  0.0f,  0.0f);
    const vec4 vec4::negy = vec4( 0.0f, -1.0f,  0.0f,  0.0f);
    const vec4 vec4::negz = vec4( 0.0f,  0.0f, -1.0f,  0.0f);
    const vec4 vec4::negw = vec4( 0.0f,  0.0f,  0.0f, -1.0f);


    //////////////////////////////////
    // quat

    const quat quat::identity = quat(0.0f, 0.0f, 0.0f, 1.0f);


    //////////////////////////////////
    // mat4

    const mat4 mat4::zero     = mat4(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 0.0f));
    const mat4 mat4::identity = mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
}
