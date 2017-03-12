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
#include <atlstr.h>
#include <iostream>

#include "ObjectManager.h"

void
DumpObjectManager(
	CObjectManager& tObjManager,
	const CString& szRootPath
)
{
	auto items = tObjManager.GetFolders(szRootPath);

	for each(auto item in items)
	{
		// Print "%(ObjectPath)s,%(ObjectType)s"
		std::cout << szRootPath << "\\" << item.first << "," << item.second;

		// Print "->%(SymlinkTarget)s\n"
		CString szLinkTarget = "";
		if (L"SymbolicLink" == item.second)
		{
			szLinkTarget = tObjManager.GetSymbolicLinkFromName(szRootPath, item.first);
			std::cout << "->" << szLinkTarget;
		}
		std::cout << std::endl;

		if (L"Directory" == item.second)
		{
			if (szRootPath == L"\\")
			{
				DumpObjectManager(tObjManager, szRootPath + item.first);
			}
			else
			{
				DumpObjectManager(tObjManager, szRootPath + L"\\" + item.first);
			}
		}
	}
}

int main(void)
{
	CObjectManager tObjManager;
	const CString& pszRootPath = L"\\";

	DumpObjectManager(tObjManager, pszRootPath);
}
