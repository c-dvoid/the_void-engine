
#pragma once

struct AABB {
    float x, y;
    float w, h;
};

bool intersect(const AABB& a, const AABB& b);