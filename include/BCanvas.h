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
    void Show() const;
    std::wstring Title() const;
    void SetTitle(const std::wstring& title);
    void Move(const BPosition& pos, const BSize& size, bool repaint = false);
    void Move(const BPosition& pos, bool repaint = false);
    void Move(int32_t x, int32_t y, uint32_t width, uint32_t height, bool repaint = false);
    void Move(int32_t x, int32_t y, bool repaint = false);
    void Resize(const BSize& size, bool repaint = false);
    void Resize(uint32_t width, uint32_t height, bool repaint = false);

public:
    void MoveEvent(const BPosition& pos);
    void ResizeEvent(const BSize& size);

    virtual BCanvasID GetCanvasID() const override;
    virtual int Width() const override;
    virtual int Height() const override;

private:
    BSize GetMonitorSize() const;

private:
    BCanvasID id_{};
    std::wstring title_{};
    BPosition position_{};
    BSize size_{};
};