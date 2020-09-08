#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <android/log.h>
#define TAG "SDL"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#include "SDL.h"

//typedef struct Sprite
//{
//	SDL_Texture* texture;
//	Uint16 w;
//	Uint16 h;
//} Sprite;
//
///* Adapted from SDL's testspriteminimal.c */
//Sprite LoadSprite(const char* file, SDL_Renderer* renderer)
//{
//	Sprite result;
//	result.texture = NULL;
//	result.w = 0;
//	result.h = 0;
//
//    SDL_Surface* temp;
//
//    /* Load the sprite image */
//    temp = SDL_LoadBMP(file);
//    if (temp == NULL)
//	{
//        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
//        return result;
//    }
//    result.w = temp->w;
//    result.h = temp->h;
//
//    /* Create texture from the image */
//    result.texture = SDL_CreateTextureFromSurface(renderer, temp);
//    if (!result.texture) {
//        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
//        SDL_FreeSurface(temp);
//        return result;
//    }
//    SDL_FreeSurface(temp);
//
//    return result;
//}
//
//void draw(SDL_Window* window, SDL_Renderer* renderer, const Sprite sprite)
//{
//	int w, h;
//	SDL_GetWindowSize(window, &w, &h);
//	SDL_Rect destRect = {w/2 - sprite.w/2, h/2 - sprite.h/2, sprite.w, sprite.h};
//	/* Blit the sprite onto the screen */
//	SDL_RenderCopy(renderer, sprite.texture, NULL, &destRect);
//}
//
//int SDL_main(int argc, char *argv[])
//{
//    SDL_Window *window;
//    SDL_Renderer *renderer;
//
//    LOGD("SDL_main");
//    SDL_Log("SDL log main ruby");
//    if(SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer) < 0)
//        exit(2);
//
//	Sprite sprite = LoadSprite("image.bmp", renderer);
//    if(sprite.texture == NULL)
//        exit(2);
//
//    /* Main render loop */
//    Uint8 done = 0;
//    SDL_Event event;
//    while(!done)
//	{
//        /* Check for events */
//        while(SDL_PollEvent(&event))
//		{
//            if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_FINGERDOWN)
//			{
//                done = 1;
//            }
//        }
//
//
//		/* Draw a gray background */
//		SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
//		SDL_RenderClear(renderer);
//
//		draw(window, renderer, sprite);
//
//		/* Update the screen! */
//		SDL_RenderPresent(renderer);
//
//		SDL_Delay(10);
//    }
//
//    exit(0);
//}


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    //配置一个图像缩放的效果,linear效果更平滑,也叫抗锯齿
    //SDL_setenv(SDL_HINT_RENDER_SCALE_QUALITY,"linear",0);
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;
    // 创建一个窗口
    window = SDL_CreateWindow("SDL_RenderClear" , SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_SHOWN);
    // 创建一个渲染器
    renderer = SDL_CreateRenderer(window, -1, 0);
    // 创建一个Surface
    SDL_Surface *bmp = SDL_LoadBMP("sdl.bmp" );
    SDL_Log("after SDL_loadBMP");
    //设置图片中的白色为透明色
    SDL_SetColorKey(bmp, SDL_TRUE, 0xffffff);
    // 创建一个Texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bmp);
    SDL_Log("after SDL_CreateTextureFromSurface");
    //清除所有事件
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    //进入主循环
    while  (1) {
        if  (SDL_PollEvent(&event)) {
            if  (event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_FINGERDOWN)
                break;
        }
        //使用红色填充背景
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        // 将纹理布置到渲染器
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        // 刷新屏幕
        SDL_RenderPresent(renderer);
    }
    // 释放Surface
    SDL_FreeSurface(bmp);
    //  释放Texture
    SDL_DestroyTexture(texture);
    //释放渲染器
    SDL_DestroyRenderer(renderer);
    //释放窗口
    SDL_DestroyWindow(window);
    //延时
    //SDL_Delay(8000);
    //退出
    SDL_Quit();
    return  0;
}