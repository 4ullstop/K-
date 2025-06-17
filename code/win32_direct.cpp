#include <windows.h>
#include <directxmath.h>
#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include <dxgiformat.h>
#include <d3d11.h>


typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 bool32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

using namespace DirectX;

#define local_persist static
#define global_variable static
#define internal static


global_variable bool32 running;
global_variable bool32 paused;
global_variable bool32 resizing;
global_variable bool32 minimized;
global_variable bool32 maximized;


namespace Colors
{
    XMGLOBALCONST XMVECTORF32 Blue = {0.0f, 0.0f, 1.0f, 1.0f};
}

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
		char buffer[256];				       \
		sprintf_s(buffer, "Error: HRESULT = 0x%08x\n", hr);    \
		OutputDebugString(buffer);			       \
								       \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

typedef struct window_info
{
    i32 clientWidth;
    i32 clientHeight;
} window_info;

typedef struct d3d_chain_info
{
    IDXGISwapChain* swapChain;
    ID3D11Device* d3dDevice;
    ID3D11DeviceContext* d3dImmediateContext;
    D3D11_VIEWPORT vp;
} d3d_chain_info;

typedef struct d3d_render_view
{
    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView;

    bool32 enable4xMsaa;
    UINT msaaQuality;
} d3d_render_view;

typedef struct timer_info
{
    bool32 stopped;
    i64 stopTime;
    i64 baseTime;
    i64 prevTime;
    i64 currTime;
    i64 pausedTime;
    i64 countsPerSec;

    r64 secondsPerCount;
    r32 deltaTime;
} timer_info;

#define ReleaseCOM(x) {if (x) {x->Release(); x = 0;} }

#if DIRECT_SLOW
#define Assert(Expression) if(!(Expression)) *(int*)0 = 0;
#else
#define Assert(Expression)
#endif

internal r32
TotalTime(timer_info* timer)
{
    r32 result = 0;
    if (timer->stopped)
    {
	result = (r32)(((timer->stopTime - timer->pausedTime) - timer->baseTime) * timer->secondsPerCount);
    }
    else
    {
	result = (r32)(((timer->currTime - timer->pausedTime) - timer->baseTime) * timer->secondsPerCount);
    }
    return(result);
}

internal void
StartGameTimer(timer_info* timer)
{
    i64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

    if (timer->stopped)
    {
	timer->pausedTime += (startTime - timer->stopTime);
	timer->prevTime = startTime;
	timer->stopTime = 0;
	timer->stopped = false;
    }
}

internal void
StopGameTimer(timer_info* timer)
{

    if (!timer->stopped)
    {
	i64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	timer->stopTime = currTime;
	timer->stopped = true;
    }
}

internal void
OnResize(d3d_chain_info* chainInfo, d3d_render_view* renderViewInfo, i32 newWidth, i32 newHeight)
{
    Assert(chainInfo->d3dImmediateContext);
    Assert(chainInfo->d3dDevice);
    Assert(chainInfo->swapChain);

    //Release the old views as they hold references to buffers we will be destroying
    ReleaseCOM(renderViewInfo->renderTargetView);
    ReleaseCOM(renderViewInfo->depthStencilView);
    ReleaseCOM(renderViewInfo->depthStencilBuffer);

    HR(chainInfo->swapChain->ResizeBuffers(1, newWidth, newHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    ID3D11Texture2D* backBuffer;
    HR(chainInfo->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
    HR(chainInfo->d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderViewInfo->renderTargetView));
    ReleaseCOM(backBuffer);

    D3D11_TEXTURE2D_DESC depthStencilDesc = {};

    depthStencilDesc.Width = newWidth;
    depthStencilDesc.Height = newHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //Check if msaa is enabled
    if (renderViewInfo->enable4xMsaa)
    {
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = renderViewInfo->msaaQuality - 1;
    }
    else
    {
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = 0;
    }

    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    HR(chainInfo->d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &renderViewInfo->depthStencilBuffer));
    HR(chainInfo->d3dDevice->CreateDepthStencilView(renderViewInfo->depthStencilBuffer, 0, &renderViewInfo->depthStencilView));

    chainInfo->d3dImmediateContext->OMSetRenderTargets(1, &renderViewInfo->renderTargetView, renderViewInfo->depthStencilView);

    chainInfo->vp.TopLeftX = 0;
    chainInfo->vp.TopLeftY = 0;
    chainInfo->vp.Width = (float)newWidth;
    chainInfo->vp.Height = (float)newHeight;
    chainInfo->vp.MinDepth = 0.0f;
    chainInfo->vp.MaxDepth = 1.0f;

    chainInfo->d3dImmediateContext->RSSetViewports(1, &chainInfo->vp);
    
}

LRESULT CALLBACK Win32MainWindowProc(HWND hwnd,
				     UINT uMsg,
				     WPARAM wParam,
				     LPARAM lParam)
{
    LRESULT result = 0;
    switch(uMsg)
    {
    case WM_QUIT:
    {
	
    } break;
    default:
    {
	result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    } break;
    }
    return result;
}

internal bool32
Win32ProcessPendingMessages(d3d_chain_info* chainInfo, d3d_render_view* renderInfo, i32* clientWidth, i32* clientHeight, timer_info* timer)
{
    MSG message;
    bool32 messageFound = false;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
	messageFound = true;
	switch(message.message)
	{
	case WM_ACTIVATE:
	{
	    if ((message.wParam & 0xffff) == WA_INACTIVE)
	    {
		paused = true;
		//Stop the timer here
		StopGameTimer(timer);
	    }
	    else
	    {
		paused = false;
		//Start the timer here
		StopGameTimer(timer);
	    }
	} break;
	case WM_SIZE:
	{
	    *clientWidth = (i32)(message.lParam & 0xffff);
	    *clientHeight = (i32)((message.lParam >> 16) & 0xffff);
	    if (chainInfo->d3dDevice)
	    {
		if (message.wParam == SIZE_MINIMIZED)
		{
		    paused = true;
		    minimized = true;
		    maximized = false;
		}
		else if (message.wParam == SIZE_MAXIMIZED)
		{
		    paused = false;
		    minimized = false;
		    maximized = true;
		}
		else if (message.wParam == SIZE_RESTORED)
		{
		    if (minimized)
		    {
			paused = false;
			minimized = false;
			OnResize(chainInfo, renderInfo, *clientWidth, *clientHeight);
		    }
		    else if (maximized)
		    {
			paused = false;
			maximized = false;
			OnResize(chainInfo, renderInfo, *clientWidth, *clientHeight);			
		    }
		    else if (resizing)
		    {
			
		    }
		    else
		    {
			OnResize(chainInfo, renderInfo, *clientWidth, *clientHeight);			
		    }
		}
	    }
	} break;
	case WM_ENTERSIZEMOVE:
	{
	    paused = true;
	    resizing = true;
	    //stop the timer here
	    StopGameTimer(timer);
	} break;
	case WM_EXITSIZEMOVE:
	{
	    paused = false;
	    resizing = false;
	} break;
	case WM_GETMINMAXINFO:
	{
	    ((MINMAXINFO*)message.lParam)->ptMinTrackSize.x = 200;
	    ((MINMAXINFO*)message.lParam)->ptMinTrackSize.y = 200;
	} break;
	case WM_DESTROY:
	case WM_QUIT:
	{
	    running = false;
	} break;
	case WM_MENUCHAR:
	{

	} break;
	
	default:
	{
	    TranslateMessage(&message);
	    DispatchMessage(&message);
	} break;
	}
    }
    return(messageFound);
}

int CALLBACK WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow)
{
    UINT createDeviceFlags = 0;
#if defined (DEBUG) || defined (_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif



    //Initialize the Game Timer
    paused = false;

    timer_info timer = {};

    i64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    timer.secondsPerCount = 1.0 / (r64)countsPerSec;



    //This is the 'reset' function which if you see necessary to create, then create it
    i64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    timer.baseTime = currTime;
    timer.prevTime = currTime;
    timer.stopTime = 0;

    
    MSG msg = {};

    //Init our window
    WNDCLASS windowClass = {};
    windowClass.style = CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = Win32MainWindowProc; //Make this
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    //Mess around with this 
    windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    windowClass.lpszClassName = "D3DWndClassname";

    if (RegisterClass(&windowClass))
    {
	window_info windowInfo = {};
	windowInfo.clientWidth = 800;
	windowInfo.clientHeight = 400;
	
	HWND windowHandle = CreateWindowEx(
	    0,
	    windowClass.lpszClassName,
	    "D3D Testing",
	    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    windowInfo.clientWidth,
	    windowInfo.clientHeight,
	    0,
	    0,
	    hInstance,
	    0);
	if (windowHandle)
	{
	    D3D_FEATURE_LEVEL featureLevels[2] =
		{
		    D3D_FEATURE_LEVEL_11_0,
		    D3D_FEATURE_LEVEL_10_1
		};

	    d3d_chain_info chainInfo = {};
	    d3d_render_view renderInfo = {};
	    
	    D3D_FEATURE_LEVEL featureLevel;


	    HRESULT hresult = D3D11CreateDevice(
		0, //default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&chainInfo.d3dDevice,
		&featureLevel,
		&chainInfo.d3dImmediateContext);

	    if (FAILED(hresult))
	    {
		MessageBox(0, (LPCSTR)"D3D11CreateDevice failed.", 0, 0);
		return false;
	    }

	    if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	    {
		MessageBox(0, (LPCSTR)"Direct3D feature level 11 unsupported.", 0, 0);
		return false;
	    }

	    renderInfo.enable4xMsaa = true;

	    //Now we check the support for 4X MSAA quality

	    HR(chainInfo.d3dDevice->CheckMultisampleQualityLevels(
		   DXGI_FORMAT_R8G8B8A8_UNORM, 4, &renderInfo.msaaQuality));

	    Assert(renderInfo.msaaQuality > 0);

	    //Next we fill out some info necessary to create the swap chain
	    DXGI_SWAP_CHAIN_DESC sd;
	    sd.BufferDesc.Width = windowInfo.clientWidth;
	    sd.BufferDesc.Height = windowInfo.clientHeight;
	    sd.BufferDesc.RefreshRate.Numerator = 60;
	    sd.BufferDesc.RefreshRate.Denominator = 1;
	    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	    if (renderInfo.enable4xMsaa)
	    {
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = renderInfo.msaaQuality - 1;
	    }
	    else
	    {
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	    }
	    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	    sd.BufferCount = 1;
	    sd.OutputWindow = windowHandle;
	    sd.Windowed = true;
	    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	    sd.Flags = 0;

	    //
	    //Creating the swap chain buffer
	    //

	    //Complete the following COM queries before we can call our CreateSwapChain function

	    IDXGIDevice* dxgiDevice = 0;
	    HR(chainInfo.d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	    IDXGIAdapter* dxgiAdapter = 0;
	    HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	    IDXGIFactory* dxgiFactory = 0;
	    HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));


	    HR(dxgiFactory->CreateSwapChain(chainInfo.d3dDevice, &sd, &chainInfo.swapChain));

	    //Release our acquired COM interfaces2
	    ReleaseCOM(dxgiDevice);
	    ReleaseCOM(dxgiAdapter);
	    ReleaseCOM(dxgiFactory);


	    ID3D11Texture2D* backBuffer;
	    chainInfo.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				  reinterpret_cast<void**>(&backBuffer));
	    chainInfo.d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderInfo.renderTargetView);
	    ReleaseCOM(backBuffer);

	    //Now to create the depth/stencil buffer

	    D3D11_TEXTURE2D_DESC depthDesc;
	    depthDesc.Width = windowInfo.clientWidth;
	    depthDesc.Height = windowInfo.clientHeight;
	    depthDesc.MipLevels = 1;
	    depthDesc.ArraySize = 1;
	    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	    //If using 4x msaa we must match the values to the swap chain
	    if (renderInfo.enable4xMsaa)
	    {
		depthDesc.SampleDesc.Count = 4;
		depthDesc.SampleDesc.Quality = renderInfo.msaaQuality - 1;
	    }
	    else
	    {
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;	
	    }

	    depthDesc.Usage = D3D11_USAGE_DEFAULT;
	    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	    depthDesc.CPUAccessFlags = 0;
	    depthDesc.MiscFlags = 0;

	    //And now we have to bind the resource to the pipeline stage



	    HR(chainInfo.d3dDevice->CreateTexture2D(
		   &depthDesc, //Description of the texture
		   0, 
		   &renderInfo.depthStencilBuffer)); //Return pointer to the depth stencil buffer

	    HR(chainInfo.d3dDevice->CreateDepthStencilView(
		   renderInfo.depthStencilBuffer,//Resource we want to create a view to
		   0,
		   &renderInfo.depthStencilView)); //Return depth/stencil view

	    //Now that we have created views to the back and depth buffers, we can bind these views to the output merger stage of the pipeline
	    chainInfo.d3dImmediateContext->OMSetRenderTargets(1, //The number of targets we are binding to
						     &renderInfo.renderTargetView, //A pointer to the first element in an array of render target views
						     renderInfo.depthStencilView); //A pointer to the depth/stencil view to bind to the pipeline

	    //Now to set the viewport

	    chainInfo.vp.TopLeftX = 0.0f;
	    chainInfo.vp.TopLeftY = 0.0f;
	    chainInfo.vp.Width = (float)windowInfo.clientWidth;
	    chainInfo.vp.Height = (float)windowInfo.clientHeight;
	    chainInfo.vp.MinDepth = 0.0f;
	    chainInfo.vp.MaxDepth = 1.0f;

	    chainInfo.d3dImmediateContext->RSSetViewports(1, &chainInfo.vp);

	    i32 frameCount = 0;
	    r32 timeElapsed = 0.0f;
	    
	    running = true;
	    while (running)
	    {

		//The order is
		/*
		  Calculate frame stats
		  Update the scene
		  Draw the scene
		 */
		bool32 messageFound = Win32ProcessPendingMessages(&chainInfo,
								  &renderInfo,
								  &windowInfo.clientWidth,
								  &windowInfo.clientHeight,
								  &timer);
		
		//Complete time tick stuff
		if (!messageFound)
		{
		    if (paused)
		    {
			timer.deltaTime = 0.0;
		    }
		    else
		    {
			//This is code that is technically instilled in a 'Tick' function which, if you feel the need to do could do at some point

			i64 tickCurrTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&tickCurrTime);

			timer.currTime = tickCurrTime;
			//Compute delta time 
			timer.deltaTime = (r32)((timer.currTime - timer.prevTime)*timer.secondsPerCount);
			timer.prevTime = timer.currTime;

			if (timer.deltaTime < 0.0)
			{
			    timer.deltaTime = 0.0;
			}
		    }

		    if (!paused)
		    {
			//Calcuate frame data
			frameCount++;

			r32 totalTime = TotalTime(&timer);
			if ((totalTime - timeElapsed) >= 1.0f)
			{
			    r32 fps = (r32)frameCount;
			    r32 mspf = 1000.0f / fps;

			    char buffer[256];
			    sprintf_s(buffer, "FPS: %f, MSPF: %f", fps, mspf);
			    SetWindowText(windowHandle, buffer);

			    frameCount = 0;
			    timeElapsed += 1.0f;
			}
			//Update Scene
			Assert(chainInfo.d3dImmediateContext);
			Assert(chainInfo.swapChain);

			//Clear the back buffer to blue
			chainInfo.d3dImmediateContext->ClearRenderTargetView(
			    renderInfo.renderTargetView,
			    (r32*)(&Colors::Blue));

			//Clear the depth buffer to 1.0 and the stencil value to 0
			chainInfo.d3dImmediateContext->ClearDepthStencilView(
			    renderInfo.depthStencilView,
			    D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

			//Finally present the back buffer to the screen
			HR(chainInfo.swapChain->Present(0, 0));

			//Draw Scene
		    }
		    else
		    {
			Sleep(100);
		    }
		}
	    }
	}
    }
    return(0);
}
