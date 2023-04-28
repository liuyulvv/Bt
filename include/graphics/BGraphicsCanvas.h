#pragma once

/**
 * @file BGraphicsCanvas.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#include "BPlatform.h"

class BGraphicsCanvas {
public:
    BGraphicsCanvas() = default;
    virtual ~BGraphicsCanvas() = default;
    BGraphicsCanvas(const BGraphicsCanvas& canvas) = default;
    BGraphicsCanvas(BGraphicsCanvas&& canvas) = default;
    BGraphicsCanvas& operator=(const BGraphicsCanvas& canvas) = default;
    BGraphicsCanvas& operator=(BGraphicsCanvas&& canvas) = default;

public:
    virtual BCanvasID getCanvasID() const = 0;
    virtual int width() const = 0;
    virtual int height() const = 0;
};