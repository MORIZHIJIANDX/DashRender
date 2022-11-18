#pragma once

#include "d3dx12.h"
#include <dxgi.h>
#include <strsafe.h>
#include "Utility/StringUtility.h"

namespace Dash
{
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

    // 为了调试加入下面的内联函数和宏定义，为每个接口对象设置名称，方便查看调试输出
    #if defined(DASH_DEBUG)
        FORCEINLINE void SetD3D12DebugName(ID3D12Object* pObject, LPCWSTR name)
        {
            pObject->SetName(name);
        }

		FORCEINLINE void SetD3D12DebugName(ID3D12Object* pObject, const std::string& name)
		{
            std::wstring wName = FStringUtility::UTF8ToWideString(name);
            pObject->SetName(wName.c_str());
		}

        FORCEINLINE void SetD3D12DebugNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
        {
            WCHAR _DebugName[MAX_PATH] = {};
            if (SUCCEEDED(StringCchPrintfW(_DebugName, _countof(_DebugName), L"%s[%u]", name, index)))
            {
                pObject->SetName(_DebugName);
            }
        }

		FORCEINLINE void SetD3D12DebugNameIndexed(ID3D12Object* pObject, const std::string& name, UINT index)
		{
            std::wstring wName = FStringUtility::UTF8ToWideString(name + "[" + FStringUtility::ToString(index) + "]");
            pObject->SetName(wName.c_str());
        }   
    #else
        FORCEINLINE void SetD3D12DebugName(ID3D12Object*, LPCWSTR)
        {
        }
        FORCEINLINE void SetD3D12DebugNameIndexed(ID3D12Object*, LPCWSTR, UINT)
        {
        }
    #endif
    #define DASH_SET_D3D12_DEBUGNAME(x)                     SetD3D12DebugName(x, L#x)
    #define DASH_SET_D3D12_DEBUGNAME_INDEXED(x, n)           SetD3D12DebugNameIndexed(x[n], L#x, n)
    #define DASH_SET_D3D12_DEBUGNAME_COMPTR(x)              SetD3D12DebugName(x.Get(), L#x)
    #define DASH_SET_D3D12_DEBUGNAME_INDEXED_COMPTR(x, n) SetD3D12DebugNameIndexed(x[n].Get(), L#x, n)

    #if defined(DASH_DEBUG)
        FORCEINLINE void SetDXGIDebugName(IDXGIObject* pObject, LPCWSTR name)
        {
            size_t szLen = 0;
            if (SUCCEEDED(StringCchLengthW(name, 50, &szLen)))
            {
                pObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(szLen - 1), name);
            }          
        }
        FORCEINLINE void SetDXGIDebugNameIndexed(IDXGIObject* pObject, LPCWSTR name, UINT index)
        {
            size_t szLen = 0;
            WCHAR _DebugName[MAX_PATH] = {};
            if (SUCCEEDED(StringCchPrintfW(_DebugName, _countof(_DebugName), L"%s[%u]", name, index)))
            {
                if (SUCCEEDED(StringCchLengthW(_DebugName, _countof(_DebugName), &szLen)))
                {
                    pObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(szLen), _DebugName);
                }                   
            }
        }
    #else
        FORCEINLINE void SetDXGIDebugName(ID3D12Object*, LPCWSTR)
        {
        }
        FORCEINLINE void SetDXGIDebugNameIndexed(ID3D12Object*, LPCWSTR, UINT)
        {
        }
    #endif

    #define DASH_SET_DXGI_DEBUGNAME(x)                      SetDXGIDebugName(x, L#x)
    #define DASH_SET_DXGI_DEBUGNAME_INDEXED(x, n)        SetDXGIDebugNameIndexed(x[n], L#x, n)
    #define DASH_SET_DXGI_DEBUGNAME_COMPTR(x)               SetDXGIDebugName(x.Get(), L#x)
    #define DASH_SET_DXGI_DEBUGNAME_INDEXED_COMPTR(x, n)      SetDXGIDebugNameIndexed(x[n].Get(), L#x, n)


    // ------------------------------
    // DXTraceW函数
    // ------------------------------
    // 在调试输出窗口中输出格式化错误信息，可选的错误窗口弹出(已汉化)
    // [In]strFile			当前文件名，通常传递宏__FILEW__
    // [In]hlslFileName     当前行号，通常传递宏__LINE__
    // [In]hr				函数执行出现问题时返回的HRESULT值
    // [In]strMsg			用于帮助调试定位的字符串，通常传递L#x(可能为NULL)
    // [In]bPopMsgBox       如果为TRUE，则弹出一个消息弹窗告知错误信息
    // 返回值: 形参hr
    HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);

        // ------------------------------
        // HR宏
        // ------------------------------
        // Debug模式下的错误提醒与追踪
#if defined(DEBUG) | defined(DASH_DEBUG)
    #ifndef DX_CALL
    #define DX_CALL(x)												\
	    {															\
		    HRESULT hr = (x);										\
		    if(FAILED(hr))											\
		    {														\
			    DXTraceW(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
		    }														\
	    }
    #endif
#else
    #ifndef DX_CALL
    #define DX_CALL(x) (x)
    #endif 
#endif
}