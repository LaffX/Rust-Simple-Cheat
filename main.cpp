#include "driver.h"
#include "main.h"
#include "offsets.h"

using namespace std;

bool NoRecoilCheckbox;
bool PlayerEspCheckbox;
bool FatBulletCheckbox;
bool NoSpreadCheckbox;
bool AutomaticCheckbox;
bool AlwaysDayCheckbox;
bool SpidermanCheckbox;
bool HardcoreMapCheckbox;
bool hidescript1;
bool hidescript2 = true;

UINT64 GetHeldItem(UINT64 LocalPlayer)
{
    const auto base_player = LocalPlayer;
    const auto active_item_id = read<UINT64>(base_player + oActiveUID);
    const auto player_inventory = read<UINT64>(base_player + oPlayerInventory);
    const auto inventory_belt = read<UINT64>(player_inventory + oContainerBelt);
    const auto belt_contents_list = read<UINT64>(inventory_belt + oBeltContentList);
    const auto belt_contents_sz = read<uint32_t>(belt_contents_list + 0x14);
    const auto belt_contents = read<UINT64>(belt_contents_list + 0x10);
    for (auto i = 0u; i <= 6; i++)
    {
        if (belt_contents != 0)
        {
            const auto current_item = read<UINT64>(belt_contents + (0x20 + (i * 8)));
            if (!current_item) continue;
            if (active_item_id == read<uint32_t>(current_item + 0x28)) return current_item;
        }
    }
}
std::wstring get_item_name(uintptr_t item)
{
    auto unk = read<UINT64>(item + 0x20);
    unk = read<UINT64>(unk + 0x20);
    return read_wstring(unk + 0x14, 14);
}
bool is_weapon(uintptr_t helditem)
{
    auto ItemName = get_item_name(helditem);
    if (ItemName.find(L"rifle") != std::string::npos) return true;
    if (ItemName.find(L"pistol") != std::string::npos) return true;
    if (ItemName.find(L"bow") != std::string::npos) return true;
    if (ItemName.find(L"lmg") != std::string::npos) return true;
    if (ItemName.find(L"shotgun") != std::string::npos) return true;
    if (ItemName.find(L"smg") != std::string::npos) return true;
    else return false;
}

void cheat()
{
    while (true)
    {
        auto main_offsets = read<uintptr_t>(assemblybase + BaseGameMode);
        Local_Player = read_chain<uintptr_t>(main_offsets, { 0xb8, 0x20, 0x10, 0x28, 0x18, 0x20 });
        auto Movement = read<uintptr_t>(Local_Player + oPlayerMovement);
        auto OcclusionCulling = read<uintptr_t>(assemblybase + OclusionCulling);
        auto CullingDebugOnOff = read<uintptr_t>(OcclusionCulling + 0xB8);
        auto OcclusionCullingEditted = read_chain<uintptr_t>(OcclusionCulling, { 0xB8, 128, 0x18 });
        auto ConvarCulling = read<uintptr_t>(assemblybase + 52992632); // 	"Signature": "ConVar_Culling_c*"
        auto ConvarCulling2 = read<uintptr_t>(ConvarCulling + 0xB8);
        auto klass = read<uintptr_t>(assemblybase + 52982424); // convar_admin_c*
        auto static_fields = read<uintptr_t>(klass + 0xb8);
        const auto current_item = GetHeldItem(Local_Player);
        auto base_projectile = read<uintptr_t>(current_item + oHeld);
        auto recoil_prop = read<uintptr_t>(base_projectile + oRecoilProperties);
        auto newRecoilOverride = read<uintptr_t>(recoil_prop + 0x78);
        bool isWeapon = is_weapon(current_item);
        if (PlayerEspCheckbox)
        {
            write<float>(ConvarCulling2 + 0x0, 1000.f); // culling.entityupdaterate 1000 komutu
            write<uintptr_t>(OcclusionCullingEditted + 0x20, 131072);
            write<uintptr_t>(CullingDebugOnOff + 0x94, 1); // culling.debug 1 komutu
        }
        else
        {
            write<uintptr_t>(CullingDebugOnOff + 0x94, 0);// culling.debug 0 komutu
        }
        if (AlwaysDayCheckbox)
        {
            write<float>(static_fields + 0x0, 12); // public static float admintime;
        }
        if (SpidermanCheckbox)
        {
            write<float>(Movement + 0xC4, 0.f);
            write<float>(Movement + 0xC8, 0.f);
        }
        if (HardcoreMapCheckbox)
        {
            auto HardCoreMap = read<uintptr_t>(assemblybase + 52985192); //       "Signature": "BaseGameMode_c*"	
            auto HardCoreMapOn = read_chain<uintptr_t>(HardCoreMap, { 0xb8, 0x8 });
            write<bool>(HardCoreMapOn + 0x17C, true); // public bool ingameMap;
            write<bool>(HardCoreMapOn + 0x17D, true); // public bool compass;
        }
        if (isWeapon) {
            if (NoRecoilCheckbox) {
                if (newRecoilOverride) {
                    if (read<int>(newRecoilOverride + oRecoilYawMin) != 0) {
                        write<int>(newRecoilOverride + oRecoilYawMin, 0);
                        write<int>(newRecoilOverride + oRecoilYawMax, 0);
                        write<int>(newRecoilOverride + oRecoilPitchMin, 0);
                        write<int>(newRecoilOverride + oRecoilPitchMax, 0);
                    }
                }
                if (!newRecoilOverride) {
                    if (read<int>(recoil_prop + oRecoilYawMin) != 0) {
                        write<int>(recoil_prop + oRecoilYawMin, 0);
                        write<int>(recoil_prop + oRecoilYawMax, 0);
                        write<int>(recoil_prop + oRecoilPitchMin, 0);
                        write<int>(recoil_prop + oRecoilPitchMax, 0);
                    }
                }
            }
            if (FatBulletCheckbox)
            {

                auto List = read<uintptr_t>(base_projectile + 0x370); //private List<Projectile> createdProjectiles;
                int size = read<uintptr_t>(List + 0x18);
                List = read<uintptr_t>(List + 0x10);
                for (int i = 0; i < size; ++i)
                {
                    auto Item = read<uintptr_t>(List + 0x20 + (i * 0x8));
                    write<float>(Item + 0x2C, 0.7f);
                }
            }
            if (NoSpreadCheckbox)
            {

                if (newRecoilOverride)
                {
                    if (read<int>(newRecoilOverride + oAimconeCurveScale) != -1) {
                        write<int>(newRecoilOverride + oAimconeCurveScale, -1);
                        write<int>(newRecoilOverride + oStancePenalty, -1);
                        write<int>(newRecoilOverride + oAimCone, -1);
                        write<int>(newRecoilOverride + oHipAimCone, -1);
                        write<int>(newRecoilOverride + oAimconePenaltyPerShot, -1);
                    }
                }
                if (!newRecoilOverride)
                {
                    if (read<int>(recoil_prop + oAimconeCurveScale) != -1) {
                        write<int>(recoil_prop + oAimconeCurveScale, -1);
                        write<int>(recoil_prop + oStancePenalty, -1);
                        write<int>(recoil_prop + oAimCone, -1);
                        write<int>(recoil_prop + oHipAimCone, -1);
                        write<int>(recoil_prop + oAimconePenaltyPerShot, -1);
                    }
                }
            }
            if (AutomaticCheckbox)
            {
                if (read<bool>(base_projectile + oAutomatic) != AutomaticCheckbox) {
                    write<bool>(base_projectile + oAutomatic, AutomaticCheckbox);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

IDirect3DTexture9* Logo;
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);
    D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, logooooooooo, sizeof(logooooooooo), 100, 100, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &Logo);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoLight_compressed_data_base85, 13.5F);
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        void Theme(); {
            ImGui::GetStyle().FrameRounding = 4.0f;
            ImGui::GetStyle().GrabRounding = 4.0f;
            ImGuiStyle& style = ImGui::GetStyle();
            style.Alpha = 1.0f;
            style.WindowPadding = ImVec2(0, 0);
            style.WindowMinSize = ImVec2(32, 32);
            style.WindowRounding = 3.0f;
            style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
            style.FramePadding = ImVec2(4, 3);
            style.FrameRounding = 1.0f;
            style.ChildRounding = 5.0f;
            style.FrameBorderSize = 0.5f;
            style.ItemSpacing = ImVec2(8, 8);
            style.ItemInnerSpacing = ImVec2(8, 8);
            style.TouchExtraPadding = ImVec2(0, 0);
            style.IndentSpacing = 21.0f;
            style.ColumnsMinSpacing = 0.0f;
            style.ScrollbarSize = 6.0f;
            style.ScrollbarRounding = 0.0f;
            style.GrabMinSize = 5.0f;
            style.GrabRounding = 0.0f;
            style.DisplayWindowPadding = ImVec2(22, 22);
            style.DisplaySafeAreaPadding = ImVec2(4, 4);
            style.AntiAliasedLines = true;
            style.CurveTessellationTol = 1.f;
            ImVec4* colors = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text] = ImVec4(.6f, .6f, .6f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
            colors[ImGuiCol_WindowBg] = ImColor(21, 21, 21, 160);
            colors[ImGuiCol_ChildBg] = ImColor(15, 15, 15, 160);
            colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
            colors[ImGuiCol_Border] = ImColor(54, 100, 139);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0, 0, 1.00f);
            colors[ImGuiCol_FrameBg] = ImColor(54, 100, 139);
            colors[ImGuiCol_FrameBgHovered] = ImColor(54, 100, 139);
            colors[ImGuiCol_FrameBgActive] = ImColor(54, 100, 139);
            colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(32 / 255.f, 58 / 255.f, 67 / 255.f, 1.f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(149 / 255.f, 20 / 255.f, 225 / 255.f, 1.f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(149 / 255.f, 20 / 255.f, 255 / 255.f, 1.f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(149 / 255.f, 20 / 255.f, 255 / 255.f, 1.f);
            colors[ImGuiCol_Separator] = ImVec4(0.654, 0.094, 0.278, 1.f);
            colors[ImGuiCol_CheckMark] = ImVec4{ 1.f, 0.13f, 0.f, 1.f };
            colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(78 / 255.f, 78 / 255.f, 87 / 255.f, 1);
            colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
            colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        }
    }
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    DWORD window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    auto x = float(screen_rect.right - WINDOW_WIDTH) / 2.f;
    auto y = float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f;
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));  
    int MenuSayfasi = 0;
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowBgAlpha(1.0f);

            ImGui::Begin(LOADER_BRAND, &loader_active, window_flags);
            {
                ImVec2 curPos = ImGui::GetCursorPos();
                ImVec2 curWindowPos = ImGui::GetWindowPos();
                curPos.x += curWindowPos.x;
                curPos.y += curWindowPos.y;

                ImGui::SetCursorPos({ 192.f,41.f });
                ImGui::Image(Logo, ImVec2(50, 50));

                ImGui::SetCursorPos({ 20.f,58.f });
                if (ImGui::Button(("MAIN"), { 160.f,27.f }))
                {
                    MenuSayfasi = 0;
                }
                if (MenuSayfasi == 0)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(curPos.x + 21, curPos.y + 67), ImVec2(curPos.x + WINDOW_WIDTH - 260, curPos.y + 69), ImColor(255, 255, 255));
                }
                ImGui::SetCursorPos({ 250.f,58.f });
                if (ImGui::Button(("MISC"), { 160.f,27.f }))
                {
                    MenuSayfasi = 1;
                }
                if (MenuSayfasi == 1)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(curPos.x + 251, curPos.y + 67), ImVec2(curPos.x + WINDOW_WIDTH - 30, curPos.y + 69), ImColor(255, 255, 255));
                }
                ImGui::SetCursorPos({ 20.f,91.f });
                ImGui::BeginChild(("CHILD 1"), ImVec2(195.0f, 240.0f));

                if (MenuSayfasi == 0)
                {
                    ImGui::SetCursorPos({ 10.f,10.f });
                    ImGui::Checkbox(("Player Esp"), &PlayerEspCheckbox);
                }
                if (MenuSayfasi == 1)
                {
                    ImGui::SetCursorPos({ 10.f,10.f });
                    ImGui::Checkbox(("Spiderman"), &SpidermanCheckbox);
                    ImGui::SetCursorPos({ 10.f,40.f });
                    ImGui::Checkbox(("Hardcore Mode Map"), &HardcoreMapCheckbox);
                    ImGui::SetCursorPos({ 10.f,70.f });
                    ImGui::Checkbox(("Always Day"), &AlwaysDayCheckbox);
                }
                ImGui::EndChild();
                ImGui::SetCursorPos({ 220.f,91.f });
                ImGui::BeginChild(("CHILD 2"), ImVec2(195.0f, 240.0f));
                if (MenuSayfasi == 0)
                {
                    ImGui::SetCursorPos({ 10.f,10.f });
                    ImGui::Checkbox(("No Recoil"), &NoRecoilCheckbox);
                    ImGui::SetCursorPos({ 10.f,40.f });
                    ImGui::Checkbox(("Fat Bullet"), &FatBulletCheckbox);
                    ImGui::SetCursorPos({ 10.f,70.f });
                    ImGui::Checkbox(("No Spread"), &NoSpreadCheckbox);
                    ImGui::SetCursorPos({ 10.f,100.f });
                    ImGui::Checkbox(("Automatic"), &AutomaticCheckbox);
                }
                if (MenuSayfasi == 1)
                {

                }

                if (GetAsyncKeyState(VK_HOME) & 1)
                {
                    if (hidescript2 == true)
                    {
                        ::ShowWindow(FindWindowA(0, LOADER_BRAND), SW_HIDE);
                        Sleep(100);
                        hidescript2 = false; hidescript1 = true;
                    }
                    else if (hidescript1 == true)
                    {
                        ::ShowWindow(FindWindowA(0, LOADER_BRAND), SW_SHOW);
                        Sleep(100);
                        hidescript2 = true; hidescript1 = false;
                    }

                }
                Sleep(1);
                ImGui::EndChild();
            }
            ImGui::End();
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);


        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int main()
{
	r8:
	initdrv();
	r_socket = connect();
	if (r_socket == INVALID_SOCKET)
	{
        if (process_id("RustClient.exe"))
        {
            system(("CLS"));
            std::cout << ("CLOSE RUST") << std::endl;
            goto r8;
        }
        else
        {
            printf(("driver not found\n"));
            LoadDriver();
            goto r8;
        }
	}
	else
	{
        while (process_id("RustClient.exe") == 0)
        {
            printf(("[>] waiting for game\n"));
            Sleep(1);
            r_pid = process_id("RustClient.exe");
            printf(("[>] pid: %d\n"), r_pid);
            Sleep(1);
        }
        Sleep(1500);
        r_pid = process_id("RustClient.exe");
		assemblybase = getdll(r_socket, r_pid, 0);
		unitybase = getdll(r_socket, r_pid, 1);
		if (!assemblybase) { printf(("[>] GameAssembly can't found!\n")); return 0; }
		if (!unitybase) { printf(("[>] UnityPlayer can't found\n")); return 0; }
		printf(("\n[+] Hex:\n[>] Game Assembly: 0x%p\n[>] Unity Player: 0x%p\n"), assemblybase, unitybase);
		std::cout << ("\n[+] Decimal:\n[>] Game Assembly: ") << assemblybase << ("\n[>] Unity Player: ") << unitybase  << std::endl;
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
        std::thread(cheat).detach();
        WinMain(0, 0, 0, 0);
	}
}
