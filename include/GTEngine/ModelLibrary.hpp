
#ifndef __GTEngine_ModelLibrary_hpp_
#define __GTEngine_ModelLibrary_hpp_

#include "Model.hpp"
#include <GTCore/IO.hpp>

namespace GTEngine
{
    /// Library class for models.
    ///
    /// Models are a many-instantiation resource, meaning we use a Create/Delete system.
    ///
    /// The reason Models are many-instantiation is because models can be in different animations states and also have different
    /// materials applied at a time. Geometric data is shared, and is managed by the library.
    ///
    /// When models are loaded, the bind-pose mesh data remains in-memory
    class ModelLibrary
    {
    // Startup/Shutdown.
    public:

        /// Starts up the model library.
        static bool Startup();

        /// Shutsdown the model library.
        static void Shutdown();


    // Load/Save
    public:

        /// Loads a model from a file.
        ///
        /// @param fileName [in] The name of the file to load.
        ///
        /// @return A pointer to the new model, or null if the file could not be loaded.
        ///
        /// @remarks
        ///     This will not reload the file if it has previously been loaded. Thus, calling this function with the same file will
        ///     be quick after the first load.
        static Model* LoadFromFile(const char* fileName);

        /// Generically loads a file from Neutral File Format (NFF) content.
        ///
        /// @param content [in] The content of the NFF file.
        /// @param name    [in] The name to use for uniquely identifying this NFF. This will usually be based on the format of the model.
        ///
        /// @return A pointer to the new model, or null if the model could not be created.
        ///
        /// @remarks
        ///     This will not reload the model if the same content has been used before.
        static Model* LoadFromNFF(const char* content, const char* name);

        /// Loads a .gtmodel file.
        ///
        /// @param fileName [in] The name of the file to load.
        ///
        /// @return A pointer to the new model, or null if the model could not be created.
        static Model* LoadFromGTMODEL(const char* fileName);


        /// Saves the givem model as a .gtmodel.
        ///
        /// @param model    [in] A reference to the model to save.
        /// @param fileName [in] the file name of the model.
        ///
        /// @return True if the model is saved successfully; false otherwise.
        ///
        /// @remarks
        ///     This will overwrite any existing file without warning.
        ///     @par
        ///     If <fileName> does not have the .gtmodel extension, it will be appended on.
        ///     @par
        ///     This function does not save the model as any format other than .gtmodel.
        static bool WriteToFile(const Model &model, const char* fileName);


    // Create functions.
    public:

        /// Creates a model from a definition.
        ///
        /// @param definition [in] A reference to the model defintinitin to create the model from.
        ///
        /// @return A pointer to the new model if successful; null otherwise.
        static Model* CreateFromDefinition(const ModelDefinition &definition);



        /// Creates a flat plane on the XZ axis, facing the +Y axis.
        static Model* CreatePlaneXZ(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);
        static Model* CreatePlaneXY(float width, float height, VertexFormat &format = VertexFormat::P3T2N3);


        /// Creates a box.
        ///
        /// @remarks
        ///     The box will use the P3T2N3T3B3 vertex format.
        static Model* CreateBox(float halfWidth, float halfHeight, float halfDepth);

        /// Creates a sphere.
        ///
        /// @radius [in] The radius of the sphere.
        static Model* CreateSphere(float radius);

        /// Creates a cylinder, standing up on the y axis.
        ///
        /// @param radius [in] The radius of the cylinder.
        /// @param length [in] The length of the cylinder.
        static Model* CreateCylinder(float radius, float length);


        /// Creates a model from a convex-hull.
        ///
        /// @param convexHull [in] A reference to the convexHull
        static Model* CreateFromConvexHull(const ConvexHull &convexHull);


    // Delete functions.
    public:

        /// Deletes a model that was created with the model library.
        static void Delete(Model* model);



    // Misc functions.
    public:

        /// Determines whether or not the given file extension is supported.
        ///
        /// @remarks
        ///     If this function returns true, it does not necessarily mean it can load every file of that extension.
        static bool IsExtensionSupported(const char* extension);



    /////////////////////////////////////////////////////
    // Private Functions.
    private:

        /// A helper function for importing a file via Assimp and saving the corresponding .gtmodel file.
        ///
        /// @param sourceInfo  [in] The file info of the source file.
        /// @param gtmodelInfo [in] The file info of the gtmodel file.
        static Model* LoadFromAssimp(const GTCore::IO::FileInfo &sourceInfo, const GTCore::IO::FileInfo &gtmodelInfo);


        /// A helper function for only loading the engine-specific metadata of the given .gtmodel file.
        ///
        /// @param file       [in] The file to load the metadata from.
        /// @param definition [in] The definition to read the data into.
        ///
        /// @remarks
        ///     This function asserts that 'file' is located at the start of the metadata.
        static bool LoadGTMODELMetadata(FILE* file, ModelDefinition &definition);

        /// A helper function for writing the engine-specific metadata to the given .gtmodel file.
        ///
        /// @param file       [in] The file to write the metadata to.
        /// @param definition [in] The definition to write the data from.
        ///
        /// @remarks
        ///     This function asserts that 'file' is located at the start of the metadata. This function will write
        ///     the 'metadata' ID.
        static bool WriteGTMODELMetadata(FILE* file, const ModelDefinition &definition);

        /// A helper function for only loading the engine-specific metadata of the given .gtmodel file.
        ///
        /// @param file       [in] The file to load the metadata from.
        /// @param definition [in] The definition to read the data into.
        static bool LoadGTMODELMetadata(const char* file, ModelDefinition &definition);
    };
}

#endif