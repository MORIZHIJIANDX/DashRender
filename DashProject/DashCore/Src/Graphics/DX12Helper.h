#pragma once

#include "d3dx12.h"
#include <dxgi.h>
#include <strsafe.h>
#include "Utility/StringUtility.h"

namespace Dash
{
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

    // Ϊ�˵��Լ�����������������ͺ궨�壬Ϊÿ���ӿڶ����������ƣ�����鿴�������
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
    // DXTraceW����
    // ------------------------------
    // �ڵ�����������������ʽ��������Ϣ����ѡ�Ĵ��󴰿ڵ���(�Ѻ���)
    // [In]strFile			��ǰ�ļ�����ͨ�����ݺ�__FILEW__
    // [In]hlslFileName     ��ǰ�кţ�ͨ�����ݺ�__LINE__
    // [In]hr				����ִ�г�������ʱ���ص�HRESULTֵ
    // [In]strMsg			���ڰ������Զ�λ���ַ�����ͨ������L#x(����ΪNULL)
    // [In]bPopMsgBox       ���ΪTRUE���򵯳�һ����Ϣ������֪������Ϣ
    // ����ֵ: �β�hr
    HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);

        // ------------------------------
        // HR��
        // ------------------------------
        // Debugģʽ�µĴ���������׷��
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