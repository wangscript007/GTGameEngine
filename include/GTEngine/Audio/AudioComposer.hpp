// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Audio_AudioComposer_hpp_
#define __GTEngine_Audio_AudioComposer_hpp_

#include "../Math.hpp"

namespace GTEngine
{
    /// Static class for playing sounds.
    class AudioComposer
    {
    // Playback.
    public:

        /// Plays the sound given by the specified file name.
        ///
        /// @param fileName [in] The file name of the sound to play.
        static bool Play(const char* fileName, bool relativePositioning = true);
        static bool Play(const char* fileName, float x, float y, float z, bool relativePositioning = false);
        static bool Play(const char* fileName, const glm::vec3 &position, bool relativePositioning = false) { return Play(fileName, position.x, position.y, position.z, relativePositioning); }



    // Listener.
    public:

        /// Sets the position of the listener.
        static void SetListenerPosition(float x, float y, float z);
        static void SetListenerPosition(const glm::vec3 &position) { SetListenerPosition(position.x, position.y, position.z); }

        /// Sets the orientation of the listener.
        static void SetListenerOrientation(const glm::quat &orientation);


    // Misc.
    public:

        /// Determines whether or not the given extension is supported by the composer.
        ///
        /// @param extension [in] The extension to check. For example: "wav", "ogg"
        static bool IsFileExtensionSupported(const char* extension);
    };
}


#endif
