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
    virtual BCanvasID GetCanvasID() const = 0;
    virtual int Width() const = 0;
    virtual int Height() const = 0;
};