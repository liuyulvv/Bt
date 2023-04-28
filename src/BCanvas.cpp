/**
 * @file BCanvas.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BCanvas.h"

#include <stdexcept>

BCanvas::BCanvas() {
#if defined(_WIN32)
    m_id = CreateWindowEx(
        0,
        B_CLASS_NAME,
        m_title.c_str(),
        WS_OVERLAPPEDWINDOW,
        m_position.x,
        m_position.y,
        m_size.width,
        m_size.height,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    if (!m_id) {
        throw std::runtime_error("Failed to create canvas");
    }
    SetWindowLongPtr(m_id, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif
    auto size = getMonitorSize();
    move({static_cast<int32_t>(size.width / 4), static_cast<int32_t>(size.height / 4)}, {size.width / 2, size.height / 2});
}

void BCanvas::show() const {
#if defined(_WIN32)
    ShowWindow(m_id, 5);
#endif
}

std::wstring BCanvas::title() const {
    return m_title;
}

void BCanvas::setTitle(const std::wstring& title) {
    m_title = title;
#if defined(_WIN32)
    SetWindowText(m_id, m_title.c_str());
#endif
}

void BCanvas::move(const BPosition& pos, const BSize& size, bool repaint) {
#if defined(_WIN32)
    MoveWindow(m_id, pos.x, pos.y, size.width, size.height, repaint);
#endif
}

void BCanvas::move(const BPosition& pos, bool repaint) {
    move(pos, m_size, repaint);
}

void BCanvas::move(int32_t x, int32_t y, uint32_t width, uint32_t height, bool repaint) {
    move({x, y}, {width, height}, repaint);
}

void BCanvas::move(int32_t x, int32_t y, bool repaint) {
    move({x, y}, repaint);
}

void BCanvas::resize(const BSize& size, bool repaint) {
    move(m_position, size, repaint);
}

void BCanvas::resize(uint32_t width, uint32_t height, bool repaint) {
    resize({width, height}, repaint);
}

void BCanvas::moveEvent(const BPosition& pos) {
    m_position = pos;
}

void BCanvas::resizeEvent(const BSize& size) {
    m_size = size;
}

BCanvasID BCanvas::getCanvasID() const {
    return m_id;
}

int BCanvas::width() const {
    return m_size.width;
}

int BCanvas::height() const {
    return m_size.height;
}

BSize BCanvas::getMonitorSize() const {
    BSize size{};
#if defined(_WIN32)
    auto monitor = MonitorFromWindow(m_id, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX info;
    info.cbSize = sizeof(info);
    if (!GetMonitorInfo(monitor, &info))
        return size;
    DEVMODE dm{};
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    if (!EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &dm))
        return size;
    size.width = dm.dmPelsWidth;
    size.height = dm.dmPelsHeight;
#endif
    return size;
}