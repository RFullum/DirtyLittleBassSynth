/*
  ==============================================================================

    ModifiersColumn.cpp

  ==============================================================================
*/

#include "ModifiersColumn.h"

//============================================================

ModifiersColumn::ModifiersColumn(GuiResources &res)
: resources    (res)
, modifierPanel(res)
{
    addAndMakeVisible(modifierPanel);
}

void ModifiersColumn::resized()
{
    modifierPanel.setBounds(getLocalBounds());
}
