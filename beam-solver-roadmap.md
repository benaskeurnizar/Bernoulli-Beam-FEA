# Beam FEA Solver: Roadmap

Goal: one deep, well-verified project for FEA / programming job applications, then a second project that builds on it.

Legend: `[ ]` to do, `[~]` in progress, `[x]` done

---

## Phase 1: Pre-push cleanup (do first)

A public repo that builds and runs on the first try matters more than any new feature.

- [ ] Remove hardcoded file and asset paths (use relative paths or command-line args)
- [ ] Fix the solver pivot bug
- [ ] Remove the node-count assumption (support arbitrary node counts)
- [ ] Repo cleanup (no build artifacts, no stray files, `.gitignore`)
- [ ] Write the README (what it does, screenshots, input format, how to build and run)
- [ ] Reproducible build (Makefile or CMake, documented dependencies: Vulkan SDK, GLFW)
- [ ] Push to the public repo

## Phase 2: Solver performance

- [ ] Sparse storage: replace the dense stiffness matrix with CSR
- [ ] Linear solver, pick one or both:
  - [ ] Hand-written conjugate gradient (with Jacobi or incomplete Cholesky preconditioner)
  - [ ] Sparse direct solve via a library (SuiteSparse/CHOLMOD or Eigen)
- [ ] OpenMP on element stiffness computation and assembly (handle write conflicts: atomics or coloring)
- [ ] OpenMP on the sparse matrix-vector product
- [ ] Benchmark table for the README: N elements vs. solve time, dense vs. sparse, 1 thread vs. 8 threads

## Phase 3: Verification

- [ ] Compare against analytical solutions (cantilever, simply supported beam)
- [ ] Compare against SAP2000 / midas Civil on the same model
- [ ] Add the comparison results to the README
- [ ] Add a few automated regression tests

## Phase 4: Extend the solver

- [ ] Add a truss element type (axial stiffness only) behind a common element interface
- [ ] Load a model from a simple text input format
- [ ] Optional: modal analysis (natural frequencies and mode shapes, Lanczos or subspace iteration)
- [ ] Optional: moving load / influence line analysis on a bridge model

## Phase 5: Next project

- [ ] 2D plane-stress solver (triangle or quad elements)
  - [ ] Isoparametric elements and Gauss integration
  - [ ] Stress recovery
  - [ ] Contour plots in the existing Vulkan viewer

---

## Notes / Decisions

-

## Log

| Date | What I did | Next |
|------|------------|------|
|      |            |      |
