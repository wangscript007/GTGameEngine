// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_GameUpdateJob
#define GT_GameUpdateJob

#include <GTEngine/Core/Threading/Job.hpp>

namespace GT
{
    class Game;

    class GameUpdateJob : public Job
    {
    public:

        /**
        *   \brief  Constructor.
        */
        GameUpdateJob(Game &game);

        /**
        *   \brief  Destructor.
        */
        ~GameUpdateJob();

        /// Threading::Job::Run().
        void Run();


    private:

        /// The game this update job is for.
        Game &game;
    };
}

#endif