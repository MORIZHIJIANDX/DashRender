#include "PCH.h"

#include "ApplicationDX12.h"
#include "d3dx12.h"
#include "DX12Helper.h"

#include "../Utility/Keyboard.h"
#include "../Utility/Mouse.h"


//生成mipmap
//导入DDS
//导入Cube map
//create from memory
//grab screen
//Resize


//Vertex Buffer和Index Buffer封装

//sphere, box等内置模型支持
//Assimp或FBXSDK导入模型

//constant buffer 封装
//材质
//多线程
//ECS
//反射
//IMGUI

namespace Dash
{
    struct Vertex
    {
        FVector3f Pos;
        FVector3f Normal;
        FVector4f Tangent;
        FVector4f Color;
        FVector2f UV;
    };

    struct MeshData
    {
        std::vector<Vertex> VertexData;    // 顶点数组
        std::vector<UINT> IndexData;    // 索引数组

        MeshData()
        {
            // 需检验索引类型合法性
            //static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "The size of IndexType must be 2 bytes or 4 bytes!");
            //static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
        }
    };

    static MeshData CreateBoxMesh(Scalar width, Scalar height, Scalar depth, FVector4f color)
    {
        const Scalar halfWidth = width * 0.5f;
        const Scalar halfHeight = height * 0.5f;
        const Scalar halfDepth = depth * 0.5f;

        MeshData data;
        data.VertexData.resize(24);

        // 右面(+X面)
        data.VertexData[0].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.VertexData[1].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.VertexData[2].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[3].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        // 左面(-X面)
        data.VertexData[4].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.VertexData[5].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[6].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[7].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 顶面(+Y面)
        data.VertexData[8].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[9].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[10].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[11].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        // 底面(-Y面)
        data.VertexData[12].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.VertexData[13].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.VertexData[14].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.VertexData[15].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 背面(+Z面)
        data.VertexData[16].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.VertexData[17].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[18].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[19].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        // 正面(-Z面)
        data.VertexData[20].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        data.VertexData[21].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[22].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.VertexData[23].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);

        for (UINT i = 0; i < 4; ++i)
        {
            // 右面(+X面)
            data.VertexData[i].Normal = FVector3f(1.0f, 0.0f, 0.0f);
            data.VertexData[i].Tangent = FVector4f(0.0f, 0.0f, 1.0f, 1.0f);
            data.VertexData[i].Color = color;
            // 左面(-X面)
            data.VertexData[i + 4].Normal = FVector3f(-1.0f, 0.0f, 0.0f);
            data.VertexData[i + 4].Tangent = FVector4f(0.0f, 0.0f, -1.0f, 1.0f);
            data.VertexData[i + 4].Color = color;
            // 顶面(+Y面)
            data.VertexData[i + 8].Normal = FVector3f(0.0f, 1.0f, 0.0f);
            data.VertexData[i + 8].Tangent = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 8].Color = color;
            // 底面(-Y面)
            data.VertexData[i + 12].Normal = FVector3f(0.0f, -1.0f, 0.0f);
            data.VertexData[i + 12].Tangent = FVector4f(-1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 12].Color = color;
            // 背面(+Z面)
            data.VertexData[i + 16].Normal = FVector3f(0.0f, 0.0f, 1.0f);
            data.VertexData[i + 16].Tangent = FVector4f(-1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 16].Color = color;
            // 正面(-Z面)
            data.VertexData[i + 20].Normal = FVector3f(0.0f, 0.0f, -1.0f);
            data.VertexData[i + 20].Tangent = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 20].Color = color;
        }

        for (UINT i = 0; i < 6; ++i)
        {
            data.VertexData[i * 4].UV = FVector2f(0.0f, 1.0f);
            data.VertexData[i * 4 + 1].UV = FVector2f(0.0f, 0.0f);
            data.VertexData[i * 4 + 2].UV = FVector2f(1.0f, 0.0f);
            data.VertexData[i * 4 + 3].UV = FVector2f(1.0f, 1.0f);
        }

        //for (UINT i = 0; i < 24; ++i)
        //{
        //    Internal::InsertVertexElement(meshData.vertexVec[i], data.VertexData[i]);
        //}

        data.IndexData = {
            0, 1, 2, 2, 3, 0,        // 右面(+X面)
            4, 5, 6, 6, 7, 4,        // 左面(-X面)
            8, 9, 10, 10, 11, 8,    // 顶面(+Y面)
            12, 13, 14, 14, 15, 12,    // 底面(-Y面)
            16, 17, 18, 18, 19, 16, // 背面(+Z面)
            20, 21, 22, 22, 23, 20    // 正面(-Z面)
        };

        return data;
    }

    struct __declspec(align(256)) FFrameConstantBuffer
    {
        //FMatrix4x4 WorldMatrix;
        FMatrix4x4 ViewMatrix;
        FMatrix4x4 ProjectionMatrix;
        float Time;
        FVector2f Speed;
    };

    static FFrameConstantBuffer* sFrameConstantBuffer = nullptr;


	FApplicationDX12::FApplicationDX12(size_t windowWidth, size_t windowHeight)
		: FApplication(windowWidth, windowHeight)
        , mWindowedMode(true)
        , mWindowVisible(true)
	{
        {
            mViewport.Width = (FLOAT)mWindow.GetWindowWidth();
            mViewport.Height = (FLOAT)mWindow.GetWindowHeight();
            mViewport.TopLeftX = 0.0f;
            mViewport.TopLeftY = 0.0f;
            mViewport.MinDepth = 0.0f;
            mViewport.MaxDepth = 1.0f;

            mScissorRect.left = 0;
            mScissorRect.right = (UINT)mWindow.GetWindowWidth();
            mScissorRect.top = 0;
            mScissorRect.bottom = (UINT)mWindow.GetWindowHeight();
        }

        {
            float aspect = windowWidth / (float)windowHeight;
            float projectionWindowHeight = 1.0f;
            float projectionWindowWidth = aspect;

            //mCamera.SetCameraParams(projectionWindowWidth, projectionWindowHeight, 1.0f, 100.0f);

            mFov = 45.0f;

            mCamera.SetCameraParams(aspect, mFov, 0.1f, 100.0f);
            mCamera.SetPosition(FVector3f{0.0f, 0.0f, -2.0f});

            mObjectTransform = FTransform{ FIdentity{} };
        }

        mMouseWheelUpDelegate = FMouseWheelEventDelegate::Create<FApplicationDX12, &FApplicationDX12::OnMouseWhellUp>(this);
        mMouseWheelDownDelegate = FMouseWheelEventDelegate::Create<FApplicationDX12, &FApplicationDX12::OnMouseWhellDown>(this);
        mMouseMotionDelegate = FMouseMotionEventDelegate::Create<FApplicationDX12, &FApplicationDX12::OnMouseMove>(this);

        FMouse::Get().MouseWheelUp += mMouseWheelUpDelegate;
        FMouse::Get().MouseWheelDown += mMouseWheelDownDelegate;
        FMouse::Get().MouseMoved += mMouseMotionDelegate;

        mWindowResizeDelegate = FResizeEventDelegate::Create<FApplicationDX12, &FApplicationDX12::OnWindowResize>(this);

        mWindow.WindowResize += mWindowResizeDelegate;

        LoadPipeline();
        LoadAssets();
	}

	FApplicationDX12::~FApplicationDX12()
	{
        FTextureHelper::Get()->Destroy();

        WaitForPreviousFrame();

        mConstantBuffer->Unmap(0, nullptr);

        CloseHandle(mFenceEvent);

        mWindowVisible = false;

        FMouse::Get().MouseWheelUp -= mMouseWheelUpDelegate;
        FMouse::Get().MouseWheelDown -= mMouseWheelDownDelegate;

        FMouse::Get().MouseMoved -= mMouseMotionDelegate;

        mWindow.WindowResize -= mWindowResizeDelegate;
        



        mRTVDescriptorHeap.Reset();

        for (UINT i = 0; i < BackBufferFrameCount; i++)
        {
            mBackBuffers[i].Reset();
        }

        //mBackBuffers[BackBufferFrameCount];
        mRootSignature.Reset();
        mPipelineState.Reset();

        mVertexBuffer.Reset();

        mIndexBuffer.Reset();


        mTextureResource.Reset();
        mSRVCBVDescriptorHeap.Reset();

        mCommandAllocator.Reset();
        mCommandList.Reset();
        mFence.Reset();

        mTextureHeap.Reset();

        mUploadHeap.Reset();

        mRTDSHeap.Reset();

        mConstantBuffer.Reset();

        mDepthStencilBuffer.Reset();
        mDepthStencilDescriptorHeap.Reset();

        mSamplerDescriptorHeap.Reset();



        mD3DDevice.Reset();
        mD3DCommandQueue.Reset();

        mSwapChain.Reset();
        mDXGIFatory.Reset();
	}

	void FApplicationDX12::OnRender(const FRenderEventArgs& e)
	{
        if (mWindowVisible)
        {
            PopulateCommandList(e);

            ID3D12CommandList* commands[] = { mCommandList.Get() };
            mD3DCommandQueue->ExecuteCommandLists(_countof(commands), commands);

            //HR(mSwapChain->Present(1, 0));

            HR(mSwapChain->Present(1, 0));

            WaitForPreviousFrame();
        }
	}

    void FApplicationDX12::OnMouseWhellUp(FMouseWheelEventArgs&)
    {
        mCamera.ZoomIn();
    }

    void FApplicationDX12::OnMouseWhellDown(FMouseWheelEventArgs&)
    {
        mCamera.ZoomOut();
    }

    void FApplicationDX12::OnMouseMove(FMouseMotionEventArgs& args)
    {
        if ((args.mRelX != 0 || args.mRelY != 0) && args.mRightButton)
        {
            if (args.mRelX != 0)
            {
                mCamera.AddYAxisRotation(args.mRelX * 0.25f);
            }

            if (args.mRelY != 0)
            {
                mCamera.AddXAxisRotation(args.mRelY * 0.25f);
            }
        }
    }

    void FApplicationDX12::OnWindowResize(FResizeEventArgs& args)
    {
        if (!args.mMinimized)
        {
            WaitForGpu();

            for (UINT i = 0; i < FApplicationDX12::BackBufferFrameCount; i++)
            {
                mBackBuffers[i].Reset();
            }

            DXGI_SWAP_CHAIN_DESC swapchainDesc{};
            HR(mSwapChain->GetDesc(&swapchainDesc));
            HR(mSwapChain->ResizeBuffers(FApplicationDX12::BackBufferFrameCount, args.mWidth, args.mHeight, swapchainDesc.BufferDesc.Format, swapchainDesc.Flags));

            BOOL fullscreenState;
            HR(mSwapChain->GetFullscreenState(&fullscreenState, nullptr));
            mWindowedMode = !fullscreenState;

            {
                float aspect = args.mWidth / (float)args.mHeight;
                float projectionWindowHeight = 1.0f;
                float projectionWindowWidth = aspect;

                //mCamera.SetCameraParams(projectionWindowWidth, projectionWindowHeight, 1.0f, 100.0f);
                mCamera.SetCameraParams(aspect, mFov, 0.1f, 100.0f);
            }

            {
                mViewport.Width = (FLOAT)args.mWidth;
                mViewport.Height = (FLOAT)args.mHeight;
                mViewport.TopLeftX = 0.0f;
                mViewport.TopLeftY = 0.0f;
                mViewport.MinDepth = 0.0f;
                mViewport.MaxDepth = 1.0f;

                mScissorRect.left = 0;
                mScissorRect.right = (UINT)args.mWidth;
                mScissorRect.top = 0;
                mScissorRect.bottom = (UINT)args.mHeight;
            }

            {
                //Create Rendertarget view
                CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
                for (UINT i = 0; i < FApplicationDX12::BackBufferFrameCount; i++)
                {
                    HR(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffers[i])));
                    mD3DDevice->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, descriptorHandle);
                    descriptorHandle.Offset(1, mRTVDescriptorSize);
                }

                mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
            }

            {
                D3D12_RESOURCE_DESC depthstencilDesc{};
                depthstencilDesc = mDepthStencilBuffer->GetDesc();
                mDepthStencilBuffer.Reset();

                depthstencilDesc.Width = args.mWidth;
                depthstencilDesc.Height = args.mHeight;

                HR(mD3DDevice->CreatePlacedResource(
                    mRTDSHeap.Get(),
                    mRTDSHeapOffset,
                    &depthstencilDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &mDepthStencilClearValue,
                    IID_PPV_ARGS(&mDepthStencilBuffer)
                ));

                mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            }
        }

        mWindowVisible = !args.mMinimized;
    }

	void FApplicationDX12::OnUpdate(const FUpdateEventArgs& e)
	{
        if (FKeyboard::Get().GetKeyState(EKeyCode::U).RisingEdge)
        {
            mCurrentSamplerIndex = (mCurrentSamplerIndex + 1) % 5;
        }

        float moveSpeed = 0.1f;

        if (FKeyboard::Get().IsKeyPressed(EKeyCode::LShiftKey))
        {
            moveSpeed = 0.2f;
        }

        if (FKeyboard::Get().IsKeyPressed(EKeyCode::W))
        {
            //mCamera.TranslateUp(moveSpeed);
            mCamera.TranslateForward(moveSpeed);
        }

        if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
        {
            //mCamera.TranslateDown(moveSpeed);
            mCamera.TranslateBack(moveSpeed);
        }

        if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
        {
            mCamera.TranslateLeft(moveSpeed);
        }

        if (FKeyboard::Get().IsKeyPressed(EKeyCode::D))
        {
            mCamera.TranslateRight(moveSpeed);
        }
	}

	void FApplicationDX12::PopulateCommandList(const FRenderEventArgs& e)
	{
        {
            if (sFrameConstantBuffer)
            {
                sFrameConstantBuffer->Time = (float)e.mTotalTime;
                sFrameConstantBuffer->ViewMatrix = mCamera.GetViewMatrix();
                sFrameConstantBuffer->ProjectionMatrix = mCamera.GetProjectionMatrix();
            }
        }


        HR(mCommandAllocator->Reset());
        HR(mCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
        mCommandList->SetPipelineState(mPipelineState.Get());

        ID3D12DescriptorHeap* heaps[] = { mSRVCBVDescriptorHeap.Get(), mSamplerDescriptorHeap.Get() };
        mCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

        mCommandList->SetGraphicsRootDescriptorTable(0, mSRVCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle{ mSamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 
            mCurrentSamplerIndex, mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) };

        mCommandList->SetGraphicsRootDescriptorTable(1, samplerGPUHandle);

        CD3DX12_GPU_DESCRIPTOR_HANDLE constantBufferGPUHandle{ mSRVCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
        1, mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };

        mCommandList->SetGraphicsRootDescriptorTable(2, constantBufferGPUHandle);

        mCommandList->RSSetViewports(1, &mViewport);
        mCommandList->RSSetScissorRects(1, &mScissorRect);

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBuffers[mBackBufferIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtDescriptorHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), mBackBufferIndex, mRTVDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsDescriptorHandle(mDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        mCommandList->OMSetRenderTargets(1, &rtDescriptorHandle, FALSE, &dsDescriptorHandle);

       // Dash::FVector4f clearColor{ Dash::FMath::Sin((float)e.mTotalTime) * 0.5f + 0.5f, Dash::FMath::Cos((float)e.mTotalTime + 0.5f) * 0.5f + 0.5f, 0.5f, 1.0f };
        Dash::FVector4f clearColor{ 0.0f, 0.2f, 0.4f, 1.0f };

        mCommandList->ClearRenderTargetView(rtDescriptorHandle, clearColor, 0, nullptr);
        mCommandList->ClearDepthStencilView(dsDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, mDepthStencilClearValue.DepthStencil.Depth, 
            mDepthStencilClearValue.DepthStencil.Stencil, 0, nullptr);

        //mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
        mCommandList->IASetIndexBuffer(&mIndexBufferView);

        //mCommandList->DrawInstanced(4, 1, 0, 0);

        mCommandList->DrawIndexedInstanced(mIndexCount, 1, 0, 0, 0);

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBuffers[mBackBufferIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT));

        HR(mCommandList->Close());
	}

	void FApplicationDX12::WaitForPreviousFrame()
	{
        UINT valueToWait = mFenceValue;
        HR(mD3DCommandQueue->Signal(mFence.Get(), valueToWait));
        ++mFenceValue;

        if (mFence->GetCompletedValue() < valueToWait)
        {
            HR(mFence->SetEventOnCompletion(valueToWait, mFenceEvent));
            WaitForSingleObject(mFenceEvent, INFINITE);
        }

        mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

    void FApplicationDX12::WaitForGpu()
    {
        UINT valueToWait = mFenceValue;
        HR(mD3DCommandQueue->Signal(mFence.Get(), valueToWait));
        
        HR(mFence->SetEventOnCompletion(valueToWait, mFenceEvent));
        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);

        ++mFenceValue;
    }

	void FApplicationDX12::LoadPipeline()
	{
        UINT dxgiFactoryFlag = 0;

#ifdef _DEBUG
        Microsoft::WRL::ComPtr<ID3D12Debug3> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
        {
            debugInterface->EnableDebugLayer();

            dxgiFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif // DEBUG

        HR(CreateDXGIFactory2(dxgiFactoryFlag, IID_PPV_ARGS(&mDXGIFatory)));

        Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter;
        GetHardwareAdapter(mDXGIFatory.Get(), &dxgiAdapter, true);

        HR(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3DDevice)));

        FTextureHelper::Get(mD3DDevice);

        D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        HR(mD3DDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mD3DCommandQueue)));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
        swapChainDesc.Width = (UINT)mWindow.GetWindowWidth();
        swapChainDesc.Height = (UINT)mWindow.GetWindowHeight();
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.BufferCount = BackBufferFrameCount;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        HR(mDXGIFatory->CreateSwapChainForHwnd(mD3DCommandQueue.Get(),
            mWindow.GetWindowHandle(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain));

        HR(swapChain.As(&mSwapChain));

        HR(mDXGIFatory->MakeWindowAssociation(mWindow.GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER));

        mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

        {
            //Create texture heap
            mTextureHeapSize = UPPER_ALIGNMENT(32 * 1024 * 1024, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            mTextureHeapOffset = 0;

            D3D12_HEAP_DESC textureHeapDesc{};
            textureHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            textureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;
            textureHeapDesc.SizeInBytes = mTextureHeapSize;
            textureHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
            textureHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            textureHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            mD3DDevice->CreateHeap(&textureHeapDesc, IID_PPV_ARGS(&mTextureHeap));

            //Create render texture & depth stencil heap
            mRTDSHeapSize = UPPER_ALIGNMENT(32 * 1024 * 1024, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            mRTDSHeapOffset = 0;

            D3D12_HEAP_DESC rtDsHeapDesc{};
            rtDsHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            rtDsHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;
            rtDsHeapDesc.SizeInBytes = mRTDSHeapSize;
            rtDsHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
            rtDsHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            rtDsHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            mD3DDevice->CreateHeap(&rtDsHeapDesc, IID_PPV_ARGS(&mRTDSHeap));

            //Create upload heap
            mUploadHeapSize = UPPER_ALIGNMENT(32 * 1024 * 1024, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
            mUploadHeapOffset = 0;

            D3D12_HEAP_DESC uploadHeapDesc{};
            uploadHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            uploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
            uploadHeapDesc.SizeInBytes = mUploadHeapSize;
            uploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
            uploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            uploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            mD3DDevice->CreateHeap(&uploadHeapDesc, IID_PPV_ARGS(&mUploadHeap));
        }

        {
            //Create rtv descriptor heap
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
            rtvHeapDesc.NumDescriptors = BackBufferFrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            HR(mD3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRTVDescriptorHeap)));

            //Create dsv descriptor heap
            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
            dsvHeapDesc.NumDescriptors = 1;
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            HR(mD3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDepthStencilDescriptorHeap)));

            //Create srv descriptor heap
            D3D12_DESCRIPTOR_HEAP_DESC srvCBVHeapDesc{};
            srvCBVHeapDesc.NumDescriptors = 2;
            srvCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            HR(mD3DDevice->CreateDescriptorHeap(&srvCBVHeapDesc, IID_PPV_ARGS(&mSRVCBVDescriptorHeap)));

            mRTVDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        {
            //Create Rendertarget view
            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            for (UINT i = 0; i < BackBufferFrameCount; i++)
            {
                HR(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffers[i])));
                mD3DDevice->CreateRenderTargetView(mBackBuffers[i].Get(), nullptr, descriptorHandle);
                descriptorHandle.Offset(1, mRTVDescriptorSize);
            }
        }

        //Create Sampler Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC samplerDesc{};
            samplerDesc.NumDescriptors = mMaxSamplerCount;
            samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            HR(mD3DDevice->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&mSamplerDescriptorHeap)));
        }

        //Create sampler
        {
            D3D12_SAMPLER_DESC samplerDesc;
            samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_ANISOTROPIC;
            samplerDesc.MipLODBias = 0;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
            samplerDesc.MaxAnisotropy = 1;
            samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
            samplerDesc.BorderColor[0] = 0.0f;
            samplerDesc.BorderColor[1] = 0.0f;
            samplerDesc.BorderColor[2] = 0.0f;
            samplerDesc.BorderColor[3] = 0.0f;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

            UINT samplerDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

            CD3DX12_CPU_DESCRIPTOR_HANDLE samplerDescriptorHandle{ mSamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};
            mD3DDevice->CreateSampler(&samplerDesc, samplerDescriptorHandle);


            samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.BorderColor[1] = 0.0f;
            samplerDesc.BorderColor[2] = 0.0f;
            samplerDesc.BorderColor[3] = 0.0f;

            samplerDescriptorHandle.Offset(samplerDescriptorSize);
            mD3DDevice->CreateSampler(&samplerDesc, samplerDescriptorHandle);


            samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            samplerDesc.BorderColor[0] = 0.0f;
            samplerDesc.BorderColor[1] = 1.0f;
            samplerDesc.BorderColor[2] = 0.0f;
            samplerDesc.BorderColor[3] = 0.0f;

            samplerDescriptorHandle.Offset(samplerDescriptorSize);
            mD3DDevice->CreateSampler(&samplerDesc, samplerDescriptorHandle);


            samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            samplerDesc.BorderColor[0] = 0.0f;
            samplerDesc.BorderColor[1] = 0.0f;
            samplerDesc.BorderColor[2] = 1.0f;
            samplerDesc.BorderColor[3] = 0.0f;

            samplerDescriptorHandle.Offset(samplerDescriptorSize);
            mD3DDevice->CreateSampler(&samplerDesc, samplerDescriptorHandle);

            samplerDesc.Filter = D3D12_FILTER::D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
            samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.BorderColor[1] = 1.0f;
            samplerDesc.BorderColor[2] = 1.0f;
            samplerDesc.BorderColor[3] = 0.0f;

            samplerDescriptorHandle.Offset(samplerDescriptorSize);
            mD3DDevice->CreateSampler(&samplerDesc, samplerDescriptorHandle);
        }

        //Create depth stencil buffer
        {
            D3D12_RESOURCE_DESC depthStencilDesc{};
            depthStencilDesc.MipLevels = 1;
            //depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthStencilDesc.Width = mWindow.GetWindowWidth();
            depthStencilDesc.Height = (UINT)mWindow.GetWindowHeight();
            depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            depthStencilDesc.DepthOrArraySize = 1;
            depthStencilDesc.MipLevels = 0;
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
            depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;       

            mDepthStencilClearValue.DepthStencil.Depth = 1.0f;
            mDepthStencilClearValue.DepthStencil.Stencil = 0;
            mDepthStencilClearValue.Format = DXGI_FORMAT_D32_FLOAT;

            HR(mD3DDevice->CreatePlacedResource(
                mRTDSHeap.Get(),
                mRTDSHeapOffset,
                &depthStencilDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &mDepthStencilClearValue,
                IID_PPV_ARGS(&mDepthStencilBuffer)
            ));

            D3D12_RESOURCE_ALLOCATION_INFO textureAllocationInfo = mD3DDevice->GetResourceAllocationInfo(0, 1, &depthStencilDesc);

            //mRTDSHeapOffset += UPPER_ALIGNMENT(textureAllocationInfo.SizeInBytes, textureAllocationInfo.Alignment);
            
            mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }

        //Create Command Allocator
        HR(mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
	}

	void FApplicationDX12::LoadAssets()
	{
        D3D12_FEATURE_DATA_ROOT_SIGNATURE signatureFeatureData{};

        signatureFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(mD3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &signatureFeatureData, sizeof(signatureFeatureData))))
        {
            signatureFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_DESCRIPTOR_RANGE1 shaderResourceRanges[3] = {};
        shaderResourceRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        shaderResourceRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        shaderResourceRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 shaderResourceParameter[3] = {};
        shaderResourceParameter[0].InitAsDescriptorTable(1, &shaderResourceRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        shaderResourceParameter[1].InitAsDescriptorTable(1, &shaderResourceRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);
        shaderResourceParameter[2].InitAsDescriptorTable(1, &shaderResourceRanges[2], D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(shaderResourceParameter), shaderResourceParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        Microsoft::WRL::ComPtr<ID3DBlob> blob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        HR(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, signatureFeatureData.HighestVersion, &blob, &errorBlob));
        HR(mD3DDevice->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));

        {
            Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
            Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

            UINT shaderCompileFlag = 0;

#ifdef _DEBUG
            shaderCompileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

            shaderCompileFlag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

            CHAR assetsPath[512];
            DWORD size = GetModuleFileName(nullptr, assetsPath, _countof(assetsPath));

            std::filesystem::path currentPath = std::filesystem::current_path();
            //std::filesystem::path shaderPath = currentPath / "src\\resources\\shader.hlsl";

            std::filesystem::path shaderPath = currentPath.parent_path() / "DashCore\\Src\\Resources\\shader.hlsl";
           
            HR(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", shaderCompileFlag, 0, &vertexShader, nullptr));
            HR(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", shaderCompileFlag, 0, &pixelShader, nullptr));

            //struct Vertex
            //{
            //    FVector3f Pos;
            //    FVector3f Normal;
            //    FVector4f Tangent;
            //    FVector4f Color;
            //    FVector2f UV;
            //};

            D3D12_INPUT_ELEMENT_DESC inputElements[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
            pipelineStateDesc.pRootSignature = mRootSignature.Get();
            pipelineStateDesc.InputLayout = { inputElements , _countof(inputElements) };
            pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE{ vertexShader.Get() };
            pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE{ pixelShader.Get() };
            pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
            pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
            pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
            pipelineStateDesc.DepthStencilState.DepthEnable = TRUE;
            pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            pipelineStateDesc.SampleMask = UINT_MAX;
            pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineStateDesc.NumRenderTargets = 1;
            pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            pipelineStateDesc.SampleDesc.Count = 1;
            HR(mD3DDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&mPipelineState)));
        }

        HR(mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

        Microsoft::WRL::ComPtr<ID3D12Resource> uploadTextureBuffer;

        //Create Texture
        {
            FTexture texture;

            FTextureHelper::Get()->Begin();

            //FTextureHelper::Get()->CreateWICTextureFromFileEx(
            //    L"coma.png",
            //    mTextureResource,
            //    texture,
            //    D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
            //    EWIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN);

            FTextureHelper::Get()->CreateDDSTextureFromFileEx(
                L"Src/DDSTest.dds",              
                mTextureResource,
                D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE,
                EDDS_LOADER_FLAGS::DDS_LOADER_DEFAULT);

            FTextureHelper::Get()->End(mD3DCommandQueue);

            D3D12_RESOURCE_DESC textureDesc = mTextureResource->GetDesc();

            //Create shader resource view
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

            mD3DDevice->CreateShaderResourceView(mTextureResource.Get(), &srvDesc, mSRVCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }



        //Create Vertex & Index Buffer
        {
            //Vertex triangle[] =
            //{
            //    { Dash::FVector3f{ -0.5f,  -0.5f , 0.0f }, Dash::FVector2f{ 0.0f, 2.0f } },
            //    { Dash::FVector3f{ -0.5f,   0.5f , 0.0f }, Dash::FVector2f{ 0.0f, 0.0f } },
            //    { Dash::FVector3f{  0.5f,  -0.5f , 0.0f }, Dash::FVector2f{ 2.0f, 2.0f } },
            //    { Dash::FVector3f{  0.5f,   0.5f , 0.0f }, Dash::FVector2f{ 2.0f, 0.0f } }
            //};

            MeshData boxMesh= CreateBoxMesh(1, 1, 1, FVector4f{0.5f, 0.5f, 0.5f, 1.0f});

            const UINT vertexBufferSize = (UINT)boxMesh.VertexData.size() * sizeof(Vertex);
            const UINT indexBufferSize = (UINT)boxMesh.IndexData.size() * sizeof(UINT);

            mIndexCount = (UINT)boxMesh.IndexData.size();

            //HR(mD3DDevice->CreateCommittedResource(
            //    &CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD },
            //    D3D12_HEAP_FLAG_NONE,
            //    &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            //    D3D12_RESOURCE_STATE_GENERIC_READ,
            //    nullptr,
            //    IID_PPV_ARGS(&mVertexBuffer)));

            //Create Vertex Buffer

            HR(mD3DDevice->CreatePlacedResource(
                mUploadHeap.Get(),
                mUploadHeapOffset,
                & CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mVertexBuffer)));

            mUploadHeapOffset += UPPER_ALIGNMENT(vertexBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            UINT8* vertexDataBegin;
            CD3DX12_RANGE vertexReadRange{ 0, 0 };
            mVertexBuffer->Map(0, &vertexReadRange, reinterpret_cast<void**>(&vertexDataBegin));
            memcpy(vertexDataBegin, boxMesh.VertexData.data(), vertexBufferSize);
            mVertexBuffer->Unmap(0, nullptr);

            mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
            mVertexBufferView.SizeInBytes = vertexBufferSize;
            mVertexBufferView.StrideInBytes = sizeof(Vertex);

            //Create Index Buffer

            HR(mD3DDevice->CreatePlacedResource(
                mUploadHeap.Get(),
                mUploadHeapOffset,
                &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mIndexBuffer)));

            mUploadHeapOffset += UPPER_ALIGNMENT(indexBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            UINT8* indexDataBegin;
            CD3DX12_RANGE indexReadRange{ 0, 0 };
            mIndexBuffer->Map(0, &indexReadRange, reinterpret_cast<void**>(&indexDataBegin));
            memcpy(indexDataBegin, boxMesh.IndexData.data(), indexBufferSize);
            mIndexBuffer->Unmap(0, nullptr);

            mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
            mIndexBufferView.SizeInBytes = indexBufferSize;
            mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
        };

        //Create constant buffer
        {
            UINT mConstantBufferSize = UPPER_ALIGNMENT(sizeof(FFrameConstantBuffer), 256);

            HR(mD3DDevice->CreatePlacedResource(
                mUploadHeap.Get(),
                mUploadHeapOffset,
                &CD3DX12_RESOURCE_DESC::Buffer(mConstantBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mConstantBuffer)));

            mUploadHeapOffset += UPPER_ALIGNMENT(mConstantBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
            constantBufferViewDesc.SizeInBytes = mConstantBufferSize;
            constantBufferViewDesc.BufferLocation = mConstantBuffer->GetGPUVirtualAddress();

            CD3DX12_CPU_DESCRIPTOR_HANDLE constantBufferHandle{mSRVCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, 
                mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)};
            mD3DDevice->CreateConstantBufferView(&constantBufferViewDesc, constantBufferHandle);

            //mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mConstantBufferData));
            //memcpy(mConstantBufferData, &sFrameConstantBuffer, mConstantBufferSize);

            mConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&sFrameConstantBuffer));

            sFrameConstantBuffer->Time = 0.0f;
            sFrameConstantBuffer->Speed = FVector2f{1.0f, 0.0f};
            sFrameConstantBuffer->ProjectionMatrix = mCamera.GetProjectionMatrix();
            sFrameConstantBuffer->ViewMatrix = mCamera.GetViewMatrix();
        }

        HR(mCommandList->Close());
        ID3D12CommandList* commands[] = { mCommandList.Get() };
        mD3DCommandQueue->ExecuteCommandLists(_countof(commands), commands);

        //FTextureHelper::Get()->GenerateMipmap(mTextureResource, mD3DCommandQueue);

        HR(mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));
        mFenceValue = 1;

        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        if (mFenceEvent == nullptr)
        {
            HR(HRESULT_FROM_WIN32(GetLastError()));
        }

        WaitForPreviousFrame();
	}

    void FApplicationDX12::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
    {
        *ppAdapter = nullptr;

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

        Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
        if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (
                UINT adapterIndex = 0;
                DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                    IID_PPV_ARGS(&adapter));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }
        else
        {
            for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                {
                    break;
                }
            }
        }

        *ppAdapter = adapter.Detach();
    }

}
