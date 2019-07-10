#include "stdafx.h"
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#define CALL(method, object, ... )  (object->lpVtbl->method(object, ##__VA_ARGS__))

#define __riid(x) (REFIID)(&_IID_ ## x)

typedef HRESULT(WINAPI* FN_CREATE_DXGI_FACTORY1)(REFIID, void **);
typedef HRESULT(WINAPI* FN_D3D12_CREATE_DEVICE)(IUnknown*, D3D_FEATURE_LEVEL, REFIID, void**);


static const IID _IID_IDXGIFactory4 = { 0x1bc6ea02, 0xef36, 0x464f,{ 0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a } };
static const IID _IID_ID3D12Device = { 0x189819f1, 0x1db6, 0x4b57,{ 0xbe,0x54,0x18,0x21,0x33,0x9b,0x85,0xf7 } };
static const IID _IID_IDXGIAdapter = { 0x2411e7e1, 0x12ac, 0x4ccf,{ 0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0 } };
static const IID _IID_ID3D12DescriptorHeap = { 0x8efb471d, 0x616c, 0x4f49,{ 0x90,0xf7,0x12,0x7b,0xb7,0x63,0xfa,0x51 } };

int main()
{
	HRESULT hr;
	HMODULE dxgi_dll;
	FN_CREATE_DXGI_FACTORY1 CreateFactory1;
	IDXGIFactory4* factory4;
	IDXGIAdapter *adapter;
	HMODULE d3d12_dll;
	FN_D3D12_CREATE_DEVICE CreateDevice;
	ID3D12Device * device;
	ID3D12DescriptorHeap *heap;

	dxgi_dll = LoadLibraryEx(_T("dxgi.dll"), NULL, 0);
	CreateFactory1 = (FN_CREATE_DXGI_FACTORY1)GetProcAddress(dxgi_dll, "CreateDXGIFactory1");

	CreateFactory1(__riid(IDXGIFactory4), (void**)&factory4);

	hr = CALL(EnumWarpAdapter, factory4, __riid(IDXGIAdapter), (void**)&adapter);

	d3d12_dll = LoadLibraryEx(_T("d3d12.dll"), NULL, 0);
	CreateDevice = (FN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12_dll, "D3D12CreateDevice");

	hr = CreateDevice((IUnknown*)adapter, D3D_FEATURE_LEVEL_12_0, __riid(ID3D12Device), (void**)&device);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.NumDescriptors = 1,
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 0
	};
	hr = CALL(CreateDescriptorHeap, device, &desc, __riid(ID3D12DescriptorHeap), (void**)&heap);


	typedef void (STDMETHODCALLTYPE *FN_GET_DESC)(ID3D12DescriptorHeap*, D3D12_DESCRIPTOR_HEAP_DESC*);

	FN_GET_DESC GetDesc = (FN_GET_DESC)heap->lpVtbl->GetDesc;

	D3D12_DESCRIPTOR_HEAP_DESC desc2 = { 0 };

	GetDesc(heap, &desc2);

	return 0;
}
