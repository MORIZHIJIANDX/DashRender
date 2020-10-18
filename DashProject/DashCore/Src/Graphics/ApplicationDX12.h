#pragma once

#include "Application.h"
#include "Camera.h"

#include <wrl.h>
#include <shellapi.h>

//for d3d12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12sdklayers.h>

//linker
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace Dash
{
	class FApplicationDX12 : public FApplication
	{
	public:
		FApplicationDX12(size_t windowWidth = 720, size_t windowHeight = 480);
		virtual ~FApplicationDX12();

		void OnRender(const FRenderEventArgs& e) override;
		void OnUpdate(const FUpdateEventArgs& e) override;

	private:
        void PopulateCommandList(const FRenderEventArgs& e);
        void WaitForPreviousFrame();

        void WaitForGpu();

        void LoadPipeline();
        void LoadAssets();

        void GetHardwareAdapter(
            _In_ IDXGIFactory1* pFactory,
            _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
            bool requestHighPerformanceAdapter = false);

        virtual HRESULT GetDeviceRemoveReason() override
        {
            if (mD3DDevice)
            {
                return mD3DDevice->GetDeviceRemovedReason();
            }

            return 0;
        }

        void OnMouseWhellUp(FMouseWheelEventArgs& args);
        void OnMouseWhellDown(FMouseWheelEventArgs& args);

        void OnMouseMove(FMouseMotionEventArgs& args);

        void OnWindowResize(FResizeEventArgs& args);

    public:
        static const UINT BackBufferFrameCount = 2;

        static const UINT FrameCount = 2;
        static const UINT TextureWidth = 256;
        static const UINT TextureHeight = 256;
        static const UINT TexturePixelSize = 4;

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory4> mDXGIFatory;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;

        Microsoft::WRL::ComPtr<ID3D12Device> mD3DDevice;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mD3DCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffers[BackBufferFrameCount];
        Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;

        Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

        Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBuffer;
        D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
        UINT mIndexCount;

        Microsoft::WRL::ComPtr<ID3D12Resource> mTextureResource;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSRVCBVDescriptorHeap;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

        Microsoft::WRL::ComPtr<ID3D12Heap> mTextureHeap;
        UINT mTextureHeapSize;
        UINT mTextureHeapOffset;

        Microsoft::WRL::ComPtr<ID3D12Heap> mUploadHeap;
        UINT mUploadHeapSize;
        UINT mUploadHeapOffset;

        Microsoft::WRL::ComPtr<ID3D12Heap> mRTDSHeap;
        UINT mRTDSHeapSize;
        UINT mRTDSHeapOffset;

        Microsoft::WRL::ComPtr<ID3D12Resource> mConstantBuffer;
        UINT8* mConstantBufferData;
        UINT mConstantBufferSize;

        Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDepthStencilDescriptorHeap;
        D3D12_CLEAR_VALUE mDepthStencilClearValue;

        UINT mBackBufferIndex;
        UINT mRTVDescriptorSize;
        UINT mFenceValue;

        HANDLE mFenceEvent;

        D3D12_VIEWPORT mViewport;
        RECT mScissorRect;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSamplerDescriptorHeap;
        const UINT mMaxSamplerCount = 5;
        int mCurrentSamplerIndex = 0;

        //FOrthographicCamera mCamera;
        FirstPersonCamera mCamera;
        Scalar mFov;

        FTransform mObjectTransform;

        FMouseWheelEventDelegate mMouseWheelUpDelegate;
        FMouseWheelEventDelegate mMouseWheelDownDelegate;

        FMouseMotionEventDelegate mMouseMotionDelegate;

        FResizeEventDelegate mWindowResizeDelegate;

        bool mWindowedMode;
        bool mWindowVisible;
	};

}