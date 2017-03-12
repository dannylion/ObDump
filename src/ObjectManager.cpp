/**
 * MIT License
 * 
 * Copyright (c) 2017 dannylion
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Windows.h>
#include <cassert>

#include "ObjectManager.h"

#pragma comment(lib, "ntdll.lib")

using namespace NT;

CObjectManager::CObjectManager() : _size(1 << 16) {
	_buffer = (POBJECT_DIRECTORY_INFORMATION)malloc(_size);
}


CObjectManager::~CObjectManager() {
	free(_buffer);
}

std::vector<std::pair<CString, CString>> CObjectManager::GetFolders(const CString& root) {
	HANDLE hDirectory;
	OBJECT_ATTRIBUTES attr;
	UNICODE_STRING name;
	RtlInitUnicodeString(&name, root.AllocSysString());
	InitializeObjectAttributes(&attr, &name, 0, nullptr, nullptr);
	std::vector<std::pair<CString, CString>> list;
	auto status = NtOpenDirectoryObject(&hDirectory, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &attr);
	if(status < 0)
		return list;

	ULONG index = 0, bytes;
	BOOLEAN firstEntry = TRUE;
	int start = 0;
	do {
		status = NtQueryDirectoryObject(hDirectory, _buffer, _size, FALSE, firstEntry, &index, &bytes);
		if(status < 0)
			break;
		for(ULONG i = 0; i < index - start; i++)
			list.push_back(std::make_pair(_buffer[i].Name.Buffer, _buffer[i].TypeName.Buffer));
		if(status == 0)
			break;
		// more entries (STATUS_NEED_MORE_ENTRIES)
		start = index;
		firstEntry = FALSE;
	} while(true);
	CloseHandle(hDirectory);
	return list;
}

CString CObjectManager::GetSymbolicLinkFromName(const CString& directory, const CString& name) const {
	HANDLE hLink;
	OBJECT_ATTRIBUTES attr, attrDir;
	UNICODE_STRING str, strDir;
	HANDLE hRoot;
	RtlInitUnicodeString(&strDir, directory.AllocSysString());
	InitializeObjectAttributes(&attrDir, &strDir, 0, nullptr, nullptr);
	NtOpenDirectoryObject(&hRoot, GENERIC_READ, &attrDir);
	assert(hRoot != nullptr);
	RtlInitUnicodeString(&str, name.AllocSysString());
	InitializeObjectAttributes(&attr, &str, 0, hRoot, nullptr);
	NtOpenSymbolicLinkObject(&hLink, GENERIC_READ, &attr);
	WCHAR buffer[256] = { 0 };
	UNICODE_STRING target;
	RtlInitUnicodeString(&target, buffer);
	target.MaximumLength = 255 * 2;
	ULONG len;
	NtQuerySymbolicLinkObject(hLink, &target, &len);
	CloseHandle(hLink);
	CloseHandle(hRoot);
	return target.Buffer;
}

HANDLE CObjectManager::OpenObject(const CString& name, ULONG access, const CString& type, NTSTATUS* pStatus) {
	HANDLE hObject = nullptr;
	OBJECT_ATTRIBUTES attr;
	UNICODE_STRING uname;
	RtlInitUnicodeString(&uname, name.AllocSysString());
	InitializeObjectAttributes(&attr, &uname, 0, nullptr, nullptr);
	NTSTATUS status = E_FAIL;
	if(type == L"Event")
		status = NtOpenEvent(&hObject, access, &attr);
	else if(type == L"Mutant")
		status = NtOpenMutant(&hObject, access, &attr);
	else if(type == L"Section")
		status = NtOpenSection(&hObject, access, &attr);
	else if(type == L"Semaphore")
		status = NtOpenSemaphore(&hObject, access, &attr);
	else if(type == "EventPair")
		status = NtOpenEventPair(&hObject, access, &attr);
	else if(type == L"IoCompletion")
		status = NtOpenIoCompletion(&hObject, access, &attr);
	else if(type == L"SymbolicLink")
		status = NtOpenSymbolicLinkObject(&hObject, access, &attr);
	else if(type == "Key")
		status = NtOpenKey(&hObject, access, &attr);
	if(pStatus)
		*pStatus = status;
	return hObject;
}

