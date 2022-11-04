;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; TEXCOORD                 0   xy          1     NONE   float   xy  
; COLOR                    0   xyzw        2     NONE   float   xyzw
; TEXCOORD                 1   xyzw        3     NONE   float   xyzw
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Target                0   xyzw        0   TARGET   float   xyzw
;
; shader debug name: 51a7201b5aa0886d50fa1ebcf33bc64b.pdb
; shader hash: 51a7201b5aa0886d50fa1ebcf33bc64b
;
; Pipeline Runtime Information: 
;
; Pixel Shader
; DepthOutput=0
; SampleFrequency=0
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; TEXCOORD                 0                 linear       
; COLOR                    0                 linear       
; TEXCOORD                 1                 linear       
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Target                0                              
;
; Buffer Definitions:
;
; cbuffer 
; {
;
;   [300 x i8] (type annotation not present)
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
;                                   cbuffer      NA          NA     CB0            cb0     1
;
;
; ViewId state:
;
; Number of inputs: 16, outputs: 4
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%dx.alignment.legacy.FrameBuffer = type { [4 x <4 x float>], [4 x <4 x float>], [4 x <4 x float>], [4 x <4 x float>], <4 x float>, <4 x float>, float, <2 x float> }

define void @PSMain() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %13 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %14 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %15 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %16 = alloca [4 x float]
  %17 = getelementptr inbounds [4 x float], [4 x float]* %16, i32 0, i32 0
  store float %2, float* %17
  %18 = getelementptr inbounds [4 x float], [4 x float]* %16, i32 0, i32 1
  store float %3, float* %18
  %19 = getelementptr inbounds [4 x float], [4 x float]* %16, i32 0, i32 2
  store float %4, float* %19
  %20 = getelementptr inbounds [4 x float], [4 x float]* %16, i32 0, i32 3
  store float %5, float* %20
  %21 = alloca [4 x float]
  %22 = getelementptr inbounds [4 x float], [4 x float]* %21, i32 0, i32 0
  store float %6, float* %22
  %23 = getelementptr inbounds [4 x float], [4 x float]* %21, i32 0, i32 1
  store float %7, float* %23
  %24 = getelementptr inbounds [4 x float], [4 x float]* %21, i32 0, i32 2
  store float %8, float* %24
  %25 = getelementptr inbounds [4 x float], [4 x float]* %21, i32 0, i32 3
  store float %9, float* %25
  %26 = alloca [2 x float]
  %27 = getelementptr inbounds [2 x float], [2 x float]* %26, i32 0, i32 0
  store float %10, float* %27
  %28 = getelementptr inbounds [2 x float], [2 x float]* %26, i32 0, i32 1
  store float %11, float* %28
  %29 = alloca [4 x float]
  %30 = getelementptr inbounds [4 x float], [4 x float]* %29, i32 0, i32 0
  store float %12, float* %30
  %31 = getelementptr inbounds [4 x float], [4 x float]* %29, i32 0, i32 1
  store float %13, float* %31
  %32 = getelementptr inbounds [4 x float], [4 x float]* %29, i32 0, i32 2
  store float %14, float* %32
  %33 = getelementptr inbounds [4 x float], [4 x float]* %29, i32 0, i32 3
  store float %15, float* %33
  %34 = alloca [4 x float]
  %35 = alloca float, align 4
  %36 = alloca [4 x float]
  %37 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 18)  ; CBufferLoadLegacy(handle,regIndex)
  %38 = extractvalue %dx.types.CBufRet.f32 %37, 0
  %39 = call float @dx.op.unary.f32(i32 13, float %38)  ; Sin(value)
  %40 = fadd fast float %39, 1.000000e+00
  %41 = fmul fast float %40, 5.000000e-01
  store float %41, float* %35, align 4
  %42 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 0
  store float 5.000000e-01, float* %42
  %43 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 1
  store float 5.000000e-01, float* %43
  %44 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 2
  store float 5.000000e-01, float* %44
  %45 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 3
  store float 1.000000e+00, float* %45
  %46 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 0
  %47 = load float, float* %46
  %48 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 1
  %49 = load float, float* %48
  %50 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 2
  %51 = load float, float* %50
  %52 = getelementptr inbounds [4 x float], [4 x float]* %36, i32 0, i32 3
  %53 = load float, float* %52
  %54 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 0
  store float %47, float* %54
  %55 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 1
  store float %49, float* %55
  %56 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 2
  store float %51, float* %56
  %57 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 3
  store float %53, float* %57
  %58 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 0
  %59 = load float, float* %58
  %60 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 1
  %61 = load float, float* %60
  %62 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 2
  %63 = load float, float* %62
  %64 = getelementptr inbounds [4 x float], [4 x float]* %34, i32 0, i32 3
  %65 = load float, float* %64
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %59)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %61)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %63)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %65)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #2

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxc 1.2"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 5}
!3 = !{!"ps", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %dx.alignment.legacy.FrameBuffer* undef, !"", i32 0, i32 0, i32 1, i32 300, null}
!7 = !{[18 x i32] [i32 16, i32 4, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0]}
!8 = !{void ()* @PSMain, !"PSMain", !9, !4, !21}
!9 = !{!10, !19, null}
!10 = !{!11, !14, !16, !17}
!11 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 15}
!14 = !{i32 1, !"TEXCOORD", i8 9, i8 0, !12, i8 2, i32 1, i8 2, i32 1, i8 0, !15}
!15 = !{i32 3, i32 3}
!16 = !{i32 2, !"COLOR", i8 9, i8 0, !12, i8 2, i32 1, i8 4, i32 2, i8 0, !13}
!17 = !{i32 3, !"TEXCOORD", i8 9, i8 0, !18, i8 2, i32 1, i8 4, i32 3, i8 0, !13}
!18 = !{i32 1}
!19 = !{!20}
!20 = !{i32 0, !"SV_Target", i8 9, i8 16, !12, i8 0, i32 1, i8 4, i32 0, i8 0, !13}
!21 = !{i32 0, i64 1}
