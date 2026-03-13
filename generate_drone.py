import math

def generate_drone_obj(filename):
    vertices = []
    faces = []
    uvs = []
    
    # helper for box
    def add_box(center, size, uv_col):
        cx, cy, cz = center
        sx, sy, sz = size[0]/2, size[1]/2, size[2]/2
        
        # 8 vertices
        pts = [
            (cx-sx, cy-sy, cz-sz), (cx+sx, cy-sy, cz-sz),
            (cx-sx, cy+sy, cz-sz), (cx+sx, cy+sy, cz-sz),
            (cx-sx, cy-sy, cz+sz), (cx+sx, cy-sy, cz+sz),
            (cx-sx, cy+sy, cz+sz), (cx+sx, cy+sy, cz+sz),
        ]
        start_idx = len(vertices)
        for p in pts:
            vertices.append(p)
            uvs.append(uv_col)
            
        # 6 faces (two triangles each)
        f_indices = [
            (0,2,1), (1,2,3), # bottom
            (4,5,6), (5,7,6), # top
            (0,1,4), (1,5,4), # front
            (2,6,3), (3,6,7), # back
            (0,4,2), (2,4,6), # left
            (1,3,5), (3,7,5)  # right
        ]
        
        for f in f_indices:
            faces.append((f[0]+start_idx+1, f[1]+start_idx+1, f[2]+start_idx+1))

    # Center body
    add_box((0, 0, 0), (0.4, 0.4, 0.2), (0.5, 0.5))
    
    # 4 arms
    add_box((0.3, 0.3, 0), (0.5, 0.05, 0.05), (0.2, 0.2))
    add_box((-0.3, 0.3, 0), (0.5, 0.05, 0.05), (0.2, 0.2))
    add_box((0.3, -0.3, 0), (0.5, 0.05, 0.05), (0.2, 0.2))
    add_box((-0.3, -0.3, 0), (0.5, 0.05, 0.05), (0.2, 0.2))

    # 4 propellers
    def add_propeller(cx, cy, cz):
        add_box((cx, cy, cz), (0.6, 0.04, 0.02), (0.8, 0.1))
        add_box((cx, cy, cz), (0.04, 0.6, 0.02), (0.8, 0.1))

    add_propeller(0.5, 0.5, 0.05)
    add_propeller(-0.5, 0.5, 0.05)
    add_propeller(0.5, -0.5, 0.05)
    add_propeller(-0.5, -0.5, 0.05)

    with open(filename, 'w') as f:
        f.write("o Drone\n")
        for v in vertices:
            f.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
        for u in uvs:
            f.write(f"vt {u[0]:.4f} {u[1]:.4f}\n")
        for face in faces:
            # We assign the same uv index as the vertex index
            f.write(f"f {face[0]}/{face[0]} {face[1]}/{face[1]} {face[2]}/{face[2]}\n")

generate_drone_obj('Data/Models/drone.obj')
