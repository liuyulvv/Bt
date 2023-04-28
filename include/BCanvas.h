#pragma once

/**
 * @file BCanvas.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include <cstdint>
#include <string>

#include "BGraphicsCanvas.h"
#include "BPlatform.h"
#include "BPosition.h"
#include "BSize.h"

class BCanvas : public BGraphicsCanvas {
public:
    BCanvas();
    virtual ~BCanvas() = default;
    BCanvas(const BCanvas& canvas) = default;
    BCanvas(BCanvas&& canvas) = default;
    BCanvas& operator=(const BCanvas& canvas) = default;
    BCanvas& operator=(BCanvas&& canvas) = default;

public:
    void show() const;
    std::wstring title() const;
    void setTitle(const std::wstring& title);
    void move(const BPosition& pos, const BSize& size, bool repaint = false);
    void move(const BPosition& pos, bool repaint = false);
    void move(int32_t x, int32_t y, uint32_t width, uint32_t height, bool repaint = false);
    void move(int32_t x, int32_t y, bool repaint = false);
    void resize(const BSize& size, bool repaint = false);
    void resize(uint32_t width, uint32_t height, bool repaint = false);

public:
    void moveEvent(const BPosition& pos);
    void resizeEvent(const BSize& size);

    virtual BCanvasID getCanvasID() const override;
    virtual int width() const override;
    virtual int height() const override;

private:
    BSize getMonitorSize() const;

private:
    BCanvasID m_id{};
    std::wstring m_title{};
    BPosition m_position{};
    BSize m_size{};
};