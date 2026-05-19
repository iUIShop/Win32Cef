// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <windows.h>

#include <string>

#include "include/cef_browser.h"

void SimpleHandler::SetBrowserSize(int browser_id, int width, int height)
{
	m_browser_sizes[browser_id] = {width, height};

	m_spOsrRenderHandler->SetBrowserSize(width, height);
}

void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString& title) {
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  if (hwnd) {
    SetWindowText(hwnd, std::wstring(title).c_str());
  }
}
