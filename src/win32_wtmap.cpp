#include "wtmap.h" //doesn't strictly need this, but if I move do a dll thing, I'd need this
#include "wtmap.cpp"

#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

typedef struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
} win32_offscreen_buffer;

typedef struct win32_state
{
    bool ConsoleVisible;
    HWND MainWindow;
    HWND Console;
    
    
    uint64 TotalSize;
    void* GameMemoryBlock;
} win32_state;

typedef struct win32_window_dimension {
    int Width;
    int Height;
} win32_window_dimension;

bool GlobalRunning;
win32_offscreen_buffer GlobalBackbuffer;

LARGE_INTEGER GetCPUTime()
{
    LARGE_INTEGER Res;
    QueryPerformanceCounter(&Res);
    return Res;
}

HWND FindConsole()
{
    return FindWindowA("ConsoleWindowClass", 0);
}

HWND CreateConsole()
{
    HWND Console;
    long StandardOut;
    FILE *fp;

    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    Console = FindConsole();
    ShowWindow(Console, 0);
    
    return Console;
}

char* ConsoleInputMutexName = "ConsoleInputMutex";
bool NewInput;
#define CONSOLE_INPUT_MAX 512
char ConsoleInput[CONSOLE_INPUT_MAX];

bool CheckConsoleInput(LPVOID OutBuffer) {
    if (NewInput)
    {
        HANDLE mut = OpenMutex(MUTEX_ALL_ACCESS, 0, ConsoleInputMutexName);
        memcpy(OutBuffer, ConsoleInput, CONSOLE_INPUT_MAX);
        memset(ConsoleInput, 0, CONSOLE_INPUT_MAX);
        NewInput = false;
        ReleaseMutex(mut);
        return true;
    }
    else
    {
        return false;
    }
}

DWORD WINAPI Win32AsyncReadFromConsole(void* ThreadInput) {

    HANDLE hConHandle = CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    DWORD CharsToRead = CONSOLE_INPUT_MAX;
    DWORD CharsRead;
    while(true) {
        char ConsoleInputTempBuffer[CONSOLE_INPUT_MAX];
        ReadConsole(hConHandle, &ConsoleInputTempBuffer, CharsToRead, &CharsRead, 0);
        HANDLE mut = OpenMutex(MUTEX_ALL_ACCESS, 0, ConsoleInputMutexName);

        //remove endline because that's how I signal I'm done entering things? let's check it out
        ConsoleInputTempBuffer[CharsRead-2] = '\0';
        memcpy(ConsoleInput, ConsoleInputTempBuffer, CharsRead);
        NewInput = true;
        ReleaseMutex(mut);
    }
}

void ShowConsole(win32_state* State, bool Show)
{
    if (!State->Console)
    {
        State->Console = FindConsole();
        RECT MainWindowRect;
        GetWindowRect(State->MainWindow, &MainWindowRect);
        RECT ConsoleRect;
        GetWindowRect(State->Console, &ConsoleRect);
        MoveWindow(State->Console,
                   MainWindowRect.left,
                   MainWindowRect.bottom,
                   ConsoleRect.right - ConsoleRect.left,
                   ConsoleRect.bottom - ConsoleRect.top,
                   1);

    }
    
    State->ConsoleVisible = Show;
    ShowWindow(State->Console, Show);

    SetActiveWindow(State->MainWindow);
}

void ToggleConsole(win32_state* State)
{
    ShowConsole(State, !State->ConsoleVisible);
}

void FocusConsole(win32_state* State)
{
    SetActiveWindow(State->Console);
    SetFocus(State->Console);
}


win32_window_dimension GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

int64 PerfCountFrequency;
float
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	float Result = ((float)(End.QuadPart - Start.QuadPart) / 
					 (float)PerfCountFrequency);
	return Result;
}

void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
                           HDC DeviceContext,
                           int WindowWidth,
                           int WindowHeight)
{
    StretchDIBits(
        DeviceContext,
        0,0,Buffer->Width, Buffer->Height,
        0,0,Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK MainWindowCallback(HWND Window,
                                    UINT Message,
                                    WPARAM WParam,
                                    LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
        } break;
        case WM_CLOSE:
        {
            GlobalRunning = false;
            PostQuitMessage(0);
        } break;	
        case WM_SETCURSOR:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
            /*SetCursor(0)*/
            
        } break;
        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //Assert(true);
        } break;
        case WM_ACTIVATEAPP:
        {
            if (WParam == TRUE)
            {
                SetLayeredWindowAttributes(Window, RGB(0,0,0), 255, LWA_ALPHA);
            }
            else
            {
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 128, LWA_ALPHA);
            }
        } break;
	
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, Dimension.Width, Dimension.Height);
            EndPaint(Window, &Paint);
        } break;
	
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
            break;
        } break;
    }
    return Result;
}

void
Win32ProcessKeyboardMessage(game_button_state *ButtonState, bool DownState)
{
    ButtonState->Down = DownState;
}

void
Win32ProcessPendingMessages(win32_state *State, game_controller *KeyboardController)
{			
	MSG Message;
	while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		if(Message.message == WM_QUIT)
		{
			GlobalRunning = false;
		}
		switch(Message.message)
		{
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				uint32 VKCode = (uint32)Message.wParam;
				bool WasDown = ((Message.lParam & (1 << 30)) != 0);
				bool IsDown = ((Message.lParam & (1 << 31)) == 0);

				if(WasDown != IsDown)
				{
					if(VKCode == 'W')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->MoveUp, IsDown);
					}
					else if(VKCode == 'A')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->MoveLeft, IsDown);
					}
					else if(VKCode == 'S')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->MoveDown, IsDown);
					}
					else if(VKCode == 'D')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->MoveRight, IsDown);
					}
					else if(VKCode == 'Q')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->LeftShoulder, IsDown);
					}
					else if(VKCode == 'E')
					{
						Win32ProcessKeyboardMessage(&KeyboardController->RightShoulder, IsDown);
					}
					else if(VKCode == VK_UP)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionUp, IsDown);
					}
					else if(VKCode == VK_LEFT)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionLeft, IsDown);
					}
					else if(VKCode == VK_DOWN)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionDown, IsDown);
					}
					else if(VKCode == VK_RIGHT)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->ActionRight, IsDown);
					}\
					else if(VKCode == VK_ESCAPE)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Back, IsDown);
					}
					else if(VKCode == VK_SPACE)
					{
						Win32ProcessKeyboardMessage(&KeyboardController->Start, IsDown);
					}
					if (IsDown)
					{
						bool AltIsDown = (Message.lParam & (1 << 29));
						if(VKCode == VK_F4 && AltIsDown)
						{
							GlobalRunning = false;
						}
                        
                        if (VKCode == VK_F12)
                        {
                            ToggleConsole(State);
                            if (AltIsDown)
                            {
                                //TODO: fix this, it doesn't do anything
                                FocusConsole(State);
                            }
                        }
					}
				}
			} break;
			default:
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			} break;
		}
	}
}

int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{
    win32_state State = {};
    HWND Console = CreateConsole();
    CreateMutex(0, 0, ConsoleInputMutexName);

    DWORD ConsoleReadThreadID;
    CreateThread(NULL,
                 0,
                 Win32AsyncReadFromConsole,
                 0,
                 0,
                 &ConsoleReadThreadID);
    
    LARGE_INTEGER PerfCountFreqRes;
    QueryPerformanceFrequency(&PerfCountFreqRes);
    PerfCountFrequency = PerfCountFreqRes.QuadPart;
    
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName="WTMapWindowClass";
            
    win32_offscreen_buffer* Buffer = &GlobalBackbuffer;

    int ScreenWidth = 1080;
    int ScreenHeight = 608;

    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 8, MEM_RELEASE);
    }
    Buffer->Width = ScreenWidth;
    Buffer->Height = ScreenHeight;
	
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->BytesPerPixel = 4;

    int BitmapMemorySize = (ScreenWidth*ScreenHeight)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Buffer->Width*Buffer->BytesPerPixel;
    
    if (!RegisterClassA(&WindowClass))
    {
        //don't have a console but hey whatever
        OutputDebugStringA("failed to register window class");
        return 0;
    }
    
    int WindowWidth = 1080;
    int WindowHeight = 608;
    HWND WindowHandle = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        "WTMap",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WindowWidth,
        WindowHeight,
        0,
        0,
        Instance,
        0);
    State.MainWindow = WindowHandle;
    SetActiveWindow(State.MainWindow);
    
    if (!WindowHandle)
    {
        DWORD LastError = GetLastError();
        OutputDebugStringA("failed to get window handle: ");

        char buff[256];
        sprintf(buff, "%d", LastError);
        OutputDebugString(buff);
        OutputDebugStringA("\n");
        
        return 0;
    }
    int HardRefreshHz = 60;

    float GameUpdateHz = (HardRefreshHz/2.0f);
    float TargetFrameSeconds = 1.0f/GameUpdateHz;

    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = Megabytes(64);
    GameMemory.TransientStorageSize = Megabytes(256);

    State.TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
    State.GameMemoryBlock = VirtualAlloc(0,
                                         (size_t)State.TotalSize,
                                         MEM_RESERVE|MEM_COMMIT,
                                         PAGE_READWRITE);

    GameMemory.PermanentStorage = State.GameMemoryBlock;
    GameMemory.TransientStorage = (uint8 *)GameMemory.PermanentStorage +
        GameMemory.PermanentStorageSize;

    if (!(GameMemory.PermanentStorage || GameMemory.TransientStorage))
    {
        OutputDebugStringA("failed to allocate memory");
        return 0;
    }

    game_input Inputs[2];
    Inputs[0] = {0};
    Inputs[1] = {0};
    game_input *NewInput = &Inputs[0];
    game_input *LastInput = &Inputs[1];

    LARGE_INTEGER LastCounter = GetCPUTime();

    GlobalRunning = true;
    while(GlobalRunning)
    {
        //prework
        NewInput->dt = TargetFrameSeconds;

        game_controller *OldKeyboard = &LastInput->Keyboard;
        game_controller *NewKeyboard = &NewInput->Keyboard;
        
        for(int ButtonIndex = 0;
            ButtonIndex < ArrayCount(NewKeyboard->Buttons);
            ++ButtonIndex)
        {
            NewKeyboard->Buttons[ButtonIndex].Down =
                OldKeyboard->Buttons[ButtonIndex].Down;
        }

        Win32ProcessPendingMessages(&State,  NewKeyboard);

        game_screen_buffer Buffer = {};
        Buffer.Memory = GlobalBackbuffer.Memory;
        Buffer.Width = GlobalBackbuffer.Width;
        Buffer.Height = GlobalBackbuffer.Height;
        Buffer.Pitch = GlobalBackbuffer.Pitch;
        Buffer.BytesPerPixel = GlobalBackbuffer.BytesPerPixel;

        char ConsoleBuffer[512];
        if (CheckConsoleInput(&ConsoleBuffer))
        {
            if (strcmp(ConsoleBuffer, "close") == 0)
            {
                ToggleConsole(&State);
            }
            else
            {
                ProcessConsoleInput(NewInput, &GameMemory, &Buffer, ConsoleBuffer);
            }
        }
        UpdateAndRender(NewInput, &GameMemory, &Buffer);
        //post work
        LARGE_INTEGER TimeNow = GetCPUTime();
        float FrameSecondsElapsed = Win32GetSecondsElapsed(LastCounter, TimeNow);

        if (FrameSecondsElapsed < TargetFrameSeconds)
        {
            double TargetActualDiff = 1000.0f*(TargetFrameSeconds - FrameSecondsElapsed);
            Sleep(TargetActualDiff);
        }
        
        LastCounter = TimeNow;
        
        win32_window_dimension Dimension = GetWindowDimension(WindowHandle);
						
        HDC DeviceContext = GetDC(WindowHandle);
        Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext, Dimension.Width, Dimension.Height);
        ReleaseDC(WindowHandle, DeviceContext);

        game_input *Temp = NewInput;
        NewInput = LastInput;
        LastInput = Temp;
    }

    return 0;
}
