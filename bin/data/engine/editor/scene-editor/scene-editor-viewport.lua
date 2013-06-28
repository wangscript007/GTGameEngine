-- Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

function GTGUI.Element:SceneEditorViewport(sceneEditor)
    self:DefaultEditor3DViewport();
    
    function self:OnViewportLMBUpWithoutMovement()
        sceneEditor:DoMouseSelection();
    end
    
    function self:OnViewportRMBUpWithoutMovement()
        sceneEditor.ContextMenu:SetPosition(GTGUI.Server.GetMousePosition());
        sceneEditor.ContextMenu:Show();
    end
    
    function self:OnViewportLMBDown()
        return sceneEditor:TryGizmoMouseSelect();
    end
    

    return self;
end