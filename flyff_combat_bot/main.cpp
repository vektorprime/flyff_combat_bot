// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//gui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include "imgui_stdlib.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

//used for image matching
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string> 
#include <fstream>
#include <thread>
#include <future>


//////used for OCR
//#include <leptonica/allheaders.h>
//#include <tesseract/baseapi.h>

//used for reading mem
#include <tlhelp32.h>
#include <iomanip>
#include <array>
#include <cmath>

#include <stdexcept>

//custom headers
#include "trigger_time.h"
#include "game_window.h"
#include "memory_reader.h"
#include "mouse_and_keyboard.h"
#include "image_matching.h"
#include "debug_image_matching.h"
#include "heal_and_buff.h"
#include "config_reader.h"
#include "ocr_matching.h"
#include "map_image_matching.h"
#include "log.h"


using namespace std::chrono_literals;



///////////////////////////////////////////required for imgui
struct FrameContext
{
	ID3D12CommandAllocator *CommandAllocator;
	UINT64                  FenceValue;
};


// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT]{};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device *g_pd3dDevice = nullptr;
static ID3D12DescriptorHeap *g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap *g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue *g_pd3dCommandQueue = nullptr;
static ID3D12GraphicsCommandList *g_pd3dCommandList = nullptr;
static ID3D12Fence *g_fence = nullptr;
static HANDLE                       g_fenceEvent = nullptr;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3 *g_pSwapChain = nullptr;
static HANDLE                       g_hSwapChainWaitableObject = nullptr;
static ID3D12Resource *g_mainRenderTargetResource[NUM_BACK_BUFFERS]{};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS]{};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext *WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//custom forward decl
void cleanup_gui(HWND &hwnd, LPCWSTR &lpClassName, HINSTANCE &hInstanc);


cv::Mat game_window{};
cv::Mat img{}; 
cv::Mat img2{};
cv::Mat monstertempl{};
cv::Mat monstertempl2{};


// set debug to true to display image window, rect, and turn off clicking + turning.
bool debug_bot = false;

flyff::match_result disconnected_result{};


const std::array<unsigned char, 104> monster_on_map_byte_pattern_4 =
{
	//4-7 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //7

	0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01, //15
	//20-23 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //23
	//24-27 monster id
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //31
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //39
	//45-47 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //47
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //55
	//58-59, 61-63 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //63
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //71
	//77-79 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //79
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //87
	//93-95 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //95
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 //103
	//if matching this pattern it's 104 items
};

//new pattern as of 2/8
const std::array<unsigned char, 104> monster_on_map_byte_pattern_5 =
{
	//4-7 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //7

	0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01, //15
	//20-23 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //23
	//24-27 monster id
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //31
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //39
	//45-47 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //47
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //55
	//58-59, 61-63 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //63
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //71
	//77-79 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //79
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //87
	//93-95 random
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //95
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 //103
	//if matching this pattern it's 104 items
};


int last_monster_killed = 0;

int last_monster_attempted = 0;

std::vector<int> monsters_killed{};
bool monster_killed_found = false;

cv::Mat healer_img{};
cv::Mat heal_result{};
cv::Point heal_minLoc{};
cv::Point heal_maxLoc{};
cv::Point heal_matchLoc{};
LONG heal_xloc = 0;
LONG heal_yloc = 0;


int main()
{

	cv::Mat dead_templ = cv::imread("images\\dead.png");
	cv::cvtColor(dead_templ, dead_templ, cv::COLOR_BGRA2GRAY);

	cv::Mat curattacktempl = cv::imread("images\\curattack.png");
	cv::cvtColor(curattacktempl, curattacktempl, cv::COLOR_BGRA2BGR);

	cv::Mat aggroattacktempl = cv::imread("images\\aggro.png");
	cv::cvtColor(aggroattacktempl, aggroattacktempl, cv::COLOR_BGRA2BGR);

	cv::Mat combattempl = cv::imread("images\\incombat.png");
	cv::cvtColor(combattempl, combattempl, cv::COLOR_BGRA2BGR);

	cv::Mat actblockedtempl = cv::imread("images\\actionblocked.png");
	cv::cvtColor(actblockedtempl, actblockedtempl, cv::COLOR_BGRA2BGR);

	cv::Mat gianttempl = cv::imread("images\\giant.png");
	cv::cvtColor(gianttempl, gianttempl, cv::COLOR_BGRA2BGR);

	cv::Mat violettempl = cv::imread("images\\violet.png");
	cv::cvtColor(violettempl, violettempl, cv::COLOR_BGRA2BGR);

	cv::Mat giant_top_templ = cv::imread("images\\gianttop.png");
	cv::cvtColor(giant_top_templ, giant_top_templ, cv::COLOR_BGRA2BGR);

	cv::Mat violet_top_templ = cv::imread("images\\violettop.png");
	cv::cvtColor(violet_top_templ, violet_top_templ, cv::COLOR_BGRA2BGR);

	cv::Mat monster_taken_templ = cv::imread("images\\monstertaken.png");
	cv::cvtColor(monster_taken_templ, monster_taken_templ, cv::COLOR_BGRA2BGR);

	cv::Mat giant_on_map_templ = cv::imread("images\\giantonmap.png");
	cv::cvtColor(giant_on_map_templ, giant_on_map_templ, cv::COLOR_BGRA2BGR);

	cv::Mat focused_target_templ = cv::imread("images\\focused_target.png");
	cv::cvtColor(focused_target_templ, focused_target_templ, cv::COLOR_BGR2GRAY);


	cv::Mat mp_templ = cv::imread("images\\mp.png");
	cv::cvtColor(mp_templ, mp_templ, cv::COLOR_BGRA2BGR);

	cv::Mat hp_templ = cv::imread("images\\hp.png");
	cv::cvtColor(hp_templ, hp_templ, cv::COLOR_BGRA2BGR);

	cv::Mat fp_templ = cv::imread("images\\fp.png");
	cv::cvtColor(fp_templ, fp_templ, cv::COLOR_BGRA2BGR);

	double monster_target_cutoffpoint = 0.05;
	cv::Mat monster_target_templ = cv::imread("images\\monstertarget.png");
	cv::cvtColor(monster_target_templ, monster_target_templ, cv::COLOR_BGRA2BGR);
	
	double captcha_cutoffpoint = 0.05;

	cv::Mat captchatempl = cv::imread("images\\captcha.png");
	cv::cvtColor(captchatempl, captchatempl, cv::COLOR_BGRA2BGR);

	cv::Mat heal_target_health_templ = cv::imread("images\\heal_target_health.png");
	cv::cvtColor(heal_target_health_templ, heal_target_health_templ, cv::COLOR_BGRA2BGR);

	cv::Mat heal_target_in_range = cv::imread("images\\heal_target_in_range.png");
	cv::cvtColor(heal_target_in_range, heal_target_in_range, cv::COLOR_BGRA2BGR);

	cv::Mat fighter_templ = cv::imread("images\\fighter.png");
	cv::cvtColor(fighter_templ, fighter_templ, cv::COLOR_BGRA2BGR);

	double monitor_hp_mp_fp_cutoffpoint = 0.02;
	flyff::match_result heal_target_in_range_result{};

	flyff::time navigator_icon_time{};
	navigator_icon_time.set_interval(15);

	flyff::time char_screen_icon_time{};
	char_screen_icon_time.set_interval(15);

	flyff::time monster_search_time{};
	monster_search_time.set_interval(20);

	flyff::time clear_monsters_killed_time{};
	clear_monsters_killed_time.set_interval(30);

	flyff::time action_blocked_time{};
	action_blocked_time.set_interval(8);

	flyff::time dead_check_time{};
	dead_check_time.set_interval(10);

	flyff::time combat_time{};
	combat_time.set_interval(30);
	bool last_combat_status = false;

	flyff::time dmg_last_taken_time{};
	dmg_last_taken_time.set_interval(2);

	flyff::time position_time, disconnected_time, captcha_check_time, rebuff_time, geb_rebuff_time, prev_rebuff_time{};
	flyff::time find_heal_skill_time, attack_time, combat_end_phase_time, giant_on_map_time, heal_hp_mp_fp_time{};
	position_time.set_interval(80);
	disconnected_time.set_interval(30);

	geb_rebuff_time.set_interval(60);
	prev_rebuff_time.set_interval(540);
	find_heal_skill_time.set_interval(60);

	captcha_check_time.set_interval(30);
	captcha_check_time.set_start_time();

	flyff::time reset_screen_position_time{};
	reset_screen_position_time.set_interval(600);
	captcha_check_time.set_start_time();


	bool initial_self_buff = true;

	bool sent_action_blocked_move = false;

	bool mon_targetting_player = false;

	flyff::match_result monster_result{};
	flyff::match_result aggro_monster_result{};
	flyff::match_result giant_monster_result{};
	flyff::match_result violet_monster_result{};

	flyff::match_result dead_result{};

	flyff::match_result aggro_attacked_result{};
	flyff::match_result combat_result{};
	flyff::match_result actblocked_result{};

	flyff::match_result giant_top_result{};
	flyff::match_result violet_top_result{};

	flyff::match_result curattack_result{};
	flyff::match_result monster_taken_result{};

	flyff::match_result giant_on_map_result_top_left{};
	flyff::match_result giant_on_map_result_top_right{};
	flyff::match_result giant_on_map_result_bottom_left{};
	flyff::match_result giant_on_map_result_bottom_right{};
	flyff::match_result monster_target_result{};


	flyff::match_result monitor_hp_result{};
	flyff::match_result monitor_mp_result{};
	flyff::match_result monitor_fp_result{};
	flyff::match_result monster_result2{};
	flyff::match_result aggro_monster_result2{};


	int bot_mode = 1;

	///////gui
	//GUI INIT
	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"FlyFF Combat Bot", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"FlyFF Combat Bot", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
		g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());


	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool close_gui = false;
	bool bot_running = false;


	flyff::log log_file("flyffcombat.log");
	flyff::config config("flyffcombat.cfg", log_file);


	log_file.write("config loaded");

	//std::cout << "Welcome to Flyff Combat \n"
	//	"UI setup instructions: \n"
	//	"have minimap open on top right \n"
	//	"have skill bar on bottom \n"
	//	"have char window on top left \n"
	//	"have the hp/mp/fp show as % (click on it to change) \n"
	//	"have the combat skill/motion in slot 6 of the F1 bar \n"
	//	"have party on bottom left with healer as leader if using heal_party_member \n"
	//	"turn off special effects and hide all players on the fighter \n"
	//	"turn off select pets after target exclusion prompt \n"
	//	"turn off pixel perfect in cases where the mobs are small" << std::endl;

	//std::cout << "starting bot" << std::endl;
	flyff::match_result heal_skill_result{};
	double heal_skill_cutoffpoint = 0.009;
	bool initial_find_heal_skill = true;
	bool initial_functions_ran = false;

	bool initial_buff_ran = false;

	bool enable_second_player = false;
	if (config.game_settings.enable_second_player_value == "on")
	{
		enable_second_player = true;
	}

	bool find_fighter_manually = false;
	int find_fighter = 0;

	bool heal_enable_find_player = false;
	if (config.game_settings.heal_enable_find_player_value == "on")
	{
		heal_enable_find_player = true;
		find_fighter = 2;
	}

	bool enable_fighter_self_buffs = false;
	if (config.game_settings.fight_self_buff_enabled_value == "on")
	{
		enable_fighter_self_buffs = true;
	}

	char attack_skill = '0';
	bool enable_attack_skill = false;
	if (config.game_settings.enable_attack_skill_value == "on")
	{
		enable_attack_skill = true;
	}
	int heal_reactive_int = 0;
	bool heal_reactive = false;
	if (config.game_settings.heal_reactive_value == "on")
	{
		heal_reactive = true;
	}

	int heal_rebuff_interval = 300;
	heal_rebuff_interval = std::stoi(config.game_settings.heal_rebuff_interval_value);


	int heal_type = 1;


	if (config.game_settings.heal_type_value == "single_heal")
	{
		heal_type = 1;
	}
	else if (config.game_settings.heal_type_value == "circle_heal")
	{
		heal_type = 2;
	}
	else if (config.game_settings.heal_type_value == "heal_rain")
	{
		heal_type = 3;
	}

	int heal_pause_between_interval = 100;
	heal_pause_between_interval = std::stoi(config.game_settings.heal_pause_between_interval_value);
	

	bool heal_party_member = false;
	if (config.game_settings.heal_party_member_value == "on")
	{
		heal_party_member = true;
	}

	bool heal_geb_rebuff = false;

	if (config.game_settings.heal_geb_rebuff_value == "on")
	{
		heal_geb_rebuff = true;
	}

	bool heal_prev_rebuff = false;

	if (config.game_settings.heal_prev_rebuff_value == "on")
	{
		heal_prev_rebuff = true;
	}

	bool heal_start_with_buffs = false;
	if (config.game_settings.heal_start_with_buffs_value == "on")
	{
		heal_start_with_buffs = true;
	}

	if (config.game_settings.mode_value == "fight")
	{
		bot_mode = 1;
	}
	else
	{
		bot_mode = 2;
	}

	int xoffset = 0;
	int yoffset = 0;
	int heal_hp_mp_fp_interval_q = 0;


	flyff::player player{};
	//flyff::player player(log_file);
	player.set_log_file(&log_file);

	flyff::player healer_player{};
	//flyff::player healer_player(log_file);
	healer_player.set_log_file(&log_file);

	screen_cords target_to_click{};

	std::vector<flyff::monster> initial_monsters_on_field{};
	initial_monsters_on_field.reserve(65);

	char_position pos{};

	// int pet_id = 0;
	std::vector<int> all_pet_id{};

	//find the flyff pid
	flyff::process flyff_proc{};

	bool initial_vars_setup = false;

	std::vector<self_buff> char_self_buffs{};
	char_self_buffs.reserve(4);

	int skill_interval = 300;

	if (enable_fighter_self_buffs)
	{
		//int num_of_self_buffs = std::stoi(config.game_settings.fight_self_buff_quantity_value);

		for (int a = 0; a < config.game_settings.fight_self_buff_skills_slot_value_all.size(); ++a)
		{
			self_buff single_char_self_buff;
			single_char_self_buff.skill_key = config.game_settings.fight_self_buff_skills_slot_value_all[a] + 48;
			single_char_self_buff.skill_interval_int = config.game_settings.fight_self_buff_skills_intervals_value_all[a];
			single_char_self_buff.skill_interval = std::chrono::seconds(single_char_self_buff.skill_interval_int);
			char_self_buffs.push_back(single_char_self_buff);
		}
	}
	int num_of_self_buffs = char_self_buffs.size();

	cv::Mat heal_skill_templ{};
	bool initial_heal_skill_setup = false;
	bool initial_prev_rebuff_q = true;

	bool restore_hp_value = false;
	bool restore_mp_value = false;
	bool restore_fp_value = false;

	if (config.game_settings.restore_hp_value == "on")
	{
		restore_hp_value = true;
	}

	if (config.game_settings.restore_mp_value == "on")
	{
		restore_mp_value = true;
	}

	if (config.game_settings.restore_mp_value == "off")
	{
		if (config.game_settings.restore_fp_value == "on")
		{
			restore_fp_value = true;
		}
	}

	flyff::time rebuff_interval_time{};
	rebuff_interval_time.set_interval(heal_rebuff_interval);

	//don't declare vars from config below this line otherwise they get overwritten

	std::vector<LPVOID> all_text_targets{};
	all_text_targets.reserve(10);
	bool bad_mon_found = false;
	bool baby_pet_found = false;
	bool eron_found = false;

	//bool log_to_file = true;
	int current_log_level = 0;

	//used to store giant or violets found via the check_if_giant_or_violet func in the attack sequence
	//if we find any, we'll monitor their POS and avoid it
	std::vector<flyff::monster> bad_monsters_on_field{};

	flyff::window gray_win{};

	std::thread bot_thread{};

	while (!close_gui)
	{ 
		if (!initial_vars_setup)
		{

			xoffset = std::stoi(config.game_settings.x_click_offset_value);
			yoffset = std::stoi(config.game_settings.y_click_offset_value);
			heal_hp_mp_fp_interval_q = std::stoi(config.game_settings.restore_hp_mp_fp_interval_value);
			initial_vars_setup = true;
		}
		
	
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				close_gui = true;
		}
		if (close_gui)
		{
			break;
		}
		
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool use_work_area = true;
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration;
		const ImGuiViewport *viewport = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
		ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

		ImGui::Begin("FlyFF Combat", 0, flags);                        

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		//Always log to file for now
		//ImGui::Checkbox("Enable logging to file", &log_to_file);
		//ImGui::Separator();


		ImGui::Text("Select the bot mode");
		ImGui::Separator();
		ImGui::RadioButton("Fight", &bot_mode, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Heal", &bot_mode, 2);

		ImGui::PushItemWidth(200);

		//change the menu style depending on the bot mode
		if (bot_mode == 1)
		{
			ImGui::SeparatorText("Fight Mode Options");
			ImGui::Separator();

			ImGui::Text("Setup Players");
			ImGui::InputText("Fighter Name", &config.game_settings.player_name_value);

			ImGui::Checkbox("Enable monitor and defend healer", &enable_second_player);
			if (enable_second_player)
			{
				ImGui::InputText("Healer Name", &config.game_settings.healer_name_value);
			}
			ImGui::Separator();

			ImGui::Text("Self Buffs");
			ImGui::Checkbox("Enable fighter self buffs in the F2 skillbar", &enable_fighter_self_buffs);
			if (enable_fighter_self_buffs)
			{
				ImGui::InputInt("Number of self buff skills", &num_of_self_buffs);
				//only allow 1 to 10 self buffs
				if (num_of_self_buffs < 1)
				{
					num_of_self_buffs = 1;
				}
				if (num_of_self_buffs > 10)
				{
					num_of_self_buffs = 10;
				}
				//resize the vector otherwise it's always stuck to the size learned from config
				if (num_of_self_buffs != char_self_buffs.size())
				{
					char_self_buffs.resize(num_of_self_buffs);
				}
				int count = 0;
				for ( self_buff &buff : char_self_buffs)
				{
					++count;
					char text[13] = "Skill slot  ";
					text[12] = '0' + count;
					char skill_key[2] = { buff.skill_key, '\0' };
					ImGui::InputText(text, skill_key, sizeof(skill_key));
					if (skill_key[0] > 47 && skill_key[0] < 59)
					{
						buff.skill_key = skill_key[0];
					}
					else
					{
						buff.skill_key = 48;
					}
					ImGui::SameLine();
					char text2[23] = "Skill   Interval (Sec)";
					text2[6] = '0' + count;
					ImGui::InputInt(text2, &buff.skill_interval_int, sizeof(buff.skill_interval_int));
					buff.skill_interval = std::chrono::seconds(buff.skill_interval_int);
				}
			}

			ImGui::Separator();

			ImGui::Text("Attack Skill");
			ImGui::Checkbox("Enable attack skill in the F1 skillbar", &enable_attack_skill);
			if (enable_attack_skill)
			{

				ImGui::InputText("Attack skill number", &config.game_settings.attack_skill_slot_value);
				attack_skill = config.game_settings.attack_skill_slot_value[0];
			}
			ImGui::Separator();


			ImGui::Text("Restore");
			ImGui::Text("Use pots to restore HP, MP, or FP via 1 and 2 keys of F1");

			ImGui::InputText("Restore Interval (sec)", &config.game_settings.restore_hp_mp_fp_interval_value);
			ImGui::Checkbox("Restore HP", &restore_hp_value);
			ImGui::SameLine();
			ImGui::Checkbox("Restore MP", &restore_mp_value);
			ImGui::SameLine();
			ImGui::Checkbox("Restore FP", &restore_fp_value);
			if (restore_mp_value)
			{
				restore_fp_value = false;
			}

			ImGui::Separator();


			ImGui::Text("Click offset values when clicking monster target");
			ImGui::InputText("Click X Offset", &config.game_settings.x_click_offset_value);
			ImGui::InputText("Click Y Offset", &config.game_settings.y_click_offset_value);
			ImGui::Separator();

		}
		else if (bot_mode == 2)
		{
			ImGui::SeparatorText("Heal Mode Options");
			ImGui::Separator();

			ImGui::Text("Buffs");
			ImGui::Checkbox("Start by buffing the fighter", &heal_start_with_buffs);
			ImGui::Text("Buffs should be in #2-0 slots on the F1 skillbar and #2-4 slots on the F2 skillbar");

			ImGui::Separator();

			ImGui::Text("Heal Skill");

			ImGui::RadioButton("Heal", &heal_type, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Circle Heal", &heal_type, 2);
			ImGui::SameLine();
			ImGui::RadioButton("Heal Rain", &heal_type, 3);
			ImGui::Text("The heal skill should be in the #1 slot of F1 and F2 skillbars");
			ImGui::Separator();

			ImGui::Text("Intervals");
			ImGui::InputInt("Rebuff interval in seconds", &heal_rebuff_interval);
			ImGui::InputInt("Pause between heal interval in millisec", &heal_pause_between_interval);
			ImGui::Separator();

			ImGui::Text("Find Fighter");
			ImGui::RadioButton("User will target the fighter manually", &find_fighter, 0);
			ImGui::RadioButton("Find fighter in party window and always target", &find_fighter, 1);
			ImGui::RadioButton("Find and target fighter by name automatically", &find_fighter, 2);

			switch (find_fighter)
			{
				case 0:
					//nothing needed here but i'll set this bool for the future incase I implement something
					find_fighter_manually = true;
					break;

				case 1:
					heal_party_member = true;
					find_fighter_manually = false;
					heal_enable_find_player = false;
					break;

				case 2:
					heal_enable_find_player = true;
					find_fighter_manually = false;
					heal_party_member = false;
					break;

				default:
					find_fighter_manually = true;
					heal_enable_find_player = false;
					heal_party_member = false;
					break;

			}

			if (heal_enable_find_player)
			{
				ImGui::InputText("Fighter Name", &config.game_settings.player_name_value);
				ImGui::InputText("Healer Name", &config.game_settings.healer_name_value);
			}
			ImGui::Separator();

			ImGui::Text("Heal Method");
			ImGui::RadioButton("Monitor targetted fighter and only heal when hurt", &heal_reactive_int, 1);
			ImGui::RadioButton("Constantly heal every pause interval", &heal_reactive_int, 0);
			if (heal_reactive_int == 1)
			{
				heal_reactive = true;
			}
			else
			{
				heal_reactive = false;
			}


			ImGui::Separator();


			ImGui::Text("Other Rebuffs");
			ImGui::Checkbox("Rebuff party with Geburah Tiphreth every 60 sec", &heal_geb_rebuff);
			ImGui::Text("Geb skill should be #4 slot on the F2 skillbar");

			ImGui::Checkbox("Rebuff self with Prevention every 540 sec", &heal_prev_rebuff);
			ImGui::Text("Prev skill should be #5 slot on the F2 skillbar");

			ImGui::Separator();
		}

		ImGui::Text("Logging to file level");
		ImGui::Separator;
		ImGui::RadioButton("Error", &current_log_level, 0);
		ImGui::SameLine;
		ImGui::RadioButton("Verbose", &current_log_level, 1);
		ImGui::Separator();

		if (ImGui::Button("Start", ImVec2(120, 50)))
		{
			bot_running = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", ImVec2(120, 50)))
		{
			bot_running = false;
		}
		ImGui::End();
		///GUI

		flyff::window win{};
		win.get_mat();

		if (bot_running)
		{


			if (current_log_level == 1)
			{
				log_file.set_verbose_log_level();
			}

			gray_win.get_and_convert_mat(cv::COLOR_BGRA2GRAY);
			cv::Mat flyff_win_gray = flyff::window().get_and_convert_mat(cv::COLOR_BGRA2GRAY);

			dead_result = flyff::match_flyff_item(gray_win, dead_templ, cv::COLOR_BGRA2GRAY);
			//if we die stop the bot so we stop logging
			if (dead_check_time.check_if_time_elapsed())
			{
				if (dead_result.minVal <= 0.03)
				{
					std::cout << "Found dead player, stopping bot." << std::endl;
					log_file.write("Found dead player, stopping bot.");
					// don't throw here, just stop the bot.
					//throw std::runtime_error("ERROR: Found dead player, stopping bot.");
					bot_running = false;
				}
				dead_check_time.set_start_time();
			}
			//if 2 we heal
			if (bot_mode == 2)
			{
	

				if (heal_enable_find_player)
				{
					player.name = config.game_settings.player_name_value;
					healer_player.name = config.game_settings.healer_name_value;

					if (player.name.empty() || healer_player.name.empty())
					{
						std::cout << "player or healer name blank, cannot run code to find them, restart the program";
						std::cin.ignore();
					}

					if (initial_functions_ran)
					{
						healer_player.update_target();
					}

					if (!initial_functions_ran)
					{
						initial_functions_ran = true;

						std::cout << "Setting up memory reading functions" << std::endl;

						std::vector<DWORD> all_pids = find_all_flyff_pids();
						for (int i = 0; i < all_pids.size(); i++)
						{

							flyff_proc = find_flyff_string_in_pid(all_pids[i]);
							std::cout << "checking if pid " << all_pids[i] << " is the flyff window" << std::endl;
							//if anything other than 1 is returned, we got the right PID
							if (flyff_proc.pid != 1)
							{
								break;
							}
						}

						if (flyff_proc.pid == 1)
						{
							std::cout << "flyff pid is 1, error " << std::endl;
							cleanup_gui(hwnd, wc.lpszClassName, wc.hInstance);
							return 1;
						}

						//if the player target setup ran when finding the pet id to ignore, don't run it again
						healer_player.setup_initial_target(flyff_proc);
						player.setup_initial_target(flyff_proc);
						if (healer_player.id == 1 || player.id == 1)
						{
							std::cout << "The player ID for the fighter or healer was 1, cannot proceed.";
							std::cin.ignore();
						}
					}
				}
				

				
				if (!initial_heal_skill_setup)
				{
					if (heal_type == 1)
					{
						heal_skill_templ = cv::imread("images\\heal_skill.png");
						cv::cvtColor(heal_skill_templ, heal_skill_templ, cv::COLOR_BGRA2BGR);
					}


					if (heal_type == 2)
					{
						heal_skill_templ = cv::imread("images\\circle_heal_skill.png");
						cv::cvtColor(heal_skill_templ, heal_skill_templ, cv::COLOR_BGRA2BGR);
					}


					if (heal_type == 3)
					{
						heal_skill_templ = cv::imread("images\\heal_rain_skill.png");
						cv::cvtColor(heal_skill_templ, heal_skill_templ, cv::COLOR_BGRA2BGR);
					}
					initial_heal_skill_setup = true;
				}


			
		
				// If this is on we watch for when the target is hurt, we still take the heal interval into account
				//Also used to follow the fighter around and jump so healer doesn't get stuck
		

				HWND healfshwnd = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
				if (!healfshwnd)
				{
					healfshwnd = FindWindowW(NULL, L"Flyff Universe - Chromium");
				}

				if (!initial_buff_ran)
				{
					// Bring the window to the front to prepare for action.
					SetForegroundWindow(healfshwnd);
					// send z to follow and buffs
					send_key_hold_down(0x5A, 500);
					if (heal_start_with_buffs)
					{
						std::cout << "sending buffs" << std::endl;
						send_buffs();
					}
					initial_buff_ran = true;
				}

				
				if (heal_geb_rebuff)
				{
					if (geb_rebuff_time.check_if_time_elapsed())
					{
						send_key_hold_down(VK_F2, 250);
						send_key_hold_down(0x34, 250);
						send_key_hold_down(VK_F1, 250);
						geb_rebuff_time.set_start_time();
					}
				}

				if (heal_prev_rebuff)
				{
					if (initial_prev_rebuff_q)
					{
						send_key_hold_down(VK_F2, 250);
						send_key_hold_down(0x35, 250);
						send_key_hold_down(VK_F1, 250);
						initial_prev_rebuff_q = false;
					}

					if (prev_rebuff_time.check_if_time_elapsed())
					{
						send_key_hold_down(VK_F2, 250);
						send_key_hold_down(0x35, 250);
						send_key_hold_down(VK_F1, 250);
						prev_rebuff_time.set_start_time();
					}
				}

				if (captcha_check_time.check_if_time_elapsed())
				{
					//back to the captcha check
					flyff::match_result captcha_result = flyff::match_flyff_item(win, captchatempl);
					heal_xloc = captcha_result.minLoc.x + 10;
					heal_yloc = captcha_result.minLoc.y + 10;
					//std::cout << "checking for captcha, the current heal_minVal is " << heal_minVal << std::endl;

					if (captcha_result.minVal < captcha_cutoffpoint)
					{
						std::cout << "found captcha, pausing to solve" << std::endl;
						send_left_click_delay(heal_xloc, heal_yloc);
						std::this_thread::sleep_for(std::chrono::seconds(30));
						send_left_click_delay(50, 100);
					}
					//reset the start time for the next run
					captcha_check_time.set_start_time();
				}

				//sleep between the heal intervals, will probably change this later to use chrono instead of sleep
				std::this_thread::sleep_for(std::chrono::milliseconds(heal_pause_between_interval));

				//on first run find the heal skill in skillbar
				if (initial_find_heal_skill)
				{
					heal_skill_result = flyff::match_specific_flyff_item(win, heal_skill_templ, flyff::type_of_match::skillbar);
					initial_find_heal_skill = false;

				}

				if (find_heal_skill_time.check_if_time_elapsed())
				{
					heal_skill_result = flyff::match_specific_flyff_item(win, heal_skill_templ, flyff::type_of_match::skillbar);
					find_heal_skill_time.set_start_time();
				}

				//find player 
				flyff::match_result heal_target_result;
				if (heal_party_member || heal_reactive)
					{ 
						heal_target_result = flyff::match_specific_flyff_item(win, monster_target_templ, flyff::type_of_match::monster_target);
					}

				if (heal_enable_find_player)
				{
					healer_player.update_target();
					if (healer_player.target != player.id)
					{
						healer_player.set_target(player.id);
						send_key_hold_down(0x5A, 50);
						send_key_hold_down(0x53, 50);
						std::this_thread::sleep_for(std::chrono::milliseconds(250));
						std::cout << "Getting target coords" << std::endl;
						target_to_click = get_target_to_click_cords(win);
						if (target_to_click.x != 0 && target_to_click.y != 0)
						{
							std::cout << "Sending escape" << std::endl;
							send_key_hold_down(VK_ESCAPE, 50);
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							std::cout << "target to click x is " << target_to_click.x << " y is " << target_to_click.y << std::endl;
							std::cout << "Sending left click" << std::endl;
							bool valid_click = check_for_valid_click(target_to_click.y - 5);

							if (valid_click)
							{
								//I subtract 5 because the target matches aren't exactly centered, and we need to move the target cursor up
								send_left_click(target_to_click.x + 5, target_to_click.y - 5);
								//the target in mem isn't set right away, so we need to pause
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								healer_player.update_target();
								if (healer_player.target == player.id)
								{
									send_key_hold_down(0x5A, 50);
								}
							}
							else
							{
								std::cout << "invalid click location, exiting fight sequence" << std::endl;
							}
						}
					}
				}
				
				//heal hurt player
				if (heal_reactive)
				{
					if (heal_target_result.minVal < monster_target_cutoffpoint)
					{
						std::cout << "found player target" << std::endl;
						//player is targetted, check if the player should be healed
						flyff::match_result heal_target_health_result = flyff::match_specific_flyff_item(win, heal_target_health_templ, flyff::type_of_match::monster_target);
						if (heal_target_health_result.minVal > monster_target_cutoffpoint)
						{
							std::cout << "player is hurt, healing" << std::endl;
							//find the heal skill and click it
							std::cout << "heal skill minval is " << heal_skill_result.minVal << std::endl;
							if (heal_skill_result.minVal <= heal_skill_cutoffpoint)
							{
								send_key_hold_down(0x5A, 250);
								std::cout << "sending heal skill via click" << std::endl;
								send_left_click_delay(heal_skill_result.minLoc.x, heal_skill_result.minLoc.y + 5);
								//send_left_click(heal_skill_result.matchLoc.x, heal_skill_result.matchLoc.y);
							}
						}
						else
						{
							std::cout << "player is not hurt, skipping" << std::endl;
							send_key_hold_down(0x5A, 250);
							std::cout << "checking if target is in range" << std::endl;
							//heal_target_in_range_result = flyff::match_flyff_item(win, heal_target_in_range, cv::COLOR_BGRA2BGR);
							bool heal_target_near = find_target_icons(win);

							//std::cout << "heal_target_in_range_result minVal is " << heal_target_in_range_result.minVal << std::endl;
							//used to jump while running to the fighter

							if (!heal_target_near)
							{
								std::cout << "target not in range, pressing space bar in case we are stuck" << std::endl;
								send_jump();
							}
						}
					}
					else
					{
						//if we aren't targetting a player, find the party member and click their name via the party window on bottom left of screen
						std::cout << "did not find player target" << std::endl;
						if (heal_party_member)
						{
							std::cout << "heal party member check will run" << std::endl;
							//The healer should be party leader because the second member health bar will be the match at bottom left of screen
							//Thus, the mini-party window should be just above the "menu" button of screen
							flyff::match_result party_member_result = flyff::match_specific_flyff_item(win, fighter_templ, flyff::type_of_match::party_window);
							if (party_member_result.minVal < 0.008)
							{
								std::cout << "found fighter in party window, sending click" << std::endl;
								send_left_click_delay(party_member_result.minLoc.x + 3, party_member_result.minLoc.y + 5);
								send_key_hold_down(0x5A, 250);
							}
						}
						else if (find_fighter_manually)
						{
							std::cout << "not configured to find the fighter, please target the player manually" << std::endl;
						}
					
					}
					//Check our hp bar and heal ourselves
					if (heal_type == 2 || heal_type == 3)
					{
						flyff::match_result monitor_hp_result = flyff::match_specific_flyff_item(win, hp_templ, flyff::type_of_match::char_screen);
						if (monitor_hp_result.minVal > monitor_hp_mp_fp_cutoffpoint)
						{
							std::cout << "We are hurt, healing" << std::endl;
							//find the heal skill and click it
							std::cout << "heal skill minval is " << heal_skill_result.minVal << std::endl;

							if (heal_skill_result.minVal <= heal_skill_cutoffpoint)
							{
								std::cout << "sending heal skill via click" << std::endl;
								send_left_click_delay(heal_skill_result.minLoc.x, heal_skill_result.minLoc.y + 5);
								//send_left_click(heal_skill_result.matchLoc.x, heal_skill_result.matchLoc.y);

							}
						}
					}
				}
				else
				{
					//This logic is for heal_reactive_value being off, we go back to the normal heal per interval clicking
					//find the heal skill and click it
					if (heal_skill_result.minVal <= heal_skill_cutoffpoint)
					{
						send_key_hold_down(0x5A, 250);
						std::cout << "sending heal skill via click" << std::endl;
						send_left_click_delay(heal_skill_result.minLoc.x, heal_skill_result.minLoc.y + 5);
						//send_left_click(heal_skill_result.matchLoc.x, heal_skill_result.matchLoc.y);

					}
				}

				if (rebuff_interval_time.check_if_time_elapsed())
				{
					SetForegroundWindow(healfshwnd);
					std::cout << "we have reached the buff interval of " << heal_rebuff_interval << " seconds, sending buffs" << std::endl;
					send_buffs();
					//rebuff_start_time = std::chrono::steady_clock::now();
					rebuff_interval_time.set_start_time();
				}
			
				bool heal_dc = check_if_disconnected(win, disconnect_message::disconnected, disconnected_time, disconnected_result);
				bool heal_dc2 = check_if_disconnected(win, disconnect_message::cannot_connect, disconnected_time, disconnected_result);
				bool heal_dc3 = check_if_disconnected(win, disconnect_message::account_connected, disconnected_time, disconnected_result);
				bool heal_dc4 = check_if_disconnected(win, disconnect_message::connecting, disconnected_time, disconnected_result);


				// if we were disconnected and set to heal the party member, find their name and click it
				if (heal_dc || heal_dc2 || heal_dc3 || heal_dc4)
				{
					std::cout << " heal_dc or dc2 was triggered" << std::endl;
					if (heal_party_member)
					{
						std::cout << "heal party member check will run" << std::endl;
						flyff::match_result party_member_result = flyff::match_specific_flyff_item(win, fighter_templ, flyff::type_of_match::char_screen);
						if (party_member_result.minVal < 0.008)
						{
							std::cout << "found fighter in party window, sending click" << std::endl;
							send_left_click_delay(party_member_result.minLoc.x + 3, party_member_result.minLoc.y + 5);
							send_key_hold_down(0x5A, 250);
						}
					}
				}
			}

			///fight mode
			//if 1 it means we fight
			if (bot_mode == 1)
			{

				std::string debug_q;
				if (config.game_settings.debug_value == "on")
				{
					debug_q = "yes";
				}
				else if (config.game_settings.debug_value == "off")
				{
					debug_q = "no";
				}

				if (debug_q == "yes")
				{
					debug_bot = true;
				}
				else
				{
					debug_bot = false;
				};



				if (reset_screen_position_time.check_if_time_elapsed())
				{
					if (log_file.log_level_is_verbose())
					{
						log_file.write("reset_screen_position_time timer expired, fixing screen");
					}
					reset_screen_position();
					reset_screen_position_time.set_start_time();
				}
				

				HWND supportwindowcheck = FindWindowW(NULL, L"Flyff Universe - Google Chrome");
				if (!supportwindowcheck)
				{
					supportwindowcheck = FindWindowW(NULL, L"Flyff Universe - Chromium");
				}

				heal_hp_mp_fp_time.set_interval(heal_hp_mp_fp_interval_q);

				player.name = config.game_settings.player_name_value;

			
				if (enable_second_player)
				{
					healer_player.name = config.game_settings.healer_name_value;
				}

						/////////////////////////////////////

						monster_killed_found = false;

						

						if (enable_fighter_self_buffs)
						{
							// this bool makes sure we only runs this once
							if (initial_self_buff)
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("running intial fighter self buffs");
								}
								SetForegroundWindow(supportwindowcheck);
								std::cout << "sending intial self buff" << std::endl;
								send_key_hold_down(VK_F2);
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								for (auto it = char_self_buffs.begin(); it != char_self_buffs.end(); ++it)
								{
									std::cout << "sending a self buff " << (*it).skill_key << std::endl;
									send_key_hold_down(VkKeyScan((*it).skill_key), 1000);
									(*it).start_time = std::chrono::steady_clock::now();
									(*it).current_time = std::chrono::steady_clock::now();
								}
								send_key_hold_down(VK_F1);
								// don't run initial self buff again
								initial_self_buff = false;
								//std::cout << "pausing for 5 sec after initial self buffs" << std::endl;
								//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
							}

						}

						game_window = win.get_and_convert_mat(cv::COLOR_BGRA2BGR);


						if (heal_hp_mp_fp_time.check_if_time_elapsed())
						{
							/*if (log_file.log_level_is_verbose())
							{
								log_file.write("checking hp/mp/fp");
							}*/
							if (restore_hp_value)
							{
								monitor_hp_result = flyff::match_specific_flyff_item(win, hp_templ, flyff::type_of_match::char_screen);
								if (monitor_hp_result.minVal > monitor_hp_mp_fp_cutoffpoint)
								{
									std::cout << "did not find full hp bar, healing with 1" << std::endl;
									send_key_hold_down('1', 250);
								}
							}
							if (restore_mp_value)
							{
								monitor_mp_result = flyff::match_specific_flyff_item(win, mp_templ, flyff::type_of_match::char_screen);
								if (monitor_mp_result.minVal > monitor_hp_mp_fp_cutoffpoint)
								{
									std::cout << "found low mp, restoring with 2" << std::endl;
									send_key_hold_down('2', 250);
								}
							}
							if (restore_fp_value)
							{
								monitor_fp_result = flyff::match_specific_flyff_item(win, fp_templ, flyff::type_of_match::char_screen);
								if (monitor_fp_result.minVal > monitor_hp_mp_fp_cutoffpoint)
								{
									std::cout << "found low fp, restoring with 2" << std::endl;
									send_key_hold_down('2', 250);
								}
							}
							heal_hp_mp_fp_time.set_start_time();
						}

						if (initial_functions_ran)
						{
							//this is used to prevent the position function from being invoked if the fighter or healer are being targetted
							//the combat status check is not enough because we may have pulled aggro mobs
							mon_targetting_player = false;

							player.update_target();
							if (enable_second_player)
							{
								healer_player.update_target();
							}

							player.update_pos();
							for (flyff::monster &mon : initial_monsters_on_field)
							{
								if (mon.check_if_valid(flyff_proc))
								{
									mon.update_pos(flyff_proc);
									mon.update_target(flyff_proc);
									if (log_file.log_level_is_verbose())
									{
										log_file.write("Mon ID " + std::to_string(mon.id) + " target is " + std::to_string(mon.target) + " POS is - X " + std::to_string(mon.x) + " Y " + std::to_string(mon.y) + " Z " + std::to_string(mon.z));
									}
								}
								else
								{
									std::cout << "Mon ID " << mon.id << " is invalid, setting to 0" << std::endl;
									if (log_file.log_level_is_verbose())
									{
										log_file.write("Mon ID " + std::to_string(mon.id) + " is invalid, setting to 0");
									}
									mon.id = 0;
								}
							}

							remove_monster_from_field(0, initial_monsters_on_field);
							std::cout << "Starting sort_and_filter_monsters_by_distance" << std::endl;
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Starting sort_and_filter_monsters_by_distance");
							}
							sort_and_filter_monsters_by_distance(player.pos, initial_monsters_on_field);
						}

						//This runs once, the if block above will run every time after
						//set the initial cords of where we want to be
						if (!initial_functions_ran)
						{
							if (log_file.log_level_is_verbose())
							{
								log_file.write("initial_functions_ran is false, running the intial functions");
							}
							initial_functions_ran = true;

							if (log_file.log_level_is_verbose())
							{
								log_file.write("Setting up memory reading functions");
							}
							std::cout << "Setting up memory reading functions" << std::endl;

							std::vector<DWORD> all_pids = find_all_flyff_pids();
							for (int i = 0; i < all_pids.size(); i++)
							{

								flyff_proc = find_flyff_string_in_pid(all_pids[i]);

								std::cout << "checking if pid " << all_pids[i] << " is the flyff window" << std::endl;
								//if anything other than 1 is returned, we got the right PID
								if (flyff_proc.pid != 1)
								{
									break;
								}
							}

							if (flyff_proc.pid == 1)
							{
								log_file.write("ERROR: flyff pid is 1");
								std::cout << "flyff pid is 1, error " << std::endl;
								cleanup_gui(hwnd, wc.lpszClassName, wc.hInstance);
								throw std::runtime_error("ERROR: flyff pid is 1");
							}

							//if the player target setup ran when finding the pet id to ignore, don't run it again

							player.setup_initial_pos(flyff_proc);
							player.setup_initial_target(flyff_proc);
							if (player.id == 0 || player.id == 1)
							{
								log_file.write("ERROR: player ID is invalid");
								cleanup_gui(hwnd, wc.lpszClassName, wc.hInstance);
								throw std::runtime_error("ERROR: player ID is invalid");
							}
							if (enable_second_player)
							{
								healer_player.setup_initial_target(flyff_proc);
								if (healer_player.id == 0 || healer_player.id == 1)
								{
									log_file.write("ERROR: healer player ID is invalid");
									cleanup_gui(hwnd, wc.lpszClassName, wc.hInstance);
									throw std::runtime_error("ERROR: healer player ID is invalid");
								}
							}
							if (log_file.log_level_is_verbose())
							{
								log_file.write("setting player target to self so we can find the text targets in memory");
							}
							///find the target text that has our player name in it. One of these will change to giant/violet text after we target
							player.set_target(player.id);
							all_text_targets = player.find_all_text_targets(flyff_proc);
							//bad_mon_found = player.check_if_text_target_is_giant(all_text_targets, log_file);
							player.set_target(0);
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Got all of the text targets");
							}
							//The other should always be get monsters, pos, then filtered monsters
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Starting get_initial_monsters_on_field");
							}
							initial_monsters_on_field = get_initial_monsters_on_field(flyff_proc, monster_on_map_byte_pattern_5);

							if (log_file.log_level_is_verbose())
							{
								log_file.write("initial_monsters_on_field size is " + std::to_string(initial_monsters_on_field.size()));
							}
							std::cout << "initial_monsters_on_field size is " << initial_monsters_on_field.size() << std::endl;
							player.update_pos();
							for (flyff::monster &mon : initial_monsters_on_field)
							{
								mon.update_pos(flyff_proc);
								mon.update_target(flyff_proc);
								if (log_file.log_level_is_verbose())
								{
									log_file.write("Mon ID " + std::to_string(mon.id) + " target is " + std::to_string(mon.target) + " POS is - X " + std::to_string(mon.x) + " Y " + std::to_string(mon.y) + " Z " + std::to_string(mon.z));
								}
								if (mon.target == player.id)
								{
									bool is_pet = mon.check_if_pet(flyff_proc);
									if (is_pet)
									{
										if (log_file.log_level_is_verbose())
										{
											log_file.write("Mon ID " + std::to_string(mon.id) + " is pet, adding to all_pet_id");
										}
										std::cout << "Mon ID " + std::to_string(mon.id) + " is pet, adding to all_pet_id" << std::endl;

										//std::cout << "found pet pattern for mon " << mon.id << std::endl;
										all_pet_id.push_back(mon.id);
									}
								}
							}
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Removing pet and sorting monsters by distance in initial_monsters_on_field");
							}
							for (int pet_id : all_pet_id)
							{
								remove_monster_from_field(pet_id, initial_monsters_on_field);
							}
						
							sort_and_filter_monsters_by_distance(player.pos, initial_monsters_on_field);
						}

						if (clear_monsters_killed_time.check_if_time_elapsed())
						{
							std::cout << "Clearing monsters_killed" << std::endl;
							log_file.write("Clearing monsters_killed");
							monsters_killed.clear();
							clear_monsters_killed_time.set_start_time();
						}

						if (monster_search_time.check_if_time_elapsed() || initial_monsters_on_field.empty() || initial_monsters_on_field.size() == 1)
						{
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Monster search time elapsed or filtered monsters empty, running search");
							}
							std::cout << "Monster search time elapsed or filtered monsters empty, running search" << std::endl;
							initial_monsters_on_field = get_initial_monsters_on_field(flyff_proc, monster_on_map_byte_pattern_5);
							if (log_file.log_level_is_verbose())
							{
								log_file.write("initial_monsters_on_field size is " + std::to_string(initial_monsters_on_field.size()));
							}
							std::cout << "initial_monsters_on_field size is " << initial_monsters_on_field.size() << std::endl;
							player.update_pos();

							for (flyff::monster &mon : initial_monsters_on_field)
							{
								mon.update_pos(flyff_proc);
								mon.update_target(flyff_proc);
							}
							if (!all_pet_id.empty())
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("all_pet_id is not empty, removing those IDs from initial_monsters_on_field");
								}
								std::cout << "all_pet_id is not empty, removing those IDs from initial_monsters_on_field" << std::endl;
								for (int pet_id : all_pet_id)
								{
									remove_monster_from_field(pet_id, initial_monsters_on_field);
								}
							}

							if (!bad_monsters_on_field.empty())
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("bad_monsters_on_field is not empty, removing those IDs from initial_monsters_on_field");
								}
								std::cout << "bad_monsters_on_field is not empty, removing those IDs from initial_monsters_on_field" << std::endl;
								for (flyff::monster &giant : bad_monsters_on_field)
								{
									remove_monster_from_field(giant.id, initial_monsters_on_field);
								}
							}
						
							sort_and_filter_monsters_by_distance(player.pos, initial_monsters_on_field);

							monster_search_time.set_start_time();

						}


						//we should preemptively delete monsters that have invalid targets before we begin the attack seq
						//otherwise, we have to iterate the loop again to find a good monster
						for (flyff::monster &mon : initial_monsters_on_field)
						{
							//bad id
							if (mon.id == 0)
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("Found monster with ID 0, removing");
								}
								std::cout << "Found monster with ID 0, removing" << std::endl;
								remove_monster_from_field(0, initial_monsters_on_field);
							}
							// bad target
							// default player id is 1 to differentiate from unused player vs 0
							if (!(mon.target == 0 || mon.target == player.id || mon.target == healer_player.id))
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("Mon ID " + std::to_string(mon.id) + " target is " + std::to_string(mon.target) + " and it's NOT valid. Removing.");
								}
								std::cout << "Mon ID " + std::to_string(mon.id) + " target is " + std::to_string(mon.target) + " and it's NOT valid. Removing." << std::endl;
								remove_monster_from_field(mon.id, initial_monsters_on_field);
							}
						}


						check_if_navigator_icon_missing(win, navigator_icon_time, log_file);
						check_if_char_screen_icon_missing(win, char_screen_icon_time, log_file);

						///
						///
						///
						//attack sequence
						///
						///
						/// 
						bool sent_up = false;
						bool sent_down = false;
						bool sent_up_2 = false;
						bool sent_down_2 = false;

						if (!player.combat)
						{
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Not in combat, searching for monster to attack");
							}
							std::cout << "Not in combat, searching for monster to attack" << std::endl;
							//attack a monster and remove it from the vector
							if (!initial_monsters_on_field.empty())
							{
								flyff::monster mon_to_attack{};
								//this bool is used to make sure we don't replace the target, and it's also used to contorl how we remove vector elements later
								bool mon_set = false;

								//prefer a monster that is attacking the healer or fighter, if we don't find one, pull one out of the pool
								for (flyff::monster &mon : initial_monsters_on_field)
								{
									if (enable_second_player)
									{
										if (mon.target == healer_player.id)

										{
											if (log_file.log_level_is_verbose())
											{
												log_file.write("Found mon ids " + std::to_string(mon.id) + " is attacking our healer id " + std::to_string(healer_player.id));
											}
											std::cout << "Found mon id " << mon.id << " is attacking our healer id " << healer_player.id << std::endl;
											mon_to_attack = mon;
											mon_set = true;
											mon_targetting_player = true;
											break;
										}
									}
									if (mon.target == player.id)
									{
										if (log_file.log_level_is_verbose())
										{
											log_file.write("Found mon id " + std::to_string(mon.id) + " is attacking our player id " + std::to_string(player.id));
										}
										std::cout << "Found mon id " << mon.id << " is attacking our player id " << player.id << std::endl;
										mon_to_attack = mon;
										mon_set = true;
										mon_targetting_player = true;
										break;
									}
								}

								//if we don't need to defend the player or healer, find a monster to attack
								if (!mon_set)
								{
									mon_to_attack = initial_monsters_on_field.back();
								}
								

								//last_monster_attempted = mon_to_attack.id;
								//We need a valid monster, but also a monster that isn't already in combat
								if (mon_to_attack.id != 0)
								{
									mon_to_attack.update_target(flyff_proc);
									if (log_file.log_level_is_verbose())
									{
										log_file.write("mon_to_attack target is " + std::to_string(mon_to_attack.target));
									}
									std::cout << "mon_to_attack target is " << mon_to_attack.target << std::endl;
									if (mon_to_attack.target == 0 || mon_to_attack.target == player.id || mon_to_attack.target == healer_player.id)
									{
										//if (!monsters_killed.empty())
										//{
											//for (int mon_id_to_check : monsters_killed)
											//{
											//	if (mon_id_to_check == mon_to_attack.id)
											//	{
											//		monster_killed_found = true;

											//		if (initial_monsters_on_field.size() > 1)
											//		{
											//			//there should always be at least 1 item in the vector since we made it this far
											//			int last_mon_check = mon_to_attack.id;
											//			initial_monsters_on_field.pop_back();
											//			mon_to_attack = initial_monsters_on_field.back();
											//			if (mon_to_attack.id != last_mon_check)
											//			{
											//				monster_killed_found = false;
											//			}
											//		}  
											//	}
											//}
										//}

										//if (!monster_killed_found)
										//{
											if (log_file.log_level_is_verbose())
											{
												log_file.write("Setting target as " + std::to_string(mon_to_attack.id) + " in normal fighting");
											}
											std::cout << "Setting target as " << mon_to_attack.id << " in normal fighting" << std::endl;
											player.set_target(mon_to_attack.id);
											send_key_hold_down(0x5A, 50);
											send_key_hold_down(0x53, 50);
											bad_mon_found = player.check_if_text_target_is_bad_mon(all_text_targets);
											if (!bad_mon_found)
											{
												correct_screen_position_for_monster_target(mon_to_attack, player.pos, sent_down, sent_up, sent_down_2, sent_up_2);
												std::this_thread::sleep_for(std::chrono::milliseconds(250));
												if (log_file.log_level_is_verbose())
												{
													log_file.write("Getting target coords");
												}
												std::cout << "Getting target coords" << std::endl;
												target_to_click = get_target_to_click_cords(win);

												if (target_to_click.x != 0 && target_to_click.y != 0)
												{
													if (log_file.log_level_is_verbose())
													{
														log_file.write("Sending escape");
													}
													std::cout << "Sending escape" << std::endl;
													send_key_hold_down(VK_ESCAPE, 50);
													std::this_thread::sleep_for(std::chrono::milliseconds(50));
													if (log_file.log_level_is_verbose())
													{
														log_file.write("Target to click x is " + std::to_string(target_to_click.x) + " y is " + std::to_string(target_to_click.y));
													}
													std::cout << "Ttarget to click x is " << target_to_click.x << " y is " << target_to_click.y << std::endl;
													bool valid_click = check_for_valid_click(target_to_click.y - 5);
													if (valid_click)
													{
														if (log_file.log_level_is_verbose())
														{
															log_file.write("Sending left click");
														}
														std::cout << "Sending left click" << std::endl;
														//I subtract 5 because the target matches aren't exactly centered, and we need to move the target cursor up
														send_left_click(target_to_click.x + 5, target_to_click.y - 5);
														//the target in mem isn't set right away, so we need to pause
														std::this_thread::sleep_for(std::chrono::milliseconds(100));
														player.update_target();
														if (player.target == mon_to_attack.id)
														{
															delete_monster_from_field(initial_monsters_on_field, mon_to_attack, mon_set);
															if (log_file.log_level_is_verbose())
															{
																log_file.write("Sending 6 key");
															}
															std::cout << "Sending 6" << std::endl;
															send_key_hold_down(0x36, 50);
															if (enable_attack_skill)
															{
																send_key_hold_down(attack_skill, 50);
															}
															monsters_killed.push_back(mon_to_attack.id);
														}
														else if (player.target != mon_to_attack.id && player.target != 0)
														{
															for (flyff::monster &mon : initial_monsters_on_field)
															{
																if (player.target == mon.id)
																{
																	std::cout << "player target is " + std::to_string(player.target) + " adding it to monsters_killed" << std::endl;
																	if (log_file.log_level_is_verbose())
																	{
																		log_file.write("player target is " + std::to_string(player.target) + " adding it to monsters_killed");
																	}
																	monsters_killed.push_back(mon.id);
																	break;
																}
															}
															if (log_file.log_level_is_verbose())
															{
																log_file.write("Sending 6 key");
															}
															std::cout << "Sending 6 key" << std::endl;
															send_key_hold_down(0x36, 50);
															if (enable_attack_skill)
															{
																send_key_hold_down(attack_skill, 50);
															}
														}
														else
														{
															if (log_file.log_level_is_verbose())
															{
																log_file.write("Didn't find monster target match");
															}
															std::cout << "Didn't find monster target match" << std::endl;
															send_key_hold_down(0x53, 50);
														}
													}
													else
													{
														if (log_file.log_level_is_verbose())
														{
															log_file.write("Invalid click location, exiting fight sequence");
														}
														std::cout << "Invalid click location, exiting fight sequence" << std::endl;
													}
												}
												else
												{
													if (log_file.log_level_is_verbose())
													{
														log_file.write("Trying to target monster, but can't get coords");
													}
													std::cout << "Trying to target monster, but can't get coords" << std::endl;
												}
											}
											else
											{
												if (log_file.log_level_is_verbose())
												{
													log_file.write("Found giant or violet text in target, skipping monster");

												}
												std::cout << "Found giant or violet text in target, skipping monster" << std::endl;
												bad_monsters_on_field.push_back(mon_to_attack);
												delete_monster_from_field(initial_monsters_on_field, mon_to_attack, mon_set);
											}

										//}
										//else
										//{
											//if (log_file.log_level_is_verbose())
											//{
												//log_file.write("Mon ID  " + std::to_string(mon_to_attack.id) + " already killed, skipping");
											//}
											//std::cout << "Mon ID  " + std::to_string(mon_to_attack.id) + " already killed, skipping" << std::endl;
											//delete_monster_from_field(initial_monsters_on_field, mon_to_attack, mon_set);
										//}
									}
									else
									{
										if (log_file.log_level_is_verbose())
										{
											log_file.write("Mon target " + std::to_string(mon_to_attack.id) + " is not valid");
										}
										std::cout << "Mon target " + std::to_string(mon_to_attack.id) + " is not valid" << std::endl;
										remove_monster_from_field(mon_to_attack.id, initial_monsters_on_field);
										//delete_monster_from_field(initial_monsters_on_field, mon_to_attack, mon_set);
									}

								}
								else
								{
									if (log_file.log_level_is_verbose())
									{
										log_file.write("Mon to attack ID is 0, skipping");
									}
									std::cout << "Mon to attack ID is 0, skipping" << std::endl;
									delete_monster_from_field(initial_monsters_on_field, mon_to_attack, mon_set);
								}
							}
							else
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("No initial_monsters_on_field to attack");
								}
								std::cout << "No initial_monsters_on_field to attack" << std::endl;
							}
						}

						return_screen_position_for_monster_target(sent_down, sent_up, sent_down_2, sent_up_2);

						player.update_combat_status();
			

						if (!last_combat_status && player.combat)
						{
							combat_time.set_start_time();
						}

						if (player.combat)
						{
							if (combat_time.check_if_time_elapsed())
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("Incombat for too long, sending escape");
								}
								std::cout << "Incombat for too long, sending escape" << std::endl;
								flyff::match_result target_result = flyff::match_specific_flyff_item(win, monster_target_templ, flyff::type_of_match::monster_target);
								if (target_result.minVal < 0.01)
								{
									send_key_hold_down(VK_ESCAPE);

								}
								if (initial_monsters_on_field.size() > 1)
								{
									initial_monsters_on_field.pop_back();
								}
								send_key_hold_down(0x53, 50);
								player.combat = false;
							}
						}


						if (!player.combat && !mon_targetting_player)
						{
							//get current position and move if required (every interval)
							if (position_time.check_if_time_elapsed())
							{
								if (log_file.log_level_is_verbose())
								{
									log_file.write("Position interval has passed, checking position");
								}
								std::cout << "Position interval has passed, checking position" << std::endl;
								player.set_position(win);
								exit_monster_target(win, monster_target_templ);
								position_time.set_start_time();
								//initial_monsters_on_field.clear();
							}
						}

						

						//turned off white text OCR because I don't really use it and it wastes CPU time
						/*	flyff_white_alert_text_future = std::async(std::launch::async, match_flyff_text_ocr, flyff_win_gray, 170);
						std::string flyff_white_alert_text = match_flyff_text_ocr(flyff_win_gray, 170);*/


						std::string flyff_red_alert_text = match_flyff_text_ocr(flyff_win_gray, 60);
						//std::string flyff_white_alert_text = flyff_white_alert_text_future.get();
						//std::cout << "flyff_white_alert_text is " << flyff_white_alert_text << std::endl;
						std::cout << "flyff_red_alert_text is " << flyff_red_alert_text << std::endl;

						player.avoid_giant_on_map(win);

						bool disconnected_message_present = false;
						bool cannot_connect_message_present = false;
						bool account_connected_message_present = false;
						bool connecting_message_present = false;

						disconnected_message_present = check_if_disconnected(win, disconnect_message::disconnected, disconnected_time, disconnected_result);
						cannot_connect_message_present = check_if_disconnected(win, disconnect_message::cannot_connect, disconnected_time, disconnected_result);
						account_connected_message_present = check_if_disconnected(win, disconnect_message::account_connected, disconnected_time, disconnected_result);
						connecting_message_present = check_if_disconnected(win, disconnect_message::connecting, disconnected_time, disconnected_result);

						if (disconnected_message_present ||
							cannot_connect_message_present ||
							account_connected_message_present ||
							connecting_message_present)
						{
							if (log_file.log_level_is_verbose())
							{
								log_file.write("Found a disconnected message present, setting the memory initial functions to run again");
							}
							std::cout << "Found a disconnected message present, setting the memory initial functions to run again" << std::endl;
							initial_functions_ran = false;
						}

						//only self buff if we are not in combat
						if (!player.combat)
						{
							// run through every self buff in the vector
							for (self_buff &buff : char_self_buffs)
							{
								buff.current_time = std::chrono::steady_clock::now();
								if (buff.current_time > buff.start_time + buff.skill_interval)
								{
									if (log_file.log_level_is_verbose())
									{
										log_file.write("Sending self buff skill " + buff.skill_key);
									}
									std::cout << "Sending skill " << buff.skill_key << std::endl;
									send_key_hold_down(VK_F2);
									send_key_hold_down(VkKeyScan(buff.skill_key));
									send_key_hold_down(VK_F1);
									buff.start_time = std::chrono::steady_clock::now();
								}
							}
					/*		for (auto it = char_self_buffs.begin(); it != char_self_buffs.end(); ++it)
							{
								(*it).current_time = std::chrono::steady_clock::now();
								if ((*it).current_time > (*it).start_time + (*it).skill_interval)
								{
									if (log_file.log_level_is_verbose())
									{
										log_file.write("Sending self buff skill " + (*it).skill_key);
									}
									std::cout << "Sending skill " << (*it).skill_key << std::endl;
									send_key_hold_down(VK_F2);
									send_key_hold_down(VkKeyScan((*it).skill_key));
									send_key_hold_down(VK_F1);
									(*it).start_time = std::chrono::steady_clock::now();
								}
							}*/
						}


						if (debug_bot)
						{
							cv::Mat hp_mp_fp_debug = debug_hp_mp_fp(win);
							cv::imshow("hp_mp_fp_debug", hp_mp_fp_debug);

							cv::Mat alert_text_debug = debug_alert_text(win);
							cv::imshow("alert_text_debug", alert_text_debug);

							cv::Mat combat_debug = debug_combat(win);
							cv::imshow("combat_debug", combat_debug);

							cv::waitKey(1);
						}


						//if (action_blocked_time.check_if_time_elapsed())
						//{
						//	sent_action_blocked_move = false;
						//}

						//if (!sent_action_blocked_move)
						//{

						//	if (flyff_red_alert_text.std::string::find("been blocked") != std::string::npos)
						//	{
						//		if (log_file.log_level_is_verbose())
						//		{
						//			log_file.write("Found action blocked. Moving away and clearing initial_monsters_on_field.");
						//		}
						//		std::cout << "Found action blocked. Moving away and clearing initial_monsters_on_field." << std::endl;
						//		if (!debug_bot)
						//		{
						//			player.avoid_giant_on_map(win);
						//			turn_run_and_jump(1250);
						//			sent_action_blocked_move = true;
						//			action_blocked_time.set_start_time();
						//			initial_monsters_on_field.clear();
						//		}
						//	}
						//}

						if (player.combat)
						{
							last_combat_status = true;
						}
						else
						{
							last_combat_status = false;
						}
				
			}
		}

		///gui 
		////////////
		ImGui::Render();

		FrameContext *frameCtx = WaitForNextFrameResources();
		UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
		g_pd3dCommandList->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
		g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
		g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		g_pd3dCommandList->ResourceBarrier(1, &barrier);
		g_pd3dCommandList->Close();

		g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList *const *)&g_pd3dCommandList);

		g_pSwapChain->Present(1, 0); 

		UINT64 fenceValue = g_fenceLastSignaledValue + 1;
		g_pd3dCommandQueue->Signal(g_fence, fenceValue);
		g_fenceLastSignaledValue = fenceValue;
		frameCtx->FenceValue = fenceValue;
		///gui
	}
	
	///gui
	///imgui cleanup
	WaitForLastSubmittedFrame();

	// Cleanup
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	///gui

	//clean up bot
	bot_thread.join();

	return 0;
}





// Helper functions for gui

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = NUM_BACK_BUFFERS;
		sd.Width = 0;
		sd.Height = 0;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug *pdx12Debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
		return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != nullptr)
	{
		ID3D12InfoQueue *pInfoQueue = nullptr;
		g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			g_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
			return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 1;
		if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
			return false;

	if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
		g_pd3dCommandList->Close() != S_OK)
		return false;

	if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
		return false;

	g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (g_fenceEvent == nullptr)
		return false;

	{
		IDXGIFactory4 *dxgiFactory = nullptr;
		IDXGISwapChain1 *swapChain1 = nullptr;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
			return false;
		if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
			return false;
		if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
			return false;
		swapChain1->Release();
		dxgiFactory->Release();
		g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
		g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
	}

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, nullptr); g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_hSwapChainWaitableObject != nullptr) { CloseHandle(g_hSwapChainWaitableObject); }
	for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
		if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = nullptr; }
	if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = nullptr; }
	if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
	if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
	if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
	if (g_fence) { g_fence->Release(); g_fence = nullptr; }
	if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }

#ifdef DX12_ENABLE_DEBUG_LAYER
	IDXGIDebug1 *pDebug = nullptr;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
	{
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		pDebug->Release();
	}
#endif
}

void CreateRenderTarget()
{
	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
	{
		ID3D12Resource *pBackBuffer = nullptr;
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
		g_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void CleanupRenderTarget()
{
	WaitForLastSubmittedFrame();

	for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
		if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
}

void WaitForLastSubmittedFrame()
{
	FrameContext *frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue == 0)
		return; // No fence was signaled

	frameCtx->FenceValue = 0;
	if (g_fence->GetCompletedValue() >= fenceValue)
		return;

	g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
	WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext *WaitForNextFrameResources()
{
	UINT nextFrameIndex = g_frameIndex + 1;
	g_frameIndex = nextFrameIndex;

	HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
	DWORD numWaitableObjects = 1;

	FrameContext *frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
	UINT64 fenceValue = frameCtx->FenceValue;
	if (fenceValue != 0) // means no fence was signaled
	{
		frameCtx->FenceValue = 0;
		g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
		waitableObjects[1] = g_fenceEvent;
		numWaitableObjects = 2;
	}

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			WaitForLastSubmittedFrame();
			CleanupRenderTarget();
			HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
			assert(SUCCEEDED(result) && "Failed to resize swapchain.");
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


void cleanup_gui(HWND &hwnd, LPCWSTR &lpClassName, HINSTANCE &hInstance)
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(lpClassName, hInstance);
}