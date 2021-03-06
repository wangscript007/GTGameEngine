// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneNode
#define GT_SceneNode

#include "SceneNodeEventHandler.hpp"

// Though not required for compilation of this file, we will include the components here to make using the SceneNode class a bit easier. They're
// pretty light-weight, so they don't add too much time to compilation.
#include "Components/CameraComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/ModelComponent.hpp"
#include "Components/DynamicsComponent.hpp"
#include "Components/ProximityComponent.hpp"
#include "Components/OccluderComponent.hpp"
#include "Components/GenericConstraintComponent.hpp"
#include "Components/ConeTwistConstraintComponent.hpp"
#include "Components/PointToPointConstraintComponent.hpp"
#include "Components/ParticleSystemComponent.hpp"
#include "Components/ScriptComponent.hpp"
#include "Components/PrefabComponent.hpp"
#include "Components/EditorMetadataComponent.hpp"

#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/List.hpp>
#include <GTGE/Core/Map.hpp>
#include <GTGE/Core/String.hpp>

#undef GetNextSibling
#undef GetPrevSibling
#undef GetFirstChild
#undef GetClassName

namespace GT
{
    class Scene;            // <-- Needed by SceneNode.
    

    /// Class representing a scene node.
    ///
    /// Scene nodes have a single parent and many children. The positions of scenes nodes are always relative to the parent
    /// node. Helper functions exist for retrieving and setting the world transformation of the node.
    ///
    /// Nodes have a series of Components attached to them. The components essentially define what the node actually does. For
    /// example, if the node has a point light component attached, it means the node will emit a point light.
    ///
    /// Nodes also have a list (or map, specifically) or data pointers. It is through these data pointers that the node can
    /// have custom data attached to it. There can be many data pointers attached to a node. Each different data pointer is
    /// distinguished with a key. When retrieving or setting a data pointer, you must also specify a key for that data pointer.
    /// The node itself does not use any predefined keys, but if using classes/objects which also use a scene node, you must
    /// ensure your data pointer keys do not conflict.
    class SceneNode
    {
    public:

        /// Constructor.
        SceneNode();

        /// Destructor.
        ///
        /// The destructor will orphan/detach any children that are still attached. Children must be deleted manually.
        ///
        /// The destructor will detach the node from it's parent, if it has one. After being detached from
        /// the parent, it will call the OnDestroy() event. OnDestroy() is called before the constructor orphans
        /// the child nodes.
        ///
        /// The destructor will delete any attached components. The destructor does not delete the attached event
        /// handler or data pointers.
        virtual ~SceneNode();


        /// Retrieves the name of the node.
        const char* GetName() const;

        /// Sets the name of the node.
        void SetName(const char* name);


        /// Retrieves the ID of the scene node.
        ///
        /// @remarks
        ///     The ID of the scene node is set by the scene it is contained in.
        uint64_t GetID() const;

        /// Sets the ID of the scene node.
        ///
        /// @param newID [in] The new ID of the scene node.
        ///
        /// @remarks
        ///     This should only ever be used internally by Scene. Do not call this directly.
        void SetID(uint64_t newID);



        ////////////////////////////////////////////////
        // Hierarchy

        /// Retrieves the parent node, or null if the node does not have a parent.
              SceneNode* GetParent();
        const SceneNode* GetParent() const;


        /// Retrieves a pointer to the first child node.
              SceneNode* GetFirstChild();
        const SceneNode* GetFirstChild() const;

        /// Retrieves a pointer to the last child node.
              SceneNode* GetLastChild();
        const SceneNode* GetLastChild() const;

        /// Retrieves a pointer to the previous sibling. Returns null if the node is the first of it's siblings, or does not have any.
              SceneNode* GetPrevSibling();
        const SceneNode* GetPrevSibling() const;

        /// Retrieves a pointer to the next sibling. Returns null if the node if the last of it's siblings, or does not have any.
              SceneNode* GetNextSibling();
        const SceneNode* GetNextSibling() const;


        /// Retrieves the top-level ancestor.
        ///
        /// @remarks
        ///     Returns <this> if the node has not parents.
              SceneNode* GetTopAncestor();
        const SceneNode* GetTopAncestor() const;


        /// Retrieves the number of children.
        ///
        /// @remarks
        ///     This runs in O(n) time.
        size_t GetChildCount() const;


        /// Attaches an event handler to the node.
        ///
        /// @param  handler [in] A reference to the event handler to attach to the node.
        ///
        /// @remarks
        ///     This function will not attach the handler if it is already attached.
        ///     @par
        ///     The event handler can not be deleted while it is in scope. A node will not make a copy of the event handler. Instead
        ///     it just references the object directly. It is done this way so that a single event handler can be used across multiple
        ///     nodes, thus saving a bit of memory.
        void AttachEventHandler(SceneNodeEventHandler &handler);

        /// Detaches an event handler from the node.
        ///
        /// @param  handler [in] A reference to the event handler to detach.
        ///
        /// @remarks
        ///     Note that this function does not actually delete the event handler. Deletion must be managed by the caller.
        void DetachEventHandler(SceneNodeEventHandler &handler);


        /// Attaches a child to this node.
        ///
        /// @param  childNode [in] A pointer to the child node to attach to this node.
        ///
        /// @remarks
        ///     If the child is already attached to another parent, it is first detached from that parent. OnDetach() will be called
        ///     like normal.
        ///     @par
        ///     If the child is already attached to a different scene as this node, it will be removed from that scene.
        void AttachChild(SceneNode &childNode, bool keepWorldTransform = false);

        /// Attaches this node to another node.
        ///
        /// @param  parent [in] A pointer to the parent node that this node should be attached to.
        ///
        /// @remarks
        ///     This function is the same as Attach(), only the other way around. This is more intuitive in some cases.
        void AttachTo(SceneNode &parent, bool keepWorldTransform = false);

        /// Detaches/orphans a child from the node.
        ///
        /// @param  childNode                     [in] A pointer to the child node to detach.
        /// @param  postParentChangedEventToScene [in] Controls whether or not we post an OnParentChanged event to the 
        ///
        /// @remarks
        ///     This method does NOT delete the child.
        void DetachChild(SceneNode &childNode, bool keepWorldTransform = false, bool postParentChangedEventToScene = true);

        /// Safely detaches/orphans all children.
        void DetachAllChildren(bool keepWorldTransform = false);

        /// Helper function for detaching this node from it's parent.
        void DetachFromParent(bool keepWorldTransform = false, bool postParentChangedEventToScene = true);

        /// Retrieves the first child who has the given name, or nullptr if a child is not found.
        ///
        /// @param  name [in] The name of the child to return.
        /// @param  recursive [in] Whether or not to do a deep recursive search. False by default.
        SceneNode* FindFirstChild(const char* name, bool recursive = false);

        /// Retrieves the first child node with the given component.
        SceneNode* FindFirstChildWithComponent(const char* componentName, bool recursive = false);

        template <typename T>
        SceneNode* FindFirstChildWithComponent(bool recursive = false)
        {
            return this->FindFirstChildWithComponent(T::Name, recursive);
        }


        /// Determines whether or not the given node is an ancestor.
        bool IsAncestor(const SceneNode &other) const;

        /// Determines whether or not the given node is a descendant.
        bool IsDescendant(const SceneNode &other) const;

        /// Determines whether or not the given scene node is related to this node (is an ancestor or descendant).
        bool IsRelated(const SceneNode &other) const { return this->IsAncestor(other) || this->IsDescendant(other); }



        ////////////////////////////////////////////////
        // Transformation

        /// Retrieves the position of the object.
        const glm::vec3 & GetPosition() const { return m_position; }

        /// Sets the position of the node relative to the parent.
        ///
        /// @param  position             [in] The new relative position of the node.
        /// @param  updateDynamicsObject [in] Whether or not the attached dynamics object should be updated.
        void SetPosition(const glm::vec3 &position, bool updateDynamicsObject);
        void SetPosition(float x, float y, float z, bool updateDynamicsObject) { this->SetPosition(glm::vec3(x, y, z), updateDynamicsObject); }

        void SetPosition(const glm::vec3 &position) { this->SetPosition(position, true); }
        void SetPosition(float x, float y, float z) { this->SetPosition(glm::vec3(x, y, z), true); }
        

        /// Retrieves the world/absolute position of node.
        glm::vec3 GetWorldPosition() const;

        /// Sets the world/absolute position of the node.
        void SetWorldPosition(const glm::vec3 &worldPosition, bool updateDynamicsObject = true);
        void SetWorldPosition(float x, float y, float z, bool updateDynamicsObject = true) { this->SetWorldPosition(glm::vec3(x, y, z), updateDynamicsObject); }



        /// Retrieves the orientation of the object.
        const glm::quat & GetOrientation() const { return m_orientation; }

        /// Sets the orientation of the node relative to the parent.
        ///
        /// @param  orientation [in] The new orientation of the node.
        void SetOrientation(const glm::quat &orientation, bool updateDynamicsObject);
        void SetOrientation(const glm::quat &orientation) { this->SetOrientation(orientation, true); }
        

        /// Retrieves the world/absolute orientation of the node.
        glm::quat GetWorldOrientation() const;

        /// Sets the world/absolute orientation of the node.
        void SetWorldOrientation(const glm::quat &worldOrientation, bool updateDynamicsObject = true);



        /// Retrieves the scale of the object.
        const glm::vec3 & GetScale() const { return m_scale; }

        /// Sets the scale of the node relative to the parent.
        ///
        /// @param  scale [in] The new scale of the node relative to the parent.
        void SetScale(const glm::vec3 &scaleIn);
        void SetScale(float x, float y, float z) { this->SetScale(glm::vec3(x, y, z)); }


        /// Retrieves the world/absolute scale of the node.
        glm::vec3 GetWorldScale() const;

        /// Sets the world/absolute scale of the node.
        ///
        /// @param  worldScale [in] The new world/absolute scale of the node.
        void SetWorldScale(const glm::vec3 &worldScale);
        void SetWorldScale(float x, float y, float z) { this->SetWorldScale(glm::vec3(x, y, z)); }



        /// Looks at a point in the world.
        ///
        /// @param  target [in] The position in the world to look at.
        /// @param  up     [in] The up direction. Defaults to (0, 1, 0). Must be normalized.
        ///
        /// @remarks
        ///     For efficiency, the up vector is not normalized internally.
        void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
        void LookAt(float targetX, float targetY, float targetZ, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) { this->LookAt(glm::vec3(targetX, targetY, targetZ), glm::vec3(upX, upY, upZ)); }

        /// Looks at another scene node.
        ///
        /// @param  target [in] The other scene node to look at.
        /// @param  up     [in] The up direction. Defaults to (0, 1, 0). Must be normalized.
        void LookAt(const SceneNode &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));


        /// Causes the scene node to orientation itself to look in the given direction.
        ///
        /// @param direction [in] The direction to look in.
        /// @param up        [in] The up direction. Defaults to (0, 1, 0). Must be normalized.
        void LookInDirection(const glm::vec3 &direction, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));
        void LookInDirection(float directionX, float directionY, float directionZ, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) { this->LookInDirection(glm::vec3(directionX, directionY, directionZ), glm::vec3(upX, upY, upZ)); }


        /// Retrieves the normalised vector pointing in the forward direction of the node, in world coordinates.
        glm::vec3 GetWorldForwardVector() const;

        /// Retrieves the normalised vector pointing in the right direction of the node, in world coordinates.
        glm::vec3 GetWorldRightVector() const;

        /// Retrieves the normalised vector pointing in the up direction of the node, in world coordinates.
        glm::vec3 GetWorldUpVector() const;


        /// Retrieves the position, orientation and scale components of the scene node in a single call.
        void GetWorldTransformComponents(glm::vec3 &position, glm::quat &orientation, glm::vec3 &scale) const;

        /// Sets the position, orientation and scale components of the scene node in a single call.
        void SetWorldTransformComponents(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, bool updateDynamicsObject = true);


        /// Calculates a transformation matrix for this object, in world space.
        glm::mat4 GetWorldTransform() const;

        /// Retrieves the world transform, not including the scale.
        glm::mat4 GetWorldTransformWithoutScale() const;

        /// Retrieves the Bullet transform for use with physics.
        ///
        /// @param  worldTransform [out] A reference to the Bullet btTransform object that will receive the world transformation.
        void GetWorldTransform(btTransform &worldTransform) const;

        /// Sets the world transform of the scene node based on the given btTransform object.
        ///
        /// @param  worldTransform [in] A reference to the btTransform object containing the new world transformation.
        /// @param  offset         [in] Physics objects can be given a center of mass offset. This represents that offset.
        void SetWorldTransform(const btTransform &worldTransform, bool updateDynamicsObject = true);



        /// Translates the object.
        ///
        /// @param offset [in] The amount to translate on each axis.
        ///
        /// @remarks
        ///     This does a local transformation where the orientation of the object is taken into acount.
        void Translate(const glm::vec3 &offset);

        /// Rotates the node.
        ///
        /// @param angleDegrees [in] The amount of degrees to rotate the object.
        /// @param axis         [in] The axis to rotate around.
        void Rotate(float angleDegrees, const glm::vec3 &axis);

        /// Scales the node.
        ///
        /// @param scale [in] The amount to scale the object.
        void Scale(const glm::vec3 &scale);


        /// Moves the object in the direction of its forward vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveForward(float distance) { this->Translate(glm::vec3(0.0f, 0.0f, -distance)); }

        /// Moves the object in the direction of its right vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveRight(float distance) { this->Translate(glm::vec3(distance, 0.0f, 0.0f)); }

        /// Moves the object in the direction of its up vector.
        ///
        /// @param  distance [in] The distance to move.
        void MoveUp(float distance) { this->Translate(glm::vec3(0.0f, distance, 0.0f)); }


        /// Rotates the object around it's local x axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateX(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f)); }

        /// Rotates the object around it's local y axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateY(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f)); }

        /// Rotates the object around it's local z axis.
        ///
        /// @param  angleDegress [in] The angle in degrees to rotate.
        void RotateZ(float angleDegrees) { this->Rotate(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f)); }



        /// Performs a linear interpolation of the object's position.
        ///
        /// @param dest [in] The destination position.
        /// @param a    [in] The factor controlling how far to move towards <dest>. A value of 1.0 will move the object to <dest>, whereas a value of 0.0 will not move it at all.
        void InterpolatePosition(const glm::vec3 &dest, float a);

        /// Performs a spherical-linear interpolation of the object's rotation.
        ///
        /// @param dest [in] The destination orientation.
        /// @param a    [in] The factor controlling how far to rotate towards <dest>.
        ///
        /// @remarks
        ///     Use Slerp() to perform a shortest-path interpolation.
        void InterpolateOrientation(const glm::quat &dest, float a);

        /// Performs a linear interpolation of the object's scale.
        ///
        /// @param dest [in] The destination scale.
        /// @param a    [in] The factor controlling how far to scale towards <dest>.
        void InterpolateScale(const glm::vec3 &dest, float a);

        /// Performs a shortest-path spherical-linear interpolation of the objects orientation.
        ///
        /// @param dest [in] The destination orientation.
        /// @param a    [in] The interpolation delta.
        void Slerp(const glm::quat &dest, float a);


        
        /// Clampes the relative position of the scene node.
        ///
        /// @param posMin [in] The min relative position.
        /// @param posMax [in] The max relative position.
        void ClampPosition(const glm::vec3 &posMin, const glm::vec3 &posMax);

        /// Clampes the world position of the scene node.
        ///
        /// @param posMin [in] The min world position.
        /// @param posMax [in] The max world position.
        void ClampWorldPosition(const glm::vec3 &posMin, const glm::vec3 &posMax);

        /// Clampes the relative scale of the scene node.
        ///
        /// @param scaleMin [in] The min relative scale.
        /// @param scaleMax [in] The max relative scale.
        void ClampScale(const glm::vec3 &scaleMin, const glm::vec3 &scaleMax);

        /// Clampes the world position of the scene node.
        ///
        /// @param scaleMin [in] The min world scale.
        /// @param scaleMax [in] The max world scale.
        void ClampWorldScale(const glm::vec3 &scaleMin, const glm::vec3 &scaleMax);



        /// Retrieves a normalised vector pointing in the forward direction of the object.
        glm::vec3 GetForwardVector() const;

        /// Retrieves a normalised vector pointing in the right direction of the object.
        glm::vec3 GetRightVector() const;

        /// Retrieves a normalised vector pointing in the up direction of the object.
        glm::vec3 GetUpVector() const;


        /// Rotates the object about the world axis.
        ///
        /// @param angle [in] The angle of rotation.
        /// @param axis  [in] The axis of rotation.
        void RotateAroundWorldAxis(float angle, const glm::vec3 &axis);

        /// Rotates the object about the world axis around the given pivot point.
        ///
        /// @param angle [in] The angle of rotation.
        /// @param axis  [in] The axis of rotation.
        /// @param pivot [in] The pivot point to rotate around.
        void RotateAtPivotAroundWorldAxis(float angle, const glm::vec3 &axis, const glm::vec3 &pivot);


        /// Calculates a transformation matrix for this object.
        ///
        /// @param dest [in] A reference to the matrix that will receive the transformation.
        ///
        /// @remarks
        ///     This is not a simple accessor, but rather a calculate of the transform from the position, orientation and scale.
        void GetTransform(glm::mat4 &dest);





        ////////////////////////////////////////////////
        // Components

        /// Retrieves a pointer to the component as specified by 'T'.
        ///
        /// @return A pointer to the component as specified by 'T", or null if the component does not exist.
        template <typename T>
        T * GetComponent()
        {
            return static_cast<T*>(this->GetComponentByName(T::Name));
        }

        template <typename T>
        const T* GetComponent() const
        {
            return static_cast<const T*>(this->GetComponentByName(T::Name));
        }


        /// Retrievse a component based on it's name.
        ///
        /// @param  name [in] The name of the component to retrieve.
        ///
        /// return A pointer to the component whose name is that of 'name'. Returns null if no such component exists.
              Component* GetComponentByName(const char* name);
        const Component* GetComponentByName(const char* name) const { return const_cast<SceneNode*>(this)->GetComponentByName(name); }


        /// Adds a component of the type given by 'T'.
        ///
        /// @return A pointer to the new component.
        ///
        /// @remarks
        ///     If a component of the same type already exists, the existing one is returned and is NOT overwritten.
        template <typename T>
        T* AddComponent()
        {
            return static_cast<T*>(this->AddComponentByName(T::Name));
        }

        /// Adds a component by it's name.
        ///
        /// @param name [in] The name of the component to instantiate and add.
        ///
        /// @remarks
        ///     This will use CreateComponentByName() to do the instantiation, which will in turn call Game::CreateCustomComponent() if it fails.
        Component* AddComponentByName(const char* name);


        /// Removes the component of the type given by 'T'.
        template <typename T>
        void RemoveComponent()
        {
            this->RemoveComponentByName(T::Name);
        }

        /// Removes a component by it's name.
        ///
        /// @param componentName [in] The name of the component to remove.
        void RemoveComponentByName(const char* componentName);

        /// Removes every component.
        void RemoveAllComponents();

        
        /// Determines whether or not the node has the component as specified by 'T'.
        ///
        /// @return True if the scene node has the component; false otherwise.
        inline bool HasComponent(const char *componentName) const
        {
            return this->GetComponentByName(componentName) != nullptr;
        }

        template <typename T>
        bool HasComponent() const
        {
            return this->HasComponent(T::Name);
        }


        /// Retrieves a list containing the names of the components that are currently attached to the scene node.
        void GetAttachedComponentNames(Vector<String> &output) const;





        /// Retrieves the data pointer for the given key.
        ///
        /// @remarks
        ///     If a data pointer at the given key can not be found, null is returned, casted to T.
        ///     @par
        ///     Ideally, this should not be called too often since it needs to access a map.
        template <typename T>
        T* GetDataPointer(size_t key)
        {
            auto item = this->dataPointers.Find(key);
            if (item != nullptr)
            {
                return static_cast<T*>(item->value);
            }

            return nullptr;
        }

        /// Sets the data pointer for the given key.
        void SetDataPointer(size_t key, void *data)
        {
            this->dataPointers.Add(key, data);
        }

        /// Removes a data pointer from the scene node for the given key.
        ///
        /// Note that this does not delete the data at the given pointer. This is the responsibility of the caller.
        void RemoveDataPointer(size_t key)
        {
            this->dataPointers.RemoveByKey(key);
        }



        /// Retrieves the scene this scene node is attached to, if any.
        ///
        /// @remarks
        ///     This will transcend the parent nodes if required.
              Scene* GetScene();
        const Scene* GetScene() const;

        /// Simple scene setting for internal use only.
        ///
        /// @param newScene [in] A pointer to the new scene.
        ///
        /// @remarks
        ///     This should only be used internally by Scene.
        void _SetScene(Scene* newScene);


        /// A helper for removing the scene node from it's current scene.
        void RemoveFromScene();


        /// Retrieves a pointer to the game that owns this scene node.
        Context* GetContext();



        /// Sets whether or not the scene node is static.
        void SetStatic(bool isStatic);

        /// Determines whether or not the node is static.
        bool IsStatic() const;

        

        /// Sets whether or not the scene node is visible.
        void SetVisible(bool isVisible);

        /// Makes the scene node visible.
        inline void Show() { this->SetVisible(true); }

        /// Makes the scene node invisible.
        inline void Hide() { this->SetVisible(false); }

        /// Determines if the scene node is visible.
        ///
        /// @param  recursive [in] Whether or not the visibility should be check recursively. If a parent is invisible, so is it's children.
        bool IsVisible(bool recursive = true) const;

        

        /// Disables position inheritance.
        void DisablePositionInheritance();

        /// Enables position inheritance.
        void EnablePositionInheritance();

        /// Determines whether or not position inheritance is enabled.
        bool IsPositionInheritanceEnabled() const;


        /// Disables orientation inheritance.
        void DisableOrientationInheritance();

        /// Enables orientation inheritance.
        void EnableOrientationInheritance();

        /// Determines whether or not orientation inheritance is enabled.
        bool IsOrientationInheritanceEnabled() const;


        /// Disables scale inheritance.
        void DisableScaleInheritance();

        /// Enables scale inheritance.
        void EnableScaleInheritance();

        /// Determines whether or not scale inheritance is enabled.
        bool IsScaleInheritanceEnabled() const;


        /// Sets the flags of the scene node.
        ///
        /// @remarks
        ///     In order for flags to take effect, the scene node should be removed and then re-added to the scene.
        ///     @par
        ///     The accepted flags are those defined in SceneNode::Flags.
        void SetFlags(unsigned int newFlags) { m_flags = newFlags; }

        /// Retrieves the scene node's flags.
        ///
        /// @return The scene node's current flags.
        unsigned int GetFlags() const { return m_flags; }



        //////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the scene node, ignoring children.
        ///
        /// @param serializer [in] The serializer to write the data to.
        ///
        /// @remarks
        ///     This ignores children. Serialization of children should be done at a higher level.
        ///     @par
        ///     This will serialize every attached component also.
        void Serialize(Serializer &serializer, unsigned int flags = 0) const;

        /// Deserializes the scene node, ignoring children.
        ///
        /// @param deserializer [in] The deserializer to read the data from.
        ///
        /// @remarks
        ///     This will not deserialize children. That should be done at a higher level.
        ///     @par
        ///     This is the opposite of Serialize(), so for any rule that applies to Serialize(), it will also apply here.
        ///     @par
        ///     Sometimes a scene will be deserialized before it's added to a scene. Therefore, the a reference to the scene that the
        ///     scene node will eventually be made a part of will be passed to Deserialize().
        void Deserialize(Deserializer &deserializer, unsigned int flags = 0);

        /// A special deserialization method for deserializing just the ID of the scene node, but leaving the read pointer as it.
        ///
        /// @param deserializer [in] The deserializer to retrieve the ID from.
        ///
        /// @return True if the ID was found; false otherwise.
        ///
        /// @remarks
        ///     This not change the state of either the deserializer nor the scene node. Indeed, this is actually a static function.
        ///     @par
        ///     If this function returns false, 'idOut' will be left unmodified.
        static bool Deserialize_PeekID(Deserializer &deserializer, uint64_t &idOut);


        /// Disables serialization of the scene node when serialized from a scene.
        void DisableSerialization();

        /// Enables serialization of the scene node when serialized from a scene.
        void EnableSerialization();

        /// Determines whether or not serialization is enabled.
        ///
        /// @remarks
        ///     This is called recursively. If a parent is not serialized, neither is it children.
        bool IsSerializationEnabled() const;





        /// Disables staging on the scene's state stack.
        void DisableStateStackStaging();

        /// Enables staging on the scene's state stack.
        void EnableStateStackStaging();

        /// Determines whether or not state stack staging is enabled.
        ///
        /// @remarks
        ///     This is called recursively. If a parent has staging disabled, so does it's children.
        bool IsStateStackStagingEnabled() const;



    // Events. Use these to make posting events a bit easier.
    public:

        void OnAttach(SceneNode& childNode);
        void OnDetach(SceneNode& childNode);
        void OnDestroy();
        void OnTransform(bool updateDynamicsObject);
        void OnScale();
        void OnLayerChanged(unsigned int prevLayer);
        void OnStaticChanged();
        void OnVisibleChanged();
        //void OnComponentAttached(Component& component);
        //void OnComponentDetached(Component& component);
        void OnUpdate(double deltaTimeInSeconds);
        void OnContact(SceneNode &other, const btManifoldPoint &pt);


    public:

        /// Helper for setting the parent. Do not call this function directly - it is only used internally.
        ///
        /// @param  parent [in] A pointer to the new parent node. Can be null.
        ///
        /// @remarks
        ///     Do not call this function directly. Instead use Attach()/Detach() to set the parent.
        void _SetParent(SceneNode *parent);

        void _SetPrevSibling(SceneNode* newPrevSibling);
        void _SetNextSibling(SceneNode* newNextSibling);



    private:

        /// Locks event posting. Any events that are posted while events are locked will be lost.
        void LockEvents();

        /// Unlocks event posting.
        void UnlockEvents();

        /// Determines whether or not events are locked.
        bool EventsLocked() const;



    private:

        /// The unique ID of the scene node, for use by the scene that the node is currently contained in.
        uint64_t uniqueID;

        /// The name of this node. Should usually be unique, but doesn't need to be. This can be modified, so we'll use a String object
        /// to make things easier.
        String m_name;

        /// The parent of the scene node. If this is null, it is a root object.
        SceneNode* m_parent;

        /// A pointer to the first child node.
        SceneNode* firstChild;

        /// A pointer to the last child node.
        SceneNode* lastChild;

        /// A pointer to the previous sibling.
        SceneNode* prevSibling;

        /// A pointer to the next sibling.
        SceneNode* nextSibling;


        /// The position of the scene node, relative to the parent.
        glm::vec3 m_position;

        /// The orientation of the scene node, relative to the parent.
        glm::quat m_orientation;

        /// The scale of the scene node, relative to the parent.
        glm::vec3 m_scale;



        /// The list of pointers of the event handlers that are attached to this node. This should usually always have at
        /// least a single entry, but doesn't have to.
        List<SceneNodeEventHandler*> eventHandlers;

        /// A map of components. More than one type of component is not allowed. We index this map by the component name.
        Dictionary<Component*> components;

        /// A map of data pointers. A SceneNode should not be inheritted. Instead, additional data can be attached to the scene
        /// node with a size_t as it's key. We use a size_t so we can use a pointer as a key if needed. This could be useful for
        /// component-specific data (use a key equal to the pointer to the component).
        Map<size_t, void*> dataPointers;


        /// The scene this node is attached to, if any. Usually, a scene node is part of a scene. We need to keep track of the
        /// scene so we can notify it of certain events, such as when a component is added or removed. This can be null, in
        /// which case it's assumed that the node is not part of a scene.
        Scene* scene;


        /// The scene node's flags. Defaults to 0. Changing a flags requires the node be removed and re-added to the scene in order to take effect.
        unsigned int m_flags;


        /// The counter used for event locks. If it is > 0, the events are locked. Otherwise they are unlocked. Defaults to 0. LockEvents()
        /// will increment, whereas UnlockEvents() will decrement.
        int eventLockCounter;


        // The component pointers below are used for doing fast retrievals of common components.

        /// A pointer to the model component.
        ModelComponent* modelComponent;

        /// A pointer to the point light component.
        PointLightComponent* pointLightComponent;

        /// A pointer to the spot light component.
        SpotLightComponent* spotLightComponent;

        /// A pointer to the editor metadata component. Renderer's will want access to this component for doing things like selections. This needs fast access.
        EditorMetadataComponent* editorMetadataComponent;
        
        

    public:

        /////////////////////////////////////////////////////////
        // Flags.

        enum Flags
        {
            Nothing                  = 0,

            NoUpdate                 = (1 << 1),
            Static                   = (1 << 2),
            Invisible                = (1 << 3),
            NoPositionInheritance    = (1 << 4),
            NoOrientationInheritance = (1 << 5),
            NoScaleInheritance       = (1 << 6),
            NoSerialization          = (1 << 7),            // <-- Only used when doing serialization from a scene.
            NoStateStackStaging      = (1 << 8),            // <-- Disables staging on the state stack.
        };

        enum SerializationFlags
        {
            NoID                           = (1 << 1),          // <-- Set the ID to 0 when serializing.
            NoEditorMetadataComponent      = (1 << 2),          // <-- Do not save the EditorMetadataComponent if it exists.
            NoScriptPublicVariableOverride = (1 << 3),          // <-- Do not override public variables of a script component.
            NoScriptOnSerialize            = (1 << 4),          // <-- Do not call OnSerialize() on script components.
            NoScriptOnDeserialize          = (1 << 5),          // <-- Do not call OnDeserialize() on script components.
            ForceDeselected                = (1 << 6)           // <-- Force the scene node to be serialized as deselected. Needed for prefabs.
        };


    private:    // No copying.
        SceneNode(const SceneNode &);
        SceneNode & operator=(const SceneNode &);
    };



    // ModelComponent Specialisation.
    template <> inline       ModelComponent* SceneNode::GetComponent<ModelComponent>()       { return this->modelComponent; }
    template <> inline const ModelComponent* SceneNode::GetComponent<ModelComponent>() const { return this->modelComponent; }

    template <> inline bool SceneNode::HasComponent<ModelComponent>() const
    {
        return this->modelComponent != nullptr;
    }



    // PointLightComponent Specialisation.
    template <> inline       PointLightComponent* SceneNode::GetComponent<PointLightComponent>()       { return this->pointLightComponent; }
    template <> inline const PointLightComponent* SceneNode::GetComponent<PointLightComponent>() const { return this->pointLightComponent; }

    template <> inline bool SceneNode::HasComponent<PointLightComponent>() const
    {
        return this->pointLightComponent != nullptr;
    }



    // PointLightComponent Specialisation.
    template <> inline       SpotLightComponent* SceneNode::GetComponent<SpotLightComponent>()       { return this->spotLightComponent; }
    template <> inline const SpotLightComponent* SceneNode::GetComponent<SpotLightComponent>() const { return this->spotLightComponent; }

    template <> inline bool SceneNode::HasComponent<SpotLightComponent>() const
    {
        return this->spotLightComponent != nullptr;
    }



    // EditorMetadataComponent Specialisation.
    template <> inline       EditorMetadataComponent* SceneNode::GetComponent<EditorMetadataComponent>()       { return this->editorMetadataComponent; }
    template <> inline const EditorMetadataComponent* SceneNode::GetComponent<EditorMetadataComponent>() const { return this->editorMetadataComponent; }

    template <> inline bool SceneNode::HasComponent<EditorMetadataComponent>() const
    {
        return this->editorMetadataComponent != nullptr;
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE: If you are adding a specialisation, remember to delete them manually in RemoveAllComponents() and add
    //       them to GetAttachedComponentNames()!
    //
    // Keep this comment at th bottom here so it's never forgotten.
}

#endif
