#include <windows.h>
#include <string>

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 设置透明背景
            SetBkMode(hdc, TRANSPARENT);
            
            // 创建微软雅黑字体
            HFONT hFont = CreateFont(
                40,                     // 字体高度
                0,                      // 字体宽度
                0,                      // 文本倾斜度
                0,                      // 文本倾斜度
                FW_BOLD,                // 字体粗细
                FALSE,                  // 是否使用斜体
                FALSE,                  // 是否使用下划线
                FALSE,                  // 是否使用删除线
                DEFAULT_CHARSET,        // 字符集
                OUT_OUTLINE_PRECIS,     // 输出精度
                CLIP_DEFAULT_PRECIS,    // 裁剪精度
                CLEARTYPE_QUALITY,      // 输出质量
                DEFAULT_PITCH | FF_DONTCARE, // 间距和族
                TEXT("微软雅黑")        // 字体名称
            );
            
            // 选择字体
            auto hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));
            
            // 获取窗口客户区大小
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            // 设置文本颜色
            SetTextColor(hdc, RGB(0, 0, 0));
            
            // 在窗口中央绘制文本
            TCHAR text[] = TEXT("Shit");
            DrawText(hdc, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            
            // 恢复旧字体并删除创建的字体
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// 使用正确的Unicode入口点函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 注册窗口类
    constexpr TCHAR CLASS_NAME[] = TEXT("ShitWindowClass");
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    // 使用标准箭头光标
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    
    RegisterClass(&wc);
    
    // 计算窗口大小，使客户区为300x100
    RECT rect = {0, 0, 300, 100};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), FALSE);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    // 创建窗口
    HWND hwnd = CreateWindow(
        CLASS_NAME,                 // 窗口类名
        TEXT("Shit"),               // 窗口标题
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), // 窗口样式
        
        // 窗口位置和大小
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        
        nullptr,       // 父窗口
        nullptr,       // 菜单
        hInstance,     // 实例句柄
        nullptr        // 附加数据
    );
    
    if (hwnd == nullptr) {
        return 1;  // 返回错误代码
    }
    
    // 显示窗口
    ShowWindow(hwnd, nCmdShow);
    
    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return static_cast<int>(msg.wParam);  // 返回退出代码
}