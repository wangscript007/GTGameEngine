// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Animation
#define GT_Animation

#include "AnimationChannel.hpp"
#include "AnimationSegment.hpp"
#include "AnimationSequence.hpp"
#include "AnimationTrack.hpp"
#include "../Serialization.hpp"
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    /// Class representing an animation.
    ///
    /// An animation is made up of a timeline and a bunch of channels that are linked to the timeline.
    class Animation
    {
    public:

        /// Constructor.
        Animation();

        /// Destructor.
        ~Animation();


        /// Appends a new key frame.
        ///
        /// @param time [in] The time in the timeline to append the keyframe.
        ///
        /// @return The index of the key frame.
        ///
        /// @remarks
        ///     If a key frame at the same time already exists, this function will return the existing frame.
        size_t AppendKeyFrame(double time);

        /// Retrieves the number of key frames in the timeline.
        ///
        /// @return The number of key frames in the animation.
        size_t GetKeyFrameCount() const { return this->keyFrames.count; }

        /// Retrieves the time of the key frame at the given index.
        ///
        /// @param index [in] The index of the key frame whose time is being retrieved.
        ///
        /// @return The time of the given key frame.
        double GetKeyFrameTimeByIndex(size_t index) const;


        /// Adds a channel to the animation.
        ///
        /// @return The new channel.
        ///
        /// @remarks
        ///     Delete the channel with DeleteChannel().
        AnimationChannel & CreateChannel();

        /// Removes a channel from the animation.
        ///
        /// @param channel [in] A reference to the channel to delete.
        void DeleteChannel(AnimationChannel &channel);





        /// Adds a named segment to the animation.
        ///
        /// @param name          [in] The name of the segment.
        /// @param startKeyFrame [in] The index of the key frame to start at.
        /// @param endKeyFrame   [in] The index of the key frame to end at.
        void AddNamedSegment(const char* name, size_t startKeyFrame, size_t endKeyFrame);

        /// Removes the first segment with the given name.
        ///
        /// @param name [in] The name of the segment to remove.
        void RemoveFirstSegmentByName(const char* name);

        /// Removes every segment with the given name.
        ///
        /// @param name [in] The name of the segments to remove.
        void RemoveSegmentsByName(const char* name);

        /// Removes the segment at the given index.
        ///
        /// @param name [in] The index of the segment to remove.
        void RemoveSegmentByIndex(size_t index);


        /// Retrieves a pointer to a first segment with the given name.
        ///
        /// @param name [in] The name of the segment to retrieve.
              AnimationSegment* GetNamedSegment(const char* name);
        const AnimationSegment* GetNamedSegment(const char* name) const;

        /// Retrieves the number of named segments.
        size_t GetNamedSegmentCount() const { return this->segments.count; }

        /// Retrieves a named segment by it's index.
        ///
        /// @param index [in] The index of the segment to retrieve.
        ///
        /// @remarks
        ///     Segments are sorted alphabetically by name, so the index of items will change as segments are added or removed.
              AnimationSegment* GetNamedSegmentByIndex(size_t index);
        const AnimationSegment* GetNamedSegmentByIndex(size_t index) const;

        /// Retrieves the name of a named segment by it's index.
        ///
        /// @param index [in] The index of the segment whose name is being retrieved.
        const char* GetNamedSegmentNameByIndex(size_t index) const;


        /////////////////////////////////////////////////////////////////////////
        // Playback

        /// Plays the given sequence.
        ///
        /// @param sequence       [in] A reference to the sequence that should be played.
        /// @param transitionTime [in] The time to spend transitioning to the new animation.
        void Play(const AnimationSequence &sequence, double transitionTime = 0.0);

        /// Completely stops playback of whatever is playing.
        ///
        /// @remarks
        ///     This will reset the state of the animation. To halt the animation in place, use Pause().
        void Stop();

        /// Pauses playback.
        void Pause();

        /// Resumes playback from a paused state.
        void Resume();

        /// Steps the animation by the given amount of time, in seconds.
        ///
        /// @param step [in] The number of seconds to step the animation by.
        void Step(double step);

        /// Sets the playback time of the current animation.
        ///
        /// @param time [in] The new playback time.
        void SetPlaybackTime(double time);

        /// Retrieves the current playback time.
        double GetPlaybackTime() const { return this->playbackTime; }


        /// Retrieves the key frames and an interpolation factor for the given time in the currently playing animation.
        ///
        /// @param time          [in ] The time in the current animation whose keys are being retrieved.
        /// @param startKeyFrame [out] A reference to the variable that will receive the index to the first key frame.
        /// @param endKeyFrame   [out] A reference to the variable that will receive the index to the second key frame.
        ///
        /// @return The interpolation factor to interpolated between startKeyFrame and endKeyFrame.
        float GetKeyFramesAtTime(double time, size_t &startKeyFrame, size_t &endKeyFrame);

        /// Retrieves the key frames and an interpolation factor for the current playback.
        ///
        /// @param startKeyFrame [out] A reference to the variable that will receive the index to the first key frame.
        /// @param endKeyFrame   [out] A reference to the variable that will receive the index to the second key frame.
        ///
        /// @return The interpolation factor to interpolated between startKeyFrame and endKeyFrame.
        float GetKeyFramesAtCurrentPlayback(size_t &startKeyFrame, size_t &endKeyFrame);


        /// Determines whether or not the animation is playing.
        ///
        /// @return True if the animation is playing; false otherwise.
        bool IsPlaying() const { return this->isPlaying; }

        /// Determines whether or not the animation is paused.
        ///
        /// @return True if the animation is paused; false otherwise.
        bool IsPaused() const { return this->isPaused; }



        /// Clears the animation.
        ///
        /// @param clearNamedSegments [in] Whether or not named segments should be cleared.
        void Clear(bool clearNamedSegments = true);

        /// Clears only the named animation segments.
        void ClearNamedSegments();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Serializes the animation state.
        ///
        /// @param serializer [in] A reference ot the serializer to write to.
        ///
        /// @remarks
        ///     Currently, this only saves the current playback state. It does not save the key frames, channels and named segments. The serialized data
        ///     is marked with a version number, which will make this easy enough to change in the future if required.
        void Serialize(Serializer &serializer) const;

        /// Deserializes the animation state.
        ///
        /// @param deserializer [in] A reference to the deserializer for reading the data from.
        void Deserialize(Deserializer &deserializer);



    private:

        /// The map of key frames, sorted by time.
        Map<double, AnimationKeyFrame> keyFrames;

        /// The list of channels attached to the animation.
        Vector<AnimationChannel*> channels;

        /// The list of animation segments.
        Vector<AnimationSegment> segments;


        /// The animation track of the currently running animation.
        AnimationTrack currentAnimationTrack;


        /// Keeps track of whether or not the animation is playing.
        bool isPlaying;

        /// Keeps track of whether or not the animation is paused. If this is true, 'isPlaying' will also be true.
        bool isPaused;

        /// The current playback time in the currently running animation.
        double playbackTime;

        /// The index of the local key frame in the current animation track to loop back to when looping.
        size_t loopStartIndex;
    };
}

#endif
