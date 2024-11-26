#pragma once

#include <include\cef_render_handler.h>


class COsrRenderHandler : public CefRenderHandler
{
public:
	///
/// Called to retrieve the view rectangle in screen DIP coordinates. This
/// method must always provide a non-empty rectangle.
///
/*--cef()--*/
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);

    ///
/// Called when an element should be painted. Pixel values passed to this
/// method are scaled relative to view coordinates based on the value of
/// CefScreenInfo.device_scale_factor returned from GetScreenInfo. |type|
/// indicates whether the element is the view or the popup widget. |buffer|
/// contains the pixel data for the whole image. |dirtyRects| contains the set
/// of rectangles in pixel coordinates that need to be repainted. |buffer|
/// will be |width|*|height|*4 bytes in size and represents a BGRA image with
/// an upper-left origin. This method is only called when
/// CefWindowInfo::shared_texture_enabled is set to false.
///
/*--cef()--*/
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width,
        int height);

protected:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(COsrRenderHandler);
};
