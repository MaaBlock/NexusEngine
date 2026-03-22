#!/bin/bash
set -e

# === NexusEngine Release Packaging Script ===
# Packages NexusApp with dynamic libraries and Data for deployment on Kylin (x86_64, glibc 2.31)

VERSION="0.1.0"
BUILD_DIR="build-linux-release"
PKG_NAME="NexusEngine-v${VERSION}-linux-x64"
PKG_DIR="${BUILD_DIR}/${PKG_NAME}"
VCPKG_LIB_DIR="${BUILD_DIR}/vcpkg_installed/x64-linux-static/lib"

# Windows G: drive output (WSL path)
WIN_OUTPUT="/mnt/g"

echo "=== NexusEngine Release Packager ==="
echo "Version: ${VERSION}"
echo ""

# Check build exists
if [ ! -f "${BUILD_DIR}/NexusApp" ]; then
    echo "ERROR: ${BUILD_DIR}/NexusApp not found. Run 'cmake --build --preset build-linux-release' first."
    exit 1
fi

# Clean previous package
rm -rf "${PKG_DIR}"
mkdir -p "${PKG_DIR}/lib"

# 1. Copy main binary
echo "[1/5] Copying NexusApp binary..."
cp "${BUILD_DIR}/NexusApp" "${PKG_DIR}/"

# 2. Copy dynamic libraries from vcpkg
echo "[2/5] Copying dynamic libraries..."
cp "${VCPKG_LIB_DIR}/libvulkan.so.1" "${PKG_DIR}/lib/"
# Copy mujoco (with symlink-safe approach)
cp -L "${VCPKG_LIB_DIR}/libmujoco.so.3.2.7" "${PKG_DIR}/lib/"
ln -sf libmujoco.so.3.2.7 "${PKG_DIR}/lib/libmujoco.so"

# 3. Copy libstdc++ from GCC 13 (in case Kylin has older GCC)
echo "[3/5] Copying libstdc++ (GCC 13)..."
GCC13_LIBSTDCXX=$(gcc-13 -print-file-name=libstdc++.so.6 2>/dev/null || echo "")
if [ -n "$GCC13_LIBSTDCXX" ] && [ -f "$GCC13_LIBSTDCXX" ]; then
    cp -L "$GCC13_LIBSTDCXX" "${PKG_DIR}/lib/"
else
    echo "  WARNING: Could not find GCC 13 libstdc++.so.6, skipping (Kylin must have compatible version)"
fi

# 4. Copy Data directory
echo "[4/5] Copying Data directory..."
cp -r "${BUILD_DIR}/Data" "${PKG_DIR}/Data"

# 5. Create launch script
echo "[5/5] Creating launch script..."
cat > "${PKG_DIR}/run.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
exec "${SCRIPT_DIR}/NexusApp" "$@"
EOF
chmod +x "${PKG_DIR}/run.sh"
chmod +x "${PKG_DIR}/NexusApp"

# Package as tar.gz
echo ""
echo "=== Creating archive ==="
ARCHIVE="${PKG_NAME}.tar.gz"
(cd "${BUILD_DIR}" && tar -czvf "${ARCHIVE}" "${PKG_NAME}/")

echo ""
echo "=== Package created: ${BUILD_DIR}/${ARCHIVE} ==="

# Copy to Windows Desktop if available
if [ -n "$WIN_OUTPUT" ] && [ -d "$WIN_OUTPUT" ]; then
    cp "${BUILD_DIR}/${ARCHIVE}" "${WIN_OUTPUT}/"
    echo "=== Copied to Windows Desktop: ${WIN_OUTPUT}/${ARCHIVE} ==="
else
    echo "NOTE: Could not detect Windows Desktop path."
    echo "  You can manually copy: cp ${BUILD_DIR}/${ARCHIVE} /mnt/c/Users/<YourName>/Desktop/"
fi

echo ""
echo "=== Done! ==="
echo "On Kylin system, extract and run:"
echo "  tar -xzvf ${ARCHIVE}"
echo "  cd ${PKG_NAME}"
echo "  ./run.sh"
