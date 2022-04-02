/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_platform.h"
#include "hal_uart.h"
#include "hal_mtimer.h"
#include <iostream>

class Shape {
protected:
    int width, height;

public:
    Shape(int a = 0, int b = 0)
    {
        width = a;
        height = b;
    }
    virtual int area()
    {
        bflb_platform_printf((char*)"this is Shape class area\r\n");
        return 0;
    }
};
class Rectangle : public Shape {
public:
    Rectangle(int a = 0, int b = 0)
        : Shape(a, b)
    {
    }
    int area()
    {
        bflb_platform_printf((char*)"this is Rectangle class area\r\n");
        return (width * height);
    }
};
class Triangle : public Shape {
public:
    Triangle(int a = 0, int b = 0)
        : Shape(a, b)
    {
    }
    int area()
    {
        bflb_platform_printf((char*)"this is Triangle class area\r\n");
        return (width * height / 2);
    }
};

class Box {
public:
    Box()
    {
        bflb_platform_printf((char*)"Calling the constructor func\r\n");
    }
    ~Box()
    {
        bflb_platform_printf((char*)"Calling the destructor func\r\n");
    }
};

int main(void)
{
    uint32_t a=0;
    bflb_platform_init(0);
    a = mtimer_get_time_us();

    /*polymorphic test*/
    Shape *shape;
    Rectangle rec(10, 7);
    Triangle tri(10, 5);

    shape = &rec;
    shape->area();

    shape = &tri;
    shape->area();

    /*new and delete test*/
    int **p;
    int i, j; //p[4][8]

    p = new int *[4];
    for (i = 0; i < 4; i++) {
        p[i] = new int[8];
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            p[i][j] = j * i;
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            if (j == 0)
                bflb_platform_printf((char*)"\r\n");
            bflb_platform_printf("%d ", p[i][j]);
        }
    }
    bflb_platform_printf((char*)"\r\n");
    for (i = 0; i < 4; i++) {
        delete[] p[i];
    }
    delete[] p;

    Box *myBoxArray = new Box[4];

    delete[] myBoxArray;

    bflb_platform_printf((char*)"box address %d\r\n",myBoxArray);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
