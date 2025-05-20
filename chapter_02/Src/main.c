#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdint.h>
#include <math.h>

#include "main.h"

int fb_fd;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize;
char *fbp;

typedef uint32_t color_t;

void fb_init(const char *device) {
    printf("Initializing %s\n", device);
    // 打开设备
    fb_fd = open(device, O_RDWR);
    if (fb_fd == -1) {
        perror("Error opening framebuffer device\n");
        exit(1);
    }

    // 获取屏幕信息
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed information\n");
        exit(2);
    }
    printf("fb_fd: %d\n", fb_fd);

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information\n");
        exit(3);
    }

    // 计算屏幕缓冲区大小
    screensize = vinfo.yres_virtual * finfo.line_length;
    printf("screensize: %ld\n", screensize);

    // 将framebuffer映射到用户空间
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((intptr_t)fbp == -1) {
        perror("Error mapping framebuffer device to memory\n");
        exit(4);
    }
    printf("Done initializing fb device\n");
}

/*
 * 此处的 set_pixel 函数针对显示屏的情形如下
mode "1024x600-0"
        # D: 0.000 MHz, H: 0.000 kHz, V: 0.000 Hz
        geometry 1024 600 1024 600 32
        timings 0 0 0 0 0 0 0
        accel true
        rgba 8/16,8/8,8/0,0/0
endmode
 */
void set_pixel(int x, int y, color_t color) {
    long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                        (y + vinfo.yoffset) * finfo.line_length;
    *(color_t *)(fbp + location) = color; // 现在假设是32位色深
}

void draw_rectangle(int x1, int y1, int x2, int y2, color_t color) {
    for (int i = x1; i <= x2; i++) {
        set_pixel(i, y1, color);
        set_pixel(i, y2, color);
    }
    for (int i = y1; i <= y2; i++) {
        set_pixel(x1, i, color);
        set_pixel(x2, i, color);
    }
}

void draw_line(int x1, int y1, int x2, int y2, color_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (1) {
        set_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void draw_circle(int xc, int yc, int radius, color_t color) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        set_pixel(xc + x, yc + y, color);
        set_pixel(xc - x, yc + y, color);
        set_pixel(xc + x, yc - y, color);
        set_pixel(xc - x, yc - y, color);
        set_pixel(xc + y, yc + x, color);
        set_pixel(xc - y, yc + x, color);
        set_pixel(xc + y, yc - x, color);
        set_pixel(xc - y, yc - x, color);
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void clear_screen() {
    memset(fbp, 0, screensize);
}

int main() {
    fb_init("/dev/fb0");

    clear_screen();

    // 设置颜色为白色（假设背景是黑色）
    color_t color = 0x00FFFFFF;

    // 画点
    set_pixel(100, 100, color);
    printf("Done setting pixel\n");

    // 画矩形
    draw_rectangle(50, 50, 150, 150, color);
    printf("Done drawing rectangle\n");

    // 画线
    draw_line(200, 200, 300, 300, color);
    printf("Done drawing line\n");

    // 画圆
    draw_circle(400, 400, 50, color);
    printf("Done drawing circle\n");

    // 释放资源
    printf("Ending drawing\n");
    munmap(fbp, screensize);
    close(fb_fd);

    return 0;
}
