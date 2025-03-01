#pragma once

struct vec2_t {

    constexpr vec2_t() : x(0), y(0) {}
    constexpr vec2_t (float x, float y) : x(x), y(y) {};

    union { float x, width, lower; };
    union { float y, height, upper; };

};

struct vec3_t {

    constexpr vec3_t() : x(0), y(0), z(0) {};
    constexpr vec3_t (float x, float y, float z) : x(x), y(y), z(z) {};

    union { float x, r, red; };
    union { float y, g, green; };
    union { float z, b, blue; };

};

struct vec4_t {

    constexpr vec4_t() : x(0), y(0), z(0), w(0) {};
    constexpr vec4_t (float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
    constexpr vec4_t (vec3_t vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {};

    union { float x, r, red; };
    union { float y, g, green; };
    union { float z, b, blue; };
    union { float w, a, alpha; };

};