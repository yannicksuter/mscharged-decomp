#ifndef GAME_MOVIE_RENDER_TASK_H
#define GAME_MOVIE_RENDER_TASK_H

#include "NL/nlTask.h"

class UnidentifiedMovieRenderTask : public nlTask
{
public:
    UnidentifiedMovieRenderTask();

    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Movie Render";
    }
    virtual void StateTransition(u32, u32);
};

#endif // GAME_MOVIE_RENDER_TASK_H
