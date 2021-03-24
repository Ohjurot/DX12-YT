#pragma once

#include <d3d12.h>

template <typename T>
inline const wchar_t* typeNameW() { return L"unknown"; }

// Device

template <>
inline const wchar_t* typeNameW<ID3D12Device>() { return L"ID3D12Device"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device2>() { return L"ID3D12Device2"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device3>() { return L"ID3D12Device3"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device4>() { return L"ID3D12Device4"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device5>() { return L"ID3D12Device5"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device6>() { return L"ID3D12Device6"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device7>() { return L"ID3D12Device7"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device8>() { return L"ID3D12Device8"; }

template <>
inline const wchar_t* typeNameW<ID3D12Device9>() { return L"ID3D12Device9"; }

// CMD Queue

template <>
inline const wchar_t* typeNameW<ID3D12CommandQueue>() { return L"ID3D12CommandQueue"; }

// CMD Allocator

template <>
inline const wchar_t* typeNameW<ID3D12CommandAllocator>() { return L"ID3D12CommandAllocator"; }

// CMD List

template <>
inline const wchar_t* typeNameW<ID3D12CommandList>() { return L"ID3D12CommandList"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList>() { return L"ID3D12GraphicsCommandList"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList1>() { return L"ID3D12GraphicsCommandList1"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList2>() { return L"ID3D12GraphicsCommandList2"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList3>() { return L"ID3D12GraphicsCommandList3"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList4>() { return L"ID3D12GraphicsCommandList4"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList5>() { return L"ID3D12GraphicsCommandList5"; }

template <>
inline const wchar_t* typeNameW<ID3D12GraphicsCommandList6>() { return L"ID3D12GraphicsCommandList6"; }

// Fence

template <>
inline const wchar_t* typeNameW<ID3D12Fence>() { return L"ID3D12Fence"; }

template <>
inline const wchar_t* typeNameW<ID3D12Fence1>() { return L"ID3D12Fence1"; }

// Heap

template <>
inline const wchar_t* typeNameW<ID3D12Heap>() { return L"ID3D12Heap"; }

// Resource

template <>
inline const wchar_t* typeNameW<ID3D12Resource>() { return L"ID3D12Resource"; }

template <>
inline const wchar_t* typeNameW<ID3D12Resource1>() { return L"ID3D12Resource1"; }

template <>
inline const wchar_t* typeNameW<ID3D12Resource2>() { return L"ID3D12Resource2"; }

// Descriptor Heap

template <>
inline const wchar_t* typeNameW<ID3D12DescriptorHeap>() { return L"ID3D12DescriptorHeap"; }
