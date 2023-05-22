#include "StyleView.h"

#include "IMainWindow.h"
#include "StyleViewModel.h"

#include "IResourceRepository.h"
#include "ITextureRepository.h"

#include <API/GEM_ContentStoreItem.h>
#include <string>

StyleView::StyleView(IMainWindow* parent)
    : BaseView(parent)
    , m_viewModel(nullptr)
    , m_mapFilterIndex(0)
{

}

IViewModel* StyleView::GetViewModel()
{
    return m_viewModel;
}

void StyleView::SetViewModel(IViewModel* viewModel)
{
    m_viewModel = static_cast<StyleViewModel*>(viewModel);
}

static std::string FormatFileSize(gem::LargeInteger sz)
{
    if (sz < 1024 * 1024)
        return gem::String::formatString(u"%.2lf KB", sz / 1024.);

    if (sz < 1024 * 1024 * 1024)
        return gem::String::formatString(u"%.2lf MB", sz / (1024. * 1024.));

    return gem::String::formatString(u"%.2lf GB", sz / (1024. * 1024. * 1024.));
}

void StyleView::Render()
{
    auto resourceRepository = m_viewModel->GetResourceRepository();
    auto textureRepository = m_viewModel->GetTextureRepository();

    m_parentWindow->BeginView("Styles_View");

    if (!resourceRepository->IsResourceAvailable(EResourceType::Style))
    {
        m_parentWindow->LoadingWindow("Loading styles", ImGuiColor_Green, ImGuiColor_Black);
    }
    else
    {
        gem::ContentStoreItemList contentStoreItems = resourceRepository->GetStyles();

        ImGui::SetNextWindowBgAlpha(0.8f);

        const float SCROLL_BAR_SIZE = DPI(30);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, SCROLL_BAR_SIZE);

        static const char* contentStoreFilter[5] = { "All", "Downloaded", "Not downloaded", "In progress", "Paused" };
        m_parentWindow->Combo("##filterstylescombo", contentStoreFilter, IM_ARRAYSIZE(contentStoreFilter), m_mapFilterIndex, []() {});

        if (ImGui::BeginTable("##table_styles", 3, ImGuiTableFlags_ScrollY))
        {
            const ImVec2 STYLE_IMAGE_SIZE(DPI(100), DPI(50));

            static float COLUMN2_SIZE = 0;
            if (COLUMN2_SIZE < 0.1)
                COLUMN2_SIZE = ImGui::CalcTextSize("386.96 KB").x;

            const float COLUMN0_SIZE = STYLE_IMAGE_SIZE.x;
            const float COLUMN1_SIZE = ImGui::GetWindowWidth() - COLUMN0_SIZE - COLUMN2_SIZE - SCROLL_BAR_SIZE - DPI(20);

            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible 
            ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, COLUMN0_SIZE);
            ImGui::TableSetupColumn("Style", ImGuiTableColumnFlags_WidthFixed, COLUMN1_SIZE);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, COLUMN2_SIZE);

            ImGui::TableHeadersRow();

            if (!contentStoreItems.empty())
            {
                int itemIndex = 0;

                for (auto item : contentStoreItems)
                {
                    auto itemState = resourceRepository->GetItemState(item);

                    if (m_mapFilterIndex != 0 && m_mapFilterIndex != (int)itemState)
                        continue;

                    gem::String itemName = item.getName();
                    if (itemState == EItemState::Paused)
                        itemName = gem::String::formatString(u"%s %s", "[PAUSED]", itemName);
                    if (itemState == EItemState::InProgress)
                        itemName = gem::String::formatString(u"[%02d%%] %s", item.getDownloadProgress(), item.getName());

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);

                    if (item.isImagePreviewAvailable())
                    {
                        auto textureId = textureRepository->GetTexture(item.getImagePreview(), STYLE_IMAGE_SIZE.x, STYLE_IMAGE_SIZE.y, false);
                        if (textureId != -1)
                            ImGui::Image((void*)textureId, STYLE_IMAGE_SIZE);
                    }

                    ImGui::TableSetColumnIndex(1);

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (STYLE_IMAGE_SIZE.y - ImGui::GetFontSize()) / 2);

                    switch (itemState)
                    {
                    case EItemState::Unavailable:
                    case EItemState::Paused:
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor_Black);
                        ImGui::BeginDisabled(!m_viewModel->IsConnected() && itemState == EItemState::Paused);

                        if (ImGui::Button(itemName.toStdString().c_str()))
                            resourceRepository->DownloadAsync(item);

                        ImGui::EndDisabled();
                        ImGui::PopStyleColor();
                        break;
                    }
                    case EItemState::Completed:
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor_Black);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor_Green);

                        if (ImGui::Button(itemName.toStdString().c_str()))
                        {
                            m_viewModel->SetStyleId(item.getId());
                            
                        }

                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        break;
                    }
                    case EItemState::InProgress:
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImGuiColor_Black);
                        ImGui::BeginDisabled(true);

                        ImGui::Button(itemName.toStdString().c_str());

                        ImGui::EndDisabled();
                        ImGui::PopStyleColor();
                        break;
                    }
                    }

                    ImGui::TableSetColumnIndex(2);

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (STYLE_IMAGE_SIZE.y - ImGui::GetFontSize()) / 2);

                    ImGui::TextUnformatted(FormatFileSize(item.getTotalSize()).toStdString().c_str());

                    itemIndex++;
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar(1);
    }

    m_parentWindow->EndView();
}

void StyleView::OnEvent(EVmEvent event)
{
    switch (event)
    {
    case EVmEvent::Generic_ChangedConnection:
        break;
    default:
        BaseView::OnEvent(event);
        break;
    }
}

