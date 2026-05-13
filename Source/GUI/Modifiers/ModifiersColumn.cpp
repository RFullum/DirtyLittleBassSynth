/*
  ==============================================================================

    ModifiersColumn.cpp

  ==============================================================================
*/

#include "ModifiersColumn.h"

//============================================================

ModifiersColumn::ModifiersColumn(GuiResources &res)
: modifierPanel(res)
{
    setOpaque(false);
    addAndMakeVisible(modifierPanel);
}

void ModifiersColumn::resized()
{
    modifierPanel.setBounds(getLocalBounds());
}
