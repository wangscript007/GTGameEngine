// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameScript
#define GT_GameScript

#include <GTGE/Script.hpp>
#include <GTGE/Core/String.hpp>
#include <GTGE/Core/Vector.hpp>
#include <dr_libs/dr_fs.h>

namespace GT
{
    class Context;

    /// Class representing the scripting environment of a game.
    ///
    /// This class is actually a GT::Script object, but with additional stuff to make it suitable for use by a game.
    ///
    /// Anything involving a script is done through this, including configuration.
    ///
    /// The commands sent to the engine are actually just Lua scripts which are executed through the scripting environment.
    class GameScript : public GT::Script
    {
    public:

        /// Constructor.
        GameScript(Context &context);

        /// Destructor.
        ~GameScript();

        /// Starts up the game script.
        ///
        /// @return True if the script is started up properly.
        bool Startup();


        /// GT::Script::LoadFile().
        ///
        /// @remarks
        ///     This is a special implementation that keeps track of the file so we can do auto-reloading.
        ///     @par
        ///     This will also look for the absolute path using the search directories system.
        bool LoadFile(drfs_context* pVFS, const char* fileName);


        /// Retrieves a reference to the game object that owns this environment.
              Context & GetContext()       { return this->context; }
        const Context & GetContext() const { return this->context; }


        /// Retrieves a string containing the last error message for a failed script.
        const char* GetLastError() { return this->lastError.c_str(); }

        /// Sets the last error message.
        ///
        /// @remarks
        ///     This should only really be used internally. Mainly used for the error handler.
        void SetLastError(const char* newLastError) { this->lastError = newLastError; }


        /// Determines whether or not the file has been loaded.
        ///
        /// @param fileName [in] The name of the file to check.
        ///
        /// @return True if the file has already been loaded, false otherwise.
        bool HasFileBeenLoaded(const char* fileName) const;



        /////////////////////////////////////////////
        // Public FFI functions.

        //struct FFI
        //{
            /// A helper for retrieving the Game instance from the script.
            //static Game & GetGameObject(GT::Script &script);
        //};


    private:

        /// Private function for registering the foreign-function interface.
        //bool RegisterFFI();


    private:

        /// A reference to the game that owns this environment.
        Context &context;

        /// The last error message generated by a call to execute.
        String lastError;

        /// Keeps track of the loaded files.
        Vector<String> loadedFiles;


        /// The error handler of the game script.
        struct ErrorHandler : GT::ScriptErrorHandler
        {
            /// Constructor.
            ErrorHandler(GameScript &script);

            /// GT::ScriptErrorHandler::OnError().
            void OnError(GT::Script &, const char* message);



            /// A reference to the game script that owns this error handler.
            GameScript &script;


        private:    // No copying.
            ErrorHandler(const ErrorHandler &);
            ErrorHandler & operator=(const ErrorHandler &);

        }errorHandler;
    };
}

#endif
