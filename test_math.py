import numpy as np

def computeLocalMatrix(pos, rot, scale):
    qx, qy, qz, qw = rot
    sx, sy, sz = scale
    x2, y2, z2 = qx+qx, qy+qy, qz+qz
    xx, yy, zz = qx*x2, qy*y2, qz*z2
    xy, xz, yz = qx*y2, qx*z2, qy*z2
    wx, wy, wz = qw*x2, qw*y2, qw*z2
    return [
        (1.0 - (yy + zz))*sx, (xy + wz)*sx,       (xz - wy)*sx,       0.0,
        (xy - wz)*sy,       (1.0 - (xx + zz))*sy, (yz + wx)*sy,       0.0,
        (xz + wy)*sz,       (yz - wx)*sz,       (1.0 - (xx + yy))*sz, 0.0,
        pos[0],             pos[1],             pos[2],             1.0
    ]

# The user's RenderDoc output earlier for _child10:
# { 1.29326, -0.11006, -0.30166, -0.30163 }
# { 7.20256E-08, -2.3871, 0.14945, 0.14944 }
# { -0.41427, -0.34358, -0.94173, -0.94164 }
# { 26.71512, 39.82096, 9.51747, 9.61652 }  <-- Translation is here! X, Y, Z, W? No, 9.61652 is W? W should be 1.0.

# Notice the last row is { 26.71512, 39.82096, 9.51747, 9.61652 }
# If this is MVP matrix (Model * View * Projection), the last row contains the W component of the projection, which is NOT 1.0!
# Wait. _child10 is a node in the hierarchy? Or is it a matrix?
# RenderDoc labeled it "64 float4x4 (row_major)". It's the constants.mvp!
# Yes, perspective projection puts Z into W, so W is not 1.0.
# So the translation and everything is mixed with the projection matrix. Let's look at the view matrix.

# What if the user is right and `mul(constants.mvp, pos)` or `Parent * Local` or `Local * Parent` had bugs?
# Let's think about Assimp `node->mTransformation.Decompose`.
# Assimp row major:
# [ R R R Tx ]
# [ R R R Ty ]
# [ R R R Tz ]
# [ 0 0 0 1  ]
# When `Decompose` is called, it gives `position = (Tx, Ty, Tz)`. This is correct.
# `rotation` is a quaternion. `scale` is scale.
# Then `computeLocalMatrix` builds a Col-Major matrix:
# [ R R R 0 ]
# [ R R R 0 ]
# [ R R R 0 ]
# [ Tx Ty Tz 1 ]
# Which perfectly matches OpenGL/Vulkan memory layout for column-major!
# Therefore `computeLocalMatrix` and `Assimp::Decompose` are correct!
# The bug must be in `multiplyMat4` or the order of `Parent * Local`.

def check_multiplyMat4(a, b):
    # Test if `multiplyMat4(a, b)` does A * B or B * A in standard math notation?
    # standard math: (A * B)_ij = sum_k A_ik * B_kj
    # In 1D array column-major: M[col*4 + row] = M_row,col
    # The code does: r[col*4 + row] = sum_k a[k*4 + row] * b[col*4 + k]
    # This exactly matches sum_k A_row,k * B_k,col = (A * B)_row,col!
    # So `multiplyMat4(A, B)` calculates A * B in column-major!
    pass

# IF multiplyMat4 calculates A * B, and we use Column-Vector convention (v' = M * v):
# To apply Local transform, then Parent transform, the math is:
# v_world = Parent * Local * v_local
# Therefore, WorldMatrix = Parent * Local
# Therefore, `multiplyMat4(Parent, Local)` is CORRECT!

# Then why did the user say "顺序有问题" (order is wrong)?
# AND why did they say `mul` should be `*` in shader?
# Shader is Row-Major. memory is Column-Major.
# So Shader Matrix M_shader = M_C++^T
# Standard math: v' = M_C++ * v
# In shader, we want to compute M_C++ * v.
# But we only have M_shader = M_C++^T.
# Notice that (M_C++ * v)^T = v^T * M_C++^T = v^T * M_shader !
# In HLSL, `v_row * M_shader` calculates exactly this!
# In HLSL, `v * M` (if v is float4) treats v as a row vector `v_row`.
# So `pos * mvp` calculates `pos_row * M_shader`, which is EXACTLY correct!
# SO `pos * mvp` + `Parent * Local` without transpose in C++ is MAThemtically PERFECT.

# BUT the user said it is STILL BROKEN ("还是破碎的").
# What is wrong? Maybe `viewProj` calculation?
