
//
//The following are enum types that are used for textures
//

DXGI_FORMAT_R32G32B32_FLOAT //Each element has three 32 bit floating point components
DXGI_FORMAT_R16G16B16_UNORM //Each element has four 16 bit components mapped to the [0, 1] range
DXGI_FORMAT_R32G32_UINT //Each element has two 32 bit unsigned integer components
DXGI_FORMAT_R8G8B8_UNORM  //Each elment has four 8 bit unsigned components mapped to the [0, 1] range
DXGI_FORMAT_R8G8B8_SNORM //Each element has four 8 bit signed components mapped to the [-1, 1] range
DXGI_FORMAT_R8G8B8_SINT //Each element has four 8 bit signed integer components mapped to [-128, 127] range
DXGI_FORMAT_R8G8B8_UINT //Each element has four 8 bit unsigned integer components mapped to [0, 255] range

//We also have typeless formats where we just reserve the memory and then specify how to reinterpret the data at a later time
DXGI_FORMAT_R8G8B8_TYPELESS

//
//The following are the data types specific to the Depth Buffer texture
//
DXGI_FORMAT_D32_FLOATS8X24_UINT //Specifiesa 32-bit floating point depth buffer, with 8 bits (unsigned int)
//resereved for the stencil buffer mapped to the [0, 255] range and 24 bits not used for padding
DXGI_FORMAT_D32_FLOAT //Specifies a 32 bit floating point depth buffer
DXGI_FORMAT_D24UNORM_S8_UINT //Specifies an unsigned 24 bit depth buffer mapped to the [0, 1] range with 8 bits
// (unsigned int) reserved for stencil buffer mapped to the [0, 255] range
DXGI_FORMAT_D16_UNORM //Specifies an unsigned 16 bit depth buffer mapped to the [0, 1] range

//Applications are not required to used stencil buffers but are always attached to the depth buffer for example
//DXGI_FORMAT_D24_UNORM_S8_UINT uses 24 bits for the depth buffer and 8 bits for the stencil buffer, using the
//stencil buffer is more of an advanced topic to be covered later

//Structure for Multisampling
typedef struct DXGI_SAMPLE_DESC
{
    UINT Count; //The number of samples to take per pixel
    UINT Quality; //Specifies the desired quality level
}DXGI_SAMPLE_DESC, *LPDXGI_SAMPLE_DESC;

//The following method can be used to query the number quality lecel for a given texture format and sample count:
HRESULT ID3D11Device::CheckMultisampleQualityLevels(
    DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels);
//Returns 0 if the fomat and sample count combination is not supported by the device
//Otherwise the number of quality levels for the  given combination will be returned through
//the pNumQualityLevels pointer

//The following defines the max sample count possible for d3dx11 (typically you'll want yours to either be 4 || 8)
#define D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT (32)

//If you don't want to use MSAA, set the sample count to one and quality level to 0

//The following is the concept of feature levels which roughly corresponds to various d3d versions
typedef enum D3D_FEATURE_LEVEL
{
    D3D_FEATURE_LEVEL_9_1 = 0x9100,
    D3D_FEATURE_LEVEL_9_2 = 0x9200,
    D3D_FEATURE_LEVEL_9_3 = 0x9300,
    D3D_FEATURE_LEVEL_10_0 = 0xa000,
    D3D_FEATURE_LEVEL_10_1 = 0xa100,
    D3D_FEATURE_LEVEL_11_0 = 0xb000,
} D3D_FEATURE_LEVEL;


//
//The following is a detailed description of how to create the device and ocntext necessary for dx11
//

//The following two interfaces can be thought of as our software controller of the physical graphics hardware
ID3D11Device //The interface used to check feature support and allocate resources
ID3D11DeviceContext //Used to set render states, bind resources to the graphics pipeline and issue rendering commands

//These interfaces can be created using the following function
HRESULT D3D11CreateDevice(
    IDXGIAdapter *pAdapter, 
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    CONST D3D_FEATURE_LEVEL *pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext
    );

pAdapter //The display adapter we ant to the create device to represent. NULL == primary display adapter, we will always want to use the primary display adapter
DriverType //You will always want to specify D3D_DRIVER_TYPE_HARDWARE for this parameter to use 3D hardware acceleration for rendering, the addtional options are:
/*
  D3D_DRIVER_TYPE_REFERENCE: Creates a reference deivce which allows you to test code your hardware does not support
  Or to test driver ebugs

  D3D_DRIVER_TYPE_SOFTWARE: Creates a software driver used to emulate 3D hardware

  D3D_DRIVER_TYPE_WARP: Creates a high performace d3d 10.1 software driver, WARP (Windows Advanced Rasterization Platform), does not support dx11
 */
Software //Used for supply software driver, we always specify as null we are using hardware for rendering
Flags //Optional device creation flags (that can be ORed together) two common ones are:
/*
  D3D11_CREATE_DEVICE_DEBUG: For debug mode builds, should be set to enable the debug layer, sends debug info to the VC++ output window

  D3D11_CREATE_DEVICE_SINGLETHREADED: Improves performance if you can guarantee that d3d will not be called from multiple threads, if enabled, the CreateDeferredContext method will fail
 */
pFeatureLevels //An array of D3D_FEATURE_LEVEL elements whose order indicates which to test feature level support, specifying null for this parameter indicates to choose the greatest feature level supported
FeatureLevels //The number of D3D_FEATURE_LEVELs in the array pFeatureLevels, 0 == null value for the previous parameter
SDKVersion //Always specify D3D11_SDK_VERSION
ppDevice //Returns the created device
pFeatureLevel //Returns the first supported feature level in pFeaturelevels or greatest feature level supported
ppImmediateContext //Returns the created device context


//
//After creating our devices, we need to create the swap chain which is done by filling out the following structs
//

typedef struct DXGI_SWAP_CHAIN_DESC
{
    DXGI_MODE_DESC BufferDesc;
    DXGI_SAMPLE_DESC SampleDesc;
    DXGI_USAGE BufferUsage;
    UINT BufferCount;
    HWND OutputWindow;
    BOOL Windowed;
    DXGI_SWAP_EFFECT SwapEffect;
    UINT Flags;
} DXGI_SWAP_CHAIN_DESC;

typedef struct DXGI_MODE_DESC
{
    UINT Width; //desired back buffer width;
    UINT Height; //desired back buffer height;
    DXGI_RATIONAL RefreshRate; //display mode refresh rate
    DXGI_FORMAT Format; //back buffer pixel format
    DXGI_MODE_SCANLINE_ORDER ScanlineOrdering; //display scanline mode
    DXGI_MODE_SCALING Scaling; //display scaling mode
} DXGI_MODE_DESC;

BufferDesc //A struct that describes the properties of the back buffer we want to create, mostly just concerened with
//width, height and pixel format.
SampleDesc //The number of multisamples and quality level
BufferUsage //Specify DXGI_USAGE_RENBDER_TARGET_OUTPUT because we are going to be rendering to the back buffer
BufferCount //The number of back buffers to use in the swap chain; we usually only use one back buffer for double buffering though you could use 2 for triple back buffering
OutputWindow //A handle to the window we are rendering into
Windowed //Specify true to run in windowed mode or false for fullscreen mode
SwapEffect //Specify DXGI_SWAP_EFFECT_DISCARD in order to let teh display driver select the most efficient presentation method
Flags //Optional flags, ie: DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH which allows the game to be able to be swapped from fullscreen mode where it will choose a display that best matches the current back buffer settings. If not specified, then when switching to full screen mode, it wil use the current desktop display mode.

//The following is the function required to create our IDXGISwapChain interface

HRESULT IDXGIFactory::CreateSwapChain(
    IUnknown *pDevice, //Pointer to the ID3D11Device
    DXGI_SWAP_CHAIN_DESC *pDesc, //Pointer to swap chain description
    IDXGISwapChain **ppSwapChain); //Returns created swap chain interface
//We obtain a pointer to an IDXGIFactor instance with the CreateIDXGIFactor function
//But before we call our CreateSwapChain function, we have to complete the following COM queries to avoid an error

//The following is an example of how to create a render target view, which is necessary to bind a resource view
//instead of the resource itself to a particular stage of the render pipeline
mSwapChain->GetBuffer(0, //Refers to the index identifying the particular back buffer we want to get
		      __uuidof(ID3D11Texture2D), //The interface type of the buffer, usually always a Texture2D
		      reinterpre_cast<void**>(&backBuffer)); //A pointer to the back buffer

//To actually create the render target view
md3dDevice->CreateRenderTargetView(backBuffer, //Specifies the resource that will be used as the render target (we are creating the render target view to the back buffer)
				   0, //A pointer to a D3D11_RENDER_TARGET_VIEW_DESC (describes the format of the elements among other things)
				   &mRenderTargetView); //A pointer to the created render target view object

//
//The following is the information need to create a depth/stencil buffer starting with the following structx
//

typedef struct D3D11_TEXTURE2D_DESC
{
    UINT Width;
    UINT Height;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
} D3D11_TEXTURE2D_DESC;

Width //The width of the texture in texels
Height //The height of the texture in texels
MipLevels //The number of mipmap levels, while covered later, the depth buffer will only need one
ArraySize //The number of textures in a texture array, for the depth buffer we only need one
Format //A member of the DXGI_FORMAT enumerated type specifying the format of the texels, For a depth buffer, this needs to be one of the formats show previously
SampleDesc //The number of multisamples and quality level, the settings for the depth buffer must match the render target
Usage //D3D11_USAGE TYPE specifying how the texture will be used
/*
  D3D11_USAGE_DEFAULT: Specifying this usage if the GPU will be reading and writing to the resource. The CPU cannot write to a resource with this usage. We want to specify this for the depth buffer because the GPUI will be doing all of the reading and writing to this buffer

  D3D11_USAGE_IMMUTABLE: Specifying this usage if the contents of the resource does not ever change after creation. It allows for potential optimizations as the resource will be read only by the GPU. The CPU and the GPU cannot write to an immutable resource except at creation time.

  D3D11_DYNAMIC: Used if the CPU needs to update the data contents of the resource frequently (per frame basis). A r esource with this usage can be read by  the GPU and written to by the CPU.d

  D3D11_USAGE_STAGING: If the application (CPU) needs to be able to read a copy of the resource. Copying from the GPU to the CPU is a slow operation
 */
BindFlags //One or more flags ORed together specifying where the resource will be b ound to the pipeline for the depth buffer this needs to be D3D11_BIND_DEPTH_STENCIL, but there also exists D3D11_BIND_RENDER_TARGET and D3D11_BIND_SHADER_RESOURCE
CPUAccessFlags //How the CPU will access the resource, if it needs to write: D3D11_CPU_ACCESS_WRITE, without write access must have D3D11_USAGE_DYNAMIC or D3D11_USAGE_STAGING, we also have D3D11_CPU_ACCES_READ
MiscFlags // optional flags which do not apply to the stencil buffer

//The following is the viewport structure which describes the subrectangle the back buffer is drawn onto
typedef struct D3D11_VIEWPORT
{
    FLOAT TopLeftX;
    FLOAT TopLeftY;
    FLOAT Width;
    FLOAT Height;
    FLOAT MinDepth;
    FLOAT MaxDepth;
} D3D11_VIEWPORT;


