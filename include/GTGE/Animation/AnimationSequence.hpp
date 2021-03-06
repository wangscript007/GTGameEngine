// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_AnimationSequence
#define GT_AnimationSequence

#include <GTGE/Core/String.hpp>
#include <GTGE/Core/Vector.hpp>

namespace GT
{
    /// Class representing an individual frame in an animation sequence.
    struct AnimationSequenceFrame
    {
        AnimationSequenceFrame()
            : segmentName(), startKeyFrame(0), endKeyFrame(0),
              transitionTime(0.0),
              loop(false)
        {
        }

        /// The name of the segment to play. Can be an empty string, in which case startKeyFrame and endKeyFrame will be used instead.
        String segmentName;

        /// The index of the key frame to start at.
        size_t startKeyFrame;

        /// The index of the key frame to end at.
        size_t endKeyFrame;

        // The amount of time in seconds to spend transitioning to this frame from the previous.
        double transitionTime;

        /// Whether or not the frame should loop. Only valid for the last frame. This allows transitions like attack -> idle.
        bool loop;
    };
}

namespace GT
{
    /// Class used for playing a sequence of animations.
    ///
    /// This allows for flexible animation control. Each sequence has "frames" which control which segment of the animation track should
    /// be played at certain points, and how.
    class AnimationSequence
    {
    public:

        /// Constructor.
        AnimationSequence();

        /// Destructor.
        virtual ~AnimationSequence();


        /// Sets the name of the animation sequence.
        ///
        /// @param newName [in] The new name of the sequence.
        void SetName(const char* newName);

        /// Retrieves the name of the animation sequence.
        const char* GetName() const;


        /// Adds a named sequence frame.
        ///
        /// @param segmentName    [in] The name of the segment that should be played.
        /// @param transitionTime [in] The time to spend transitioning from the previous frame.
        /// @param loop           [in] Whether or not the sequence should loop.
        ///
        /// @remarks
        ///     <loop> should only be set to try for the last frame. It does not make sense to loop on anything other than that.
        void AddFrame(const char* segmentName, double transitionTime, bool loop = false);

        /// Adds a frame from a start and end key frame index.
        ///
        /// @param startKeyFrame [in] The index of the key frame to start at.
        /// @param endKeyFrame   [in] The index of the key frame to end at.
        /// @param loop          [in] Whether or not the sequence should loop.
        ///
        /// @remarks
        ///     <loop> should only be set to try for the last frame. It does not make sense to loop on anything other than that.
        void AddFrame(size_t startKeyFrame, size_t endKeyFrame, double transitionTime, bool loop = false);


        /// Returns a constant reference to the internal list of frames.
        const Vector<AnimationSequenceFrame> & GetFrames() const { return this->frames; }

        /// Retrieves the number of frames.
        size_t GetFrameCount() const { return this->frames.count; }

        /// Retrieves a frame at the given index.
        ///
        /// @param index [in] The index of the frame to retrieve.
              AnimationSequenceFrame & GetFrame(size_t index)       { return this->frames[index]; }
        const AnimationSequenceFrame & GetFrame(size_t index) const { return this->frames[index]; }


    private:

        /// The name of the animation sequence.
        String name;

        /// The list of sequence frames.
        Vector<AnimationSequenceFrame> frames;
    };
}

#endif