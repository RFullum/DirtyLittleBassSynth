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
    addAndMakeVisible(modifierPanel);
}

void ModifiersColumn::resized()
{
    modifierPanel.setBounds(getLocalBounds());
}
