﻿//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstring>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "glad.h"

//browser
#include "gui.hpp"
#include "core.hpp"
#include "configfile.hpp"
#include "fileutils.hpp"
#include "render.hpp"
#include "networkmanager.hpp"
#include "htmlparser.hpp"
#include "content.hpp"

using Core::Browser;
using Files::ConfigFile;
using Utils::File;
using Graphics::Render;
using Networking::NetworkManager;
using Networking::HTMLParser;
using Graphics::Content;

using std::filesystem::exists;
using std::filesystem::path;
using std::cout;
using std::thread;
using std::lock_guard;
using std::atomic;
using std::mutex;
using std::strncpy;

namespace GUI
{
	static ImVec4 bgrColor;
	static ImVec4 cubeColor;
	static ImVec4 specularColor;

	atomic<bool> isPageLoading(false);
	mutex loadingMutex;

	void GUI_Browser::Initialize()
	{
		//copies template file to documents folder if imgui file does not exist
		string imguiConfigFile = (path(Browser::docsPath) / "imgui.ini").string();
		string imguiTemplateFile = (path(Browser::filesPath) / "imgui.ini").string();
		if (!exists(imguiConfigFile))
		{
			File::CopyFileOrFolder(imguiTemplateFile, imguiConfigFile);

			ConfigFile::SaveConfigFile();
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = (path(Browser::docsPath) / "imgui.ini").string();
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF(((path(Browser::filesPath) / "fonts" / "coda" / "Coda-Regular.ttf").string()).c_str(), 16.0f);

		SetStyle();

		isImguiInitialized = true;

		cout << "ImGui initialized successfully!\n";
	}

	void GUI_Browser::SetStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = stof(ConfigFile::GetValue("gui_fontScale"));

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 4.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 4.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 4.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 4.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 4.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 20.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.1630901098251343f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.08627451211214066f, 0.08627451211214066f, 0.08627451211214066f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.105882354080677f, 0.105882354080677f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1137254908680916f, 0.1137254908680916f, 0.1137254908680916f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8784313797950745f, 0.8784313797950745f, 0.8784313797950745f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9803921580314636f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.1490196138620377f, 0.1490196138620377f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2470588237047195f, 0.2470588237047195f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3294117748737335f, 0.3294117748737335f, 0.3294117748737335f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.3098039329051971f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 0.7803921699523926f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7490196228027344f, 0.7490196228027344f, 0.7490196228027344f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 0.6705882549285889f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9764705896377563f, 0.9764705896377563f, 0.9764705896377563f, 0.9490196108818054f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.2235294133424759f, 0.2235294133424759f, 0.2235294133424759f, 0.8627451062202454f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.321568638086319f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 0.9725490212440491f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4235294163227081f, 0.4235294163227081f, 0.4235294163227081f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
	}

	void GUI_Browser::SetSearchBarText(const string& text)
	{
		string correctUrl = "/" + (path(text)).stem().string();
		strncpy(websiteChar, correctUrl.c_str(), static_cast<size_t>(bufferSize) - 1);
		websiteChar[bufferSize - 1] = '\0';
	}

	ImVec2 GUI_Browser::CenterWindow(const ImVec2& size)
	{
		int intWidth, intHeight;
		glfwGetFramebufferSize(Render::window, &intWidth, &intHeight);

		float glfwWindowWidth = static_cast<float>(intWidth);
		float glfwWindowHeight = static_cast<float>(intHeight);

		float posX = (glfwWindowWidth - size.x) / 2.0f;
		float posY = (glfwWindowHeight - size.y) / 2.0f;

		return ImVec2(posX, posY);
	}

	void GUI_Browser::Render()
	{
		if (isImguiInitialized)
		{
			if (!Browser::IsUserIdle())
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				RenderTopBar();

				ImGuiDockNodeFlags dockFlags =
					ImGuiDockNodeFlags_PassthruCentralNode;

				ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

				RenderBrowserTopWindow();

				ImGui::Render();
			}

			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData != nullptr)
			{
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	void GUI_Browser::RenderTopBar()
	{
		float fontScale = stof(ConfigFile::GetValue("gui_fontScale"));

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("test1"))
			{

			}

			ImGui::EndMenu();
		}

		ImGui::SameLine(40 * fontScale * 0.75f);

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("test2"))
			{

			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void GUI_Browser::RenderBrowserTopWindow()
	{
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings;

		int fbWidth, fbHeight;
		glfwGetFramebufferSize(Render::window, &fbWidth, &fbHeight);

		ImGui::SetNextWindowPos(ImVec2(0.0f, 30.0f));
		ImGui::SetNextWindowSize(ImVec2(fbWidth, 80.0f), ImGuiCond_Always);

		if (ImGui::Begin("TopWindow", NULL, windowFlags))
		{
			if (ImGui::InputText("##websiteSearch", websiteChar, bufferSize, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				websiteString = string(websiteChar);
				cout << "Searching for '" << websiteString << "'!\n";

				LoadPageInThread(websiteString);
			}

			ImGui::End();
		}
	}

	void GUI_Browser::LoadPageInThread(const string& URL)
	{
		lock_guard<mutex> lock(loadingMutex);

		if (isPageLoading)
		{
			cout << "Error: Cannot load page! Old page is still loading...\n";
			return;
		}

		isPageLoading = true;

		thread([URL]() 
		{
			NetworkManager::ParseURL(URL);

			//ensure isPageLoading is reset only when the page finishes loading
			{
				lock_guard<mutex> lock(loadingMutex);
				isPageLoading = false;
			}
		}).detach();
	}

	void GUI_Browser::Shutdown()
	{
		if (isImguiInitialized)
		{
			isImguiInitialized = false;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}
}