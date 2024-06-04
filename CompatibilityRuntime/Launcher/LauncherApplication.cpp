#include "LauncherApplication.h"
#include "LauncherPlatform.h"

#include <sstream>
#include <algorithm>

LauncherApplication::LauncherApplication() {
    m_customGameServer[0] = 0;
}

LauncherApplication::~LauncherApplication() = default;

void LauncherApplication::update(LauncherPlatform *platform) {
    if(m_dataPathFS.empty()) {
        m_dataPathFS = platform->dataPath();

        m_dataPath = m_dataPathFS.string();

        std::filesystem::create_directories(m_dataPathFS);
    }

    auto ctx = platform->nuklear();

    if (nk_begin(ctx, "Launcher", platform->screenRect(), 0)) {
        nk_layout_row_dynamic(ctx, 0, 1);

        nk_label(ctx, "Screen resolution:", NK_TEXT_LEFT);

        nk_layout_row_template_begin(ctx, 0);
        nk_layout_row_template_push_static(ctx, ctx->style.font->height);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        nk_spacer(ctx);
        if(nk_option_label(ctx, "Allow the game to select", !m_customizeResolution))
            m_customizeResolution = false;

        nk_spacer(ctx);
        if(nk_option_label(ctx, "As set below", m_customizeResolution))
            m_customizeResolution = true;

        nk_layout_row_template_begin(ctx, 0);
        nk_layout_row_template_push_static(ctx, 2 * ctx->style.font->height);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_end(ctx);

        if(!m_customizeResolution)
            nk_widget_disable_begin(ctx);

        nk_spacer(ctx);

        nk_label(ctx, "Display mode:", NK_TEXT_LEFT);

        const char *items[]{
            "Fullscreen",
            "Borderless",
            "Windowed",
        };

        m_displayMode = static_cast<DisplayMode>(autoSizedCombo(ctx, items, sizeof(items) / sizeof(items[0]), static_cast<int>(m_displayMode)));

        nk_spacer(ctx);

        nk_label(ctx, "Resolution:", NK_TEXT_LEFT);

        if(m_resolutions.empty()) {
            m_resolutions = platform->queryResolutionList();

            std::stable_sort(m_resolutions.begin(), m_resolutions.end(), [](const LauncherPlatform::Resolution &a, const LauncherPlatform::Resolution &b) {
                return a.first * a.second > b.first * b.second;
            });
        }

        m_selectedResolution = autoSizedCombo(ctx, &LauncherApplication::resolutionListItem, m_resolutions.size(), m_selectedResolution);

        if(!m_customizeResolution)
            nk_widget_disable_end(ctx);

        nk_layout_row_dynamic(ctx, 0, 1);

        nk_spacer(ctx);

        if (nk_tree_push(ctx, NK_TREE_TAB, "Advanced settings", NK_MINIMIZED)) {
            nk_layout_row_dynamic(ctx, 0, 1);
            m_disableTouchscreenEmulation = nk_check_label(ctx, "Disable touchscreen emulation", m_disableTouchscreenEmulation);

            nk_layout_row_dynamic(ctx, 0, 2);
            m_useCustomGameServer = nk_check_label(ctx, "Custom game server URL:", m_useCustomGameServer);

            if(!m_useCustomGameServer)
                nk_widget_disable_begin(ctx);

            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, m_customGameServer.data(), m_customGameServer.size(), nullptr);

            if(!m_useCustomGameServer)
                nk_widget_disable_end(ctx);

            nk_layout_row_dynamic(ctx, 0, 1);
            m_useOpenGLES = nk_check_label(ctx, "Use OpenGL ES instead of core profile OpenGL", m_useOpenGLES);

            nk_tree_pop(ctx);
        }

        nk_layout_row_dynamic(ctx, 0, 3);
        nk_spacer(ctx);

        if(nk_button_label(ctx, "Launch")) {
            auto commandLine = buildCommandLine();

            printf("Launching the game with the command line options: ");
            for(const auto &option: commandLine) {
                printf("%s ", option.c_str());
            }

            printf("\n");

            platform->startGameWithCommandLine(commandLine);
        }

        nk_spacer(ctx);

        nk_layout_row_dynamic(ctx, 0, 1);

        nk_label(ctx, "The player data, including the save files, is stored in:", NK_TEXT_LEFT);


        nk_layout_row_template_begin(ctx, 0);
        nk_layout_row_template_push_dynamic(ctx);
        nk_layout_row_template_push_static(ctx, 6 * ctx->style.font->height);
        nk_layout_row_template_end(ctx);

        int len = m_dataPath.size() + 1;
        nk_edit_string(ctx, static_cast<nk_edit_flags>(NK_EDIT_FIELD) | NK_EDIT_READ_ONLY,
                       m_dataPath.data(), &len, len, nullptr);

        if(nk_button_label(ctx, "Open")) {

            platform->openDirectory(platform->dataPath());
        }

        nk_layout_row_dynamic(ctx, 0, 1);

        nk_spacer(ctx);

        nk_label(ctx, "To access the server console, press the '~' key while in the game or on the title screen.", NK_TEXT_LEFT);

    }
}


const char *LauncherApplication::resolutionListItem(int index) {
    if(index < 0 || index >= m_resolutions.size()) {
        m_resolutionString.clear();
    } else {
        const auto &resolution = m_resolutions[index];

        std::stringstream stream;
        stream << resolution.first << "x" << resolution.second;

        m_resolutionString = stream.str();
    }

    return m_resolutionString.c_str();
}

int LauncherApplication::autoSizedCombo(nk_context *ctx, const char **items, int itemCount, int selectedIndex) {

    auto comboBounds = nk_widget_bounds(ctx);
    auto windowSize = nk_window_get_content_region_size(ctx);

    return nk_combo(ctx, items, itemCount, selectedIndex, ctx->style.font->height * 1.5f, nk_vec2(comboBounds.w, windowSize.y - comboBounds.y - comboBounds.h));
}

int LauncherApplication::autoSizedCombo(nk_context *ctx, ComboItemGetter itemGetter, int itemCount, int selectedIndex) {
    ComboCallbackContext context{
        .this_ = this,
        .itemGetter = itemGetter
    };

    auto comboBounds = nk_widget_bounds(ctx);
    auto windowSize = nk_window_get_content_region_size(ctx);

    return nk_combo_callback(ctx, comboItemGetter, &context,
                             selectedIndex, itemCount,
                             ctx->style.font->height * 1.5f, nk_vec2(comboBounds.w, windowSize.y - comboBounds.y - comboBounds.h));
}

void LauncherApplication::comboItemGetter(void *userData, int index, const char** item) {
    auto ctx = static_cast<ComboCallbackContext *>(userData);
    *item = (ctx->this_->*ctx->itemGetter)(index);
}

std::vector<std::string> LauncherApplication::buildCommandLine() const {
    std::vector<std::string> commandLine;

    if(m_customizeResolution) {
        commandLine.emplace_back("-disable-downscale");

        if(m_displayMode == DisplayMode::Window) {
            commandLine.emplace_back("-screen-fullscreen");
            commandLine.emplace_back("0");
        } else {
            commandLine.emplace_back("-screen-fullscreen");
            commandLine.emplace_back("1");

            if(m_displayMode == DisplayMode::Borderless) {
                commandLine.emplace_back("-window-mode");
                commandLine.emplace_back("borderless");
            } else {
                commandLine.emplace_back("-window-mode");
                commandLine.emplace_back("exclusive");
            }
        }

        if(m_selectedResolution >= 0 && m_selectedResolution < m_resolutions.size()) {
            const auto &resolution = m_resolutions[m_selectedResolution];

            commandLine.emplace_back("-screen-width");
            commandLine.emplace_back(std::to_string(resolution.first));
            commandLine.emplace_back("-screen-height");
            commandLine.emplace_back(std::to_string(resolution.second));
        }
    }

    if(m_disableTouchscreenEmulation)
        commandLine.emplace_back("-disable-touch-emulation");

    if(m_useCustomGameServer) {
        commandLine.emplace_back("-gameserver");
        commandLine.emplace_back(m_customGameServer.data());
    }

    if(m_useOpenGLES) {
        commandLine.emplace_back("-force-gles");
    }

#ifndef _WIN32
    commandLine.emplace_back("-logfile");
    commandLine.emplace_back((m_dataPathFS / "Player.log").c_str());
#endif

    return commandLine;
}
