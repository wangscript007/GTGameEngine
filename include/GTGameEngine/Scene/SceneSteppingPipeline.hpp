// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneSteppingPipeline
#define GT_SceneSteppingPipeline

namespace GT
{
    class Scene;
    class SceneNode;

    /// Base class for scene stepping pipelines.
    class SceneSteppingPipeline
    {
    public:

        /// Destructor.
        virtual ~SceneSteppingPipeline() {}


        /// Inserts a scene node.
        virtual void InsertSceneNode(SceneNode* pSceneNode) = 0;

        /// Removes a scene node.
        virtual void RemoveSceneNode(SceneNode* pSceneNode) = 0;


        /// Performs the step.
        virtual void Step(double deltaTimeInSeconds) = 0;

        /// Determines whether or not the pipeline is currently stepping.
        virtual bool IsStepping() const = 0;
    };
}

#endif
