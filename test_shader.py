import ctypes
import os

source = b"""
float4x4 mvp;
float4 VSMain(float3 pos : POSITION) : SV_POSITION {
    return mvp * float4(pos, 1.0);
}
"""

# We'll just use the shaderc dll directly if possible, or simpler: just write the file and use spirv-cross? No, we don't need to.
