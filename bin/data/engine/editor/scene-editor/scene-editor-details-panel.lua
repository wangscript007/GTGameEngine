-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorDetailsPanel(parentPanel, sceneEditor)
    self:PanelGroupBox("Details");
    self.ParentPanel = parentPanel;
    
    self.NameContainer = GTGUI.Server.New("<div parentid='" .. self.Body:GetID()          .. "' style='width:100%; height:auto; child-plane:horizontal; flex-child-width:true; vertical-align:center;' />");
    self.NameLabel     = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' style='width:auto; margin-right:4px; text-color:std-text-color;'>Name</div>");
    self.NameTextBox   = GTGUI.Server.New("<div parentid='" .. self.NameContainer:GetID() .. "' styleclass='textbox' style='width:100%;' />");
    
    self.PrefabContainer = GTGUI.Server.CreateElement(self.Body,            "scene-editor-details-panel-prefab-container");
    self.PrefabLabel     = GTGUI.Server.CreateElement(self.PrefabContainer, "scene-editor-details-panel-prefab-label");
    self.PrefabLabel:SetText("Prefab");
    self.PrefabName      = GTGUI.Server.CreateElement(self.PrefabContainer, "scene-editor-details-panel-prefab-name");
    self.PrefabName:SetText("None");
    
    self.SceneNode = nil;
    
    
    function self:Update(node)
        self.SceneNode = node;
        self.NameTextBox:SetText(node:GetName());
    end
    
    
    self.NameTextBox:OnKeyPressed(function(data)
        if data.key == GTCore.Keys.Enter then
            self.SceneNode:SetName(self.NameTextBox:GetText());
            self.ParentPanel:OnSceneNodeChanged();
        end
    end);
    
    return self;
end