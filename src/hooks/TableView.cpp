#include "QuestomPlaylists.hpp"

#include "HMUI/ScrollView.hpp"
#include "HMUI/TableView.hpp"
#include "System/Tuple_2.hpp"
#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RectTransform.hpp"

using namespace HMUI;
using namespace System;
using namespace UnityEngine;

using TupleType = Tuple_2<int, int>;

// This code actually fixes a bug in the game.
MAKE_HOOK_OFFSETLESS(
    TableView_GetVisibleCellsIdRange,
    TupleType*,
    TableView* self
) {
    Rect rect = self->viewportTransform->get_rect();

    float heightWidth = (self->tableType == TableView::TableType::Vertical) ? rect.get_height() : rect.get_width();
    float position = (self->tableType == TableView::TableType::Vertical) ? self->scrollView->get_position() : -self->scrollView->get_position();

    int min = Mathf::FloorToInt(position / self->cellSize + self->cellSize * 0.001f);
    if (min < 0) {
        min = 0;
    }

    int max = Mathf::FloorToInt((position + heightWidth - self->cellSize * 0.001f) / self->cellSize);
    if (max > self->numberOfCells - 1) {
        max = self->numberOfCells - 1;
    }

    return TupleType::New_ctor(min, max);
}

void QuestomPlaylists::Hooks::TableView() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), TableView_GetVisibleCellsIdRange, il2cpp_utils::FindMethodUnsafe("HMUI", "TableView", "GetVisibleCellsIdRange", 0));
}