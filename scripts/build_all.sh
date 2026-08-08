#!/usr/bin/env bash
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ROOT="${ROOT_DIR}/build"
BUILD_TYPE="${BUILD_TYPE:-Release}"
GENERATOR="${GENERATOR:-}"

mapfile -t MODULE_DIRS < <(find "${ROOT_DIR}/remote_cod" -mindepth 1 -maxdepth 1 -type d | sort)

if [ ${#MODULE_DIRS[@]} -eq 0 ]; then
  echo "No modules found under remote_cod/."
  exit 1
fi

mkdir -p "${BUILD_ROOT}"

ok_modules=()
failed_modules=()

for module_path in "${MODULE_DIRS[@]}"; do
  module_name="$(basename "${module_path}")"
  cmake_file="${module_path}/CMakeLists.txt"

  if [ ! -f "${cmake_file}" ]; then
    echo "[SKIP] ${module_name}: no CMakeLists.txt"
    continue
  fi

  build_dir="${BUILD_ROOT}/${module_name}"
  echo "\n=== Building ${module_name} ==="

  cmake_args=(
    -S "${module_path}"
    -B "${build_dir}"
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
  )

  if [ -n "${GENERATOR}" ]; then
    cmake_args+=( -G "${GENERATOR}" )
  fi

  if ! cmake "${cmake_args[@]}"; then
    echo "[FAIL] configure: ${module_name}"
    failed_modules+=("${module_name}")
    continue
  fi

  if ! cmake --build "${build_dir}" -j"$(nproc)"; then
    echo "[FAIL] build: ${module_name}"
    failed_modules+=("${module_name}")
    continue
  fi

  echo "[OK] ${module_name}"
  ok_modules+=("${module_name}")
done

echo "\n=== Build Summary ==="
echo "Success (${#ok_modules[@]}): ${ok_modules[*]:-none}"
echo "Failed  (${#failed_modules[@]}): ${failed_modules[*]:-none}"

if [ ${#failed_modules[@]} -gt 0 ]; then
  exit 2
fi
