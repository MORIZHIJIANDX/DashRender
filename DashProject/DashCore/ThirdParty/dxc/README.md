# DirectX Shader Compiler Redistributable  Package

This package contains a copy of the DirectX Shader Compiler redistributable and its associated development headers. 

For help getting started, please see:

https://github.com/microsoft/DirectXShaderCompiler/wiki

## Licenses

The included licenses apply to the following files:

| License file | Applies to |
|---|---|
|LICENSE-MS.txt     |dxil.dll (if included in package)|
|LICENSE-MIT.txt    |d3d12shader.h|
|LICENSE-LLVM.txt   |all other files|

## Changelog

### Version 1.8.2403.1

DX Compiler Release for March 2024 - Patch 1

- Fix regression: [#6419](https://github.com/microsoft/DirectXShaderCompiler/issues/6419) crash when using literal arguments with `fmod`.

### Version 1.8.2403

DX Compiler release for March 2024

- Shader Model 6.8 is fully supported
  - Work Graphs allow node shaders with user-defined input and output payloads
  - New Barrier builtin functions with specific memory types and semantics
  - Expanded Comparison sampler intrinsics: SampleCmpBias, SampleCmpGrad, and CalculateLevelOfDetail
  - StartVertexLocation and StartInstanceLocation semantics
  - WaveSizeRange entry point attribute allows specifying a range of supported wave sizes
- Improved compile-time validation and runtime validation information
- Various stability improvements including numerous address sanitation fixes
- Several Diagnostic improvements
  - Many diagnostics are generated earlier and with more detailed information
  - Library profile diagnostic improvements
  - No longer infer library shader type when not specified
  - More helpful diagnostics for numthreads and other entry point attributes
  - Validation errors more accurately determine usage by the entry point
- Improve debug info generation
- Further improvements to Linux build quality


### Version 1.7.2308

DX Compiler release for August 2023

- HLSL 2021 is now enabled by default
- Various HLSL 2021 fixes have been made to
  - Operator overloading fixes
  - Templates fixes
  - Select() with samplers
  - Bitfields show in reflections
  - Bitfields can be used on enums
  - Allow function template default params
- Issues with loading and using Linux binaries have been resolved
- Support #pragma region/endregion
- Various stability and diagnostic improvements
- Dxcapi.h inline documentation is improved
- Linking of libraries created by different compilers is disallowed to prevent interface Issues
- Inout parameter correctness improved


The package includes dxc.exe, dxcompiler.dll, corresponding lib and headers, and dxil.dll for x64 and arm64 platforms on Windows.
The package also includes Linux version of the compiler with corresponding executable, libdxcompiler.so, corresponding headers, and libdxil.so for x64 platforms.

The new DirectX 12 Agility SDK (Microsoft.Direct3D.D3D12 nuget package) and a hardware driver with appropriate support
are required to run shader model 6.7 shaders. Please see https://aka.ms/directx12agility for details.

The SPIR-V backend of the compiler has been enabled in this release.

### Version 1.7.2212

DX Compiler release for December 2022.

- Includes full support of HLSL 2021 for SPIRV generation as well as many HLSL 2021 fixes and enhancements:
  - HLSL 2021's `and`, `or` and `select` intrinsics are now exposed in all language modes. This was done to ease porting codebases to HLSL2021, but may cause name conflicts in existing code.
  - Improved template utility with user-defined types
  - Many additional bug fixes
- Linux binaries are now included.
 This includes the compiler executable, the dynamic library, and the dxil signing library.
- New flags for inspecting compile times:
  - `-ftime-report` flag prints a high level summary of compile time broken down by major phase or pass in the compiler. The DXC
command line will print the output to stdout.
  - `-ftime-trace` flag prints a Chrome trace json file. The output can be routed to a specific file by providing a filename to
the arguent using the format `-ftime-trace=<filename>`. Chrome trace files can be opened in Chrome by loading the built-in tracing tool
at chrome://tracing. The trace file captures hierarchial timing data with additional context enabling a much more in-depth profiling
experience.
  - Both new options are supported via the DXC API using the `DXC_OUT_TIME_REPORT` and `DXC_OUT_TIME_TRACE` output kinds respectively.
- IDxcPdbUtils2 enables reading new PDB container part
- `-P` flag will now behave as it does with cl using the file specified by `-Fi` or a default
- Unbound multidimensional resource arrays are allowed
- Diagnostic improvements
- Reflection support on non-Windows platforms; minor updates adding RequiredFeatureFlags to library function reflection and thread group size for AS and MS.

The package includes dxc.exe, dxcompiler.dll, corresponding lib and headers, and dxil.dll for x64 and arm64 platforms on Windows.
For the first time the package also includes Linux version of the compiler with corresponding executable, libdxcompiler.so, corresponding headers, and libdxil.so for x64 platforms.

The new DirectX 12 Agility SDK (Microsoft.Direct3D.D3D12 nuget package) and a hardware driver with appropriate support
are required to run shader model 6.7 shaders. Please see https://aka.ms/directx12agility for details.

The SPIR-V backend of the compiler has been enabled in this release. Please note that Microsoft does not perform testing/verification of the SPIR-V backend.


### Version 1.7.2207

DX Compiler release for July 2022. Contains shader model 6.7 and many bug fixes and improvements, such as:

- Features: Shader Model 6.7 includes support for Raw Gather, Programmable Offsets, QuadAny/QuadAll, WaveOpsIncludeHelperLanes, and more!
- Platforms: ARM64 support
- HLSL 2021 : Enable “using” keyword
- Optimizations: Loop unrolling and dead code elimination improvements
- Developer tools: Improved disassembly output

The package includes dxc.exe, dxcompiler.dll, corresponding lib and headers, and dxil.dll for x64 and, for the first time, arm64 platforms!

The new DirectX 12 Agility SDK (Microsoft.Direct3D.D3D12 nuget package) and a hardware driver with appropriate support
are required to run shader model 6.7 shaders. Please see https://aka.ms/directx12agility for details.

The SPIR-V backend of the compiler has been enabled in this release. Please note that Microsoft does not perform testing/verification of the SPIR-V backend.
